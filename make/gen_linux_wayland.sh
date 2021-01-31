#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ..

# generate makefile
src+=("src/globox.c")
src+=("src/globox_error.c")
src+=("src/wayland/globox_wayland.c")
src+=("src/wayland/globox_wayland_callbacks.c")

src+=("inc/xdg-shell-protocol.c")
src+=("inc/xdg-decoration-protocol.c")
src+=("inc/kde-blur-protocol.c")
src+=("inc/zwp-relative-pointer-protocol.c")
src+=("inc/zwp-pointer-constraints-protocol.c")

obj+=("res/icon/iconpix.o")

flags+=("-std=c99" "-pedantic" "-g")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")
flags+=("-Iinc")

defines+=("-DGLOBOX_ERROR_LOG_BASIC")
defines+=("-DGLOBOX_ERROR_LOG_THROW")
defines+=("-DGLOBOX_ERROR_LOG_DEBUG")
#defines+=("-DGLOBOX_ERROR_SKIP")
#defines+=("-DGLOBOX_ERROR_ABORT")

defines+=("-DGLOBOX_PLATFORM_WAYLAND")

link+=("wayland-client")
linkraw+=("-lrt")

# context-dependent additions
read -p "select context type ([1] software | [2] egl): " action

case $action in
	[1]* )
# software context
makefile=makefile_linux_wayland_software
src+=("example/software.c")
src+=("src/wayland/software/globox_wayland_software.c")
src+=("src/wayland/software/globox_wayland_software_helpers.c")
defines+=("-DGLOBOX_CONTEXT_SOFTWARE")
	;;

	[2]* )
# egl context
makefile=makefile_linux_wayland_egl
src+=("example/egl.c")
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

# generate source files
mkdir -p inc
wayland-scanner private-code \
	< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	> inc/xdg-shell-protocol.c
wayland-scanner client-header \
	< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	> inc/xdg-shell-client-protocol.h
wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml \
	> inc/xdg-decoration-protocol.c
wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml \
	> inc/xdg-decoration-client-protocol.h
wayland-scanner private-code \
	< /usr/share/plasma-wayland-protocols/blur.xml \
	> inc/kde-blur-protocol.c
wayland-scanner client-header \
	< /usr/share/plasma-wayland-protocols/blur.xml \
	> inc/kde-blur-client-protocol.h
wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml \
	> inc/zwp-pointer-constraints-protocol.c
wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml \
	> inc/zwp-pointer-constraints-protocol.h
wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml \
	> inc/zwp-relative-pointer-protocol.c
wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml \
	> inc/zwp-relative-pointer-protocol.h

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

for file in ${obj[@]}; do
	echo "OBJ+= $file" >> $makefile
done

# generate binary target
echo "" >> $makefile
cat make/templates/targets_linux.make >> $makefile

# generate object targets
echo "" >> $makefile
for file in ${src[@]}; do
	gcc $defines -MM -MG $file >> $makefile
done

# generate utilitary targets
echo "" >> $makefile
cat make/templates/targets_extra.make >> $makefile
