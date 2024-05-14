#!/bin/bash

echo "please clean docker"

sudo rm -rf globuf_bin_v* log
sudo docker rm globuf_container_win_mingw_software
sudo docker rm globuf_container_win_mingw_wgl
sudo docker rmi globuf_image_win_mingw
sudo docker rmi alpine:edge

sudo ./build.sh

sudo ./run.sh /scripts/build_win.sh release win software none native
sudo ./run.sh /scripts/build_win.sh release win wgl none native

sudo ./artifact.sh software
sudo ./artifact.sh wgl

sudo chown -R $(id -un):$(id -gn) globuf_bin_v*
