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

#endif