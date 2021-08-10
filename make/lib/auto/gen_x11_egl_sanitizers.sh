#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../gen

echo -e "3\n2\n" | ./gen_x11.sh
