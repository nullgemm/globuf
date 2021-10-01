#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

./make/lib/gen/gen_windows_mingw.sh release software
make -f makefile_lib_windows_software clean
make -f makefile_lib_windows_software
