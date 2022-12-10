#ifndef H_GLOBOX_GLX
#define H_GLOBOX_GLX

#include "globox.h"

// common to all backends supporting GLX rendering

struct globox_config_glx
{
	int major_version;
	int minor_version;
	int* attributes;
};

#endif
