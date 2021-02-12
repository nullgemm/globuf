#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"

echo -e "1\n2\n" | ./gen_macos.sh
echo -e "2\n2\n" | ./gen_macos.sh
