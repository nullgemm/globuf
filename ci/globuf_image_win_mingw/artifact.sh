#!/bin/bash

tag=$(git tag --sort v:refname | tail -n 1)
release=globuf_bin_"$tag"

docker cp globuf_container_win_mingw_"$1":/globuf/"$release" .
