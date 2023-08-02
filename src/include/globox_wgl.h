#ifndef H_GLOBOX_WGL
#define H_GLOBOX_WGL

#include "globox.h"

// common to all backends supporting WGL rendering

struct globox_config_wgl
{
	int major_version;
	int minor_version;
	int* attributes;
};

#endif
