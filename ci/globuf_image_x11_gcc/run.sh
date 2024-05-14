#!/bin/bash

# example for a release build
# $ ./run.sh /scripts/build_x11.sh release x11 software none
docker run --name globuf_container_x11_gcc_"$4" globuf_image_x11_gcc "$@" &>> log
