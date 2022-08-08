#ifndef H_GLOBOX_SOFTWARE
#define H_GLOBOX_SOFTWARE

#include "globox.h"

#include <stdint.h>

struct globox_update_software
{
	uint32_t* buf;

	int x;
	int y;

	unsigned width;
	unsigned height;
};

uint32_t* globox_buffer_alloc_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error);

void globox_buffer_free_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error);

#endif
