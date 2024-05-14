#ifndef H_GLOBUF_WAYLAND_EGL
#define H_GLOBUF_WAYLAND_EGL

#include "globuf.h"
#include "globuf_wayland.h"
#include "globuf_opengl.h"

void globuf_prepare_init_wayland_egl(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);

#endif
