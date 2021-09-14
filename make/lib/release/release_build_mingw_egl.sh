#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/gen/gen_windows_mingw.sh release egl
make -f makefile_lib_windows_egl clean
make -f makefile_lib_windows_egl
