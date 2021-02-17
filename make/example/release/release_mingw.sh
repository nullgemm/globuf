#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

# generate example binaries
make -f makefile_example_windows_software
make -f makefile_example_windows_egl
make -f makefile_example_windows_wgl
