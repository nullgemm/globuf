#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

mkdir -p res/egl_headers/GL
mkdir -p res/egl_headers/GLES2
mkdir -p res/egl_headers/EGL
mkdir -p res/egl_headers/KHR
curl -L "https://www.khronos.org/registry/OpenGL/api/GL/wglext.h" -o \
res/egl_headers/GL/wglext.h
curl -L "https://www.khronos.org/registry/OpenGL/api/GLES2/gl2.h" -o \
res/egl_headers/GLES2/gl2.h
curl -L "https://www.khronos.org/registry/OpenGL/api/GLES2/gl2platform.h" -o \
res/egl_headers/GLES2/gl2platform.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/egl.h" -o \
res/egl_headers/EGL/egl.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/eglext.h" -o \
res/egl_headers/EGL/eglext.h
curl -L "https://www.khronos.org/registry/EGL/api/EGL/eglplatform.h" -o \
res/egl_headers/EGL/eglplatform.h
curl -L "https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h" -o \
res/egl_headers/KHR/khrplatform.h
