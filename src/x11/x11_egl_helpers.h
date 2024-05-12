#ifndef H_GLOBOX_INTERNAL_X11_EGL_HELPERS
#define H_GLOBOX_INTERNAL_X11_EGL_HELPERS

#include "include/globox.h"
#include "include/globox_opengl.h"
#include "x11/x11_common.h"
#include <EGL/egl.h>

struct x11_egl_backend
{
	struct x11_platform platform;
	struct globox_config_opengl* config;

	EGLContext egl;
	EGLDisplay display;
	EGLSurface surface;
	EGLConfig attr_config;
};

bool x11_helpers_egl_ext_support(
	const char *list,
	const char *extension);

void x11_helpers_egl_bind(
	struct globox* context,
	struct globox_error_info* error);

#endif
