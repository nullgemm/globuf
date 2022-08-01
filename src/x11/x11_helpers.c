#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_common.h"
#include "x11/x11_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>

void x11_helpers_set_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
}
