#!/bin/bash

mkdir -p ./scripts/sdk

if [ ! -d "../../res/osxcross/docker/volumes/sdk" ]; then
	echo "please make sure the res/osxcross folder contains a macOS SDK"
	echo "please refer to https://github.com/tpoechtrager/osxcross"
	echo "or use https://github.com/nullgemm/instant_macos_sdk"
	exit
fi

sudo mount -o bind ../../res/osxcross ./scripts/sdk
docker build -t globox_image_appkit_osxcross .
sudo umount ./scripts/sd
