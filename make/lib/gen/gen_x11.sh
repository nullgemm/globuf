#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1
backend=$2

tag=$(git tag --sort v:refname | tail -n 1)
folder="globox_bin_$tag/lib/globox/x11"
folder_include="globox_bin_$tag/include"

# library makefile data
output="make/output"
name="globox_x11"
cc="gcc"

src+=("src/common/globox.c")
src+=("src/x11/x11_common.c")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")
flags+=("-fPIC")

#defines+=("-DGLOBOX_ERROR_ABORT")
#defines+=("-DGLOBOX_ERROR_SKIP")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
ldflags+=("")
link+=("")

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
	;;

	sanitized_memory)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=leak")
flags+=("-fsanitize-recover=all")
	;;

	sanitized_undefined)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=undefined")
flags+=("-fsanitize-recover=all")
	;;

	sanitized_address)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=address")
flags+=("-fsanitize-address-use-after-scope")
flags+=("-fsanitize-recover=all")
	;;

	sanitized_thread)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=thread")
flags+=("-fsanitize-recover=all")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# backend
if [ -z "$backend" ]; then
	read -rp "select backend (software | egl | vulkan): " backend
fi

case $backend in
	software)
makefile=makefile_lib_x11_software
name+="_software"
src+=("src/x11/x11_software.c")
	;;

	egl)
makefile=makefile_lib_x11_egl
name+="_egl"
src+=("src/x11/x11_egl.c")
	;;

	vulkan)
makefile=makefile_lib_x11_vulkan
name+="_vulkan"
src+=("src/x11/x11_vulkan.c")
	;;

	*)
echo "invalid backend"
exit 1
	;;
esac

# add the library as default target
default+=("$folder/$name.a")

# makefile start
mkdir -p "$output"

{ \
echo ".POSIX:"; \
echo "FOLDER = $folder";\
echo "FOLDER_INCLUDE = $folder_include";\
echo "PLATFORM = x11";\
echo "BACKEND = $backend";\
echo "NAME = $name"; \
echo "CC = $cc"; \
} > "$output/$makefile"

# makefile linking info
echo "" >> "$output/$makefile"
for flag in $(pkg-config "${link[@]}" --cflags) "${ldflags[@]}"; do
	echo "LDFLAGS+= $flag" >> "$output/$makefile"
done

echo "" >> "$output/$makefile"
for flag in $(pkg-config "${link[@]}" --libs); do
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
cat make/lib/templates/targets_linux.make >> "$output/$makefile"

# makefile object targets
echo "" >> "$output/$makefile"
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> "$output/$makefile"
done

# makefile extra targets
echo "" >> "$output/$makefile"
cat make/lib/templates/targets_extra.make >> "$output/$makefile"
