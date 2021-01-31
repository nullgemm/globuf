#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ..

mkdir -p inc/GL
mkdir -p inc/GLES2
mkdir -p inc/EGL
mkdir -p inc/KHR
curl -L "https://www.khronos.org/registry/OpenGL/api/GL/wglext.h" -o \
inc/GL/wglext.h
curl -L "https://www.khronos.org/registry/OpenGL/api/GLES2/gl2.h" -o \
inc/GLES2/gl2.h
curl -L "https://www.khronos.org/registry/OpenGL/api/GLES2/gl2platform.h" -o \
inc/GLES2/gl2platform.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/egl.h" -o \
inc/EGL/egl.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/eglext.h" -o \
inc/EGL/eglext.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/eglplatform.h" -o \
inc/EGL/eglplatform.h
curl -L "https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h" -o \
inc/KHR/khrplatform.h
