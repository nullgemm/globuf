#!/bin/sh

git clone https://github.com/nullgemm/globox.git
cd ./globox || exit

# test build
export PATH=/scripts/sdk/target/bin/:/bin:/sbin:/usr/bin:/usr/sbin
export LD_LIBRARY_PATH=/scripts/sdk/target/lib/

if [ "$3" == "egl" ]; then
./make/scripts/angle_release_get.sh
fi

if [ "$3" == "vulkan" ]; then
./make/scripts/moltenvk_release_get.sh
fi

if [ "$4" == "complex" ]; then
./make/scripts/cursoryx_get.sh
./make/scripts/dpishit_get.sh
./make/scripts/willis_get.sh
fi

./make/scripts/build.sh "$@"
