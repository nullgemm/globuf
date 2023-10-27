#include "include/globox.h"
#include "include/globox_opengl.h"
#include "common/globox_private.h"
#include <stdint.h>

void globox_init_opengl(
	struct globox* context,
	struct globox_config_opengl* config,
	struct globox_error_info* error)
{
	struct globox_calls_opengl* opengl = context->backend_callbacks.data;

	opengl->init(
		context,
		config,
		error);
}
