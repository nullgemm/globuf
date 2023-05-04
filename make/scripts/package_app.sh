#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

# copy template app
cp -r res/app/globox.app "$1.app"

# copy the binary in the app
mkdir "$1.app/Contents/MacOS"
cp "$1" "$1.app/Contents/MacOS/globox"

# copy the icons bundle in the app
mkdir "$1.app/Contents/Resources"
iconutil -c icns res/app/icon.iconset
cp res/app/icon.icns "$1.app/Contents/Resources/globox.icns"
