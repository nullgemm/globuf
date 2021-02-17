#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

tag=$(git tag | tail -n 1)
release=globox_bin_$tag

cp license.md "$release/lib/"
zip -r "$release.zip" "$release"
