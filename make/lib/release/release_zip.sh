#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

tag=$(git tag | tail -n 1)
release=globox_bin_$tag

cp license.md "$release/lib/"
chmod +x $release/lib/globox/macos/*.dylib
chmod +x $release/lib/globox/windows/*.dll
zip -r "$release.zip" "$release"
