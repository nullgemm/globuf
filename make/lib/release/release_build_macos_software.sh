#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/auto/gen_by_macos_for_macos_software_release.sh
make -f makefile_lib_macos_software_native clean
make -f makefile_lib_macos_software_native
