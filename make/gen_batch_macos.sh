#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"

if [ -n "$1" ] && [ "$1" -eq 1 ]; then
optimize=1
else
optimize=2
fi

echo -e "$optimize\n1\n2\n" | ./gen_macos.sh
echo -e "$optimize\n2\n2\n" | ./gen_macos.sh
