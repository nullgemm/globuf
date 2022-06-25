#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

make makefile_development_lib_wayland_software
/scripts/infer/bin/infer run -- make build_lib_wayland_software
