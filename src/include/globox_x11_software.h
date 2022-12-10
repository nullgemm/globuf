#ifndef H_GLOBOX_X11_SOFTWARE
#define H_GLOBOX_X11_SOFTWARE

#include "globox_x11.h"
#include "globox_software.h"

void globox_prepare_init_x11_software(
	struct globox_config_backend* config,
	struct globox_error_info* error);

uint32_t* globox_buffer_alloc_x11_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error);

void globox_buffer_free_x11_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error);

#endif
