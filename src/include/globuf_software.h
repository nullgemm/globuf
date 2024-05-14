#ifndef H_GLOBUF_SOFTWARE
#define H_GLOBUF_SOFTWARE

#include "globuf.h"

#include <stdint.h>

// common to all backends supporting software rendering

struct globuf_update_software
{
	uint32_t* buf;

	int x;
	int y;

	unsigned width;
	unsigned height;
};

struct globuf_calls_software
{
	uint32_t* (*alloc)(
		struct globuf* context,
		unsigned width,
		unsigned height,
		struct globuf_error_info* error);

	void (*free)(
		struct globuf* context,
		uint32_t* buffer,
		struct globuf_error_info* error);
};

uint32_t* globuf_buffer_alloc_software(
	struct globuf* context,
	unsigned width,
	unsigned height,
	struct globuf_error_info* error);

void globuf_buffer_free_software(
	struct globuf* context,
	uint32_t* buffer,
	struct globuf_error_info* error);

#endif
