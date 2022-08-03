#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_common.h"
#include "x11/x11_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>

void* x11_helpers_event_loop(void* data)
{
	struct x11_thread_event_loop_data* thread_event_loop_data = data;

	struct globox* context = thread_event_loop_data->globox;
	struct x11_platform* platform = thread_event_loop_data->platform;
	struct globox_error_info* error = thread_event_loop_data->error;

	xcb_generic_event_t* event;

	while (platform->closed == false)
	{
		// block until we get an event
		event = xcb_wait_for_event(platform->conn);

		// IO error
		if (event == NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_EVENT_WAIT);
			return NULL;
		}

		// run developer callback
		context->event_callbacks.handler(
			context->event_callbacks.data,
			event);

		free(event);
	}

	return NULL;
}

void x11_helpers_features_init(
	struct globox* context,
	struct x11_platform* platform,
	void** features)
{
	if ((context->feature_state != NULL)
		&& (features[GLOBOX_FEATURE_STATE] != NULL))
	{
		*(context->feature_state) =
			*((struct globox_feature_state*)
				features[GLOBOX_FEATURE_STATE]);
	}

	if ((context->feature_title != NULL)
		&& (features[GLOBOX_FEATURE_TITLE] != NULL))
	{
		*(context->feature_title) =
			*((struct globox_feature_title*)
				features[GLOBOX_FEATURE_TITLE]);
	}

	if ((context->feature_icon != NULL)
		&& (features[GLOBOX_FEATURE_ICON] != NULL))
	{
		*(context->feature_icon) =
			*((struct globox_feature_icon*)
				features[GLOBOX_FEATURE_ICON]);
	}

	// handled directly in xcb's window creation code
	if ((context->feature_size!= NULL)
		&& (features[GLOBOX_FEATURE_SIZE] != NULL))
	{
		*(context->feature_size) =
			*((struct globox_feature_size*)
				features[GLOBOX_FEATURE_SIZE]);
	}

	// handled directly in xcb's window creation code
	if ((context->feature_pos != NULL)
		&& (features[GLOBOX_FEATURE_POS] != NULL))
	{
		*(context->feature_pos) =
			*((struct globox_feature_pos*)
				features[GLOBOX_FEATURE_POS]);
	}

	if ((context->feature_frame != NULL)
		&& (features[GLOBOX_FEATURE_FRAME] != NULL))
	{
		*(context->feature_frame) =
			*((struct globox_feature_frame*)
				features[GLOBOX_FEATURE_FRAME]);
	}

	// handled directly in xcb's window creation code for transparency,
	// but some more configuration has to take place afterwards for blur
	if ((context->feature_background != NULL)
		&& (features[GLOBOX_FEATURE_BACKGROUND] != NULL))
	{
		*(context->feature_background) =
			*((struct globox_feature_background*)
				features[GLOBOX_FEATURE_BACKGROUND]);
	}

	if ((context->feature_vsync_callback != NULL)
		&& (features[GLOBOX_FEATURE_VSYNC_CALLBACK] != NULL))
	{
		*(context->feature_vsync_callback) =
			*((struct globox_feature_vsync_callback*)
				features[GLOBOX_FEATURE_VSYNC_CALLBACK]);
	}
}

void set_state_event(
	struct globox* context,
	struct x11_platform* platform,
	xcb_atom_t atom,
	uint32_t action,
	struct globox_error_info* error)
{
	xcb_client_message_event_t event =
	{
		.response_type = XCB_CLIENT_MESSAGE,
		.type = platform->atoms[X11_ATOM_STATE],
		.format = 32,
		.window = platform->win,
		.data =
		{
			.data32 =
			{
				action,
				atom,
				XCB_ATOM_NONE,
				0,
				0,
			},
		},
	};

	uint32_t mask =
		XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
		| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;

	xcb_void_cookie_t cookie =
		xcb_send_event_checked(
			platform->conn,
			1,
			platform->win,
			mask,
			(const char*)(&event));

	xcb_generic_error_t* xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EVENT_SEND);
	}
}

void set_state_atoms(
	struct globox* context,
	struct x11_platform* platform,
	uint32_t action_maximized_horizontal,
	uint32_t action_maximized_vertical,
	uint32_t action_fullscreen,
	struct globox_error_info* error)
{
	set_state_event(
		context,
		platform,
		platform->atoms[X11_ATOM_STATE_MAXIMIZED_HORIZONTAL],
		action_maximized_horizontal,
		error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	set_state_event(
		context,
		platform,
		platform->atoms[X11_ATOM_STATE_MAXIMIZED_VERTICAL],
		action_maximized_vertical,
		error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	set_state_event(
		context,
		platform,
		platform->atoms[X11_ATOM_STATE_FULLSCREEN],
		action_fullscreen,
		error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}
}

// there is a bug in ewmh that prevents fullscreen from working properly
// since keeping xcb-ewmh around only for initialization would be kind
// of silly we removed the dependency and used raw xcb all the way
bool x11_helpers_set_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	if (context->feature_state == NULL)
	{
		return true;
	}

	xcb_void_cookie_t cookie;
	xcb_generic_error_t* xcb_error;

	switch (context->feature_state->state)
	{
		case GLOBOX_STATE_REGULAR:
		{
			cookie =
				xcb_map_window_checked(
					platform->conn,
					platform->win);

			xcb_error =
				xcb_request_check(
					platform->conn,
					cookie);

			if (xcb_error != NULL)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_X11_WIN_MAP);
				return false;
			}

			set_state_atoms(context, platform, 0, 0, 0, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
				return false;
			}

			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			set_state_atoms(context, platform, 0, 0, 0, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
				return false;
			}

			cookie =
				xcb_unmap_window_checked(
					platform->conn,
					platform->win);

			xcb_error =
				xcb_request_check(
					platform->conn,
					cookie);

			if (xcb_error != NULL)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_X11_WIN_MAP);
				return false;
			}

			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			cookie =
				xcb_map_window_checked(
					platform->conn,
					platform->win);

			xcb_error =
				xcb_request_check(
					platform->conn,
					cookie);

			if (xcb_error != NULL)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_X11_WIN_MAP);
				return false;
			}

			set_state_atoms(context, platform, 1, 1, 0, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
				return false;
			}

			break;
		}
		case GLOBOX_STATE_FULLSCREEN:
		{
			cookie =
				xcb_map_window_checked(
					platform->conn,
					platform->win);

			xcb_error =
				xcb_request_check(
					platform->conn,
					cookie);

			if (xcb_error != NULL)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_X11_WIN_MAP);
				return false;
			}

			set_state_atoms(context, platform, 0, 0, 1, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
				return false;
			}

			break;
		}
		default:
		{
			return false;
		}
	}

	return true;
}

bool x11_helpers_set_title(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	// TODO
	return true;
}

bool x11_helpers_set_icon(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	// TODO
	return true;
}

bool x11_helpers_set_frame(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	if ((context->feature_frame == NULL)
		|| (context->feature_frame->frame == true))
	{
		return true;
	}

	uint32_t motif_hints[5] =
	{
		2, // flags
		0, // functions
		0, // decorations
		0, // input_mode
		0, // status
	};

	xcb_void_cookie_t cookie =
		xcb_change_property(
			platform->conn,
			XCB_PROP_MODE_REPLACE,
			platform->win,
			platform->atoms[X11_ATOM_HINTS_MOTIF],
			platform->atoms[X11_ATOM_HINTS_MOTIF],
			32,
			5,
			motif_hints);

	xcb_generic_error_t* xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		return false;
	}

	return true;
}

bool x11_helpers_set_background(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	if ((context->feature_background == NULL)
		|| (context->feature_background->background != GLOBOX_BACKGROUND_BLURRED))
	{
		return true;
	}

	if ((platform->atoms[X11_ATOM_BLUR_KDE] != XCB_ATOM_NONE)
		|| (platform->atoms[X11_ATOM_BLUR_DEEPIN] != XCB_ATOM_NONE))
	{
		// kde blur
		xcb_void_cookie_t cookie =
			xcb_change_property(
				platform->conn,
				XCB_PROP_MODE_REPLACE,
				platform->win,
				platform->atoms[X11_ATOM_BLUR_KDE],
				XCB_ATOM_CARDINAL,
				32,
				0,
				NULL);

		xcb_generic_error_t* xcb_error =
			xcb_request_check(
				platform->conn,
				cookie);

		if (xcb_error != NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
			return false;
		}

		// deepin blur
		cookie =
			xcb_change_property(
				platform->conn,
				XCB_PROP_MODE_REPLACE,
				platform->win,
				platform->atoms[X11_ATOM_BLUR_DEEPIN],
				XCB_ATOM_CARDINAL,
				32,
				0,
				NULL);

		xcb_error =
			xcb_request_check(
				platform->conn,
				cookie);

		if (xcb_error != NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool x11_helpers_set_vsync_callback(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	// TODO
	return true;
}
