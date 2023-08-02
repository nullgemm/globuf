#ifndef H_GLOBOX_WIN_SOFTWARE
#define H_GLOBOX_WIN_SOFTWARE

#include "globox.h"
#include "globox_win.h"
#include "globox_software.h"

void globox_prepare_init_win_software(
	struct globox_config_backend* config,
	struct globox_error_info* error);

uint32_t* globox_buffer_alloc_win_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error);

void globox_buffer_free_win_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error);

#endif
