#ifndef H_GLOBOX_WINDOWS_EGL
#define H_GLOBOX_WINDOWS_EGL

#include <EGL/egl.h>

struct globox_windows_egl
{
	EGLDisplay globox_egl_display;
	EGLContext globox_egl_context;
	EGLSurface globox_egl_surface;
	EGLConfig globox_egl_config;
	EGLint globox_egl_config_size;
};

#endif
