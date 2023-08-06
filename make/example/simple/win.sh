#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

# params
build=$1
backend=$2

function syntax {
echo "syntax reminder: $0 <build type> <backend type>"
echo "build types: development, release, sanitized"
echo "backend types: software, wgl, vulkan"
}

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
output="make/output"
name_lib="globox_win"

# ninja file variables
folder_ninja="build"
folder_objects="\$builddir/obj"
folder_globox="globox_bin_$tag"
folder_library="\$folder_globox/lib/globox"
folder_include="\$folder_globox/include"
name="globox_example_simple_win"
cmd="./\$name"
cc="x86_64-w64-mingw32-gcc"
ld="x86_64-w64-mingw32-gcc"

# compiler flags
flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Wno-cast-function-type")
flags+=("-I\$folder_include")
flags+=("-Ires/egl")
flags+=("-Iexample/helpers")
defines+=("-DGLOBOX_EXAMPLE_WIN")
#defines+=("-DGLOBOX_EXAMPLE_LOG_ALL")

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
flags+=("-fno-stack-protector")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
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
syntax
exit 1
	;;
esac

# customize depending on the chosen backend type
if [ -z "$backend" ]; then
	backend=software
fi

case $backend in
	software)
ninja_file=example_simple_win_software.ninja
src+=("example/simple/software.c")
	;;

	wgl)
ninja_file=example_simple_win_wgl.ninja
src+=("example/simple/opengl.c")
obj+=("\$folder_objects/res/shaders/gl1/shaders.o")
defines+=("-DGLOBOX_EXAMPLE_WGL")
ldlibs+=("-lopengl32")
	;;

	vulkan)
ninja_file=example_simple_win_vulkan.ninja
src+=("example/simple/vulkan.c")
src+=("example/helpers/vulkan_helpers.c")
obj+=("\$folder_objects/res/shaders/vk1/shaders.o")
libs+=("\$folder_library/globox_pe_vulkan.a")
ldlibs+=("-lvulkan-1")
	;;

	*)
echo "invalid backend"
syntax
exit 1
	;;
esac

# additional object files
obj+=("\$folder_objects/res/icon/iconpix.o")
libs+=("\$folder_library/win/$name_lib""_$backend.a")
libs+=("\$folder_library/win/$name_lib""_common.a")
libs+=("\$folder_library/globox_pe.a")
ldlibs+=("-lshcore")
ldlibs+=("-lgdi32")
ldlibs+=("-ldwmapi")

# default target
default+=("\$builddir/\$name")

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
echo "    command = \$cc -Ires/icon -c res/icon/iconpix_pe.S -o \$out"; \
echo "    description = \$cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule shaders_object_gl1"; \
echo "    command = \$cc -Ires/shaders/gl1 -c res/shaders/gl1/shaders_pe.S -o \$out"; \
echo "    description = \$cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule shaders_object_vk1"; \
echo "    command = \$cc -Ires/shaders/vk1 -c res/shaders/vk1/shaders_pe.S -o \$out"; \
echo "    description = \$cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule leak"; \
echo "    command = cd \$builddir \$"; \
echo "    && valgrind \$valgrind 2> valgrind.log \$cmd \$"; \
echo "    && less valgrind.log"; \
echo "    description = running valgrind \$in"; \
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
echo "    command = make/lib/win_$backend.sh $build $backend"; \
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

## special targets
{ \
echo "# run special targets"; \
echo "build leak: leak \$builddir/\$name"; \
echo "build run: run \$builddir/\$name"; \
echo "build regen: generator"; \
echo "build clean: clean"; \
echo "default" "${default[@]}"; \
} >> "$output/$ninja_file"
