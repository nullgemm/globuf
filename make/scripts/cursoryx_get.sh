#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

# get libs
version="v0.3.5"

cd res
curl -L https://github.com/nullgemm/cursoryx/releases/download/"$version"/cursoryx_bin_dbg_"$version".zip -o cursoryx.zip
unzip cursoryx.zip
mv cursoryx_bin_dbg_"$version" cursoryx
