#!/bin/sh

echo "# making tmp folder"
rm -rf ./tmp
mkdir -p ./tmp
cd ./tmp

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
mv ./objconv ..
