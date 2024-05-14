#ifndef H_GLOBUF_OPENGL
#define H_GLOBUF_OPENGL

#include "globuf.h"

// common to all backends supporting OpenGL rendering

struct globuf_config_opengl
{
	int major_version;
	int minor_version;
	int* attributes;
};

struct globuf_calls_opengl
{
	void (*init)(
		struct globuf* context,
		struct globuf_config_opengl* config,
		struct globuf_error_info* error);
};

void globuf_init_opengl(
	struct globuf* context,
	struct globuf_config_opengl* config,
	struct globuf_error_info* error);

#endif
