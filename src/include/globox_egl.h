#ifndef H_GLOBOX_EGL
#define H_GLOBOX_EGL

#include "globox.h"

struct globox_config_egl
{
	int major_version;
	int minor_version;
};

void globox_init_egl(
	struct globox* context,
	struct globox_config_egl* config);

#endif