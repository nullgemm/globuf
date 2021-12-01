#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

rm -rf res/angle
mkdir -p res/angle/libs
mkdir -p res/angle/include
cd res/angle/libs || exit
version=0.0.7

curl \
	-L "https://github.com/kakashidinho/metalangle/releases/download/gles3-$version/MetalANGLE.dylib.mac.zip" \
	-o angle_dev.zip

unzip angle_dev.zip
mv include/* ../include/
