#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../..

# params
build=$1
backend=$2

function syntax {
echo "syntax reminder: $0 <build type> <backend type>"
echo "build types: development, release, sanitized"
echo "backend types: common, software, glx, egl, vulkan"
}

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
output="make/output"

# ninja file variables
folder_ninja="build"
folder_objects="\$builddir/obj"
folder_globuf="globuf_bin_$tag"
folder_library="\$folder_globuf/lib/globuf/wayland"
folder_include="\$folder_globuf/include"
name="globuf_wayland"
cc="gcc"
ld="ld"
ar="ar"
objcopy="objcopy"

# compiler flags
flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Wno-unused-variable")
flags+=("-Isrc")
flags+=("-Isrc/include")
flags+=("-Ires/wayland_headers")
flags+=("-fPIC")
flags+=("-fdiagnostics-color=always")

#defines+=("-DGLOBUF_ERROR_ABORT")
#defines+=("-DGLOBUF_ERROR_SKIP")
defines+=("-DGLOBUF_ERROR_LOG_DEBUG")

# customize depending on the chosen build type
if [ -z "$build" ]; then
	build=development
fi

case $build in
	development)
flags+=("-g")
defines+=("-DGLOBUF_ERROR_LOG_THROW")
	;;

	release)
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fstack-protector-strong")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
defines+=("-DGLOBUF_ERROR_LOG_MANUAL")
	;;

	sanitized_memory)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=leak")
flags+=("-fsanitize-recover=all")
defines+=("-DGLOBUF_ERROR_LOG_THROW")
	;;

	sanitized_undefined)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=undefined")
flags+=("-fsanitize-recover=all")
defines+=("-DGLOBUF_ERROR_LOG_THROW")
	;;

	sanitized_address)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=address")
flags+=("-fsanitize-address-use-after-scope")
flags+=("-fsanitize-recover=all")
defines+=("-DGLOBUF_ERROR_LOG_THROW")
	;;

	sanitized_thread)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=thread")
flags+=("-fsanitize-recover=all")
defines+=("-DGLOBUF_ERROR_LOG_THROW")
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
	common)
ninja_file=lib_wayland_common.ninja
name+="_common"
src+=("src/wayland/wayland_common.c")
src+=("src/wayland/wayland_common_helpers.c")
src+=("src/wayland/wayland_common_registry.c")
	;;

	software)
ninja_file=lib_wayland_software.ninja
name+="_software"
src+=("src/wayland/wayland_software.c")
src+=("src/wayland/wayland_software_helpers.c")
	;;

	egl)
ninja_file=lib_wayland_egl.ninja
name+="_egl"
src+=("src/wayland/wayland_egl.c")
src+=("src/wayland/wayland_egl_helpers.c")
	;;

	vulkan)
ninja_file=lib_wayland_vulkan.ninja
name+="_vulkan"
src+=("src/wayland/wayland_vulkan.c")
src+=("src/wayland/wayland_vulkan_helpers.c")
	;;

	*)
echo "invalid backend"
syntax
exit 1
	;;
esac

src+=("res/wayland_headers/xdg-shell-protocol.c")
src+=("res/wayland_headers/xdg-decoration-protocol.c")
src+=("res/wayland_headers/kde-blur-protocol.c")
src+=("res/wayland_headers/zwp-relative-pointer-protocol.c")
src+=("res/wayland_headers/zwp-pointer-constraints-protocol.c")

# save symbols file path
symbols_file="src/wayland/symbols_$backend.txt"

# default target
default+=("\$folder_library/\$name.a")

# ninja start
mkdir -p "$output"

{ \
echo "# vars"; \
echo "builddir = $folder_ninja"; \
echo "folder_objects = $folder_objects"; \
echo "folder_globuf = $folder_globuf"; \
echo "folder_library = $folder_library"; \
echo "folder_include = $folder_include"; \
echo "name = $name"; \
echo "cc = $cc"; \
echo "ld = $ld"; \
echo "ar = $ar"; \
echo "objcopy = $objcopy"; \
echo ""; \
} > "$output/$ninja_file"

# ninja flags
echo "# flags" >> "$output/$ninja_file"

echo -n "flags =" >> "$output/$ninja_file"
for flag in "${flags[@]}"; do
	echo -ne " \$\n$flag" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

echo -ne "defines =" >> "$output/$ninja_file"
for define in "${defines[@]}"; do
	echo -ne " \$\n$define" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

# ninja rules
{ \
echo "# rules"; \
echo "rule global"; \
echo "    command = \$objcopy -D --globalize-symbols=$symbols_file \$in \$out"; \
echo "    description = globalize \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule local"; \
echo "    command = \$objcopy -w -L \"*\" \$in \$out"; \
echo "    description = localize \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule ar"; \
echo "    command = \$ar rcs \$out \$in"; \
echo "    description = ar \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule ld"; \
echo "    command = \$ld -r \$in -o \$out"; \
echo "    description = ld \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule cc"; \
echo "    deps = gcc"; \
echo "    depfile = \$out.d"; \
echo "    command = \$cc \$flags \$defines -MMD -MF \$out.d -c \$in -o \$out"; \
echo "    description = cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule cp"; \
echo "    command = cp \$in \$out"; \
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
echo "    command = make/lib/wayland.sh $build $backend"; \
echo "    description = re-generating the ninja build file"; \
echo ""; \
} >> "$output/$ninja_file"

# ninja targets
## copy headers
if [ $backend != "common" ]; then
{ \
echo "# copy headers"; \
echo "build \$folder_include/globuf_wayland_$backend.h: \$"; \
echo "cp src/include/globuf_wayland_$backend.h"; \
echo ""; \
echo "build \$folder_include/globuf_wayland.h: \$"; \
echo "cp src/include/globuf_wayland.h"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "build headers: phony \$"; \
echo "\$folder_include/globuf_wayland_$backend.h \$"; \
echo "\$folder_include/globuf_wayland.h"; \
echo ""; \
} >> "$output/$ninja_file"
fi

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

## merge objects
echo "# merge objects" >> "$output/$ninja_file"
echo -n "build \$folder_objects/\$name.o: ld" >> "$output/$ninja_file"
for file in "${obj[@]}"; do
	echo -ne " \$\n$file" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

## archive object
{ \
echo "# archive objects"; \
echo "build \$folder_objects/\$name.a: ar \$"; \
echo "\$folder_objects/\$name.o"; \
echo ""; \
} >> "$output/$ninja_file"

## make API symbols local
{ \
echo "# make API symbols local"; \
echo "build \$folder_objects/\$name.local.a: local \$"; \
echo "\$folder_objects/\$name.a"; \
echo ""; \
} >> "$output/$ninja_file"

## make API symbols global
{ \
echo "# make API symbols global"; \
echo "build \$folder_library/\$name.a: global \$"; \
echo "\$folder_objects/\$name.local.a"; \
echo ""; \
} >> "$output/$ninja_file"

## special targets
{ \
echo "# run special targets"; \
echo "build regen: generator"; \
echo "build clean: clean"; \
echo "default" "${default[@]}"; \
} >> "$output/$ninja_file"
