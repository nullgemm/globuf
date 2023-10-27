#ifndef H_GLOBOX_APPKIT_EGL
#define H_GLOBOX_APPKIT_EGL

#include "globox.h"
#include "globox_appkit.h"
#include "globox_opengl.h"

void globox_prepare_init_appkit_egl(
	struct globox_config_backend* config,
	struct globox_error_info* error);

double globox_appkit_egl_get_scale(
	struct globox* context,
	struct globox_error_info* error);

#endif
