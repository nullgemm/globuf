#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../gen

echo -e "2\n3\n" | ./gen_windows_mingw.sh