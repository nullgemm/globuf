#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd globox || exit

# build lib
make makefile_sanitized_memory_lib_wayland_egl
make build_lib_wayland_egl
make release_headers

# build example
make makefile_sanitized_memory_example_simple_wayland_egl_static
make build_example_simple_wayland_egl_static 

# run example
export XDG_RUNTIME_DIR=/scripts/run
weston -B headless-backend.so &
sleep 10

WAYLAND_DISPLAY=wayland-0 ./bin/globox_example_simple_wayland_egl &
sleep 10

pkill globox
pkill weston
