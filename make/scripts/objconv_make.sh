#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

rm -rf res/objconv
mkdir -p res/objconv/tmp
cd res/objconv/tmp

echo "# downloading"
curl \
-L "https://www.agner.org/optimize/objconv.zip" \
-o objconv.zip

echo "# extracting"
unzip ./objconv.zip
unzip ./source.zip

echo "# building"
./build.sh

echo "# moving binary"
mv objconv ..
