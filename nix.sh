#!/bin/bash

path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"

./make/lib/elf.sh development
./make/lib/x11.sh development common
./make/lib/x11.sh development software
./make/example/simple/x11.sh development software

samu -f ./make/output/lib_elf.ninja
samu -f ./make/output/lib_x11_common.ninja
samu -f ./make/output/lib_x11_software.ninja

samu -f ./make/output/lib_elf.ninja headers
samu -f ./make/output/lib_x11_software.ninja headers

samu -f ./make/output/example_simple_x11_software.ninja
