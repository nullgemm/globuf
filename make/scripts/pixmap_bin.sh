#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

cd res/icon || exit

# create an icon X11 pixmap
{ \
echo -n -e "\x10\x00\x00\x00\x10\x00\x00\x00"; \
cat 16.data; \
echo -n -e "\x20\x00\x00\x00\x20\x00\x00\x00"; \
cat 32.data; \
echo -n -e "\x40\x00\x00\x00\x40\x00\x00\x00"; \
cat 64.data; \
} >> iconpix.bin
