#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

cp globox_bin_*/lib/globox/windows/"$1" bin/globox.dll
