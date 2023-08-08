#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# ninja  					building the binaries
# mingw-w64-binutils
# mingw-w64-gcc
# mingw-w64-headers
# mingw-w64-crt

apk add --no-cache \
	git \
	bash \
	ninja \
	mingw-w64-binutils \
	mingw-w64-gcc \
	mingw-w64-headers \
	mingw-w64-crt
