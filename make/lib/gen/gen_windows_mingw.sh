#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")"
cd ../../..

# library makefile data
name="globox"
cc="x86_64-w64-mingw32-gcc"

src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/globox_private_getters.c")
src+=("src/windows/globox_windows.c")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Wno-implicit-fallthrough")
flags+=("-Wno-cast-function-type")
flags+=("-Wno-incompatible-pointer-types")
flags+=("-Isrc")
flags+=("-fPIC")

#defines+=("-DGLOBOX_ERROR_ABORT")
#defines+=("-DGLOBOX_ERROR_SKIP")
defines+=("-DGLOBOX_ERROR_LOG_THROW")

# library platform
defines+=("-DGLOBOX_INTERNAL")
defines+=("-DGLOBOX_PLATFORM_WINDOWS")
defines+=("-DGLOBOX_COMPATIBILITY_WINE")
defines+=("-DUNICODE")
defines+=("-D_UNICODE")
defines+=("-DWINVER=0x0A00")
defines+=("-D_WIN32_WINNT=0x0A00")
defines+=("-DCINTERFACE")
defines+=("-DCOBJMACROS")

# generated linker arguments
ldlibs+=("-lgdi32")
ldlibs+=("-ldwmapi")
ldlibs+=("-mwindows")

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
read -p "select context type ([1] software | [2] egl | [3] wgl): " context

case $context in
	[1]* ) # software context
makefile=makefile_lib_windows_software
src+=("src/windows/software/globox_windows_software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	[2]* ) # egl context
makefile=makefile_lib_windows_egl
src+=("src/windows/egl/globox_windows_egl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldflags+=("-Lres/eglproxy/lib/mingw")
ldlibs+=("-leglproxy")
ldlibs+=("-lopengl32")
default+=("res/egl_headers")
default+=("res/eglproxy")
	;;

	[3]* ) # wgl context
makefile=makefile_lib_windows_wgl
src+=("src/windows/wgl/globox_windows_wgl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_WGL")
ldlibs+=("-lgdi32")
ldlibs+=("-ldwmapi")
ldlibs+=("-mwindows")
ldlibs+=("-lopengl32")
default+=("res/egl_headers")
	;;
esac

# add the libraries as default targets
default+=("bin/$name.dll")

# makefile start
echo ".POSIX:" > $makefile
echo "NAME = $name" >> $makefile
echo "CC = $cc" >> $makefile

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
cat make/lib/templates/targets_windows_mingw.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cc $defines -MM -MG $file >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/lib/templates/targets_extra.make >> $makefile
