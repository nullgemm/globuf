#!/bin/bash

cd sdk

echo "# Downloading an osxcross source snapshot"

curl -LO https://github.com/tpoechtrager/osxcross/archive/master.zip
unzip master.zip
mv osxcross-master osxcross

echo "# Preparing the SDK with osxcross"

XCODEDIR=/scripts/pkg ./osxcross/tools/gen_sdk_package.sh
mv MacOSX*.*.sdk.tar.xz ./osxcross/tarballs/

echo "# Installing the SDK"

UNATTENDED=1 ./osxcross/build.sh
