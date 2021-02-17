#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

# generate examples
make -f makefile_example_macos_software_native
make -f makefile_example_macos_egl_native
