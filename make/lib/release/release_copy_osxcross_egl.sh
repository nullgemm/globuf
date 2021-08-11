#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_$tag

mkdir -p "$release/lib/globox/macos"
mv bin/globox.a $release/lib/globox/macos/globox_macos_egl.a
mv bin/libglobox.dylib $release/lib/globox/macos/libglobox_macos_egl.dylib
