#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/release/release_headers.sh
./make/lib/auto/gen_windows_msvc_wgl_release.sh
make -f makefile_lib_windows_wgl_native clean
make -f makefile_lib_windows_wgl_native
