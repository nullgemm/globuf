#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")"
cd ../../..

build=$1
context=$2

# library makefile data
name="globox"
cc="gcc"

src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/globox_private_getters.c")
src+=("src/wayland/globox_wayland.c")
src+=("src/wayland/globox_wayland_callbacks.c")

src+=("res/wayland_headers/xdg-shell-protocol.c")
src+=("res/wayland_headers/kde-blur-protocol.c")
src+=("res/wayland_headers/zwp-relative-pointer-protocol.c")
src+=("res/wayland_headers/zwp-pointer-constraints-protocol.c")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")
flags+=("-Ires/wayland_headers")
flags+=("-fPIC")

#defines+=("-DGLOBOX_ERROR_ABORT")
#defines+=("-DGLOBOX_ERROR_SKIP")
defines+=("-DGLOBOX_ERROR_LOG_THROW")

# library platform
defines+=("-DGLOBOX_PLATFORM_WAYLAND")

# generated linker arguments
link+=("wayland-client")
ldlibs+=("-lrt")

if [ -z "$build" ]; then
	read -p "select build type (development | release | sanitized): " build
fi

case $build in
	development)
flags+=("-g")
defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
	;;

	release)
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fstack-protector-strong")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
ldflags+=("-z relro")
ldflags+=("-z now")
	;;

	sanitized)
cc="clang"
flags+=("-g")
flags+=("-fno-omit-frame-pointer")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
flags+=("-fsanitize=undefined")
flags+=("-mllvm")
flags+=("-msan-keep-going=1")
flags+=("-fsanitize=memory")
flags+=("-fsanitize-memory-track-origins=2")
flags+=("-fsanitize=function")
ldflags+=("-fsanitize=undefined")
ldflags+=("-fsanitize=memory")
ldflags+=("-fsanitize-memory-track-origins=2")
ldflags+=("-fsanitize=function")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# context type
if [ -z "$context" ]; then
	read -p "select context type (software | egl): " context
fi

case $context in
	software)
makefile=makefile_lib_wayland_software
src+=("src/wayland/software/globox_wayland_software.c")
src+=("src/wayland/software/globox_wayland_software_helpers.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
ldlibs+=("-lpthread")
	;;

	egl)
makefile=makefile_lib_wayland_egl
src+=("src/wayland/egl/globox_wayland_egl.c")
src+=("src/wayland/egl/globox_wayland_egl_helpers.c")
defines+=("-DGLOBOX_CONTEXT_EGL")
link+=("wayland-egl")
link+=("egl")
link+=("glesv2")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# add the libraries as default targets
default+=("res/wayland_headers")
default+=("bin/$name.a")
default+=("bin/$name.so")

# generate required headers
make/scripts/wayland_get.sh

# makefile start
echo ".POSIX:" > $makefile
echo "NAME = $name" >> $makefile
echo "CC = $cc" >> $makefile

# makefile linking info
echo "" >> $makefile
for flag in $(pkg-config ${link[@]} --cflags) ${ldflags[@]}; do
	echo "LDFLAGS+= $flag" >> $makefile
done

echo "" >> $makefile
for flag in $(pkg-config ${link[@]} --libs) "${ldlibs[@]}"; do
	echo "LDLIBS+= $flag" >> $makefile
done

# makefile compiler flags
echo "" >> $makefile
for flag in "${flags[@]}"; do
	echo "CFLAGS+= $flag" >> $makefile
done

echo "" >> $makefile
for define in "${defines[@]}"; do
	echo "CFLAGS+= $define" >> $makefile
done

# makefile object list
echo "" >> $makefile
for file in "${src[@]}"; do
	folder=$(dirname "$file")
	filename=$(basename "$file" .c)
	echo "OBJ+= $folder/$filename.o" >> $makefile
done

# makefile default target
echo "" >> $makefile
echo "default: ${default[@]}" >> $makefile

# makefile library targets
echo "" >> $makefile
cat make/lib/templates/targets_linux.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cc $defines -MM -MG $file >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/lib/templates/targets_extra.make >> $makefile
