#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"

if [ -n "$1" ] && [ "$1" -eq 1 ] ; then
optimize=1
else
optimize=2
fi

echo -e "$optimize\n1\n" | ./gen_linux_x11.sh
echo -e "$optimize\n2\n" | ./gen_linux_x11.sh
echo -e "$optimize\n3\n" | ./gen_linux_x11.sh

echo -e "$optimize\n1\n" | ./gen_linux_wayland.sh 
echo -e "$optimize\n2\n" | ./gen_linux_wayland.sh 

echo -e "$optimize\n1\n" | ./gen_windows_mingw.sh
echo -e "$optimize\n2\n" | ./gen_windows_mingw.sh
echo -e "$optimize\n3\n" | ./gen_windows_mingw.sh

echo -e "$optimize\n1\n" | ./gen_windows_msvc.sh
echo -e "$optimize\n2\n" | ./gen_windows_msvc.sh
echo -e "$optimize\n3\n" | ./gen_windows_msvc.sh
