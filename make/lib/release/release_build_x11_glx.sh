#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/auto/gen_x11_glx_release.sh
make -f makefile_lib_x11_glx clean
make -f makefile_lib_x11_glx
