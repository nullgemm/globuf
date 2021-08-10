#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../auto

./gen_x11_egl_debug.sh
./gen_x11_glx_debug.sh
./gen_x11_software_debug.sh
