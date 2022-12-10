#ifndef H_GLOBOX_EGL
#define H_GLOBOX_EGL

#include "globox.h"

// common to all backends supporting EGL rendering

struct globox_config_egl
{
	int major_version;
	int minor_version;
	int* attributes;
};

#endif
