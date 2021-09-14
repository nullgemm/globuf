#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/gen/gen_macos.sh release software native native
make -f makefile_lib_macos_software_native clean
make -f makefile_lib_macos_software_native
