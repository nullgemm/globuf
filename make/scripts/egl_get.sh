#!/bin/bash

# get in the right folder
path="$(pwd)/$0"
folder=$(dirname "$path")
cd "$folder"/../.. || exit

# get EGL
mkdir -p res/egl/GL
mkdir -p res/egl/GLES2
mkdir -p res/egl/EGL
mkdir -p res/egl/KHR

curl -L "https://www.khronos.org/registry/OpenGL/api/GL/wglext.h" -o \
	res/egl/GL/wglext.h
curl -L "https://www.khronos.org/registry/OpenGL/api/GLES2/gl2.h" -o \
	res/egl/GLES2/gl2.h
curl -L "https://www.khronos.org/registry/OpenGL/api/GLES2/gl2platform.h" -o \
	res/egl/GLES2/gl2platform.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/egl.h" -o \
	res/egl/EGL/egl.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/eglext.h" -o \
	res/egl/EGL/eglext.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/eglplatform.h" -o \
	res/egl/EGL/eglplatform.h
curl -L "https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h" -o \
	res/egl/KHR/khrplatform.h
