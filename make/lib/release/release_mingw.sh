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

# generate libraries
mkdir -p "$release/lib/globox/windows"
make -f makefile_lib_windows_software clean
make -f makefile_lib_windows_software
mv bin/libglobox.a $release/lib/globox/windows/globox_windows_software_mingw.a
mv bin/globox.dll $release/lib/globox/windows/globox_windows_software_mingw.dll

make -f makefile_lib_windows_egl clean
make -f makefile_lib_windows_egl
mv bin/libglobox.a $release/lib/globox/windows/globox_windows_egl_mingw.a
mv bin/globox.dll $release/lib/globox/windows/globox_windows_egl_mingw.dll

make -f makefile_lib_windows_wgl clean
make -f makefile_lib_windows_wgl
mv bin/libglobox.a $release/lib/globox/windows/globox_windows_wgl_mingw.a
mv bin/globox.dll $release/lib/globox/windows/globox_windows_wgl_mingw.dll
