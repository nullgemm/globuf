#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ..

# versions
ver_windows=10
ver_windows_sdk=10.0.19041.0
ver_msvc=14.28.29333
ver_visual_studio=2019

# generate makefile
cc="\"/c/Program Files (x86)/Microsoft Visual Studio/\
$ver_visual_studio/BuildTools/VC/Tools/MSVC/\
$ver_msvc/bin/Hostx64/x64/cl.exe\""

lib="\"/c/Program Files (x86)/Microsoft Visual Studio/\
$ver_visual_studio/BuildTools/VC/Tools/MSVC/\
$ver_msvc/bin/Hostx64/x64/lib.exe\""

src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/windows/globox_windows.c")

example+=("res/icon/iconpix_pe.obj")

flags+=("-Z7 -Zc:inline")
flags+=("-Isrc")
flags+=("-I\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Include/$ver_windows_sdk/ucrt\"")
flags+=("-I\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Include/$ver_windows_sdk/um\"")
flags+=("-I\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Include/$ver_windows_sdk/shared\"")
flags+=("-I\"/c/Program Files (x86)/Microsoft Visual Studio/\
$ver_visual_studio/BuildTools/VC/Tools/MSVC/$ver_msvc/include\"")

defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
#defines+=("-DGLOBOX_ERROR_SKIP")
#defines+=("-DGLOBOX_ERROR_ABORT")

defines+=("-DGLOBOX_PLATFORM_WINDOWS")
defines+=("-DGLOBOX_COMPATIBILITY_WINE")
defines+=("-DGLOBOX_COMPILER_MSVC")
defines+=("-DUNICODE")
defines+=("-D_UNICODE")
defines+=("-DWINVER=0x0A00")
defines+=("-D_WIN32_WINNT=0x0A00")
defines+=("-DCINTERFACE")
defines+=("-DCOBJMACROS")

ldflags+=("-SUBSYSTEM:windows")
ldflags+=("-DEBUG:FULL")
ldflags+=("-LIBPATH:\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Lib/$ver_windows_sdk/um/x64\"")
ldflags+=("-LIBPATH:\"/c/Program Files (x86)/Microsoft Visual Studio/\
$ver_visual_studio/BuildTools/VC/Tools/MSVC/$ver_msvc/lib/spectre/x64\"")
ldflags+=("-LIBPATH:\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Lib/$ver_windows_sdk/ucrt/x64\"")

ldlibs+=("Gdi32.lib")
ldlibs+=("User32.lib")
ldlibs+=("shcore.lib")
ldlibs+=("dwmapi.lib")

drmemory+=("-report_max -1")
drmemory+=("-report_leak_max -1")
drmemory+=("-batch")

read -p "select context ([1] software | [2] egl | [3] wgl): " context

case $context in
	[1]* )
# software context
makefile=makefile_windows_software_native
example_src+=("example/software.c")
src+=("src/windows/software/globox_windows_software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	[2]* )
# egl context
makefile=makefile_windows_egl_native
example_src+=("example/egl.c")
src+=("src/windows/egl/globox_windows_egl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldflags+=("-LIBPATH:res/eglproxy/bin")
ldlibs+=("eglproxy.lib")
ldlibs+=("opengl32.lib")

make/scripts/egl_get.sh
	;;

	[3]* )
# wgl context
makefile=makefile_windows_wgl_native
example_src+=("example/wgl.c")
src+=("src/windows/wgl/globox_windows_wgl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_WGL")
ldlibs+=("Gdi32.lib")
ldlibs+=("User32.lib")
ldlibs+=("shcore.lib")
ldlibs+=("dwmapi.lib")
ldlibs+=("opengl32.lib")

make/scripts/egl_get.sh
	;;
esac

# create empty makefile
echo ".POSIX:" > $makefile
echo "NAME = globox" >> $makefile
echo "CMD = ./globox.exe" >> $makefile

# generate linking info
echo "" >> $makefile
echo "CC = $cc" >> $makefile
echo "LIB = $lib" >> $makefile
echo "LDFLAGS+= ${ldflags[@]}" >> $makefile
echo "LDLIBS+= ${ldlibs[@]}" >> $makefile

# generate Dr.Memory flags
echo "" >> $makefile
for file in ${drmemory[@]}; do
	echo "DRMEMORY+= $file" >> $makefile
done

# generate compiler flags
echo "" >> $makefile
for file in "${flags[@]}"; do
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
	echo "OBJ+= $folder/$name.obj" >> $makefile
done

for file in ${example[@]}; do
	echo "EXAMPLE+= $file" >> $makefile
done

for file in ${example_src[@]}; do
	folder=$(dirname "$file")
	name=$(basename "$file" .c)
	echo "EXAMPLE+= $folder/$name.obj" >> $makefile
done

# build eglproxy and get OpenGL header when needed
if [ $context -gt 1 ]; then
echo "" >> $makefile
cat make/templates/targets_windows_msvc_egl.make >> $makefile
fi

# generate binary targets
echo "" >> $makefile
cat make/templates/targets_windows_msvc.make >> $makefile

echo "" >> $makefile
cat make/templates/targets_windows_msvc_libs.make >> $makefile

# generate object targets
echo "" >> $makefile
for file in ${src[@]}; do
	x86_64-w64-mingw32-gcc $defines -MM -MG $file >> $makefile
done

for file in ${example_src[@]}; do
	x86_64-w64-mingw32-gcc $defines -MM -MG $file >> $makefile
done

# .obj auto-target replacement
echo "" >> $makefile
cat make/templates/targets_windows_msvc_obj.make >> $makefile

# generate utilitary targets
echo "" >> $makefile
cat make/templates/targets_extra.make >> $makefile
