#ifndef H_GLOBUF_INTERNAL_WAYLAND_EGL_HELPERS
#define H_GLOBUF_INTERNAL_WAYLAND_EGL_HELPERS

#include "include/globuf.h"
#include "include/globuf_opengl.h"
#include "wayland/wayland_common.h"

#include <EGL/egl.h>
#include <wayland-egl.h>

struct wayland_egl_backend
{
	struct wayland_platform platform;
	struct globuf_config_opengl* config;

	EGLContext egl;
	EGLDisplay display;
	EGLSurface surface;
	EGLConfig attr_config;
	EGLint attr_config_size;
	struct wl_egl_window* window;
};

void wayland_helpers_egl_bind(
	struct globuf* context,
	struct globuf_error_info* error);

void wayland_helpers_egl_toplevel_configure(
	void* data,
	struct xdg_toplevel* xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array* states);

#endif
