#!/bin/bash

mkdir -p ./scripts/pkg

# instant_macos_sdk/sdk/root
sudo mount -o bind "$1" ./scripts/pkg
docker build -t osxcross_build_image .
sudo umount ./scripts/pkg
