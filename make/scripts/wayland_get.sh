#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

mkdir -p res/wayland_headers

wayland-scanner private-code \
	< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	> res/wayland_headers/xdg-shell-protocol.c
wayland-scanner client-header \
	< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	> res/wayland_headers/xdg-shell-client-protocol.h
wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml \
	> res/wayland_headers/xdg-decoration-protocol.c
wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml \
	> res/wayland_headers/xdg-decoration-client-protocol.h
wayland-scanner private-code \
	< /usr/share/plasma-wayland-protocols/blur.xml \
	> res/wayland_headers/kde-blur-protocol.c
wayland-scanner client-header \
	< /usr/share/plasma-wayland-protocols/blur.xml \
	> res/wayland_headers/kde-blur-client-protocol.h
wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml \
	> res/wayland_headers/zwp-pointer-constraints-protocol.c
wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml \
	> res/wayland_headers/zwp-pointer-constraints-protocol.h
wayland-scanner private-code \
	< /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml \
	> res/wayland_headers/zwp-relative-pointer-protocol.c
wayland-scanner client-header \
	< /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml \
	> res/wayland_headers/zwp-relative-pointer-protocol.h
