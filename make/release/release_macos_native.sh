#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

tag=$(git tag | head -n 1)
release=globox_bin_$tag

src+=("globox.h")
src+=("globox_error.h")

src+=("macos/globox_macos.h")
src+=("macos/software/globox_macos_software.h")
src+=("macos/egl/globox_macos_egl.h")

# generate headers
for file in ${src[@]}; do
	folder=$(dirname "$file")
	name=$(basename "$file" .h)
	mkdir -p "$release/include/$folder"
	cp "src/$file" "$release/include/$file"
done

# generate wayland sources
make/scripts/angle_master_get.sh

# generate dynamic libraries
mkdir -p "$release/lib/globox/macos"
make -f makefile_macos_software_native clean
CFLAGS=-fPIC make -f makefile_macos_software_native bin/globox.so
mv bin/globox.so $release/lib/globox/macos/globox_macos_software_native.so

make -f makefile_macos_egl_native clean
CFLAGS=-fPIC make -f makefile_macos_egl_native bin/globox.so
mv bin/globox.so $release/lib/globox/macos/globox_macos_egl_native.so


# generate static archives
make -f makefile_macos_software_native clean
make -f makefile_macos_software_native bin/globox.a
mv bin/globox.a "$release/lib/globox/macos/globox_macos_software_native.a"

make -f makefile_macos_egl_native clean
make -f makefile_macos_egl_native bin/globox.a
mv bin/globox.a "$release/lib/globox/macos/globox_macos_egl_native.a"
