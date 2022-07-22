#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_common.h"

#include <stdlib.h>

void globox_x11_common_init(struct x11_platform* platform)
{
	// open a connection to the X server
	platform->conn = xcb_connect(NULL, &(platform->screen_id));
	int error = xcb_connection_has_error(platform->conn);

	if (error > 0)
	{
		xcb_disconnect(platform->conn);
		globox_error_throw(context, GLOBOX_ERROR_X11_CONN);
		return;
	}

	// get the screen obj from the id the dirty way (there is no other option)
	const struct xcb_setup_t* setup = xcb_get_setup(platform->conn);
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);

	for (int i = 0; i < platform->screen_id; ++i)
	{
		xcb_screen_next(&iter);
	}

	platform->screen_obj = iter.data;

	// get the root window from the screen object
	platform->root_win = platform->screen_obj->root;
}
