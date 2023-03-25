#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# samurai					building the binaries
# gcc						compiling the code
# musl-dev					standard C library
# libxcb-dev				libxcb
# xcb-util-wm-dev			xcb_icccm.h
# xcb-util-image-dev		xcb_image.h
# xcb-util-errors-dev		xcb_errors.h

apk add --no-cache \
	git \
	bash \
	samurai \
	gcc \
	musl-dev \
	libxcb-dev \
	libxrender-dev \
	xcb-util-wm-dev \
	xcb-util-image-dev \
	mesa-dev \
	mesa-egl \
	vulkan-tools \
	vulkan-headers

apk add --no-cache --repository=http://dl-cdn.alpinelinux.org/alpine/edge/testing/ \
	vulkan-validation-layers-dbg \
	vulkan-validation-layers-dev \
	vulkan-validation-layers-static
