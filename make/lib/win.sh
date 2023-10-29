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
echo "backend types: common, software, wgl, vulkan"
}

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
output="make/output"

# ninja file variables
folder_ninja="build"
folder_objects="\$builddir/obj"
folder_globox="globox_bin_$tag"
folder_library="\$folder_globox/lib/globox/win"
folder_include="\$folder_globox/include"
name="globox_win"
cc="x86_64-w64-mingw32-gcc"
ld="ld"
ar="x86_64-w64-mingw32-gcc-ar"
objcopy="objcopy"

# compiler flags
flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Wno-unused-variable")
flags+=("-Wno-cast-function-type")
flags+=("-Isrc")
flags+=("-Isrc/include")
flags+=("-fPIC")
flags+=("-fdiagnostics-color=always")
defines+=("-DUNICODE")
defines+=("-D_UNICODE")
defines+=("-DWINVER=0x0A00")
defines+=("-D_WIN32_WINNT=0x0A00")

#defines+=("-DGLOBOX_ERROR_ABORT")
#defines+=("-DGLOBOX_ERROR_SKIP")
#defines+=("-DGLOBOX_ERROR_HELPER_WIN")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
defines+=("-DGLOBOX_COMPAT_WINE")

# customize depending on the chosen build type
if [ -z "$build" ]; then
	build=development
fi

case $build in
	development)
flags+=("-g")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
	;;

	release)
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fno-stack-protector")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
defines+=("-DGLOBOX_ERROR_LOG_MANUAL")
	;;

	sanitized_memory)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=leak")
flags+=("-fsanitize-recover=all")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
	;;

	sanitized_undefined)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=undefined")
flags+=("-fsanitize-recover=all")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
	;;

	sanitized_address)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=address")
flags+=("-fsanitize-address-use-after-scope")
flags+=("-fsanitize-recover=all")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
	;;

	sanitized_thread)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=thread")
flags+=("-fsanitize-recover=all")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
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
ninja_file=lib_win_common.ninja
name+="_common"
src+=("src/win/win_common.c")
src+=("src/win/win_common_helpers.c")
	;;

	software)
ninja_file=lib_win_software.ninja
name+="_software"
src+=("src/win/win_software.c")
src+=("src/win/win_software_helpers.c")
	;;

	wgl)
ninja_file=lib_win_wgl.ninja
name+="_wgl"
src+=("src/win/win_wgl.c")
src+=("src/win/win_wgl_helpers.c")
	;;

	vulkan)
ninja_file=lib_win_vulkan.ninja
name+="_vulkan"
flags+=("-DVK_USE_PLATFORM_WIN32_KHR")
src+=("src/win/win_vulkan.c")
src+=("src/win/win_vulkan_helpers.c")
	;;

	*)
echo "invalid backend"
syntax
exit 1
	;;
esac

# save symbols file path
symbols_file="src/win/symbols_$backend.txt"

# default target
default+=("\$folder_library/\$name.a")

# ninja start
mkdir -p "$output"

{ \
echo "# vars"; \
echo "builddir = $folder_ninja"; \
echo "folder_objects = $folder_objects"; \
echo "folder_globox = $folder_globox"; \
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
echo "    command = \$objcopy -D -F pe-x86-64 --globalize-symbols=$symbols_file \$in \$out"; \
echo "    description = globalize \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule local"; \
echo "    command = \$objcopy -F pe-x86-64 -w -L\"*\" \$in \$out"; \
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
echo "    command = \$ld -r \$in --oformat=pe-x86-64 -o \$out"; \
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
echo "    command = make/lib/win.sh $build $backend"; \
echo "    description = re-generating the ninja build file"; \
echo ""; \
} >> "$output/$ninja_file"

# ninja targets
## copy headers
if [ $backend != "common" ]; then
{ \
echo "# copy headers"; \
echo "build \$folder_include/globox_win_$backend.h: \$"; \
echo "cp src/include/globox_win_$backend.h"; \
echo ""; \
echo "build \$folder_include/globox_win.h: \$"; \
echo "cp src/include/globox_win.h"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "build headers: phony \$"; \
echo "\$folder_include/globox_win_$backend.h \$"; \
echo "\$folder_include/globox_win.h"; \
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
