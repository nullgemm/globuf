#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ..

cp .github .gitmodules
git submodule sync
git submodule update --init --remote
