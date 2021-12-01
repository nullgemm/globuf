#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

rm -rf res/angle
mkdir -p res/angle/tmp
mkdir -p res/angle/libs
mkdir -p res/angle/include
cd res/angle/tmp || exit
version=0.0.7

curl \
	-L "https://download-chromium.appspot.com/dl/Mac?type=snapshots" \
	-o chromium.zip

curl \
	-L "https://github.com/kakashidinho/metalangle/releases/download/gles3-$version/MetalANGLE.dylib.mac.zip" \
	-o angle_dev.zip

unzip chromium.zip

cp "chrome-mac/Chromium.app/Contents/Frameworks/Chromium Framework.framework/Libraries/libGLESv2.dylib" "chrome-mac/Chromium.app/Contents/Frameworks/Chromium Framework.framework/Libraries/libEGL.dylib" ../libs/

unzip angle_dev.zip
mv include/* ../include/
