#ifndef H_GLOBOX_INTERNAL_APPKIT_EGL_HELPERS
#define H_GLOBOX_INTERNAL_APPKIT_EGL_HELPERS

#include "include/globox.h"
#include "include/globox_opengl.h"
#include "appkit/appkit_egl.h"
#include "appkit/appkit_common.h"
#include <EGL/egl.h>

struct appkit_egl_backend
{
	struct appkit_platform platform;
	struct globox_config_opengl* config;

	EGLContext egl;
	EGLDisplay display;
	EGLSurface surface;
	EGLConfig attr_config;
	EGLint attr_config_size;
};

void appkit_helpers_egl_bind(
	struct globox* context,
	struct globox_error_info* error);

#endif
