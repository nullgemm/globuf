#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd globox || exit

# build lib
./make/lib/gen/gen_wayland.sh sanitized egl
make -f makefile_lib_wayland_egl

# copy lib
./make/lib/release/release_headers.sh
./make/lib/release/release_copy_wayland_egl.sh

# build example
./make/example/simple/gen/gen_wayland.sh sanitized egl static
make -f makefile_example_simple_wayland_egl

# run example
export XDG_RUNTIME_DIR=/scripts/run
weston -B headless-backend.so &
sleep 10

WAYLAND_DISPLAY=wayland-0 ./bin/globox_example_wayland_egl &
sleep 10

pkill globox
pkill weston
