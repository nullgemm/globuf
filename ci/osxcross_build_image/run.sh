#!/bin/bash

cd "$(dirname "$0")" || exit

mkdir -p ./volumes/sdk

docker run --privileged \
	-v "$(pwd)"/volumes/sdk:/scripts/sdk \
	osxcross_build_image &> log
