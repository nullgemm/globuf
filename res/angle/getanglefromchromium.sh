#!/bin/sh

rm -rf ./tmp
rm -rf ./libs
rm -rf ./include
mkdir -p ./tmp
mkdir -p ./libs
mkdir -p ./include
cd ./tmp

curl \
-L "https://download-chromium.appspot.com/dl/Mac?type=snapshots" \
-o ./chromium.zip

curl \
-L "https://github.com/kakashidinho/metalangle/releases/download/gles3-0.0.4/MetalANGLE.dylib.mac.zip" \
-o ./metalangle.zip

unzip ./chromium.zip
cp \
"chrome-mac/Chromium.app/Contents/Frameworks/"\
"Chromium Framework.framework/Libraries/libGLESv2.dylib" \
"chrome-mac/Chromium.app/Contents/Frameworks/"\
"Chromium Framework.framework/Libraries/libEGL.dylib" \
../libs/

unzip ./metalangle.zip
mv \
./include/* \
../include/
