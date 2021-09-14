#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/gen/gen_x11.sh release egl
make -f makefile_lib_x11_egl clean
make -f makefile_lib_x11_egl
