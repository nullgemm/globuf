#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

if [[ -z ${AR} ]]; then
	make -f $1
else
	make -f $1 -e AR="$AR"
fi
