#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

# generate example binaries
make -f makefile_example_x11_software
make -f makefile_example_x11_egl
make -f makefile_example_x11_glx

make -f makefile_example_wayland_software
make -f makefile_example_wayland_egl
