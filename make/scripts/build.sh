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
	build_platform=wayland
fi

if [ -z "$build_backend" ]; then
	build_backend=egl
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

		case $build_backend in
			software|vulkan)
				./make/lib/elf.sh $build_type $build_backend
			;;

			egl|glx)
				./make/lib/elf.sh $build_type opengl
			;;
		esac

		if [ "$build_example" != "none" ]; then
			./make/example/$build_example/x11.sh $build_type $build_backend
		fi
	;;

	appkit)
		rm -rf build make/output
		./make/lib/macho.sh $build_type common $build_toolchain
		./make/lib/appkit.sh $build_type common $build_toolchain
		./make/lib/appkit.sh $build_type $build_backend $build_toolchain

		case $build_backend in
			software|vulkan)
				./make/lib/macho.sh $build_type $build_backend $build_toolchain
			;;

			egl)
				./make/lib/macho.sh $build_type opengl $build_toolchain
			;;
		esac

		if [ "$build_example" != "none" ]; then
			./make/example/$build_example/appkit.sh $build_type $build_backend $build_toolchain
		fi
	;;

	win)
		rm -rf build make/output
		./make/lib/pe.sh $build_type common
		./make/lib/win.sh $build_type common
		./make/lib/win.sh $build_type $build_backend

		case $build_backend in
			software|vulkan)
				./make/lib/pe.sh $build_type $build_backend
			;;

			wgl)
				./make/lib/pe.sh $build_type opengl
			;;
		esac

		if [ "$build_example" != "none" ]; then
			./make/example/$build_example/win.sh $build_type $build_backend
		fi
	;;

	wayland)
		rm -rf build make/output
		./make/lib/elf.sh $build_type common
		./make/lib/wayland.sh $build_type common
		./make/lib/wayland.sh $build_type $build_backend

		case $build_backend in
			software|vulkan)
				./make/lib/elf.sh $build_type $build_backend
			;;

			egl)
				./make/lib/elf.sh $build_type opengl
			;;
		esac

		if [ "$build_example" != "none" ]; then
			./make/example/$build_example/wayland.sh $build_type $build_backend
		fi
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

		case $build_backend in
			software|vulkan)
				samu -f ./make/output/lib_elf_"$build_backend".ninja
			;;

			egl|glx)
				samu -f ./make/output/lib_elf_opengl.ninja
			;;
		esac

		if [ "$build_example" != "none" ]; then
			samu -f ./make/output/example_"$build_example"_x11_"$build_backend".ninja
		fi
	;;

	appkit)
		samu -f ./make/output/lib_macho.ninja
		samu -f ./make/output/lib_appkit_common.ninja
		samu -f ./make/output/lib_appkit_"$build_backend".ninja

		samu -f ./make/output/lib_macho.ninja headers
		samu -f ./make/output/lib_appkit_"$build_backend".ninja headers

		case $build_backend in
			software|vulkan)
				samu -f ./make/output/lib_macho_"$build_backend".ninja
			;;

			egl)
				samu -f ./make/output/lib_macho_opengl.ninja
			;;
		esac

		if [ "$build_example" != "none" ]; then
			samu -f ./make/output/example_"$build_example"_appkit_"$build_backend".ninja
		fi
	;;

	win)
		ninja -f ./make/output/lib_pe.ninja
		ninja -f ./make/output/lib_win_common.ninja
		ninja -f ./make/output/lib_win_"$build_backend".ninja

		ninja -f ./make/output/lib_pe.ninja headers
		ninja -f ./make/output/lib_win_"$build_backend".ninja headers

		case $build_backend in
			software|vulkan)
				ninja -f ./make/output/lib_pe_"$build_backend".ninja
			;;

			wgl)
				ninja -f ./make/output/lib_pe_opengl.ninja
			;;
		esac

		if [ "$build_example" != "none" ]; then
			ninja -f ./make/output/example_"$build_example"_win_"$build_backend".ninja
		fi
	;;

	wayland)
		samu -f ./make/output/lib_elf.ninja
		samu -f ./make/output/lib_wayland_common.ninja
		samu -f ./make/output/lib_wayland_"$build_backend".ninja

		samu -f ./make/output/lib_elf.ninja headers
		samu -f ./make/output/lib_wayland_"$build_backend".ninja headers

		case $build_backend in
			software|vulkan)
				samu -f ./make/output/lib_elf_"$build_backend".ninja
			;;

			egl)
				samu -f ./make/output/lib_elf_opengl.ninja
			;;
		esac

		if [ "$build_example" != "none" ]; then
			samu -f ./make/output/example_"$build_example"_wayland_"$build_backend".ninja
		fi
	;;

	*)
		echo "invalid platform: $build_platform"
		exit 1
	;;
esac
