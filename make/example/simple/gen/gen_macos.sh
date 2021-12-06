#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../../..

build=$1
context=$2
toolchain=$3
current_toolchain=$4
library=$5

tag=$(git tag --sort v:refname | tail -n 1)

# library makefile data
obj+=("res/icon/iconpix_mach.o")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Iglobox_bin_$tag/include")

defines+=("-DGLOBOX_PLATFORM_MACOS")

# generated linker arguments
ldlibs+=("-framework AppKit")

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
flags+=("-O2")
	;;

	sanitized)
flags+=("-g")
flags+=("-fno-omit-frame-pointer")
flags+=("-fPIE")
flags+=("-O2")
flags+=("-fsanitize=undefined")
flags+=("-fsanitize=function")
flags+=("-fsanitize=address")
ldflags+=("-fsanitize=undefined")
ldflags+=("-fsanitize=function")
ldflags+=("-fsanitize=address")
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
makefile="makefile_example_simple_macos_software"
name="globox_example_simple_macos_software"
globox="globox_macos_software"
src+=("example/simple/software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	egl)
makefile="makefile_example_simple_macos_egl"
name="globox_example_simple_macos_egl"
globox="globox_macos_egl"
src+=("example/simple/egl.c")
flags+=("-Ires/angle/include")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldflags+=("-Lres/angle/libs")
ldlibs+=("-lEGL")
ldlibs+=("-lGLESv2")
default+=("res/angle/libs")
default+=("bin/libEGL.dylib")
obj+=("res/shaders/gles2/square_vert_gles2_mach.o")
obj+=("res/shaders/gles2/square_frag_gles2_mach.o")
	;;

	*)
echo "invalid context type"
exit 1
	;;
esac

# toolchain type
if [ -z "$toolchain" ]; then
	read -rp "select target toolchain type (osxcross | native): " toolchain
fi

case $toolchain in
	osxcross)
cc="o64-clang"
objcopy="objcopy"
	;;

	native)
makefile+="_native"
name+="_native"
globox+="_native"
cc="clang"
objcopy="/usr/local/Cellar/binutils/*/bin/objcopy"
	;;

	*)
echo "invalid target toolchain type"
exit 1
	;;
esac

if [ -z "$current_toolchain" ]; then
	read -rp "select current toolchain type (osxcross | native): " current_toolchain
fi

case $current_toolchain in
	osxcross)
cch="o64-clang"
	;;

	native)
cch="clang"
	;;

	*)
echo "invalid current toolchain type"
exit 1
	;;
esac

# link type
if [ -z "$library" ]; then
	read -rp "select library type (static | shared): " library
fi

case $library in
	static)
obj+=("globox_bin_$tag/lib/globox/macos/$globox.a")
cmd="./$name"
	;;

	shared)
ldflags+=("-Lglobox_bin_$tag/lib/globox/macos")
ldlibs+=("-l$globox")
cmd="../make/scripts/dylib_copy.sh $globox && ./$name"
	;;

	*)
echo "invalid library type"
exit 1
	;;
esac

# default target
default+=("bin/\$(NAME).app")

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
echo "OBJCOPY = $objcopy"; \
echo "CC = $cc"; \
} > $makefile

# makefile linking info
echo "" >> $makefile
for flag in "${ldflags[@]}"; do
	echo "LDFLAGS+= $flag" >> $makefile
done

echo "" >> $makefile
for flag in "${ldlibs[@]}"; do
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
cat make/example/simple/templates/targets_macos.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cch "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/example/simple/templates/targets_extra.make >> $makefile
