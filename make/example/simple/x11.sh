#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

# params
build=$1
backend=$2

echo "syntax reminder: $0 <build type> <backend type>"
echo "build types: development, release, sanitized"
echo "backend types: software, egl, vulkan"

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
output="make/output"
name_lib="globox_x11"

# ninja file variables
folder_ninja="build"
folder_objects="\$builddir/obj"
folder_globox="globox_bin_$tag"
folder_library="\$folder_globox/lib/globox"
folder_include="\$folder_globox/include"
name="globox_example_simple_x11"
cmd="./\$name"
cc="gcc"
ld="gcc"

# compiler flags
flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-I\$folder_include")
ldflags+=("-z noexecstack")
defines+=("-DGLOBOX_EXAMPLE_X11")

# customize depending on the chosen build type
if [ -z "$build" ]; then
	build=development
fi

case $build in
	development)
flags+=("-g")
link+=("xcb-errors")
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
ninja_file=example_simple_x11_software.ninja
src+=("example/simple/software.c")
link+=("xcb-shm")
link+=("xcb-sync")
link+=("xcb-randr")
link+=("xcb-render")
	;;

	egl)
ninja_file=example_simple_x11_egl.ninja
src+=("example/simple/egl.c")
link+=("egl")
link+=("glesv2")
obj+=("\$folder_objects/res/shaders/gl1/square_vert_gl1.o")
obj+=("\$folder_objects/res/shaders/gl1/square_frag_gl1.o")
	;;

	vulkan)
ninja_file=example_simple_x11_vulkan.ninja
src+=("example/simple/vulkan.c")
# TODO vulkan
	;;

	*)
echo "invalid backend"
exit 1
	;;
esac

link+=("xcb-present")
link+=("xcb")
ldlibs+=("-lpthread")

# additional object files
obj+=("\$folder_objects/res/icon/iconpix.o")
libs+=("\$folder_library/x11/$name_lib""_$backend.a")
libs+=("\$folder_library/x11/$name_lib""_common.a")
libs+=("\$folder_library/globox.a")

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
for flag in $(pkg-config "${link[@]}" --cflags) "${ldflags[@]}"; do
	echo -ne " \$\n$flag" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

echo -n "ldlibs =" >> "$output/$ninja_file"
for flag in $(pkg-config "${link[@]}" --libs) "${ldlibs[@]}"; do
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
echo "    deps = $cc"; \
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
echo "    command = objcopy \$objcopy \$"; \
echo "    --redefine-syms=res/icon/syms.map \$"; \
echo "    --rename-section .data=.iconpix \$"; \
echo "    \$in \$out"; \
echo "    description = objcopy \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule shader_vert_object"; \
echo "    command = objcopy \$objcopy \$"; \
echo "    --redefine-syms=res/shaders/gl1/syms.map \$"; \
echo "    --rename-section .data=.square_vert \$"; \
echo "    \$in \$out"; \
echo "    description = objcopy \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule shader_frag_object"; \
echo "    command = objcopy \$objcopy \$"; \
echo "    --redefine-syms=res/shaders/gl1/syms.map \$"; \
echo "    --rename-section .data=.square_frag \$"; \
echo "    \$in \$out"; \
echo "    description = objcopy \$out"; \
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
echo "    command = make/lib/x11_$backend.sh $build $backend"; \
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
echo "build \$folder_objects/res/shaders/gl1/square_vert_gl1.o: \$"; \
echo "shader_vert_object res/shaders/gl1/square_vert_gl1.glsl"; \
echo ""; \
echo "build \$folder_objects/res/shaders/gl1/square_frag_gl1.o: \$"; \
echo "shader_frag_object res/shaders/gl1/square_frag_gl1.glsl"; \
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
