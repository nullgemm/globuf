#!/bin/sh

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

# TODO remove
git checkout next

if [ "$4" == "complex" ]; then
./make/scripts/cursoryx_get.sh
./make/scripts/dpishit_get.sh
./make/scripts/willis_get.sh
fi

./make/scripts/wayland_get.sh

# test build
./make/scripts/build.sh "$@"
