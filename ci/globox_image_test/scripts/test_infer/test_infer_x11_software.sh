#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

make makefile_development_lib_x11_software
/scripts/infer/bin/infer run -- make build_lib_x11_software
