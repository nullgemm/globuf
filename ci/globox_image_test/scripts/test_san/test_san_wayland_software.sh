#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd globox

# build lib
./make/lib/gen/gen_wayland.sh sanitized software
make -f makefile_lib_wayland_software

# copy lib
./make/lib/release/release_headers.sh
./make/lib/release/release_copy_wayland_software.sh

# build example
./make/example/gen/gen_wayland.sh sanitized software static
make -f makefile_example_wayland_software

# run example
export XDG_RUNTIME_DIR=/scripts/run
weston -B headless-backend.so &
sleep 10

WAYLAND_DISPLAY=wayland-0 ./bin/globox_example_wayland_software &
sleep 10

pkill globox
pkill weston
