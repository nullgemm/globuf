#ifndef H_GLOBOX_INTERNAL_WAYLAND_EGL_HELPERS
#define H_GLOBOX_INTERNAL_WAYLAND_EGL_HELPERS

#include "include/globox.h"
#include "include/globox_egl.h"
#include "wayland/wayland_common.h"
#include <EGL/egl.h>

struct wayland_egl_backend
{
	struct wayland_platform platform;
	struct globox_config_egl* config;

	EGLContext egl;
	EGLDisplay display;
	EGLSurface surface;
	EGLConfig attr_config;
	EGLint attr_config_size;
};

void globox_wayland_helpers_egl_bind(
	struct globox* context,
	struct globox_error_info* error);

#endif
