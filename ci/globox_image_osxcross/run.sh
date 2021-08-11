#!/bin/bash

docker run --privileged \
	-e AR=x86_64-apple-darwin20.2-ar \
	globox_image_osxcross "$@" &> log
