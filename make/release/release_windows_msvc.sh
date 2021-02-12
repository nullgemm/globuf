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
make/scripts/eglproxy_get.sh

# generate dynamic libraries
mkdir -p "$release/lib/globox/windows"
make -f makefile_windows_software_native clean
make -f makefile_windows_software_native bin/globox.dll
mv bin/globox.dll "$release/lib/globox/windows/globox_windows_software_msvc.dll"

make -f makefile_windows_egl_native clean
make -f makefile_windows_egl_native bin/globox.dll
mv bin/globox.dll "$release/lib/globox/windows/globox_windows_egl_msvc.dll"

make -f makefile_windows_wgl_native clean
make -f makefile_windows_wgl_native bin/globox.dll
mv bin/globox.dll "$release/lib/globox/windows/globox_windows_wgl_msvc.dll"

# generate static archives
mkdir -p "$release/lib/globox/windows"
make -f makefile_windows_software_native clean
make -f makefile_windows_software_native bin/globox.lib
mv bin/globox.lib "$release/lib/globox/windows/globox_windows_software_msvc.lib"

make -f makefile_windows_egl_native clean
make -f makefile_windows_egl_native bin/globox.lib
mv bin/globox.lib "$release/lib/globox/windows/globox_windows_egl_msvc.lib"

make -f makefile_windows_wgl_native clean
make -f makefile_windows_wgl_native bin/globox.lib
mv bin/globox.lib "$release/lib/globox/windows/globox_windows_wgl_msvc.lib"
