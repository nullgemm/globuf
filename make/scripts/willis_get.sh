#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

# get libs
version="v0.1.0"

cd res
curl -L https://github.com/nullgemm/willis/releases/download/"$version"/willis_bin_"$version".zip -o willis.zip
unzip willis.zip
mv willis_bin_"$version" willis
