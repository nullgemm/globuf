#ifndef H_GLOBOX_APPKIT_EGL
#define H_GLOBOX_APPKIT_EGL

#include "globox.h"
#include "globox_appkit.h"
#include "globox_egl.h"

void globox_prepare_init_appkit_egl(
	struct globox_config_backend* config,
	struct globox_error_info* error);

void globox_init_appkit_egl(
	struct globox* context,
	struct globox_config_egl* config,
	struct globox_error_info* error);

#endif
