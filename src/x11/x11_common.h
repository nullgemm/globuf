#ifndef H_GLOBOX_INTERNAL_X11_COMMON
#define H_GLOBOX_INTERNAL_X11_COMMON

#include "include/globox.h"

#include <xcb.h>

// # private helpers
struct x11_platform
{
	int screen_id;
	xcb_screen_t* screen_obj;
	xcb_window_t root_win;
};

void globox_x11_common_init(void** platform_data);

#endif
