#!/bin/bash

# example for a release build
# $ ./run.sh /scripts/build_x11.sh release x11 software none
docker run --name globox_container_x11_gcc globox_image_x11_gcc "$@" &> log
