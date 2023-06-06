#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder" || exit

# get libs
./cursoryx_get.sh
./dpishit_get.sh
./willis_get.sh

# get dependencies
./egl_get.sh
./angle_release_get.sh
./moltenvk_release_get.sh

# get local tools
./infer_get.sh
