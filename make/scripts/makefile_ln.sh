#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

ln -sf $1 make/output/makefile
