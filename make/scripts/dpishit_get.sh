#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

# get libs
version="v0.1.0"

cd res
curl -L https://github.com/nullgemm/dpishit/releases/download/"$version"/dpishit_bin_"$version".zip -o dpishit.zip
unzip dpishit.zip
mv dpishit_bin_"$version" dpishit
