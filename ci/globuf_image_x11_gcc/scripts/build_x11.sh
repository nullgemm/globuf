#!/bin/sh

git clone https://github.com/nullgemm/globuf.git
cd ./globuf || exit

if [ "$4" == "complex" ]; then
./make/scripts/cursoryx_get.sh
./make/scripts/dpishit_get.sh
./make/scripts/willis_get.sh
fi

# test build
./make/scripts/build.sh "$@"
