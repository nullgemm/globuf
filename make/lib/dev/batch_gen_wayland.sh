#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../auto

./gen_wayland_egl_debug.sh
./gen_wayland_software_debug.sh
