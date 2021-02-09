#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

tag=$(git tag | head -n 1)
release=globox_bin_$tag

src+=("globox.h")
src+=("globox_error.h")

src+=("x11/globox_x11.h")
src+=("x11/software/globox_x11_software.h")
src+=("x11/egl/globox_x11_egl.h")
src+=("x11/glx/globox_x11_glx.h")

src+=("wayland/globox_wayland.h")
src+=("wayland/software/globox_wayland_software.h")
src+=("wayland/egl/globox_wayland_egl.h")

# generate headers
for file in ${src[@]}; do
	folder=$(dirname "$file")
	name=$(basename "$file" .h)
	mkdir -p "$release/include/$folder"
	cp "src/$file" "$release/include/$file"
done

# generate wayland sources
make/scripts/wayland_get.sh


# generate dynamic libraries
mkdir -p "$release/lib/globox/linux/x11"
make -f makefile_linux_x11_software clean
CFLAGS=-fPIC make -f makefile_linux_x11_software bin/globox.so
mv bin/globox.so $release/lib/globox/linux/x11/globox_linux_x11_software.so

make -f makefile_linux_x11_egl clean
CFLAGS=-fPIC make -f makefile_linux_x11_egl bin/globox.so
mv bin/globox.so $release/lib/globox/linux/x11/globox_linux_x11_egl.so

make -f makefile_linux_x11_glx clean
CFLAGS=-fPIC make -f makefile_linux_x11_glx bin/globox.so
mv bin/globox.so $release/lib/globox/linux/x11/globox_linux_x11_glx.so


mkdir -p "$release/lib/globox/linux/wayland"
make -f makefile_linux_wayland_software clean
CFLAGS=-fPIC make -f makefile_linux_wayland_software bin/globox.so
mv bin/globox.so $release/lib/globox/linux/wayland/globox_linux_wayland_software.so

make -f makefile_linux_wayland_egl clean
CFLAGS=-fPIC make -f makefile_linux_wayland_egl bin/globox.so
mv bin/globox.so $release/lib/globox/linux/wayland/globox_linux_wayland_egl.so


# generate static archives
make -f makefile_linux_x11_software clean
make -f makefile_linux_x11_software bin/globox.a
mv bin/globox.a "$release/lib/globox/linux/x11/globox_linux_x11_software.a"

make -f makefile_linux_x11_egl clean
make -f makefile_linux_x11_egl bin/globox.a
mv bin/globox.a "$release/lib/globox/linux/x11/globox_linux_x11_egl.a"

make -f makefile_linux_x11_glx clean
make -f makefile_linux_x11_glx bin/globox.a
mv bin/globox.a "$release/lib/globox/linux/x11/globox_linux_x11_glx.a"


make -f makefile_linux_wayland_software clean
make -f makefile_linux_wayland_software bin/globox.a
mv bin/globox.a "$release/lib/globox/linux/wayland/globox_linux_wayland_software.a"

make -f makefile_linux_wayland_egl clean
make -f makefile_linux_wayland_egl bin/globox.a
mv bin/globox.a "$release/lib/globox/linux/wayland/globox_linux_wayland_egl.a"
