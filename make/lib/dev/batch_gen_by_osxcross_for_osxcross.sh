#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../auto

./gen_by_osxcross_for_osxcross_egl_debug.sh
./gen_by_osxcross_for_osxcross_software_debug.sh
