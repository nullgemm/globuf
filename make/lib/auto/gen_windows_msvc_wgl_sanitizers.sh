#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../gen

echo -e "3\n3\n" | ./gen_windows_msvc.sh
