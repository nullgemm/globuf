#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

for file in $(find . -name "*.o"); do
	rm $file
done;

for file in $(find . -name "*.obj"); do
	rm $file
done;

rm -rf bin valgrind.log drmemory.log
