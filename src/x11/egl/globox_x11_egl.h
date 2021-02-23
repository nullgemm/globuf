#ifndef H_GLOBOX_X11_EGL
#define H_GLOBOX_X11_EGL

#include <EGL/egl.h>

struct globox_x11_egl
{
	EGLDisplay globox_egl_display;
	EGLContext globox_egl_context;
	EGLSurface globox_egl_surface;
	EGLConfig globox_egl_config;
	EGLint globox_egl_config_size;
};

#endif
