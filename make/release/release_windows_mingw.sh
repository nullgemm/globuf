#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

tag=$(git tag | head -n 1)
release=globox_bin_$tag

src+=("globox.h")
src+=("globox_error.h")

src+=("windows/globox_windows.h")
src+=("windows/software/globox_windows_software.h")
src+=("windows/egl/globox_windows_egl.h")
src+=("windows/wgl/globox_windows_wgl.h")

# generate headers
for file in ${src[@]}; do
	folder=$(dirname "$file")
	name=$(basename "$file" .h)
	mkdir -p "$release/include/$folder"
	cp "src/$file" "$release/include/$file"
done

# generate egl headers
make/scripts/egl_get.sh

# generate dynamic libraries
mkdir -p "$release/lib/globox/windows"
make -f makefile_windows_software clean
make -f makefile_windows_software bin/globox.dll
mv bin/globox.dll "$release/lib/globox/windows/globox_windows_software_mingw.dll"
mv bin/libglobox.a "$release/lib/globox/windows/globox_windows_software_mingw.a"

make -f makefile_windows_egl clean
make -f makefile_windows_egl bin/globox.dll
mv bin/globox.dll "$release/lib/globox/windows/globox_windows_egl_mingw.dll"
mv bin/libglobox.a "$release/lib/globox/windows/globox_windows_egl_mingw.a"

make -f makefile_windows_wgl clean
make -f makefile_windows_wgl bin/globox.dll
mv bin/globox.dll "$release/lib/globox/windows/globox_windows_wgl_mingw.dll"
mv bin/libglobox.a "$release/lib/globox/windows/globox_windows_wgl_mingw.a"
