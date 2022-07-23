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

	// get available atoms
	xcb_intern_atom_cookie_t cookie;
	xcb_intern_atom_reply_t* reply;
	xcb_generic_error_t* error;
	uint8_t replace;

	char* atom_names[X11_ATOM_COUNT] =
	{
		[X11_ATOM_STATE_MAXIMIZED_HORIZONTAL] =
			"_NET_WM_STATE_MAXIMIZED_HORZ",
		[X11_ATOM_STATE_MAXIMIZED_VERTICAL] =
			"_NET_WM_STATE_MAXIMIZED_VERT",
		[X11_ATOM_STATE_FULLSCREEN] =
			"_NET_WM_STATE_FULLSCREEN",
		[X11_ATOM_STATE_HIDDEN] =
			"_NET_WM_STATE_HIDDEN",
		[X11_ATOM_STATE] =
			"_NET_WM_STATE",
		[X11_ATOM_ICON] =
			"_NET_WM_ICON",
		[X11_ATOM_HINTS_MOTIF] =
			"_MOTIF_WM_HINTS",
		[X11_ATOM_BLUR_KDE] =
			"_KDE_NET_WM_BLUR_BEHIND_REGION",
		[X11_ATOM_BLUR_DEEPIN] =
			"_NET_WM_DEEPIN_BLUR_REGION_ROUNDED",
		[X11_ATOM_PROTOCOLS] =
			"WM_PROTOCOLS",
		[X11_ATOM_DELETE_WINDOW] =
			"WM_DELETE_WINDOW",
	};

	for (int i = 0; i < X11_ATOM_COUNT; ++i)
	{
		replace = (i == X11_ATOM_PROTOCOLS);

		cookie = xcb_intern_atom(
			platform->conn,
			replace,
			strlen(atom_names[i]),
			atom_names[i]);

		reply = xcb_intern_atom_reply(
			platform->conn,
			cookie,
			&error);

		if (error_atom != NULL)
		{
			globox_error_throw(context, GLOBOX_ERROR_X11_ATOM_GET);
			return;
		}

		platform->atoms[i] = reply->atom;
		free(reply_atom);
	}
}

// TODO implement setters
void globox_x11_common_set_interaction(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_interaction* config = request->config;
}

void globox_x11_common_set_state(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_state* config = request->config;
}

void globox_x11_common_set_title(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_title* config = request->config;
}

void globox_x11_common_set_icon(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_icon* config = request->config;
}

void globox_x11_common_set_size(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_size* config = request->config;
}

void globox_x11_common_set_pos(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_pos* config = request->config;
}

void globox_x11_common_set_frame(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_frame* config = request->config;
}

void globox_x11_common_set_background(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_background* config = request->config;
}

void globox_x11_common_set_vsync_callback(
	struct globox* context,
	struct globox_feature_request* request)
{
	struct globox_feature_vsync_callback* config = request->config;
}
