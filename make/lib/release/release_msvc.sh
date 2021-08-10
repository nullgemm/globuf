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
./make/lib/auto/gen_windows_msvc_software_release.sh
./make/lib/auto/gen_windows_msvc_egl_release.sh
./make/lib/auto/gen_windows_msvc_wgl_release.sh

# generate libraries
mkdir -p "$release/lib/globox/windows"
make -f makefile_lib_windows_software_native clean
make -f makefile_lib_windows_software_native
mv bin/globox.lib $release/lib/globox/windows/globox_windows_software_msvc.lib
mv bin/globox.dll $release/lib/globox/windows/globox_windows_software_msvc.dll

make -f makefile_lib_windows_egl_native clean
make -f makefile_lib_windows_egl_native
mv bin/globox.lib $release/lib/globox/windows/globox_windows_egl_msvc.lib
mv bin/globox.dll $release/lib/globox/windows/globox_windows_egl_msvc.dll

make -f makefile_lib_windows_wgl_native clean
make -f makefile_lib_windows_wgl_native
mv bin/globox.lib $release/lib/globox/windows/globox_windows_wgl_msvc.lib
mv bin/globox.dll $release/lib/globox/windows/globox_windows_wgl_msvc.dll
