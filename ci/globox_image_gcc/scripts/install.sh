#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# make						building the binaries
# gcc						compiling the code
# musl-dev					standard C library
# libxcb-dev				libxcb
# xcb-util-image-dev		libxcb-image
# libx11-dev				libx11
# libxrender-dev			libxrender
# mesa-dev					egl
# wayland-dev				libwayland-client
# wayland-protocols			common wayland protocols
# plasma-wayland-protocols	kde blur wayland protocol

apk add --no-cache \
	git \
	bash \
	make \
	gcc \
	musl-dev \
	libxcb-dev \
	xcb-util-image-dev \
	libx11-dev \
	libxrender-dev \
	mesa-dev \
	wayland-dev \
	wayland-protocols \
	plasma-wayland-protocols
