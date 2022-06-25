#!/bin/sh

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

# test build
make makefile_release_lib_x11_egl
make build_lib_x11_egl
