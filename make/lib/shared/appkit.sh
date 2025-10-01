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
link+=("-framework")
link+=("AppKit")
link+=("-framework")
link+=("QuartzCore")

if [ -z "$backend" ]; then
	backend=software
fi

case $backend in
	software)
	;;

	egl)
		link+=("-Lres/angle/libs")
		link+=("-lEGL")
		link+=("-lGLESv2")
	;;

	vulkan)
		link+=("-Lres/moltenvk/libs")
		link+=("-lc++")
		link+=("-framework")
		link+=("Metal")
		link+=("-framework")
		link+=("MetalKit")
		link+=("-framework")
		link+=("Foundation")
		link+=("-framework")
		link+=("QuartzCore")
		link+=("-framework")
		link+=("IOKit")
		link+=("-framework")
		link+=("IOSurface")
		link+=("-lMoltenVK")
	;;

	*)
		echo "invalid backend"
		syntax
		exit 1
	;;
esac

# parse soname
soname="$folder_library/appkit/globuf_appkit_$backend.dylib"

# extract objects from static archives
cd "$folder_objects"
ar -x "../../$folder_library/appkit/globuf_appkit_common_native.a"
ar -x "../../$folder_library/appkit/globuf_appkit_""$backend""_native.a"
ar -x "../../$folder_library/globuf_macho_native.a"
cd ../..

# build shared object
clang -shared -o $soname $folder_objects/*.o "${link[@]}"
