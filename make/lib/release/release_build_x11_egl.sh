#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/auto/gen_x11_egl_release.sh
make -f makefile_lib_x11_egl clean
make -f makefile_lib_x11_egl
