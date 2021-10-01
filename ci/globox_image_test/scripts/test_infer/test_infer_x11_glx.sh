#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

./make/lib/gen/gen_x11.sh development glx
/scripts/infer/bin/infer run -- make -f makefile_lib_x11_glx
