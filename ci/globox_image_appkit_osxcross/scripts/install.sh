#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# samurai					building the binaries
# musl-dev					standard C library
# unzip                     extracting ANGLE
# curl                      downloading ANGLE
# libbsd                    using Apple's Xcode clang
# fts                       using Apple's Xcode clang

apk add --no-cache \
	git \
	bash \
	samurai \
	musl-dev \
	unzip \
	curl \
	libbsd \
	fts
