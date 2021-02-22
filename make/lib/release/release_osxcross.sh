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

# generate libraries
mkdir -p "$release/lib/globox/macos"
make -f makefile_lib_macos_software clean
make -f makefile_lib_macos_software
mv bin/globox.a $release/lib/globox/macos/globox_macos_software.a
mv bin/libglobox.dylib $release/lib/globox/macos/libglobox_macos_software.dylib

make -f makefile_lib_macos_egl clean
make -f makefile_lib_macos_egl
mv bin/globox.a $release/lib/globox/macos/globox_macos_egl.a
mv bin/libglobox.dylib $release/lib/globox/macos/libglobox_macos_egl.dylib
