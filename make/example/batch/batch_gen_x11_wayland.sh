#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../gen

# if we have an argument we generate makefiles for this release type
if [ -n "$1" ] ; then
optimize=$1
else
optimize=1
fi

link=1

echo -e "$optimize\n1\n$link\n" | ./gen_x11.sh
echo -e "$optimize\n2\n$link\n" | ./gen_x11.sh
echo -e "$optimize\n3\n$link\n" | ./gen_x11.sh

echo -e "$optimize\n1\n$link\n" | ./gen_wayland.sh
echo -e "$optimize\n2\n$link\n" | ./gen_wayland.sh
