#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

# get infer
version="v1.1.0"

mkdir -p res/infer
cd res/infer
curl -L https://github.com/facebook/infer/releases/download/"$version"/infer-linux64-"$version".tar.xz -o infer.tar.xz
tar -xvJf infer.tar.xz
mv infer-linux64-"$version"/* .
rmdir infer-linux64-"$version"
rm infer.tar.xz
