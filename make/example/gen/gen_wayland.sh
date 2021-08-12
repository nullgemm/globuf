#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")"
cd ../../..

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

defines+=("-DGLOBOX_PLATFORM_WAYLAND")

# generated linker arguments
link+=("wayland-client")
ldlibs+=("-lrt")

read -p "select build type ([1] development | [2] release | [3] sanitizers): " build

case $build in
	[1]* ) # development build
flags+=("-g")
defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
	;;

	[2]* ) # release build
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fstack-protector-strong")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
ldflags+=("-z relro")
ldflags+=("-z now")
	;;

	[3]* ) # sanitized build
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
esac

# context type
read -p "select context type ([1] software | [2] egl): " context

case $context in
	[1]* ) # software context
makefile=makefile_example_wayland_software
name="globox_example_wayland_software"
globox="globox_wayland_software"
src+=("example/software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	[2]* ) # egl context
makefile=makefile_example_wayland_egl
name="globox_example_wayland_egl"
globox="globox_wayland_egl"
src+=("example/egl.c")
defines+=("-DGLOBOX_CONTEXT_EGL")
link+=("wayland-egl")
link+=("egl")
link+=("glesv2")
	;;
esac

# link type
read -p "select library type ([1] static | [2] shared): " library

case $library in
	[1]* ) # link statically
obj+=("globox_bin_$tag/lib/globox/wayland/$globox.a")
cmd="./$name"
	;;

	[2]* ) # link dynamically
ldflags+=("-Lglobox_bin_$tag/lib/globox/wayland")
ldlibs+=("-l:$globox.so")
cmd="LD_LIBRARY_PATH=../globox_bin_$tag/lib/globox/wayland ./$name"
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
echo ".POSIX:" > $makefile
echo "NAME = $name" >> $makefile
echo "CMD = $cmd" >> $makefile
echo "CC = $cc" >> $makefile

# makefile linking info
echo "" >> $makefile
for flag in $(pkg-config ${link[@]} --cflags) ${ldflags[@]}; do
	echo "LDFLAGS+= $flag" >> $makefile
done

echo "" >> $makefile
for flag in $(pkg-config ${link[@]} --libs) ${ldlibs[@]}; do
	echo "LDLIBS+= $flag" >> $makefile
done

# makefile compiler flags
echo "" >> $makefile
for flag in ${flags[@]}; do
	echo "CFLAGS+= $flag" >> $makefile
done

echo "" >> $makefile
for define in ${defines[@]}; do
	echo "CFLAGS+= $define" >> $makefile
done

# makefile object list
echo "" >> $makefile
for file in ${src[@]}; do
	folder=$(dirname "$file")
	filename=$(basename "$file" .c)
	echo "OBJ+= $folder/$filename.o" >> $makefile
done

echo "" >> $makefile
for prebuilt in ${obj[@]}; do
	echo "OBJ_EXTRA+= $prebuilt" >> $makefile
done

# generate valgrind flags
echo "" >> $makefile
for flag in ${valgrind[@]}; do
	echo "VALGRIND+= $flag" >> $makefile
done

# makefile default target
echo "" >> $makefile
echo "default: ${default[@]}" >> $makefile

# makefile linux targets
echo "" >> $makefile
cat make/example/templates/targets_linux.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in ${src[@]}; do
	$cc $defines -MM -MG $file >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/example/templates/targets_extra.make >> $makefile
