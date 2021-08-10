#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_$tag

src+=("globox.h")
src+=("globox_private_getters.h")

# generate headers
for file in ${src[@]}; do
	folder=$(dirname "$file")
	mkdir -p "$release/include/$folder"
	cp "src/$file" "$release/include/$file"
done

# generate makefiles
./make/lib/auto/gen_by_osxcross_for_osxcross_software_release.sh
./make/lib/auto/gen_by_osxcross_for_osxcross_egl_release.sh

# generate libraries
mkdir -p "$release/lib/globox/macos"
make -f makefile_lib_macos_software clean

if [[ -v AR ]]; then
	make -f makefile_lib_macos_software -e AR=$AR
else
	make -f makefile_lib_macos_software
fi

mv bin/globox.a $release/lib/globox/macos/globox_macos_software.a
mv bin/libglobox.dylib $release/lib/globox/macos/libglobox_macos_software.dylib

make -f makefile_lib_macos_egl clean

if [[ -v AR ]]; then
	make -f makefile_lib_macos_egl -e AR=$AR
else
	make -f makefile_lib_macos_egl
fi

mv bin/globox.a $release/lib/globox/macos/globox_macos_egl.a
mv bin/libglobox.dylib $release/lib/globox/macos/libglobox_macos_egl.dylib
