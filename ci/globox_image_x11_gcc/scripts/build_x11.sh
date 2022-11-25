#!/bin/sh

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

# TODO remove
git checkout next

# test build
./make/scripts/build.sh "$@"
