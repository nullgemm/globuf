#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../auto

./gen_windows_mingw_egl_debug.sh
./gen_windows_mingw_wgl_debug.sh
./gen_windows_mingw_software_debug.sh
