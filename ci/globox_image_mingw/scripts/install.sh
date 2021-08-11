#!/bin/sh

# git                       cloning the repo
# bash                      running the build scripts
# make                      building the binaries
# curl                      download stuff
# mingw-w64-binutils        mingw utils (objcopy...)
# mingw-w64-gcc             mingw gcc
# mingw-w64-headers         posix headers
# mingw-w64-crt             windows headers

apk add --no-cache \
	git \
	bash \
	make \
	curl \
	mingw-w64-binutils \
	mingw-w64-gcc \
	mingw-w64-headers \
	mingw-w64-crt
