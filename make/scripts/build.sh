#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

# get params
build_type=$1
build_platform=$2
build_backend=$3
build_example=$4
build_toolchain=$5

# set params default values if needed
if [ -z "$build_type" ]; then
	build_type=development
fi

if [ -z "$build_platform" ]; then
	build_platform=appkit
fi

if [ -z "$build_backend" ]; then
	build_backend=software
fi

if [ -z "$build_example" ]; then
	build_example=simple
fi

if [ -z "$build_toolchain" ]; then
	build_toolchain=native
fi

# generate ninja files
case $build_platform in
	x11)
		rm -rf build make/output
		./make/lib/elf.sh $build_type common
		./make/lib/x11.sh $build_type common
		./make/lib/x11.sh $build_type $build_backend

		if [ "$build_backend" == "vulkan" ]; then
			./make/lib/elf.sh $build_type $build_backend
		fi

		if [ "$build_example" != "none" ]; then
			./make/example/$build_example/x11.sh $build_type $build_backend
		fi
	;;

	appkit)
		rm -rf build make/output
		./make/lib/macho.sh $build_type common $build_toolchain
		#./make/lib/appkit.sh $build_type common $build_toolchain
		#./make/lib/appkit.sh $build_type $build_backend $build_toolchain

		if [ "$build_backend" == "vulkan" ]; then
			./make/lib/macho.sh $build_type $build_backend $build_toolchain
		fi

		#if [ "$build_example" != "none" ]; then
			#./make/example/$build_example/appkit.sh $build_type $build_backend $build_toolchain
		#fi
	;;

	*)
		echo "invalid platform: $build_platform"
		exit 1
	;;
esac

# build
case $build_platform in
	x11)
		samu -f ./make/output/lib_elf.ninja
		samu -f ./make/output/lib_x11_common.ninja
		samu -f ./make/output/lib_x11_"$build_backend".ninja

		samu -f ./make/output/lib_elf.ninja headers
		samu -f ./make/output/lib_x11_"$build_backend".ninja headers

		if [ "$build_backend" == "vulkan" ]; then
			samu -f ./make/output/lib_elf_"$build_backend".ninja
		fi

		if [ "$build_example" != "none" ]; then
			samu -f ./make/output/example_"$build_example"_x11_"$build_backend".ninja
		fi
	;;

	appkit)
		samu -f ./make/output/lib_macho.ninja
		#samu -f ./make/output/lib_appkit_common.ninja
		#samu -f ./make/output/lib_appkit_"$build_backend".ninja

		samu -f ./make/output/lib_macho.ninja headers
		#samu -f ./make/output/lib_appkit_"$build_backend".ninja headers

		if [ "$build_backend" == "vulkan" ]; then
			samu -f ./make/output/lib_macho_"$build_backend".ninja
		fi

		#if [ "$build_example" != "none" ]; then
			#samu -f ./make/output/example_"$build_example"_appkit_"$build_backend".ninja
		#fi
	;;

	*)
		echo "invalid platform: $build_platform"
		exit 1
	;;
esac
