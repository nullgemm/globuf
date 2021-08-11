#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/release/release_headers.sh
./make/lib/auto/gen_windows_mingw_software_release.sh
make -f makefile_lib_windows_software clean
make -f makefile_lib_windows_software
