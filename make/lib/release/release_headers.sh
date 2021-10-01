#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_"$tag"

src+=("globox.h")
src+=("globox_private_getters.h")

# generate headers
for file in "${src[@]}"; do
	folder=$(dirname "$file")
	mkdir -p "$release/include/$folder"
	cp "src/$file" "$release/include/$file"
done
