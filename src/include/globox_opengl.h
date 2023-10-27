#ifndef H_GLOBOX_OPENGL
#define H_GLOBOX_OPENGL

#include "globox.h"

// common to all backends supporting OpenGL rendering

struct globox_config_opengl
{
	int major_version;
	int minor_version;
	int* attributes;
};

struct globox_calls_opengl
{
	void (*init)(
		struct globox* context,
		struct globox_config_opengl* config,
		struct globox_error_info* error);
};

void globox_init_opengl(
	struct globox* context,
	struct globox_config_opengl* config,
	struct globox_error_info* error);

#endif
