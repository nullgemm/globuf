#ifndef H_GLOBOX_EGL_SOFTWARE
#define H_GLOBOX_EGL_SOFTWARE

#include "globox.h"
#include "wayland/globox_wayland.h"

#include <EGL/egl.h>

struct globox_wayland_egl
{
	uint32_t globox_egl_buffer_width;
	uint32_t globox_egl_buffer_height;

	EGLDisplay globox_egl_display;
	EGLContext globox_egl_context;
	EGLSurface globox_egl_surface;
	EGLConfig globox_egl_config;
	EGLint globox_egl_config_size;

	struct wl_egl_window* globox_egl_window;
};

#endif
