#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

# params
backend=$1

function syntax {
echo "syntax reminder: $0 <backend type>"
echo "backend types: software, wgl, vulkan"
}

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
folder_ninja="build"
folder_objects="$folder_ninja/shared"
folder_globuf="globuf_bin_$tag"
folder_library="$folder_globuf/lib/globuf"
mkdir -p "$folder_objects"

# list link flags (order matters)
link+=("-lshcore")
link+=("-lgdi32")
link+=("-ldwmapi")

if [ -z "$backend" ]; then
	backend=software
fi

case $backend in
	software)
	;;

	wgl)
		link+=("-lopengl32")
	;;

	vulkan)
		link+=("-lvulkan-1")
	;;

	*)
		echo "invalid backend"
		syntax
		exit 1
	;;
esac

# list objs (order matters)
obj+=("$folder_objects/globuf_win_common.o")
obj+=("$folder_objects/globuf_win_$backend.o")
obj+=("$folder_objects/globuf_pe.o")

# parse soname
soname="$folder_library/win/globuf_win_$backend.dll"

# extract objects from static archives
ar --output "$folder_objects" -x "$folder_library/win/globuf_win_common.a"
ar --output "$folder_objects" -x "$folder_library/win/globuf_win_$backend.a"
ar --output "$folder_objects" -x "$folder_library/globuf_pe.a"

# build shared object
x86_64-w64-mingw32-gcc -shared -o $soname "${obj[@]}" "${link[@]}"
