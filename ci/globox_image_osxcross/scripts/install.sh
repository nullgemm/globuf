#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# make						building the binaries
# clang                     compiling objconv
# musl-dev					standard C library
# unzip                     extracting objconv and ANGLE
# curl                      downloading objconv and ANGLE
# libbsd                    using Apple's Xcode clang
# fts                       using Apple's Xcode clang

apk add --no-cache \
	git \
	bash \
	make \
	clang \
	musl-dev \
	unzip \
	curl \
	libbsd \
	fts
