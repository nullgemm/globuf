#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# curl                      downloading osxcross
# unzip                     extracting osxcross
# make						building the toolchain
# cmake						building the toolchain
# xz                        extracting sdk resources
# tar                       extracting sdk resources
# gzip                      extracting sdk resources
# bzip2                     extracting sdk resources
# sed                       processing sdk resources
# cpio                      processing sdk resources
# patch                     processing sdk resources
# clang                     compiling tools
# libbz2                    compiling tools
# fts-dev                   compiling tools
# zlib-dev                  compiling tools
# libbsd-dev                compiling tools
# llvm11-dev                compiling tools
# libxml2-dev               compiling tools
# openssl-dev               compiling tools
# python3                   compiling tools
# musl-dev					compiling tools
# build-base                compiling tools

apk add --no-cache \
	git \
	bash \
	curl \
	unzip \
	make \
	cmake \
	xz \
	tar \
	gzip \
	bzip2 \
	sed \
	cpio \
	patch \
	clang \
	libbz2 \
	fts-dev \
	zlib-dev \
	libbsd-dev \
	llvm11-dev \
	libxml2-dev \
	openssl-dev \
	python3 \
	musl-dev \
	build-base
