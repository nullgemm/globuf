#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"

echo -e "1\n" | ./gen_linux_x11.sh
echo -e "2\n" | ./gen_linux_x11.sh
echo -e "3\n" | ./gen_linux_x11.sh

echo -e "1\n" | ./gen_linux_wayland.sh 
echo -e "2\n" | ./gen_linux_wayland.sh 

echo -e "1\n" | ./gen_windows_mingw.sh
echo -e "2\n" | ./gen_windows_mingw.sh
echo -e "3\n" | ./gen_windows_mingw.sh

echo -e "1\n" | ./gen_windows_msvc.sh
echo -e "2\n" | ./gen_windows_msvc.sh
echo -e "3\n" | ./gen_windows_msvc.sh
