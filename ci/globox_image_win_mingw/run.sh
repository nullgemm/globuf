#!/bin/bash

# example for a release build
# $ ./run.sh /scripts/build_win.sh release win software none
docker run --name globox_container_win_mingw_"$4" globox_image_win_mingw "$@" &>> log
