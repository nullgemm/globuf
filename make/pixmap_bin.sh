#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../res/icon

# create an icon X11 pixmap
echo -n -e "\x10\x00\x00\x00\x10\x00\x00\x00" >> iconpix.bin
cat 16.data >> iconpix.bin
echo -n -e "\x20\x00\x00\x00\x20\x00\x00\x00" >> iconpix.bin
cat 32.data >> iconpix.bin
echo -n -e "\x40\x00\x00\x00\x40\x00\x00\x00" >> iconpix.bin
cat 64.data >> iconpix.bin
