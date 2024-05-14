#!/bin/bash

echo "please clean docker"

sudo rm -rf globuf_bin_v* log
sudo docker rm globuf_container_appkit_osxcross_software
sudo docker rm globuf_container_appkit_osxcross_vulkan
sudo docker rm globuf_container_appkit_osxcross_egl
sudo docker rmi globuf_image_appkit_osxcross
sudo docker rmi alpine:edge

sudo ./build.sh

sudo ./run.sh /scripts/build_appkit.sh release appkit software none osxcross
sudo ./run.sh /scripts/build_appkit.sh release appkit vulkan none osxcross
sudo ./run.sh /scripts/build_appkit.sh release appkit egl none osxcross

sudo ./artifact.sh software
sudo ./artifact.sh vulkan
sudo ./artifact.sh egl

sudo chown -R $(id -un):$(id -gn) globuf_bin_v*
