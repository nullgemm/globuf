#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

./make/lib/gen/gen_wayland.sh development egl
/scripts/infer/bin/infer run -- make -f makefile_lib_wayland_egl
