#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../../..

output="make/output"

build=$1
context=$2
library=$3

tag=$(git tag --sort v:refname | tail -n 1)

# library makefile data
cc="x86_64-w64-mingw32-gcc"
ar="x86_64-w64-mingw32-ar"
obj+=("res/icon/iconpix_pe.o")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Wno-implicit-fallthrough")
flags+=("-Wno-cast-function-type")
flags+=("-Wno-incompatible-pointer-types")
flags+=("-Iglobox_bin_$tag/include")
flags+=("-Ires/cursoryx/src")
flags+=("-Ires/dpishit/src")
flags+=("-Ires/willis/src")
src+=("example/complex/desktop.c")
src+=("res/cursoryx/src/windows.c")
src+=("res/dpishit/src/windows.c")
src+=("res/willis/src/willis.c")
src+=("res/willis/src/debug.c")
src+=("res/willis/src/windows.c")

defines+=("-DGLOBOX_PLATFORM_WINDOWS")
defines+=("-DUNICODE")
defines+=("-D_UNICODE")
defines+=("-DWINVER=0x0A00")
defines+=("-D_WIN32_WINNT=0x0A00")
defines+=("-DCINTERFACE")
defines+=("-DCOBJMACROS")
defines+=("-DCURSORYX_WINDOWS")
defines+=("-DDPISHIT_WINDOWS")
defines+=("-DWILLIS_WINDOWS")
defines+=("-DWILLIS_DEBUG")

# generated linker arguments
ldflags+=("-fno-stack-protector")
ldlibs+=("-lgdi32")
ldlibs+=("-ldwmapi")
ldlibs+=("-mwindows")
ldlibs+=("-lshcore")

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
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fPIE")
flags+=("-fPIC")
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
makefile=makefile_example_complex_windows_software
name="globox_example_complex_windows_software"
globox="globox_windows_software"
src+=("example/complex/software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	egl)
makefile=makefile_example_complex_windows_egl
name="globox_example_complex_windows_egl"
globox="globox_windows_egl"
src+=("example/complex/egl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldflags+=("-Lres/eglproxy/lib/mingw")
ldlibs+=("-leglproxy")
ldlibs+=("-lopengl32")
default+=("res/egl_headers")
default+=("bin/eglproxy.dll")
obj+=("res/shaders/gl1/square_vert_gl1_pe.o")
obj+=("res/shaders/gl1/square_frag_gl1_pe.o")
obj+=("res/shaders/gl1/decorations_vert_gl1_pe.o")
obj+=("res/shaders/gl1/decorations_frag_gl1_pe.o")
	;;

	wgl)
makefile=makefile_example_complex_windows_wgl
name="globox_example_complex_windows_wgl"
globox="globox_windows_wgl"
src+=("example/complex/wgl.c")
flags+=("-Ires/egl_headers")
defines+=("-DGLOBOX_CONTEXT_WGL")
ldlibs+=("-lopengl32")
default+=("res/egl_headers")
obj+=("res/shaders/gl1/square_vert_gl1_pe.o")
obj+=("res/shaders/gl1/square_frag_gl1_pe.o")
obj+=("res/shaders/gl1/decorations_vert_gl1_pe.o")
obj+=("res/shaders/gl1/decorations_frag_gl1_pe.o")
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
obj+=("globox_bin_$tag/lib/globox/windows/""$globox""_mingw.a")
cmd="wine ./$name.exe"
	;;

	shared)
obj+=("globox_bin_$tag/lib/globox/windows/""$globox""_mingw.dll.a")
cmd="../make/scripts/dll_copy.sh ""$globox""_mingw.dll && wine ./$name.exe"
	;;

	*)
echo "invalid library type"
exit 1
	;;
esac

# default target
default+=("bin/\$(NAME)")

# makefile start
mkdir -p "$output"

{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CMD = $cmd"; \
echo "CC = $cc"; \
echo "AR = $ar"; \
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

echo "" >> "$output/$makefile"
for prebuilt in "${obj[@]}"; do
	echo "OBJ_EXTRA+= $prebuilt" >> "$output/$makefile"
done

# makefile default target
echo "" >> "$output/$makefile"
echo "default:" "${default[@]}" >> "$output/$makefile"

# makefile linux targets
echo "" >> "$output/$makefile"
cat make/example/complex/templates/targets_windows_mingw.make >> "$output/$makefile"

# makefile object targets
echo "" >> "$output/$makefile"
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> "$output/$makefile"
done

# makefile extra targets
echo "" >> "$output/$makefile"
cat make/example/complex/templates/targets_extra.make >> "$output/$makefile"
