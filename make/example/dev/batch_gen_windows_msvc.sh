#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../auto

./gen_windows_msvc_egl_debug.sh
./gen_windows_msvc_wgl_debug.sh
./gen_windows_msvc_software_debug.sh
