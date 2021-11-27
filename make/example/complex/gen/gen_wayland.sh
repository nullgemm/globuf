#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../../..

build=$1
context=$2
library=$3

tag=$(git tag --sort v:refname | tail -n 1)

# library makefile data
cc="gcc"
obj+=("res/icon/iconpix.o")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Iglobox_bin_$tag/include")
flags+=("-Ires/wayland_headers")
flags+=("-Ires/cursoryx/src")
flags+=("-Ires/dpishit/src")
flags+=("-Ires/willis/src")
src+=("example/complex/desktop.c")
src+=("res/cursoryx/src/wayland.c")
src+=("res/dpishit/src/wayland.c")
src+=("res/dpishit/src/nix.c")
src+=("res/willis/src/willis.c")
src+=("res/willis/src/debug.c")
src+=("res/willis/src/wayland.c")
src+=("res/willis/src/xkb.c")

defines+=("-DGLOBOX_PLATFORM_WAYLAND")
defines+=("-DCURSORYX_WAYLAND")
defines+=("-DDPISHIT_WAYLAND")
defines+=("-DWILLIS_WAYLAND")
defines+=("-DWILLIS_DEBUG")

# generated linker arguments
link+=("wayland-client")
link+=("wayland-cursor")
link+=("xkbcommon")
ldlibs+=("-lrt")

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
	read -rp "select context type (software | egl): " context
fi

case $context in
	software)
makefile=makefile_example_complex_wayland_software
name="globox_example_complex_wayland_software"
globox="globox_wayland_software"
src+=("example/complex/software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	egl)
makefile=makefile_example_complex_wayland_egl
name="globox_example_complex_wayland_egl"
globox="globox_wayland_egl"
src+=("example/complex/egl.c")
defines+=("-DGLOBOX_CONTEXT_EGL")
link+=("wayland-egl")
link+=("egl")
link+=("glesv2")
	;;

	*)
echo "invalid context type"
exit 1
	;;
esac

# link type
if [ -z "$library" ]; then
	read -rp "select library type (static | shared): " library
fi

case $library in
	static)
obj+=("globox_bin_$tag/lib/globox/wayland/$globox.a")
cmd="./$name"
	;;

	shared)
ldflags+=("-Lglobox_bin_$tag/lib/globox/wayland")
ldlibs+=("-l:$globox.so")
cmd="LD_LIBRARY_PATH=../globox_bin_$tag/lib/globox/wayland ./$name"
	;;

	*)
echo "invalid library type"
exit 1
	;;
esac

# default target
default+=("bin/\$(NAME)")

# valgrind flags
valgrind+=("--show-error-list=yes")
valgrind+=("--show-leak-kinds=all")
valgrind+=("--track-origins=yes")
valgrind+=("--leak-check=full")
valgrind+=("--suppressions=../res/valgrind.supp")

# makefile start
{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CMD = $cmd"; \
echo "CC = $cc"; \
} > $makefile

# makefile linking info
echo "" >> $makefile
for flag in $(pkg-config "${link[@]}" --cflags) "${ldflags[@]}"; do
	echo "LDFLAGS+= $flag" >> $makefile
done

echo "" >> $makefile
for flag in $(pkg-config "${link[@]}" --libs) "${ldlibs[@]}"; do
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

echo "" >> $makefile
for prebuilt in "${obj[@]}"; do
	echo "OBJ_EXTRA+= $prebuilt" >> $makefile
done

# generate valgrind flags
echo "" >> $makefile
for flag in "${valgrind[@]}"; do
	echo "VALGRIND+= $flag" >> $makefile
done

# makefile default target
echo "" >> $makefile
echo "default:" "${default[@]}" >> $makefile

# makefile linux targets
echo "" >> $makefile
cat make/example/complex/templates/targets_linux.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/example/complex/templates/targets_extra.make >> $makefile
