#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

cd res/icon || exit

# create an icon X11 pixmap
{ \
echo -n -e "\x10\x00\x00\x00\x10\x00\x00\x00"; \
cat 16.data; \
echo -n -e "\x20\x00\x00\x00\x20\x00\x00\x00"; \
cat 32.data; \
echo -n -e "\x40\x00\x00\x00\x40\x00\x00\x00"; \
cat 64.data; \
} > iconpix.bin

cd ../cursor || exit

# create an icon X11 pixmap
{ \
echo -n -e "\x10\x00\x00\x00\x16\x00\x00\x00"; \
cat 1.data; \
echo -n -e "\x10\x00\x00\x00\x16\x00\x00\x00"; \
cat 2.data; \
echo -n -e "\x10\x00\x00\x00\x16\x00\x00\x00"; \
cat 3.data; \
echo -n -e "\x10\x00\x00\x00\x16\x00\x00\x00"; \
cat 4.data; \
} > cursorpix.bin
