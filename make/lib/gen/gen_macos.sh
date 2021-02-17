#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")"
cd ../../..

# library makefile data
name="globox"

src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/macos/globox_macos.c")
src+=("src/macos/globox_macos_symbols.c")
src+=("src/macos/globox_macos_callbacks.c")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")
flags+=("-fPIC")

#defines+=("-DGLOBOX_ERROR_ABORT")
#defines+=("-DGLOBOX_ERROR_SKIP")
defines+=("-DGLOBOX_ERROR_LOG_THROW")

# library platform
defines+=("-DGLOBOX_INTERNAL")
defines+=("-DGLOBOX_GETTERS_PLATFORM")
defines+=("-DGLOBOX_PLATFORM_MACOS")

# raw linker arguments
ldlibs+=("-framework AppKit")

# build type
read -p "select build type ([1] debug | [2] release): " build

case $build in
	[1]* ) # debug build
flags+=("-g")
defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
	;;

	[2]* ) # release build
flags+=("-O2")
	;;
esac

# context type
read -p "select context type ([1] software | [2] egl): " context

case $context in
	[1]* ) # software context
makefile=makefile_lib_macos_software
src+=("src/macos/software/globox_macos_software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	[2]* ) # egl context
makefile=makefile_lib_macos_egl
src+=("src/macos/egl/globox_macos_egl.c")
flags+=("-Ires/angle/include")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldflags+=("-Lres/angle/libs")
ldlibs+=("-lEGL")
ldlibs+=("-lGLESv2")
default+=("res/angle/libs")
	;;
esac

# add the libraries as default targets
default+=("bin/$name.a")
default+=("bin/lib$name.dylib")

# toolchain type
read -p "select toolchain type ([1] osxcross | [2] native): " toolchain

case $toolchain in
	[1]* ) # cross-compiling
cc=o64-clang
ar=x86_64-apple-darwin19-ar
	;;

	[2]* ) # compiling from mac
makefile+="_native"
cc=clang
ar=ar
	;;
esac

read -p "select current toolchain ([1] osxcross | [2] native)" current_toolchain

case $current_toolchain in
	[1]* ) # generating from linux
cch=o64-clang
	;;

	[2]* ) # generating from mac
cch=clang
	;;
esac

# makefile start
echo ".POSIX:" > $makefile
echo "NAME = $name" >> $makefile
echo "CC = $cc" >> $makefile
echo "AR = $ar" >> $makefile

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

# makefile default target
echo "" >> $makefile
echo "default: ${default[@]}" >> $makefile

# makefile library targets
echo "" >> $makefile
cat make/lib/templates/targets_macos.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cch $defines -MM -MG $file >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/lib/templates/targets_extra.make >> $makefile
