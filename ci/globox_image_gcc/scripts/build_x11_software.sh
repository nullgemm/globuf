#!/bin/sh

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

# test build
./make/lib/release/release_build_x11_software.sh
