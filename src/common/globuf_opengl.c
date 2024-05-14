#include "include/globuf.h"
#include "include/globuf_opengl.h"
#include "common/globuf_private.h"
#include <stdint.h>

void globuf_init_opengl(
	struct globuf* context,
	struct globuf_config_opengl* config,
	struct globuf_error_info* error)
{
	struct globuf_calls_opengl* opengl = context->backend_callbacks.data;

	opengl->init(
		context,
		config,
		error);
}
