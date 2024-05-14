#ifndef H_GLOBUF_APPKIT_EGL
#define H_GLOBUF_APPKIT_EGL

#include "globuf.h"
#include "globuf_appkit.h"
#include "globuf_opengl.h"

void globuf_prepare_init_appkit_egl(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);

double globuf_appkit_egl_get_scale(
	struct globuf* context,
	struct globuf_error_info* error);

#endif
