#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1
context=$2

tag=$(git tag --sort v:refname | tail -n 1)
folder="globox_bin_$tag/lib/globox/x11"
folder_include="globox_bin_$tag/include"

# library makefile data
output="make/output"
name="globox_x11"
cc="gcc"

src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/globox_private_getters.c")
src+=("src/x11/globox_x11.c")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")
flags+=("-fPIC")

#defines+=("-DGLOBOX_ERROR_ABORT")
#defines+=("-DGLOBOX_ERROR_SKIP")
defines+=("-DGLOBOX_ERROR_LOG_THROW")

# library platform
defines+=("-DGLOBOX_PLATFORM_X11")

# generated linker arguments
link+=("xcb")

if [ -z "$build" ]; then
	read -rp "select build type (development | release | sanitized): " build
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
	read -rp "select context type (software | egl | glx): " context
fi

case $context in
	software)
makefile=makefile_lib_x11_software
name+="_software"
src+=("src/x11/software/globox_x11_software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
link+=("xcb-shm")
link+=("xcb-randr")
link+=("xcb-render")
	;;

	egl)
makefile=makefile_lib_x11_egl
name+="_egl"
src+=("src/x11/egl/globox_x11_egl.c")
defines+=("-DGLOBOX_CONTEXT_EGL")
link+=("egl")
link+=("glesv2")
	;;

	glx)
makefile=makefile_lib_x11_glx
name+="_glx"
src+=("src/x11/glx/globox_x11_glx.c")
defines+=("-DGLOBOX_CONTEXT_GLX")
link+=("gl")
link+=("glesv2")
link+=("x11 x11-xcb xrender")
	;;

	*)
echo "invalid context type"
exit 1
	;;
esac

# add the libraries as default targets
default+=("$folder/$name.a")
default+=("$folder/$name.so")

# makefile start
mkdir -p "$output"

{ \
echo ".POSIX:"; \
echo "FOLDER = $folder";\
echo "FOLDER_INCLUDE = $folder_include";\
echo "NAME = $name"; \
echo "CC = $cc"; \
} > "$output/$makefile"

# makefile linking info
echo "" >> "$output/$makefile"
for flag in $(pkg-config "${link[@]}" --cflags) "${ldflags[@]}"; do
	echo "LDFLAGS+= $flag" >> "$output/$makefile"
done

echo "" >> "$output/$makefile"
for flag in $(pkg-config "${link[@]}" --libs); do
	echo "LDLIBS+= $flag" >> "$output/$makefile"
done

# makefile compiler flags
echo "" >> "$output/$makefile"
for flag in "${flags[@]}"; do
	echo "CFLAGS+= $flag" >> "$output/$makefile"
done

echo "" >> "$output/$makefile"
for define in "${defines[@]}"; do
	echo "CFLAGS+= $define" >> "$output/$makefile"
done

# makefile object list
echo "" >> "$output/$makefile"
for file in "${src[@]}"; do
	folder=$(dirname "$file")
	filename=$(basename "$file" .c)
	echo "OBJ+= $folder/$filename.o" >> "$output/$makefile"
done

# makefile default target
echo "" >> "$output/$makefile"
echo "default:" "${default[@]}" >> "$output/$makefile"

# makefile library targets
echo "" >> "$output/$makefile"
cat make/lib/templates/targets_linux.make >> "$output/$makefile"

# makefile object targets
echo "" >> "$output/$makefile"
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> "$output/$makefile"
done

# makefile extra targets
echo "" >> "$output/$makefile"
cat make/lib/templates/targets_extra.make >> "$output/$makefile"
