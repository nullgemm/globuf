#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_"$tag"

cp license.md "$release/lib/"
chmod +x "$release"/lib/globox/macos/*.dylib
chmod +x "$release"/lib/globox/windows/*.dll
zip -r "$release.zip" "$release"
