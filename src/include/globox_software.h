#ifndef H_GLOBOX_SOFTWARE
#define H_GLOBOX_SOFTWARE

#include "globox.h"

#include <stdint.h>

// common to all backends supporting software rendering

struct globox_update_software
{
	uint32_t* buf;

	int x;
	int y;

	unsigned width;
	unsigned height;
};

struct globox_calls_software
{
	uint32_t* (*alloc)(
		struct globox* context,
		unsigned width,
		unsigned height,
		struct globox_error_info* error);

	void (*free)(
		struct globox* context,
		uint32_t* buffer,
		struct globox_error_info* error);
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
