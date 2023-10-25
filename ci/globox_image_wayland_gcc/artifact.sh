#!/bin/bash

tag=$(git tag --sort v:refname | tail -n 1)
release=globox_bin_"$tag"

docker cp globox_container_wayland_gcc_"$1":/globox/"$release" .
