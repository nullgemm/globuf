#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_common.h"
#include "x11/x11_common_helpers.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_errors.h>

void* x11_helpers_render_loop(void* data)
{
	struct x11_thread_render_loop_data* thread_render_loop_data = data;

	struct globox* context = thread_render_loop_data->globox;
	struct x11_platform* platform = thread_render_loop_data->platform;
	struct globox_error_info* error = thread_render_loop_data->error;

	while (platform->closed == false)
	{
		// handle xsync
		// lock xsync mutex
		int posix_error = pthread_mutex_lock(&(platform->mutex_xsync));

		if (posix_error != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			break;
		}

		if (platform->xsync_configure == true)
		{
			// save the current xsync value in the xsync counter
			xcb_void_cookie_t cookie =
				xcb_sync_set_counter(
					platform->conn,
					platform->xsync_counter,
					platform->xsync_value);

			xcb_generic_error_t* xcb_error =
				xcb_request_check(
					platform->conn,
					cookie);

			if (xcb_error != NULL)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_X11_SYNC_COUNTER_SET);
				break;
			}

			// remember to tell the window manager we finished rendering
			platform->xsync_request = true;

			// lock main mutex
			posix_error = pthread_mutex_lock(&(platform->mutex_main));

			if (posix_error != 0)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
				break;
			}

			// save accessible size values
			context->feature_size->width = platform->xsync_width;
			context->feature_size->height = platform->xsync_height;

			// unlock main mutex
			posix_error = pthread_mutex_unlock(&(platform->mutex_main));

			if (posix_error != 0)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
				break;
			}
		}

		// unlock xsync mutex
		posix_error = pthread_mutex_unlock(&(platform->mutex_xsync));

		if (posix_error != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			break;
		}

		// run developer callback
		context->render_callback.callback(context->render_callback.data);

		// lock xsync mutex
		posix_error = pthread_mutex_lock(&(platform->mutex_xsync));

		if (posix_error != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			break;
		}

		// tell the window manager the resize operation
		// associated with the current xsync counter completed
		if (platform->xsync_request == true)
		{
			xcb_void_cookie_t cookie =
				xcb_change_property_checked(
					platform->conn,
					XCB_PROP_MODE_REPLACE,
					platform->win,
					platform->atoms[X11_ATOM_SYNC_REQUEST_COUNTER],
					XCB_ATOM_CARDINAL,
					32,
					1,
					&(platform->xsync_counter));

			xcb_generic_error_t* xcb_error =
				xcb_request_check(
					platform->conn,
					cookie);

			// reset sync status
			platform->xsync_request = false;

			if (xcb_error != NULL)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
				break;
			}
		}

		// unlock xsync mutex
		posix_error = pthread_mutex_unlock(&(platform->mutex_xsync));

		if (posix_error != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			break;
		}
	}

	pthread_exit(NULL);
	return NULL;
}

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
			break;
		}

		// run developer callback
		context->event_callbacks.handler(
			context->event_callbacks.data,
			event);

		free(event);
	}

	pthread_exit(NULL);
	return NULL;
}

void x11_helpers_features_init(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error)
{
	for (size_t i = 0; i < count; ++i)
	{
		switch (configs[i].feature)
		{
			case GLOBOX_FEATURE_STATE:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_state) =
						*((struct globox_feature_state*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				if (configs[i].config != NULL)
				{
					struct globox_feature_title* tmp = configs[i].config;
					context->feature_title->title = strdup(tmp->title);
				}

				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				if (configs[i].config != NULL)
				{
					struct globox_feature_icon* tmp = configs[i].config;
					context->feature_icon->pixmap = malloc(tmp->len * 4);

					if (context->feature_icon->pixmap != NULL)
					{
						memcpy(context->feature_icon->pixmap, tmp->pixmap, tmp->len * 4);
						context->feature_icon->len = tmp->len;
					}
					else
					{
						context->feature_icon->len = 0;
					}
				}

				break;
			}
			case GLOBOX_FEATURE_SIZE:
			{
				// handled directly in xcb's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_size) =
						*((struct globox_feature_size*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_POS:
			{
				// handled directly in xcb's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_pos) =
						*((struct globox_feature_pos*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_frame) =
						*((struct globox_feature_frame*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				// handled directly in xcb's window creation code for transparency,
				// but some more configuration has to take place afterwards for blur
				if (configs[i].config != NULL)
				{
					*(context->feature_background) =
						*((struct globox_feature_background*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_VSYNC:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_vsync) =
						*((struct globox_feature_vsync*)
							configs[i].config);
				}

				break;
			}
			default:
			{
				globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_INVALID);
				return;
			}
		}
	}
}

void x11_helpers_set_interaction(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	uint32_t table[] =
	{
		[GLOBOX_INTERACTION_STOP] = XCB_EWMH_WM_MOVERESIZE_CANCEL,
		[GLOBOX_INTERACTION_MOVE] = XCB_EWMH_WM_MOVERESIZE_MOVE,
		[GLOBOX_INTERACTION_N]    = XCB_EWMH_WM_MOVERESIZE_SIZE_TOP,
		[GLOBOX_INTERACTION_NW]   = XCB_EWMH_WM_MOVERESIZE_SIZE_TOPLEFT,
		[GLOBOX_INTERACTION_W]    = XCB_EWMH_WM_MOVERESIZE_SIZE_LEFT,
		[GLOBOX_INTERACTION_SW]   = XCB_EWMH_WM_MOVERESIZE_SIZE_BOTTOMLEFT,
		[GLOBOX_INTERACTION_S]    = XCB_EWMH_WM_MOVERESIZE_SIZE_BOTTOM,
		[GLOBOX_INTERACTION_SE]   = XCB_EWMH_WM_MOVERESIZE_SIZE_BOTTOMRIGHT,
		[GLOBOX_INTERACTION_E]    = XCB_EWMH_WM_MOVERESIZE_SIZE_RIGHT,
		[GLOBOX_INTERACTION_NE]   = XCB_EWMH_WM_MOVERESIZE_SIZE_TOPRIGHT,
	};

	uint32_t info[5] =
	{
		platform->saved_mouse_press_x,
		platform->saved_mouse_press_y,
		table[context->feature_interaction->action],
		platform->saved_mouse_press_button,
		1,
	};

	xcb_void_cookie_t cookie =
		xcb_change_property_checked(
			platform->conn,
			XCB_PROP_MODE_REPLACE,
			platform->win,
			platform->atoms[X11_ATOM_MOVERESIZE],
			XCB_ATOM_CARDINAL,
			32,
			5,
			info);

	xcb_generic_error_t* xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		return;
	}

	globox_error_ok(error);
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
		return;
	}

	globox_error_ok(error);
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
void x11_helpers_set_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
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
				return;
			}

			set_state_atoms(context, platform, 0, 0, 0, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
				return;
			}

			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			set_state_atoms(context, platform, 0, 0, 0, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
				return;
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
				globox_error_throw(context, error, GLOBOX_ERROR_X11_WIN_UNMAP);
				return;
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
				return;
			}

			set_state_atoms(context, platform, 1, 1, 0, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
				return;
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
				return;
			}

			set_state_atoms(context, platform, 0, 0, 1, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
				return;
			}

			break;
		}
		default:
		{
			globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_STATE_INVALID);
			return;
		}
	}

	// error is always set in the switch so we don't need to set it to "ok" here
}

void x11_helpers_set_title(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	xcb_void_cookie_t cookie =
		xcb_change_property_checked(
			platform->conn,
			XCB_PROP_MODE_REPLACE,
			platform->win,
			XCB_ATOM_WM_NAME,
			XCB_ATOM_STRING,
			8,
			strlen(context->feature_title->title) + 1,
			context->feature_title->title);

	xcb_generic_error_t* xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		return;
	}

	globox_error_ok(error);
}

void x11_helpers_set_icon(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	xcb_void_cookie_t cookie =
		xcb_change_property_checked(
			platform->conn,
			XCB_PROP_MODE_REPLACE,
			platform->win,
			platform->atoms[X11_ATOM_ICON],
			XCB_ATOM_CARDINAL,
			32,
			context->feature_icon->len,
			context->feature_icon->pixmap);

	xcb_generic_error_t* xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		return;
	}

	// flush
	int error_flush = xcb_flush(platform->conn);

	if (error_flush <= 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_FLUSH);
		return;
	}

	globox_error_ok(error);
}

void x11_helpers_set_frame(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	uint32_t flags;

	if (context->feature_frame->frame == false)
	{
		flags = 2;
	}
	else
	{
		flags = 0;
	}

	uint32_t motif_hints[5] =
	{
		flags, // flags
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
		return;
	}

	globox_error_ok(error);
}

void x11_helpers_set_background(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	if ((platform->atoms[X11_ATOM_BLUR_KDE] == XCB_ATOM_NONE)
		&& (platform->atoms[X11_ATOM_BLUR_DEEPIN] == XCB_ATOM_NONE))
	{
		globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_UNAVAILABLE);
		return;
	}

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
		return;
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
		return;
	}

	globox_error_ok(error);
}

void x11_helpers_set_vsync(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	// TODO setter

	globox_error_ok(error);
}

enum globox_event x11_helpers_get_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	xcb_get_property_cookie_t cookie =
		xcb_get_property(
			platform->conn,
			0,
			platform->win,
			platform->atoms[X11_ATOM_STATE],
			XCB_ATOM_ATOM,
			0,
			32);

	xcb_generic_error_t* xcb_error;
	enum globox_event event = GLOBOX_EVENT_INVALID;

	xcb_get_property_reply_t* reply =
		xcb_get_property_reply(
			platform->conn,
			cookie,
			&xcb_error);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		free(reply);
		return event;
	}

	xcb_atom_t* value = (xcb_atom_t*) xcb_get_property_value(reply);

	if (value == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		free(reply);
		return event;
	}

	if (*value == platform->atoms[X11_ATOM_STATE_FULLSCREEN])
	{
		context->feature_state->state = GLOBOX_STATE_FULLSCREEN;
		event = GLOBOX_EVENT_FULLSCREEN;
	}
	else if ((*value == platform->atoms[X11_ATOM_STATE_MAXIMIZED_HORIZONTAL])
		|| (*value == platform->atoms[X11_ATOM_STATE_MAXIMIZED_VERTICAL]))
	{
		context->feature_state->state = GLOBOX_STATE_MAXIMIZED;
		event = GLOBOX_EVENT_MAXIMIZED;
	}
	else if (*value == platform->atoms[X11_ATOM_STATE_HIDDEN])
	{
		context->feature_state->state = GLOBOX_STATE_MINIMIZED;
		event = GLOBOX_EVENT_MINIMIZED;
	}
	else
	{
		context->feature_state->state = GLOBOX_STATE_REGULAR;
		event = GLOBOX_EVENT_RESTORED;
	}

	free(reply);
	return event;
}

void x11_helpers_get_title(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	xcb_get_property_cookie_t cookie =
		xcb_get_property(
			platform->conn,
			0,
			platform->win,
			XCB_ATOM_WM_NAME,
			XCB_ATOM_STRING,
			0,
			32);

	xcb_generic_error_t* xcb_error;

	xcb_get_property_reply_t* reply =
		xcb_get_property_reply(
			platform->conn,
			cookie,
			&xcb_error);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		return;
	}

	char* value = (char*) xcb_get_property_value(reply);

	if (value == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		free(reply);
		return;
	}

	if (context->feature_title->title != NULL)
	{
		free((void*) context->feature_title->title);
	}

	context->feature_title->title = strdup(value);
	free(reply);
}

void x11_helpers_xcb_error_log(
	struct globox* context,
	struct x11_platform* platform,
	xcb_generic_error_t* error)
{
	xcb_errors_context_t* errors_context;
	int posix_error = xcb_errors_context_new(platform->conn, &errors_context);

	if (posix_error != 0)
	{
		fprintf(stderr, "could not allocate the xcb errors context\n");
		return;
	}

	const char* error_name;
	const char* extension_name;
	const char* minor_code_name;
	const char* major_code_name;

	error_name =
		xcb_errors_get_name_for_error(
			errors_context,
			error->error_code,
			&extension_name);

	minor_code_name =
		xcb_errors_get_name_for_minor_code(
			errors_context,
			error->major_code,
			error->minor_code);

	major_code_name =
		xcb_errors_get_name_for_major_code(
			errors_context,
			error->major_code);

	if (extension_name == NULL)
	{
		extension_name = "none (no extension or couldn't find a name for it)";
	}

	if (minor_code_name == NULL)
	{
		minor_code_name = "none (couldn't find a name for this minor code)";
	}

	fprintf(
		stderr,
		"# XCB Error Report\n"
		"Error Name: %s\n"
		"Extension Name: %s\n"
		"Minor Code Name: %s\n"
		"Major Code Name: %s\n"
		"Sequence: %u\n"
		"Resource ID: %u\n",
		error_name,
		extension_name,
		minor_code_name,
		major_code_name,
		error->sequence,
		error->resource_id);

	xcb_errors_context_free(errors_context);
}
