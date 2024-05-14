#ifndef H_GLOBUF_INTERNAL_APPKIT_EGL_HELPERS
#define H_GLOBUF_INTERNAL_APPKIT_EGL_HELPERS

#include "include/globuf.h"
#include "include/globuf_opengl.h"
#include "appkit/appkit_egl.h"
#include "appkit/appkit_common.h"
#include <EGL/egl.h>

struct appkit_egl_backend
{
	struct appkit_platform platform;
	struct globuf_config_opengl* config;

	EGLContext egl;
	EGLDisplay display;
	EGLSurface surface;
	EGLConfig attr_config;
	EGLint attr_config_size;
};

void appkit_helpers_egl_bind(
	struct globuf* context,
	struct globuf_error_info* error);

#endif
