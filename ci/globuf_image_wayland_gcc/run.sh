#!/bin/bash

# example for a release build
# $ ./run.sh /scripts/build_wayland.sh release wayland software none
docker run --name globuf_container_wayland_gcc_"$4" globuf_image_wayland_gcc "$@" &>> log
