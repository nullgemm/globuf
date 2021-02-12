#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ..

# generate makefile
src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/x11/globox_x11.c")

example+=("res/icon/iconpix.o")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")

defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
#defines+=("-DGLOBOX_ERROR_SKIP")
#defines+=("-DGLOBOX_ERROR_ABORT")

defines+=("-DGLOBOX_PLATFORM_X11")

link+=("xcb")

# build type
read -p "optimize? ([1] optimize | [2] debug): " optimize

if [ $optimize -eq 1 ]; then
flags+=("-O2")
else
flags+=("-g")
fi

# context-dependent additions
read -p "select context type ([1] software | [2] egl | [3] glx): " action

case $action in
	[1]* )
# software context
makefile=makefile_linux_x11_software
example_src+=("example/software.c")
src+=("src/x11/software/globox_x11_software.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
link+=("xcb-shm")
link+=("xcb-randr")
link+=("xcb-render")
	;;

	[2]* )
# egl context
makefile=makefile_linux_x11_egl
example_src+=("example/egl.c")
src+=("src/x11/egl/globox_x11_egl.c")
defines+=("-DGLOBOX_CONTEXT_EGL")
link+=("egl")
link+=("glesv2")
	;;

	[3]* )
# glx context
makefile=makefile_linux_x11_glx
example_src+=("example/glx.c")
src+=("src/x11/glx/globox_x11_glx.c")
defines+=("-DGLOBOX_CONTEXT_GLX")
link+=("glx")
link+=("glesv2")
link+=("x11 x11-xcb xrender")
	;;
esac

# tools
valgrind+=("--show-error-list=yes")
valgrind+=("--show-leak-kinds=all ")
valgrind+=("--track-origins=yes ")
valgrind+=("--leak-check=full ")
valgrind+=("--suppressions=../res/valgrind.supp")

# create empty makefile
echo ".POSIX:" > $makefile
echo "NAME = globox" >> $makefile
echo "CMD = ./globox" >> $makefile

# generate linking info
echo "" >> $makefile
echo "CC = gcc" >> $makefile
echo "LDFLAGS+= $(pkg-config ${link[@]} --cflags)" >> $makefile
echo "LDLIBS+= $(pkg-config ${link[@]} --libs)" >> $makefile

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
