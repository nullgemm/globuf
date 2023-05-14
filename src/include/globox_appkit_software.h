#ifndef H_GLOBOX_APPKIT_SOFTWARE
#define H_GLOBOX_APPKIT_SOFTWARE

#include "globox.h"
#include "globox_appkit.h"
#include "globox_software.h"

void globox_prepare_init_appkit_software(
	struct globox_config_backend* config,
	struct globox_error_info* error);

uint32_t* globox_buffer_alloc_appkit_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error);

void globox_buffer_free_appkit_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error);

#endif
