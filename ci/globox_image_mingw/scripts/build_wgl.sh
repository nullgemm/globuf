#!/bin/sh

git clone https://github.com/nullgemm/globox.git
cd ./globox

# test build
./make/lib/release/release_build_mingw_wgl.sh
