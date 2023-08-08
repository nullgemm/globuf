#!/bin/bash

echo "please clean docker"

sudo rm -rf globox_bin_v* log
sudo docker rm globox_container_win_mingw_software
sudo docker rm globox_container_win_mingw_wgl
sudo docker rmi globox_image_win_mingw
sudo docker rmi alpine:edge

sudo ./build.sh

sudo ./run.sh /scripts/build_win.sh release win software none native
sudo ./run.sh /scripts/build_win.sh release win wgl none native

sudo ./artifact.sh software
sudo ./artifact.sh wgl

sudo chown -R $(id -un):$(id -gn) globox_bin_v*
