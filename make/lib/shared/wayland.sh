#!/bin/bash

ar -x globuf_wayland_common.a
ar -x globuf_wayland_software.a
ar -x ../globuf_elf.a
gcc -shared -o globuf_wayland_software.so *.o -lwayland-client -lwayland-cursor -lxkbcommon -lpthread
