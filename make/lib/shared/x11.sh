#!/bin/bash

ar -x globuf_x11_common.a
ar -x globuf_x11_software.a
ar -x ../globuf_elf.a
gcc -shared -o globuf_x11_software.so *.o -lxcb-errors -lxcb-shm -lxcb -lxcb-cursor -lxcb-image -lxcb-randr -lxcb-render -lxcb-render-util -lxcb-sync -lxcb-xfixes -lxcb-xinput -lxcb-xkb -lxcb-xrm -lxkbcommon -lxkbcommon-x11 -lpthread
