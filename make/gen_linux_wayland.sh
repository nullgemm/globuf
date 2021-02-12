#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ..

# generate makefile
src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/wayland/globox_wayland.c")
src+=("src/wayland/globox_wayland_callbacks.c")

src+=("res/wayland_headers/xdg-shell-protocol.c")
src+=("res/wayland_headers/xdg-decoration-protocol.c")
src+=("res/wayland_headers/kde-blur-protocol.c")
src+=("res/wayland_headers/zwp-relative-pointer-protocol.c")
src+=("res/wayland_headers/zwp-pointer-constraints-protocol.c")

example+=("res/icon/iconpix.o")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")
flags+=("-Ires/wayland_headers")

defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
#defines+=("-DGLOBOX_ERROR_SKIP")
#defines+=("-DGLOBOX_ERROR_ABORT")

defines+=("-DGLOBOX_PLATFORM_WAYLAND")

link+=("wayland-client")
linkraw+=("-lrt")

# build type
read -p "optimize? ([1] optimize | [2] debug): " optimize

if [ $optimize -eq 1 ]; then
flags+=("-O2")
else
flags+=("-g")
fi

# context-dependent additions
read -p "select context type ([1] software | [2] egl): " action

case $action in
	[1]* )
# software context
makefile=makefile_linux_wayland_software
example_src+=("example/software.c")
src+=("src/wayland/software/globox_wayland_software.c")
src+=("src/wayland/software/globox_wayland_software_helpers.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	[2]* )
# egl context
makefile=makefile_linux_wayland_egl
example_src+=("example/egl.c")
src+=("src/wayland/egl/globox_wayland_egl.c")
src+=("src/wayland/egl/globox_wayland_egl_helpers.c")
defines+=("-DGLOBOX_CONTEXT_EGL")
link+=("wayland-egl")
link+=("egl")
link+=("glesv2")
	;;
esac

# tools
valgrind+=("--show-error-list=yes")
valgrind+=("--show-leak-kinds=all ")
valgrind+=("--track-origins=yes ")
valgrind+=("--leak-check=full ")
valgrind+=("--suppressions=../res/valgrind.supp")

# generate additional sources
make/scripts/wayland_get.sh

# create empty makefile
echo ".POSIX:" > $makefile
echo "NAME = globox" >> $makefile
echo "CMD = ./globox" >> $makefile

# generate linking info
echo "" >> $makefile
echo "CC = gcc" >> $makefile
echo "LDFLAGS+= $(pkg-config ${link[@]} --cflags)" >> $makefile
echo "LDLIBS+= $(pkg-config ${link[@]} --libs)" >> $makefile
echo "LDLIBS+= ${linkraw[@]}" >> $makefile

# generate valgrind flags
echo "" >> $makefile
for file in ${valgrind[@]}; do
	echo "VALGRIND+= $file" >> $makefile
done

# generate compiler flags
echo "" >> $makefile
for file in ${flags[@]}; do
	echo "CFLAGS+= $file" >> $makefile
done

echo "" >> $makefile
for file in ${defines[@]}; do
	echo "CFLAGS+= $file" >> $makefile
done

# generate object list
echo "" >> $makefile
for file in ${src[@]}; do
	folder=$(dirname "$file")
	name=$(basename "$file" .c)
	echo "OBJ+= $folder/$name.o" >> $makefile
done

for file in ${example[@]}; do
	echo "EXAMPLE+= $file" >> $makefile
done

for file in ${example_src[@]}; do
	folder=$(dirname "$file")
	name=$(basename "$file" .c)
	echo "EXAMPLE+= $folder/$name.o" >> $makefile
done

# get wayland headers when needed
echo "" >> $makefile
cat make/templates/targets_linux_wayland.make >> $makefile

# generate binary targets
echo "" >> $makefile
cat make/templates/targets_linux.make >> $makefile

echo "" >> $makefile
cat make/templates/targets_nix_libs.make >> $makefile

# generate object targets
echo "" >> $makefile
for file in ${src[@]}; do
	gcc $defines -MM -MG $file >> $makefile
done

for file in ${example_src[@]}; do
	gcc $defines -MM -MG $file >> $makefile
done

# generate utilitary targets
echo "" >> $makefile
cat make/templates/targets_extra.make >> $makefile
