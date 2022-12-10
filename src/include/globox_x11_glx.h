#ifndef H_GLOBOX_X11_GLX
#define H_GLOBOX_X11_GLX

#include "globox.h"
#include "globox_x11.h"
#include "globox_glx.h"

void globox_prepare_init_x11_glx(
	struct globox_config_backend* config,
	struct globox_error_info* error);

void globox_init_x11_glx(
	struct globox* context,
	struct globox_config_glx* config,
	struct globox_error_info* error);

#endif
