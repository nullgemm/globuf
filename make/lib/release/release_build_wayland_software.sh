#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

./make/lib/release/release_headers.sh
./make/lib/auto/gen_wayland_software_release.sh
make -f makefile_lib_wayland_software clean
make -f makefile_lib_wayland_software
