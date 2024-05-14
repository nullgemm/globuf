#!/bin/sh

# git                       cloning the repo
# bash                      running the build scripts
# samurai                   building the binaries
# gcc                       compiling the code
# musl-dev                  standard C library
# curl                      getting companion libs
# libxkbcommon-dev          complex example
# wayland                   wayland libs for client, cursor, egl
# wayland-dev               wayland headers
# wayland-protocols         wayland XDG protocols
# plasma-wayland-protocols  wayland KDE blur protocol

apk add --no-cache \
	git \
	bash \
	samurai \
	gcc \
	musl-dev \
	curl \
	libxkbcommon-dev \
	wayland \
	wayland-dev \
	wayland-protocols \
	plasma-wayland-protocols \
	mesa-dev \
	mesa-egl \
	vulkan-tools \
	vulkan-headers \
	vulkan-loader-dev

apk add --no-cache --repository=http://dl-cdn.alpinelinux.org/alpine/edge/testing/ \
	vulkan-validation-layers-dbg \
	vulkan-validation-layers-dev \
	vulkan-validation-layers-static
