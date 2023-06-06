#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

rm -rf res/angle
mkdir -p res/angle/tmp
mkdir -p res/angle/libs
mkdir -p res/angle/include
cd res/angle/tmp || exit

# get binaries
curl \
	-L "https://download-chromium.appspot.com/dl/Mac?type=snapshots" \
	-o chromium.zip

unzip chromium.zip

cp \
	"chrome-mac/Chromium.app/Contents/Frameworks/Chromium Framework.framework/Libraries/libGLESv2.dylib" \
	"chrome-mac/Chromium.app/Contents/Frameworks/Chromium Framework.framework/Libraries/libEGL.dylib" \
	../libs/

# get headers
git clone --depth 1 https://chromium.googlesource.com/angle/angle

mv angle/include/* ../include/
