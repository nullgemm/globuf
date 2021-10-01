#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

# download target eglproxy release
version="v0.1.1"

curl -L "https://github.com/nullgemm/eglproxy/releases/download/$version/eglproxy_bin_$version.zip" -o \
	res/eglproxy.zip

cd res || exit
unzip eglproxy.zip

rm eglproxy.zip
mv "eglproxy_bin_$version" eglproxy
