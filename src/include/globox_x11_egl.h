#ifndef H_GLOBOX_X11_EGL
#define H_GLOBOX_X11_EGL

#include "globox.h"
#include "globox_x11.h"
#include "globox_egl.h"

void globox_prepare_init_x11_egl(
	struct globox_config_backend* config,
	struct globox_error_info* error);

void globox_init_x11_egl(
	struct globox* context,
	struct globox_config_egl* config,
	struct globox_error_info* error);

#endif
