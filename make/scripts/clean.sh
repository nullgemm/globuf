#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

while IFS= read -r -d '' file; do
	rm "$file"
done < <(find . -name "*.o" -print0)

while IFS= read -r -d '' file; do
	rm "$file"
done < <(find . -name "*.obj" -print0)

rm -rf bin valgrind.log drmemory.log
