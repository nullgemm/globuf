#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/auto/gen_windows_mingw_wgl_release.sh
make -f makefile_lib_windows_wgl clean
make -f makefile_lib_windows_wgl
