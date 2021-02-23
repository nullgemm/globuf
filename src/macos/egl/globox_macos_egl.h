#ifndef H_GLOBOX_MACOS_EGL
#define H_GLOBOX_MACOS_EGL

#include <objc/objc.h>
#include <EGL/egl.h>

struct globox_macos_egl
{
	EGLDisplay globox_egl_display;
	EGLContext globox_egl_context;
	EGLSurface globox_egl_surface;
	EGLConfig globox_egl_config;
	EGLint globox_egl_config_size;

	id globox_egl_layer;
};

#endif
