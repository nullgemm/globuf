#ifndef H_GLOBOX_INTERNAL_WAYLAND_EGL_HELPERS
#define H_GLOBOX_INTERNAL_WAYLAND_EGL_HELPERS

#include "include/globox.h"
#include "include/globox_opengl.h"
#include "wayland/wayland_common.h"

#include <EGL/egl.h>
#include <wayland-egl.h>

struct wayland_egl_backend
{
	struct wayland_platform platform;
	struct globox_config_opengl* config;

	EGLContext egl;
	EGLDisplay display;
	EGLSurface surface;
	EGLConfig attr_config;
	EGLint attr_config_size;
	struct wl_egl_window* window;
};

void globox_wayland_helpers_egl_bind(
	struct globox* context,
	struct globox_error_info* error);

void globox_wayland_helpers_egl_toplevel_configure(
	void* data,
	struct xdg_toplevel* xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array* states);

#endif
