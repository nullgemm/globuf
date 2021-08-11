#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/release/release_headers.sh
./make/lib/auto/gen_by_osxcross_for_osxcross_egl_release.sh

make -f makefile_lib_macos_egl clean

if [[ -v AR ]]; then
	make -f makefile_lib_macos_egl -e AR=$AR
else
	make -f makefile_lib_macos_egl
fi
