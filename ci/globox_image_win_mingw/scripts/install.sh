#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# gcc						relinking the objects
# ninja  					building the binaries
# mingw-w64-binutils
# mingw-w64-gcc
# mingw-w64-headers
# mingw-w64-crt

apk add --no-cache \
	git \
	bash \
	gcc \
	ninja \
	mingw-w64-binutils \
	mingw-w64-gcc \
	mingw-w64-headers \
	mingw-w64-crt
