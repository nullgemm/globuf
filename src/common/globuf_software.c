#include "include/globuf.h"
#include "include/globuf_software.h"
#include "common/globuf_private.h"
#include <stdint.h>

uint32_t* globuf_buffer_alloc_software(
	struct globuf* context,
	unsigned width,
	unsigned height,
	struct globuf_error_info* error)
{
	struct globuf_calls_software* software = context->backend_callbacks.data;

	return software->alloc(context, width, height, error);
}

void globuf_buffer_free_software(
	struct globuf* context,
	uint32_t* buffer,
	struct globuf_error_info* error)
{
	struct globuf_calls_software* software = context->backend_callbacks.data;

	software->free(context, buffer, error);
}
