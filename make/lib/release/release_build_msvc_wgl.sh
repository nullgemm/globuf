#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

./make/lib/gen/gen_windows_msvc.sh release wgl
make -f makefile_lib_windows_wgl_native clean
make -f makefile_lib_windows_wgl_native
