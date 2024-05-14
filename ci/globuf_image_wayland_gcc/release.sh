#!/bin/bash

echo "please clean docker"

sudo rm -rf globuf_bin_v* log
sudo docker rm globuf_container_wayland_gcc_software
sudo docker rm globuf_container_wayland_gcc_vulkan
sudo docker rm globuf_container_wayland_gcc_egl
sudo docker rmi globuf_image_wayland_gcc
sudo docker rmi alpine:edge

sudo ./build.sh

sudo ./run.sh /scripts/build_wayland.sh release wayland software none native
sudo ./run.sh /scripts/build_wayland.sh release wayland vulkan none native
sudo ./run.sh /scripts/build_wayland.sh release wayland egl none native

sudo ./artifact.sh software
sudo ./artifact.sh vulkan
sudo ./artifact.sh egl

sudo chown -R $(id -un):$(id -gn) globuf_bin_v*
