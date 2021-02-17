#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../gen

# if we have "1" as an argument we generate release makefiles
if [ -n "$1" ] && [ "$1" -eq 1 ] ; then
optimize=2
else
optimize=1
fi

echo -e "$optimize\n1\n" | ./gen_x11.sh
echo -e "$optimize\n2\n" | ./gen_x11.sh
echo -e "$optimize\n3\n" | ./gen_x11.sh

echo -e "$optimize\n1\n" | ./gen_wayland.sh
echo -e "$optimize\n2\n" | ./gen_wayland.sh
