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
make -f makefile_lib_macos_software_native clean
make -f makefile_lib_macos_software_native
mv bin/globox.a $release/lib/globox/macos/globox_macos_software_native.a
mv bin/libglobox.dylib $release/lib/globox/macos/libglobox_macos_software_native.dylib

make -f makefile_lib_macos_egl_native clean
make -f makefile_lib_macos_egl_native
mv bin/globox.a $release/lib/globox/macos/globox_macos_egl_native.a
mv bin/libglobox.dylib $release/lib/globox/macos/libglobox_macos_egl_native.dylib
