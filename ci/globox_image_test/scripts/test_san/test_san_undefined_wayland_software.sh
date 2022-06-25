#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd globox || exit

# build lib
make makefile_sanitized_undefined_lib_wayland_software
make build_lib_wayland_software
make release_headers

# build example
make makefile_sanitized_undefined_example_simple_wayland_software_static
make build_example_simple_wayland_software_static 

# run example
export XDG_RUNTIME_DIR=/scripts/run
weston -B headless-backend.so &
sleep 10

WAYLAND_DISPLAY=wayland-0 ./bin/globox_example_simple_wayland_software &
sleep 10

pkill globox
pkill weston
