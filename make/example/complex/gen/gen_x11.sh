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
cc="gcc"
obj+=("res/icon/iconpix.o")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Iglobox_bin_$tag/include")
flags+=("-Ires/cursoryx/src")
flags+=("-Ires/dpishit/src")
flags+=("-Ires/willis/src")
src+=("example/complex/desktop.c")
src+=("res/cursoryx/src/x11.c")
src+=("res/dpishit/src/x11.c")
src+=("res/dpishit/src/nix.c")
src+=("res/willis/src/willis.c")
src+=("res/willis/src/debug.c")
src+=("res/willis/src/x11.c")
src+=("res/willis/src/xkb.c")

defines+=("-DGLOBOX_PLATFORM_X11")
defines+=("-DCURSORYX_X11")
defines+=("-DDPISHIT_X11")
defines+=("-DWILLIS_X11")
defines+=("-DWILLIS_DEBUG")

# generated linker arguments
link+=("xcb")
link+=("xcb-cursor")
link+=("xcb-randr")
link+=("xcb-xfixes")
link+=("xcb-xinput")
link+=("xcb-xkb")
link+=("xcb-xrm")
link+=("xkbcommon")
link+=("xkbcommon-x11")

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

	sanitized_memory)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=leak")
flags+=("-fsanitize-recover=all")

ldflags+=("-fsanitize=leak")
ldflags+=("-fsanitize-recover=all")
	;;

	sanitized_undefined)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=undefined")
flags+=("-fsanitize-recover=all")

ldflags+=("-fsanitize=undefined")
ldflags+=("-fsanitize-recover=all")
	;;

	sanitized_address)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=address")
flags+=("-fsanitize-address-use-after-scope")
flags+=("-fsanitize-recover=all")

ldflags+=("-fsanitize=address")
ldflags+=("-fsanitize-address-use-after-scope")
ldflags+=("-fsanitize-recover=all")
	;;

	sanitized_thread)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=thread")
flags+=("-fsanitize-recover=all")

ldflags+=("-fsanitize=thread")
ldflags+=("-fsanitize-recover=all")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# context type
if [ -z "$context" ]; then
	read -rp "select context type (software | egl | glx): " context
fi

case $context in
	software)
makefile="makefile_example_complex_x11_software"
name="globox_example_complex_x11_software"
globox="globox_x11_software"
src+=("example/complex/software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
link+=("xcb-shm")
link+=("xcb-randr")
link+=("xcb-render")
	;;

	egl)
makefile="makefile_example_complex_x11_egl"
name="globox_example_complex_x11_egl"
globox="globox_x11_egl"
src+=("example/complex/egl.c")
defines+=("-DGLOBOX_CONTEXT_EGL")
link+=("egl")
link+=("glesv2")
obj+=("res/shaders/gl1/square_vert_gl1.o")
obj+=("res/shaders/gl1/square_frag_gl1.o")
obj+=("res/shaders/gl1/decorations_vert_gl1.o")
obj+=("res/shaders/gl1/decorations_frag_gl1.o")
	;;

	glx)
makefile="makefile_example_complex_x11_glx"
name="globox_example_complex_x11_glx"
globox="globox_x11_glx"
src+=("example/complex/glx.c")
defines+=("-DGLOBOX_CONTEXT_GLX")
link+=("gl")
link+=("glesv2")
link+=("x11 x11-xcb xrender")
obj+=("res/shaders/gl1/square_vert_gl1.o")
obj+=("res/shaders/gl1/square_frag_gl1.o")
obj+=("res/shaders/gl1/decorations_vert_gl1.o")
obj+=("res/shaders/gl1/decorations_frag_gl1.o")
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
obj+=("globox_bin_$tag/lib/globox/x11/$globox.a")
cmd="./$name"
	;;

	shared)
ldflags+=("-Lglobox_bin_$tag/lib/globox/x11")
ldlibs+=("-l:$globox.so")
cmd="LD_LIBRARY_PATH=../globox_bin_$tag/lib/globox/x11 ./$name"
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
mkdir -p "$output"

{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CMD = $cmd"; \
echo "CC = $cc"; \
} > "$output/$makefile"

# makefile linking info
echo "" >> "$output/$makefile"
for flag in $(pkg-config "${link[@]}" --cflags) "${ldflags[@]}"; do
	echo "LDFLAGS+= $flag" >> "$output/$makefile"
done

echo "" >> "$output/$makefile"
for flag in $(pkg-config "${link[@]}" --libs) "${ldlibs[@]}"; do
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

# generate valgrind flags
echo "" >> "$output/$makefile"
for flag in "${valgrind[@]}"; do
	echo "VALGRIND+= $flag" >> "$output/$makefile"
done

# makefile default target
echo "" >> "$output/$makefile"
echo "default:" "${default[@]}" >> "$output/$makefile"

# makefile linux targets
echo "" >> "$output/$makefile"
cat make/example/complex/templates/targets_linux.make >> "$output/$makefile"

# makefile object targets
echo "" >> "$output/$makefile"
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> "$output/$makefile"
done

# makefile extra targets
echo "" >> "$output/$makefile"
cat make/example/complex/templates/targets_extra.make >> "$output/$makefile"
