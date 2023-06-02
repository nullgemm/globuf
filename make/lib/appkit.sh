#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../..

# params
build=$1
backend=$2
toolchain=$3

echo "syntax reminder: $0 <build type> <backend type> <target toolchain type>"
echo "build types: development, release, sanitized"
echo "backend types: common, software, egl, vulkan"
echo "target toolchain types: osxcross, native"

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
output="make/output"

# ninja file variables
folder_ninja="build"
folder_objects="\$builddir/obj"
folder_globox="globox_bin_$tag"
folder_library="\$folder_globox/lib/globox/appkit"
folder_include="\$folder_globox/include"
name="globox_appkit"

# compiler flags
flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-strict-prototypes")
flags+=("-Wno-unused-parameter")
flags+=("-Wno-unused-variable")
flags+=("-Isrc")
flags+=("-Isrc/include")
flags+=("-fPIC")
flags+=("-fdiagnostics-color=always")
ldflags+=("-framework AppKit")
ldflags+=("-framework QuartzCore")

#defines+=("-DGLOBOX_ERROR_ABORT")
#defines+=("-DGLOBOX_ERROR_SKIP")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")

# customize depending on the chosen build type
if [ -z "$build" ]; then
	build=development
fi

case $build in
	development)
flags+=("-g")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
defines+=("-DGLOBOX_ERROR_HELPER_XCB")
	;;

	release)
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fstack-protector-strong")
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
exit 1
	;;
esac

# customize depending on the chosen backend type
if [ -z "$backend" ]; then
	backend=software
fi

case $backend in
	common)
ninja_file=lib_appkit_common.ninja
name+="_common"
src+=("src/appkit/appkit_common.m")
src+=("src/appkit/appkit_common_helpers.m")
	;;

	software)
ninja_file=lib_appkit_software.ninja
name+="_software"
src+=("src/appkit/appkit_software.m")
src+=("src/appkit/appkit_software_helpers.m")
	;;

	egl)
ninja_file=lib_appkit_egl.ninja
name+="_egl"
src+=("src/appkit/appkit_egl.m")
src+=("src/appkit/appkit_egl_helpers.m")
flags+=("-Ires/angle/include")
ldflags+=("-Lres/angle/libs")
ldlibs+=("-lEGL")
ldlibs+=("-lGLESv2")
	;;

	vulkan)
ninja_file=lib_appkit_vulkan.ninja
name+="_vulkan"
src+=("src/appkit/appkit_vulkan.m")
src+=("src/appkit/appkit_vulkan_helpers.m")
flags+=("-Ires/moltenvk/include")
ldflags+=("-Lres/moltenvk/libs")
ldflags+=("-lc++")
ldflags+=("-framework Metal")
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
name+="_osxcross"
cc="o64-clang"
ar="x86_64-apple-darwin21.4-ar"
	;;

	native)
name+="_native"
cc="clang"
ar="ar"
	;;

	*)
echo "invalid target toolchain type"
exit 1
	;;
esac

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
echo "ar = $ar"; \
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
echo "rule ar"; \
echo "    command = \$ar rcs \$out \$in"; \
echo "    description = ar \$out"; \
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
echo "    command = make/lib/appkit.sh $build $backend"; \
echo "    description = re-generating the ninja build file"; \
echo ""; \
} >> "$output/$ninja_file"

# ninja targets
## copy headers
if [ $backend != "common" ]; then
{ \
echo "# copy headers"; \
echo "build \$folder_include/globox_appkit_$backend.h: \$"; \
echo "cp src/include/globox_appkit_$backend.h"; \
echo ""; \
echo "build \$folder_include/globox_appkit.h: \$"; \
echo "cp src/include/globox_appkit.h"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "build headers: phony \$"; \
echo "\$folder_include/globox_appkit_$backend.h \$"; \
echo "\$folder_include/globox_appkit.h"; \
echo ""; \
} >> "$output/$ninja_file"
fi

## compile sources
echo "# compile sources" >> "$output/$ninja_file"
for file in "${src[@]}"; do
	folder=$(dirname "$file")
	filename=$(basename "$file" .m)
	obj+=("\$folder_objects/$folder/$filename.o")
	{ \
	echo "build \$folder_objects/$folder/$filename.o: \$"; \
	echo "cc $file"; \
	echo ""; \
	} >> "$output/$ninja_file"
done

## main targets
echo "# archive objects" >> "$output/$ninja_file"
echo -n "build \$folder_library/\$name.a: ar" >> "$output/$ninja_file"
for file in "${obj[@]}"; do
	echo -ne " \$\n$file" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

## special targets
{ \
echo "# run special targets"; \
echo "build regen: generator"; \
echo "build clean: clean"; \
echo "default" "${default[@]}"; \
} >> "$output/$ninja_file"
