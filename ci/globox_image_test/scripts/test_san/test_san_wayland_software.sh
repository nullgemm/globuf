#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd globox

# build lib
./make/lib/auto/gen_wayland_software_sanitizers.sh
make -f makefile_lib_wayland_software

# copy lib
./make/lib/release/release_headers.sh
./make/lib/release/release_copy_wayland_software.sh

# build example
./make/example/auto/gen_wayland_software_sanitizers_static.sh
make -f makefile_example_wayland_software

# run example
export XDG_RUNTIME_DIR=/scripts/run
weston -B headless-backend.so &
sleep 10

WAYLAND_DISPLAY=wayland-0 ./bin/globox_example_wayland_software &
sleep 10

pkill globox
pkill weston
