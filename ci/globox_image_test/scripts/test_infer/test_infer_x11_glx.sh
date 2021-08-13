#!/bin/bash

git clone https://github.com/nullgemm/globox.git
cd ./globox

./make/lib/auto/gen_x11_glx_debug.sh
/scripts/infer/bin/infer run -- make -f makefile_lib_x11_glx
