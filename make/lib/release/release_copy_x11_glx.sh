#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_$tag

mkdir -p "$release/lib/globox/x11"
mv bin/globox.a $release/lib/globox/x11/globox_x11_glx.a
mv bin/globox.so $release/lib/globox/x11/globox_x11_glx.so