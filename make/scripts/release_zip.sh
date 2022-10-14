#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_"$tag"

cp license.md "$release/lib/"
zip -r "$release.zip" "$release"
