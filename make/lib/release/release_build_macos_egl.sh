#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/auto/gen_by_macos_for_macos_egl_release.sh
make -f makefile_lib_macos_egl_native clean
make -f makefile_lib_macos_egl_native
