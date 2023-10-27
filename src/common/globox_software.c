#include "include/globox.h"
#include "include/globox_software.h"
#include "common/globox_private.h"
#include <stdint.h>

uint32_t* globox_buffer_alloc_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error)
{
	struct globox_calls_software* software = context->backend_callbacks.data;

	return software->alloc(context, width, height, error);
}

void globox_buffer_free_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error)
{
	struct globox_calls_software* software = context->backend_callbacks.data;

	software->free(context, buffer, error);
}
