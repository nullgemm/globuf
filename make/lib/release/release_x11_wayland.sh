#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

tag=$(git tag | tail -n 1)
release=globox_bin_$tag

src+=("globox.h")
src+=("globox_private_getters.h")

# generate headers
for file in ${src[@]}; do
	folder=$(dirname "$file")
	mkdir -p "$release/include/$folder"
	cp "src/$file" "$release/include/$file"
done

# generate x11 libraries
mkdir -p "$release/lib/globox/x11"
make -f makefile_lib_x11_software clean
make -f makefile_lib_x11_software
mv bin/globox.a $release/lib/globox/x11/globox_x11_software.a
mv bin/globox.so $release/lib/globox/x11/globox_x11_software.so

make -f makefile_lib_x11_egl clean
make -f makefile_lib_x11_egl
mv bin/globox.a $release/lib/globox/x11/globox_x11_egl.a
mv bin/globox.so $release/lib/globox/x11/globox_x11_egl.so

make -f makefile_lib_x11_glx clean
make -f makefile_lib_x11_glx
mv bin/globox.a $release/lib/globox/x11/globox_x11_glx.a
mv bin/globox.so $release/lib/globox/x11/globox_x11_glx.so

# generate wayland libraries
mkdir -p "$release/lib/globox/wayland"
make -f makefile_lib_wayland_software clean
make -f makefile_lib_wayland_software
mv bin/globox.a $release/lib/globox/wayland/globox_wayland_software.a
mv bin/globox.so $release/lib/globox/wayland/globox_wayland_software.so

make -f makefile_lib_wayland_egl clean
make -f makefile_lib_wayland_egl
mv bin/globox.a $release/lib/globox/wayland/globox_wayland_egl.a
mv bin/globox.so $release/lib/globox/wayland/globox_wayland_egl.so
