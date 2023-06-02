#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

rm -rf res/moltenvk
mkdir -p res/moltenvk/tmp
mkdir -p res/moltenvk/libs
cd res/moltenvk/tmp || exit

# get binaries
curl \
	-L "https://github.com/KhronosGroup/MoltenVK/releases/latest/download/MoltenVK-all.tar" \
	-o moltenvk.tar

tar xvf moltenvk.tar

cp -r MoltenVK/MoltenVK/include ..
cp -r MoltenVK/MoltenVK/dylib/macOS/libMoltenVK.dylib ../libs/
