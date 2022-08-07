#ifndef H_GLOBOX_INTERNAL_X11_SOFTWARE_HELPERS
#define H_GLOBOX_INTERNAL_X11_SOFTWARE_HELPERS

#include "include/globox.h"

void x11_helpers_visual_transparent(
	struct globox* context,
	struct globox_error_info* error);

void x11_helpers_visual_opaque(
	struct globox* context,
	struct globox_error_info* error);

void x11_helpers_shm_create(
	struct globox* context,
	struct globox_error_info* error);

#endif
