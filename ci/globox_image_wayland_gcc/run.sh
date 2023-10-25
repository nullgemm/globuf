#!/bin/bash

# example for a release build
# $ ./run.sh /scripts/build_wayland.sh release wayland software none
docker run --name globox_container_wayland_gcc_"$4" globox_image_wayland_gcc "$@" &>> log
