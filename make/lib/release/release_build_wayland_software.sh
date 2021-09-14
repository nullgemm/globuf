#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/gen/gen_wayland.sh release software
make -f makefile_lib_wayland_software clean
make -f makefile_lib_wayland_software
