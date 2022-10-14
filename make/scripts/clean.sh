#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

rm -rf build globox_bin_* valgrind.log drmemory.log
./make/scripts/clean_ninja.sh
