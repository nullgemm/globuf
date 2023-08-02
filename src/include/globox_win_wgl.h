#ifndef H_GLOBOX_WIN_WGL
#define H_GLOBOX_WIN_WGL

#include "globox.h"
#include "globox_win.h"
#include "globox_wgl.h"

void globox_prepare_init_win_wgl(
	struct globox_config_backend* config,
	struct globox_error_info* error);

void globox_init_win_wgl(
	struct globox* context,
	struct globox_config_wgl* config,
	struct globox_error_info* error);

#endif
