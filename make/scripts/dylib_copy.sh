#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

cp globox_bin_*/lib/globox/macos/lib"$1".dylib bin/libglobox.dylib
