#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../auto

./gen_by_macos_for_macos_egl_debug.sh
./gen_by_macos_for_macos_software_debug.sh
