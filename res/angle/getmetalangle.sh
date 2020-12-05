#!/bin/sh

rm -rf ./libs
rm -rf ./include
mkdir -p ./libs
mkdir -p ./include
cd ./libs

curl \
-L "https://github.com/kakashidinho/metalangle/releases/download/gles3-0.0.4/MetalANGLE.dylib.mac.zip" \
-o ./metalangle.zip

unzip ./metalangle.zip
mv ./include/* ../include/
