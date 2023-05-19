#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

# params
build=$1
backend=$2
toolchain=$3

echo "syntax reminder: $0 <build type> <backend type> <target toolchain type>"
echo "build types: development, release, sanitized"
echo "backend types: software, mgl, vulkan"
echo "target toolchain types: osxcross, native"

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
output="make/output"
name_lib="globox_appkit"

# ninja file variables
folder_ninja="build"
folder_objects="\$builddir/obj"
folder_globox="globox_bin_$tag"
folder_library="\$folder_globox/lib/globox"
folder_include="\$folder_globox/include"
name="globox_example_simple_appkit"
src+=("example/helpers/appkit.m")

# compiler flags
flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-I\$folder_include")
flags+=("-Iexample/helpers")
ldflags+=("-framework AppKit")
ldflags+=("-framework QuartzCore")
defines+=("-DGLOBOX_EXAMPLE_APPKIT")

# customize depending on the chosen build type
if [ -z "$build" ]; then
	build=development
fi

case $build in
	development)
flags+=("-g")
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

# customize depending on the chosen backend type
if [ -z "$backend" ]; then
	backend=software
fi

case $backend in
	software)
ninja_file=example_simple_appkit_software.ninja
src+=("example/simple/software.c")
	;;

	mgl)
ninja_file=example_simple_appkit_mgl.ninja
src+=("example/simple/opengl.c")
obj+=("\$folder_objects/res/shaders/gl1/shaders.o")
defines+=("-DGLOBOX_EXAMPLE_MGL")
	;;

	vulkan)
ninja_file=example_simple_appkit_vulkan.ninja
src+=("example/simple/vulkan.c")
src+=("example/helpers/vulkan_helpers.c")
obj+=("\$folder_objects/res/shaders/vk1/shaders.o")
libs+=("\$folder_library/globox_macho_vulkan_$toolchain.a")
ldflags+=("-lc++")
ldflags+=("-framework Metal")
ldflags+=("-framework MetalKit")
ldflags+=("-framework Foundation")
ldflags+=("-framework QuartzCore")
ldflags+=("-framework IOKit")
ldflags+=("-framework IOSurface")
ldflags+=("-lMoltenVK")
	;;

	*)
echo "invalid backend"
exit 1
	;;
esac

# target toolchain type
if [ -z "$toolchain" ]; then
	toolchain=osxcross
fi

case $toolchain in
	osxcross)
lib_suffix="osxcross"
cc="o64-clang"
ld="o64-clang"
	;;

	native)
lib_suffix="native"
cc="clang"
ld="clang"
	;;

	*)
echo "invalid target toolchain type"
exit 1
	;;
esac

name+="_$lib_suffix"
ldlibs+=("-lpthread")
cmd="open -n \$name.app"

# additional object files
obj+=("\$folder_objects/res/icon/iconpix.o")
libs+=("\$folder_library/appkit/$name_lib""_$backend""_$lib_suffix.a")
libs+=("\$folder_library/appkit/$name_lib""_common_$lib_suffix.a")
libs+=("\$folder_library/globox_macho_$lib_suffix.a")

# default target
default+=("\$builddir/\$name.app")

# valgrind flags
valgrind+=("--show-error-list=yes")
valgrind+=("--show-leak-kinds=all")
valgrind+=("--track-origins=yes")
valgrind+=("--leak-check=full")
valgrind+=("--suppressions=../res/valgrind.supp")

# objcopy flags
objcopy+=("-I binary")
objcopy+=("-O elf64-x86-64")
objcopy+=("-B i386:x86-64")

# ninja start
mkdir -p "$output"

{ \
echo "# vars"; \
echo "builddir = $folder_ninja"; \
echo "folder_objects = $folder_objects"; \
echo "folder_globox = $folder_globox"; \
echo "folder_library = $folder_library"; \
echo "folder_include = $folder_include"; \
echo "name = $name""_$backend"; \
echo "cmd = $cmd"; \
echo "cc = $cc"; \
echo "ld = $ld"; \
echo ""; \
} > "$output/$ninja_file"

# ninja flags
echo "# flags" >> "$output/$ninja_file"

echo -n "flags =" >> "$output/$ninja_file"
for flag in "${flags[@]}"; do
	echo -ne " \$\n$flag" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

echo -n "defines =" >> "$output/$ninja_file"
for define in "${defines[@]}"; do
	echo -ne " \$\n$define" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

echo -n "ldflags =" >> "$output/$ninja_file"
for flag in "${ldflags[@]}"; do
	echo -ne " \$\n$flag" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

echo -n "ldlibs =" >> "$output/$ninja_file"
for flag in "${ldlibs[@]}"; do
	echo -ne " \$\n$flag" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

echo -n "valgrind =" >> "$output/$ninja_file"
for flag in "${valgrind[@]}"; do
	echo -ne " \$\n$flag" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

echo -n "objcopy =" >> "$output/$ninja_file"
for flag in "${objcopy[@]}"; do
	echo -ne " \$\n$flag" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

# ninja rules
{ \
echo "# rules"; \
echo "rule cc"; \
echo "    deps = gcc"; \
echo "    depfile = \$out.d"; \
echo "    command = \$cc \$flags \$defines -MMD -MF \$out.d -c \$in -o \$out"; \
echo "    description = cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule app"; \
echo "    command = make/scripts/package_app.sh \$in"; \
echo "    description = packaging the executable as an app"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule ld"; \
echo "    command = \$ld \$ldflags -o \$out \$in \$ldlibs"; \
echo "    description = ld \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule icon_pixmap"; \
echo "    command = make/scripts/pixmap.sh"; \
echo "    description = pixmap \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule icon_object"; \
echo "    command = \$cc -Ires/icon -c res/icon/iconpix_macho.S -o \$out"; \
echo "    description = \$cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule shaders_object_gl1"; \
echo "    command = \$cc -Ires/shaders/gl1 -c res/shaders/gl1/shaders_macho.S -o \$out"; \
echo "    description = \$cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule shaders_object_vk1"; \
echo "    command = \$cc -Ires/shaders/vk1 -c res/shaders/vk1/shaders_macho.S -o \$out"; \
echo "    description = \$cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule run"; \
echo "    command = cd \$builddir && \$cmd"; \
echo "    description = running \$in"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule clean"; \
echo "    command = make/scripts/clean.sh"; \
echo "    description = cleaning repo"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule generator"; \
echo "    command = make/lib/appkit_$backend.sh $build $backend"; \
echo "    description = re-generating the ninja build file"; \
echo ""; \
} >> "$output/$ninja_file"

# ninja targets
## compile sources
echo "# compile sources" >> "$output/$ninja_file"
for file in "${src[@]}"; do
	folder=$(dirname "$file")
	filename=$(basename "$file" .c)
	obj+=("\$folder_objects/$folder/$filename.o")
	{ \
	echo "build \$folder_objects/$folder/$filename.o: \$"; \
	echo "cc $file"; \
	echo ""; \
	} >> "$output/$ninja_file"
done

## main targets
{ \
echo "# main targets"; \
echo "build res/icon/iconpix.bin: icon_pixmap"; \
echo "build \$folder_objects/res/icon/iconpix.o: \$"; \
echo "icon_object res/icon/iconpix.bin"; \
echo ""; \
echo "build \$folder_objects/res/shaders/gl1/shaders.o: \$"; \
echo "shaders_object_gl1 res/shaders/gl1/square_vert_gl1.glsl res/shaders/gl1/square_frag_gl1.glsl"; \
echo ""; \
echo "build \$folder_objects/res/shaders/vk1/shaders.o: \$"; \
echo "shaders_object_vk1 res/shaders/vk1/square_vert_vk1.spv res/shaders/vk1/square_frag_vk1.spv"; \
echo ""; \
} >> "$output/$ninja_file"

echo "# archive objects" >> "$output/$ninja_file"
echo -n "build \$builddir/\$name: ld" >> "$output/$ninja_file"
for file in "${obj[@]}" "${libs[@]}"; do
	echo -ne " \$\n$file" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"
echo -n "build \$builddir/\$name.app: app \$builddir/\$name" >> "$output/$ninja_file"
echo -e "\n" >> "$output/$ninja_file"

## special targets
{ \
echo "# run special targets"; \
echo "build run: run \$builddir/\$name"; \
echo "build regen: generator"; \
echo "build clean: clean"; \
echo "default" "${default[@]}"; \
} >> "$output/$ninja_file"
