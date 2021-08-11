#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/auto/gen_by_osxcross_for_osxcross_software_release.sh

make -f makefile_lib_macos_software clean

if [[ -v AR ]]; then
	make -f makefile_lib_macos_software -e AR=$AR
else
	make -f makefile_lib_macos_software
fi
