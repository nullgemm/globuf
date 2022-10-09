#ifndef H_GLOBOX_GLX
#define H_GLOBOX_GLX

#include "globox.h"

struct globox_config_glx
{
	int major_version;
	int minor_version;
};

void globox_init_glx(
	struct globox* context,
	struct globox_config_glx* config,
	struct globox_error_info* error);

#endif
