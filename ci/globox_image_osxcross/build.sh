#!/bin/bash

cd "$(dirname "$0")" || exit

mkdir -p ./scripts/sdk

sudo mount -o bind ../osxcross_build_image/volumes/sdk ./scripts/sdk
docker build -t globox_image_osxcross .
sudo umount ./scripts/sdk
