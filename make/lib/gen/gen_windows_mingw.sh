#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1
context=$2

tag=$(git tag --sort v:refname | tail -n 1)
folder="globox_bin_$tag/lib/globox/windows"
folder_include="globox_bin_$tag/include"

# library makefile data
output="make/output"
name="globox_windows"
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
if [ -z "$build" ]; then
	read -rp "select build type (development | release): " build
fi

case $build in
	development)
flags+=("-g")
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
makefile=makefile_lib_windows_software
name+="_software"
src+=("src/windows/software/globox_windows_software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	egl)
makefile=makefile_lib_windows_egl
name+="_egl"
src+=("src/windows/egl/globox_windows_egl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldflags+=("-Lres/eglproxy/lib/mingw")
ldlibs+=("-leglproxy")
ldlibs+=("-lopengl32")
default+=("res/egl_headers")
default+=("res/eglproxy")
	;;

	wgl)
makefile=makefile_lib_windows_wgl
name+="_wgl"
src+=("src/windows/wgl/globox_windows_wgl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_WGL")
ldlibs+=("-lgdi32")
ldlibs+=("-ldwmapi")
ldlibs+=("-mwindows")
ldlibs+=("-lopengl32")
default+=("res/egl_headers")
	;;

	*)
echo "invalid context type"
exit 1
	;;
esac

# add the libraries as default targets
default+=("$folder/$name.dll")
default+=("$folder/$name.a")

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
for flag in "${ldflags[@]}"; do
	echo "LDFLAGS+= $flag" >> "$output/$makefile"
done

echo "" >> "$output/$makefile"
for flag in "${ldlibs[@]}"; do
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
cat make/lib/templates/targets_windows_mingw.make >> "$output/$makefile"

# makefile object targets
echo "" >> "$output/$makefile"
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> "$output/$makefile"
done

# makefile extra targets
echo "" >> "$output/$makefile"
cat make/lib/templates/targets_extra.make >> "$output/$makefile"
