#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

# params
backend=$1

function syntax {
echo "syntax reminder: $0 <backend type>"
echo "backend types: software, egl, vulkan"
}

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
folder_ninja="build"
folder_objects="$folder_ninja/shared"
folder_globuf="globuf_bin_$tag"
folder_library="$folder_globuf/lib/globuf"
mkdir -p "$folder_objects"

# list link flags (order matters)
link+=("-lwayland-client")
link+=("-lwayland-cursor")
link+=("-lxkbcommon")
link+=("-lpthread")

if [ -z "$backend" ]; then
	backend=software
fi

case $backend in
	software)
	;;

	egl)
		link+=("-lEGL")
		link+=("-lGLESv2")
		link+=("-lwayland-egl")
	;;

	vulkan)
		link+=("-lvulkan")
		link+=("-lxcb-render")
	;;

	*)
		echo "invalid backend"
		syntax
		exit 1
	;;
esac

# list objs (order matters)
obj+=("$folder_objects/globuf_wayland_common.o")
obj+=("$folder_objects/globuf_wayland_$backend.o")
obj+=("$folder_objects/globuf_elf.o")

# parse soname
soname="$folder_library/wayland/globuf_wayland_$backend.so"

# extract objects from static archives
ar --output "$folder_objects" -x "$folder_library/wayland/globuf_wayland_common.a"
ar --output "$folder_objects" -x "$folder_library/wayland/globuf_wayland_$backend.a"
ar --output "$folder_objects" -x "$folder_library/globuf_elf.a"

# build shared object
gcc -shared -o $soname "${obj[@]}" "${link[@]}"
