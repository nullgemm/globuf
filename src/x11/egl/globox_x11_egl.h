#ifndef H_GLOBOX_X11_EGL
#define H_GLOBOX_X11_EGL

#include "globox.h"
#include "x11/globox_x11.h"
// context includes
#include <EGL/egl.h>

struct globox_x11_egl
{
	// egl init
	EGLDisplay globox_egl_display;
	EGLContext globox_egl_context;
	EGLSurface globox_egl_surface;

	// framebuffer info
	EGLConfig globox_egl_config;
	EGLint globox_egl_config_size;
};

#endif