#!/bin/sh

export DEBIAN_FRONTEND=noninteractive

dpkg --add-architecture i386

apt update

apt install --yes --no-install-recommends \
	git \
	bash \
	make \
	gcc \
	clang \
	pkg-config \
	libxcb1-dev \
	libxcb-glx0-dev \
	libxcb-image0-dev \
	libxcb-shm0-dev \
	libxcb-randr0-dev \
	libx11-dev \
	libx11-xcb-dev \
	libxrender-dev \
	libegl1-mesa-dev \
	libwayland-dev \
	wayland-protocols \
	plasma-wayland-protocols \
	curl \
	ca-certificates \
	xz-utils \
	binutils-mingw-w64-x86-64 \
	gcc-mingw-w64-x86-64 \
	mingw-w64-x86-64-dev \
	mingw-w64 \
	cppcheck \
	valgrind \
	xorg \
	xserver-xorg-video-dummy \
	xauth \
	xfwm4 \
	xdotool \
	weston \
	procps \
	unzip \
	wine \
	wine32 \
	wine64 \
	libwine \
	libwine:i386 \
	libc6-dbg \
	libc6-dbg:i386\
	dbus-x11 \
	fonts-wine

# infer
curl -LO https://github.com/facebook/infer/releases/download/v1.1.0/infer-linux64-v1.1.0.tar.xz
tar -xJf infer-linux64-v1.1.0.tar.xz
mv infer-linux64-v1.1.0 infer
rm infer-linux64-v1.1.0.tar.xz

# xdummy xorg config
curl -LO http://xpra.org/xorg.conf

# create fake XDG_RUNTIME_DIR for wayland
mkdir /scripts/run
chmod 700 /scripts/run
