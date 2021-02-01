#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

rm -rf res/angle
mkdir -p res/angle/libs
mkdir -p res/angle/include
cd res/angle/libs

curl \
-L "https://github.com/kakashidinho/metalangle/releases/download/gles3-0.0.4/MetalANGLE.dylib.mac.zip" \
-o angle_dev.zip

unzip angle_dev.zip
mv include/* ../include/
