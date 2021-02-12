#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ..

# generate makefile
src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/macos/globox_macos.c")
src+=("src/macos/globox_macos_symbols.c")
src+=("src/macos/globox_macos_callbacks.c")

example+=("res/icon/iconpix_mach.o")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")

defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
#defines+=("-DGLOBOX_ERROR_SKIP")
#defines+=("-DGLOBOX_ERROR_ABORT")

defines+=("-DGLOBOX_PLATFORM_MACOS")

ldlibs+=("-framework AppKit")

# build type
read -p "optimize? ([1] optimize | [2] debug): " optimize

if [ $optimize -eq 1 ]; then
flags+=("-O2")
else
flags+=("-g")
fi

# context-dependent additions
read -p "select context type ([1] software | [2] egl): " context

case $context in
	[1]* )
# software context
makefile=makefile_macos_software
example_src+=("example/software.c")
src+=("src/macos/software/globox_macos_software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	[2]* )
# egl context
makefile=makefile_macos_egl
example_src+=("example/egl.c")
src+=("src/macos/egl/globox_macos_egl.c")
flags+=("-Ires/angle/include")
ldflags+=("-Lres/angle/libs")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldlibs+=("-lEGL")
ldlibs+=("-lGLESv2")
	;;
esac

# tools
valgrind+=("--show-error-list=yes")
valgrind+=("--show-leak-kinds=all ")
valgrind+=("--track-origins=yes ")
valgrind+=("--leak-check=full ")
valgrind+=("--suppressions=../res/valgrind.supp")

# rename makefile if not cross-compiling
read -p "are you cross-compiling? ([1] yes | [2] no): " native

if [ $native -eq 2 ]; then
	makefile+="_native"
	cc=clang
else
	cc=o64-clang
fi

# create empty makefile
echo ".POSIX:" > $makefile
echo "NAME = globox" >> $makefile
echo "CMD = ./globox.app" >> $makefile

# generate linking info
echo "" >> $makefile
echo "CC = $cc" >> $makefile
echo "LDFLAGS+= ${ldflags[@]}" >> $makefile
echo "LDLIBS+= ${ldlibs[@]}" >> $makefile

# change objcopy path if not cross-compiling
if [ $native -eq 2 ]; then
	echo "OBJCOPY = /usr/local/Cellar/binutils/*/bin/objcopy" >> $makefile
else
	echo "OBJCOPY = objcopy" >> $makefile
fi

# generate valgrind flags
echo "" >> $makefile
for file in ${valgrind[@]}; do
	echo "VALGRIND+= $file" >> $makefile
done

# generate compiler flags
echo "" >> $makefile
for file in ${flags[@]}; do
	echo "CFLAGS+= $file" >> $makefile
done

echo "" >> $makefile
for file in ${defines[@]}; do
	echo "CFLAGS+= $file" >> $makefile
done

# generate object list
echo "" >> $makefile
for file in ${src[@]}; do
	folder=$(dirname "$file")
	name=$(basename "$file" .c)
	echo "OBJ+= $folder/$name.o" >> $makefile
done

for file in ${example[@]}; do
	echo "EXAMPLE+= $file" >> $makefile
done

for file in ${example_src[@]}; do
	folder=$(dirname "$file")
	name=$(basename "$file" .c)
	echo "EXAMPLE+= $folder/$name.o" >> $makefile
done

# generate binary targets
case $context in
	[1]* )
echo "" >> $makefile
cat make/templates/targets_macos_software.make >> $makefile
	;;

	[2]* )
echo "" >> $makefile
cat make/templates/targets_macos_egl.make >> $makefile
	;;
esac

echo "" >> $makefile
cat make/templates/targets_macos.make >> $makefile

echo "" >> $makefile
cat make/templates/targets_nix_libs.make >> $makefile

# generate object targets
echo "" >> $makefile
for file in ${src[@]}; do
	$cc $defines -MM -MG $file >> $makefile
done

for file in ${example_src[@]}; do
	$cc $defines -MM -MG $file >> $makefile
done

# generate utilitary targets
echo "" >> $makefile
cat make/templates/targets_extra.make >> $makefile
