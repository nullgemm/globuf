#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../../..

output="make/output"

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
flags+=("-Ires/cursoryx/src")
flags+=("-Ires/dpishit/src")
flags+=("-Ires/willis/src")
src+=("example/complex/desktop.c")
src+=("res/cursoryx/src/macos.c")
src+=("res/dpishit/src/macos.c")
src+=("res/willis/src/willis.c")
src+=("res/willis/src/debug.c")
src+=("res/willis/src/macos.c")

defines+=("-DGLOBOX_PLATFORM_MACOS")
defines+=("-DCURSORYX_MACOS")
defines+=("-DDPISHIT_MACOS")
defines+=("-DWILLIS_MACOS")
defines+=("-DWILLIS_DEBUG")

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

	sanitized_memory_undefined)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=memory")
flags+=("-fsanitize-memory-track-origins=2")
flags+=("-fsanitize=undefined")
flags+=("-fsanitize-recover=all")

ldflags+=("-fsanitize=memory")
ldflags+=("-fsanitize-memory-track-origins=2")
ldflags+=("-fsanitize=undefined")
ldflags+=("-fsanitize-recover=all")
	;;

	sanitized_address)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=address")
flags+=("-fsanitize-address-use-after-return=always")
flags+=("-fsanitize-address-use-after-scope")
flags+=("-fsanitize-recover=all")

ldflags+=("-fsanitize=address")
ldflags+=("-fsanitize-address-use-after-return=always")
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
	read -rp "select context type (software | egl): " context
fi

case $context in
	software)
makefile="makefile_example_complex_macos_software"
name="globox_example_complex_macos_software"
globox="globox_macos_software"
src+=("example/complex/software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	egl)
makefile="makefile_example_complex_macos_egl"
name="globox_example_complex_macos_egl"
globox="globox_macos_egl"
src+=("example/complex/egl.c")
flags+=("-Ires/angle/include")
defines+=("-DGLOBOX_CONTEXT_EGL")
ldflags+=("-Lres/angle/libs")
ldlibs+=("-lEGL")
ldlibs+=("-lGLESv2")
default+=("res/angle/libs")
default+=("bin/libEGL.dylib")
obj+=("res/shaders/gles2/square_vert_gles2_mach.o")
obj+=("res/shaders/gles2/square_frag_gles2_mach.o")
obj+=("res/shaders/gles2/decorations_vert_gles2_mach.o")
obj+=("res/shaders/gles2/decorations_frag_gles2_mach.o")
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
mkdir -p "$output"

{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CMD = $cmd"; \
echo "OBJCOPY = $objcopy"; \
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
cat make/example/complex/templates/targets_macos.make >> "$output/$makefile"

# makefile object targets
echo "" >> "$output/$makefile"
for file in "${src[@]}"; do
	$cch "${defines[@]}" -MM -MG "$file" >> "$output/$makefile"
done

# makefile extra targets
echo "" >> "$output/$makefile"
cat make/example/complex/templates/targets_extra.make >> "$output/$makefile"
