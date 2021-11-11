#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_"$tag"

mkdir -p "$release/lib/globox/windows"
mv bin/globox.lib "$release"/lib/globox/windows/globox_windows_egl_msvc.lib
