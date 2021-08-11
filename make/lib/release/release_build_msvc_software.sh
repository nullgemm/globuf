#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/auto/gen_windows_msvc_software_release.sh
make -f makefile_lib_windows_software_native clean
make -f makefile_lib_windows_software_native
