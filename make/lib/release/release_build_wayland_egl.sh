#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

./make/lib/gen/gen_wayland.sh release egl
make -f makefile_lib_wayland_egl clean
make -f makefile_lib_wayland_egl
