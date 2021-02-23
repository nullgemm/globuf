#ifndef H_GLOBOX_WAYLAND_EGL
#define H_GLOBOX_WAYLAND_EGL

#include <wayland-client.h>
#include <EGL/egl.h>

struct globox_wayland_egl
{
	EGLDisplay globox_egl_display;
	EGLContext globox_egl_context;
	EGLSurface globox_egl_surface;
	EGLConfig globox_egl_config;
	EGLint globox_egl_config_size;

	struct wl_egl_window* globox_egl_window;
};

#endif
