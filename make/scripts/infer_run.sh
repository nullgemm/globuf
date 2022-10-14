#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

# get params
build_type=$1
build_platform=$2
build_backend=$3

# set params default values if needed
if [ -z "$build_type" ]; then
	build_type=development
fi

if [ -z "$build_platform" ]; then
	build_platform=x11
fi

if [ -z "$build_backend" ]; then
	build_backend=software
fi

# clean
json=make/output/build_"$build_platform"_"$build_backend".json
folder=infer_"$build_platform"_"$build_backend"
rm -rf "$folder" "$json"

# generate ninja files
case $build_platform in
	x11)
		samu -f make/output/lib_elf.ninja -t compdb ar cc cp clean > "$json"
		samu -f make/output/lib_x11_common.ninja -t compdb ar cc cp clean >> "$json"
		samu -f make/output/lib_x11_"$build_backend".ninja -t compdb ar cc cp clean >> "$json"
		samu -f make/output/example_simple_x11_"$build_backend".ninja -t compdb ar cc cp clean >> "$json"
	;;

	*)
		echo "invalid platform: $build_platform"
		exit 1
	;;
esac

jq -s 'add' "$json" > "$json".new
mv "$json".new "$json"
sed -i 's/ar rcs /ar /' "$json"
./res/infer/bin/infer -o "$folder" --compilation-database "$json"
less "$folder"/report.txt
