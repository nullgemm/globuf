#ifndef H_GLOBOX_INTERNAL_X11_HELPERS
#define H_GLOBOX_INTERNAL_X11_HELPERS

#include "include/globox.h"
#include "x11/x11_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcb/xcb.h>

void x11_helpers_set_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

#endif
