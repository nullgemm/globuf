#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/release/release_headers.sh
./make/lib/auto/gen_windows_mingw_egl_release.sh
make -f makefile_lib_windows_egl clean
make -f makefile_lib_windows_egl
