#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_$tag

mkdir -p "$release/lib/globox/wayland"
mv bin/globox.a $release/lib/globox/wayland/globox_wayland_software.a
mv bin/globox.so $release/lib/globox/wayland/globox_wayland_software.so
