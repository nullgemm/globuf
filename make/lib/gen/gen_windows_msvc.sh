#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1
context=$2

# library makefile data
name="globox"

echo "getting latest Windows SDK version number from registry..."
ver_windows_sdk=$(powershell 'Get-ChildItem -Name "hklm:\SOFTWARE\Microsoft\Windows Kits\Installed Roots" | Select -Last 1')
ver_windows=$(echo "$ver_windows_sdk" | sed "s/\\..*//")

if [ -z "$ver_windows" ]; then
	echo "couldn't find a compatible Windows SDK installation"
	exit
else
	echo "found Windows SDK version $ver_windows"
fi

echo "searching for latest Visual Studio version number..."
ver_visual_studio=$(powershell '& "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" /all /latest /products Microsoft.VisualStudio.Product.BuildTools Microsoft.VisualStudio.Product.Community Microsoft.VisualStudio.Product.Professional Microsoft.VisualStudio.Product.Enterprise /property catalog_productLineVersion')

if [ -z "$ver_visual_studio" ]; then
	echo "couldn't find a compatible Visual Studio installation (no version number returned)"
	exit
else
	echo "found Visual Studio version $ver_visual_studio"
fi

echo "searching for latest Visual Studio installation path..."
path_visual_studio=$(powershell '& "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" /all /latest /products Microsoft.VisualStudio.Product.BuildTools Microsoft.VisualStudio.Product.Community Microsoft.VisualStudio.Product.Professional Microsoft.VisualStudio.Product.Enterprise /property installationPath')

if [ -z "$path_visual_studio" ]; then
	echo "couldn't find a compatible Visual Studio installation (no installation path returned)"
	exit
else
	echo "found Visual Studio installation at $path_visual_studio"
fi

echo "searching for latest MSVC version number..."
ver_msvc=$(powershell 'Get-ChildItem -Name "'$path_visual_studio'\VC\Tools\MSVC" | Select -Last 1')

if [ -z "$ver_msvc" ]; then
	echo "couldn't find a compatible MSVC installation"
	exit
else
	echo "found MSVC version $ver_msvc"
fi

cc="\"$path_visual_studio/\
VC/Tools/MSVC/$ver_msvc/bin/Hostx64/x64/cl.exe\""
lib="\"$path_visual_studio/\
VC/Tools/MSVC/$ver_msvc/bin/Hostx64/x64/lib.exe\""

src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/globox_private_getters.c")
src+=("src/windows/globox_windows.c")

flags+=("-Zc:inline")
flags+=("-Isrc")
flags+=("-I\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Include/$ver_windows_sdk/ucrt\"")
flags+=("-I\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Include/$ver_windows_sdk/um\"")
flags+=("-I\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Include/$ver_windows_sdk/shared\"")
flags+=("-I\"$path_visual_studio/\
VC/Tools/MSVC/$ver_msvc/include\"")

#defines+=("-DGLOBOX_ERROR_ABORT")
#defines+=("-DGLOBOX_ERROR_SKIP")
defines+=("-DGLOBOX_ERROR_LOG_THROW")

# library platform
defines+=("-DGLOBOX_PLATFORM_WINDOWS")
defines+=("-DGLOBOX_COMPATIBILITY_WINE")
defines+=("-DGLOBOX_COMPILER_MSVC")
defines+=("-DUNICODE")
defines+=("-D_UNICODE")
defines+=("-DWINVER=0x0A00")
defines+=("-D_WIN32_WINNT=0x0A00")
defines+=("-DCINTERFACE")
defines+=("-DCOBJMACROS")

# generated linker arguments
ldflags+=("-SUBSYSTEM:windows")
ldflags+=("-LIBPATH:\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Lib/$ver_windows_sdk/um/x64\"")
ldflags+=("-LIBPATH:\"$path_visual_studio/\
VC/Tools/MSVC/$ver_msvc/lib/x64\"")
ldflags+=("-LIBPATH:\"/c/Program Files (x86)/Windows Kits/\
$ver_windows/Lib/$ver_windows_sdk/ucrt/x64\"")

ldlibs+=("Gdi32.lib")
ldlibs+=("User32.lib")
ldlibs+=("shcore.lib")
ldlibs+=("dwmapi.lib")

# build type
if [ -z "$build" ]; then
	read -rp "select build type (development | release): " build
fi

case $build in
	development)
flags+=("-Z7")
ldflags+=("-DEBUG:FULL")
defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
	;;

	release)
flags+=("-O2")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# context type
if [ -z "$context" ]; then
	read -rp "select context type (software | egl | wgl): " context
fi

case $context in
	software)
makefile=makefile_lib_windows_software_native
src+=("src/windows/software/globox_windows_software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	egl)
makefile=makefile_lib_windows_egl_native
src+=("src/windows/egl/globox_windows_egl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldflags+=("-LIBPATH:res/eglproxy/lib/msvc")
ldlibs+=("eglproxy.lib")
ldlibs+=("opengl32.lib")
default+=("res/egl_headers")
default+=("res/eglproxy")
	;;

	wgl)
makefile=makefile_lib_windows_wgl_native
src+=("src/windows/wgl/globox_windows_wgl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_WGL")
ldlibs+=("Gdi32.lib")
ldlibs+=("User32.lib")
ldlibs+=("shcore.lib")
ldlibs+=("dwmapi.lib")
ldlibs+=("opengl32.lib")
default+=("res/egl_headers")
	;;

	*)
echo "invalid context type"
exit 1
	;;
esac

# add the libraries as default targets
default+=("bin/$name.lib")

# makefile start
{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CC = $cc"; \
echo "LIB = $lib"; \
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
	echo "OBJ+= $folder/$filename.obj" >> $makefile
done

# makefile default target
echo "" >> $makefile
echo "default:" "${default[@]}" >> $makefile

# makefile library targets
echo "" >> $makefile
cat make/lib/templates/targets_windows_msvc.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	x86_64-w64-mingw32-gcc "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/lib/templates/targets_extra.make >> $makefile
