#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "wayland/wayland_common.h"
#include "wayland/wayland_common_helpers.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// classic helpers

void* wayland_helpers_render_loop(void* data)
{
	struct wayland_thread_render_loop_data* thread_render_loop_data = data;

	struct globox* context = thread_render_loop_data->globox;
	struct wayland_platform* platform = thread_render_loop_data->platform;
	struct globox_error_info* error = thread_render_loop_data->error;

	// lock main mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return NULL;
	}

	bool closed = platform->closed;

	// unlock main mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return NULL;
	}

	// thread init callback
	if (platform->render_init_callback != NULL)
	{
		platform->render_init_callback(context, error);

		if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			return NULL;
		}
	}

	while (closed == false)
	{
		// run developer callback
		context->render_callback.callback(context->render_callback.data);

		// lock main mutex
		error_posix = pthread_mutex_lock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			break;
		}

		closed = platform->closed;

		// unlock main mutex
		error_posix = pthread_mutex_unlock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			break;
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void* wayland_helpers_event_loop(void* data)
{
	struct wayland_thread_event_loop_data* thread_event_loop_data = data;

	struct globox* context = thread_event_loop_data->globox;
	struct wayland_platform* platform = thread_event_loop_data->platform;
	struct globox_error_info* error = thread_event_loop_data->error;

	int error_posix;

	// thread init callback
	if (platform->event_init_callback != NULL)
	{
		platform->event_init_callback(context, error);

		if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			return NULL;
		}
	}

	while (platform->closed == false)
	{
		// block until we get an event
		error_posix = wl_display_dispatch(platform->display);

		// IO error
		if (error_posix == -1)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_EVENT_WAIT);
			break;
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void wayland_helpers_features_init(
	struct globox* context,
	struct wayland_platform* platform,
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
				// handled directly in the wayland window creation code
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
				// handled directly in the wayland window creation code
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
				// handled directly in the wayland window creation code for transparency,
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

// TODO HERE
// there is also a bug in ewmh that prevents interactive move & resize from
// working properly under certain desktop environments, so we implement
// everything for this feature as well
void wayland_helpers_handle_interaction(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	xcb_generic_error_t* error_xcb;

	// compute window changes
	switch (context->feature_interaction->action)
	{
		case GLOBOX_INTERACTION_MOVE:
		{
			platform->saved_window_geometry[0] += platform->saved_mouse_pos_x - platform->old_mouse_pos_x;
			platform->saved_window_geometry[1] += platform->saved_mouse_pos_y - platform->old_mouse_pos_y;
			break;
		}
		case GLOBOX_INTERACTION_N:
		{
			platform->saved_window_geometry[1] += platform->saved_mouse_pos_y - platform->old_mouse_pos_y;
			platform->saved_window_geometry[3] += platform->old_mouse_pos_y - platform->saved_mouse_pos_y;
			break;
		}
		case GLOBOX_INTERACTION_NW:
		{
			platform->saved_window_geometry[0] += platform->saved_mouse_pos_x - platform->old_mouse_pos_x;
			platform->saved_window_geometry[1] += platform->saved_mouse_pos_y - platform->old_mouse_pos_y;
			platform->saved_window_geometry[2] += platform->old_mouse_pos_x - platform->saved_mouse_pos_x;
			platform->saved_window_geometry[3] += platform->old_mouse_pos_y - platform->saved_mouse_pos_y;
			break;
		}
		case GLOBOX_INTERACTION_W:
		{
			platform->saved_window_geometry[0] += platform->saved_mouse_pos_x - platform->old_mouse_pos_x;
			platform->saved_window_geometry[2] += platform->old_mouse_pos_x - platform->saved_mouse_pos_x;
			break;
		}
		case GLOBOX_INTERACTION_SW:
		{
			platform->saved_window_geometry[0] += platform->saved_mouse_pos_x - platform->old_mouse_pos_x;
			platform->saved_window_geometry[2] += platform->old_mouse_pos_x - platform->saved_mouse_pos_x;
			platform->saved_window_geometry[3] += platform->saved_mouse_pos_y - platform->old_mouse_pos_y;
			break;
		}
		case GLOBOX_INTERACTION_S:
		{
			platform->saved_window_geometry[3] += platform->saved_mouse_pos_y - platform->old_mouse_pos_y;
			break;
		}
		case GLOBOX_INTERACTION_SE:
		{
			platform->saved_window_geometry[2] += platform->saved_mouse_pos_x - platform->old_mouse_pos_x;
			platform->saved_window_geometry[3] += platform->saved_mouse_pos_y - platform->old_mouse_pos_y;
			break;
		}
		case GLOBOX_INTERACTION_E:
		{
			platform->saved_window_geometry[2] += platform->saved_mouse_pos_x - platform->old_mouse_pos_x;
			break;
		}
		case GLOBOX_INTERACTION_NE:
		{
			platform->saved_window_geometry[1] += platform->saved_mouse_pos_y - platform->old_mouse_pos_y;
			platform->saved_window_geometry[2] += platform->saved_mouse_pos_x - platform->old_mouse_pos_x;
			platform->saved_window_geometry[3] += platform->old_mouse_pos_y - platform->saved_mouse_pos_y;
			break;
		}
		default:
		{
			break;
		}
	}

	// set window position
	xcb_void_cookie_t cookie_configure =
		xcb_configure_window_checked(
			platform->conn,
			platform->win,
			XCB_CONFIG_WINDOW_X
			| XCB_CONFIG_WINDOW_Y
			| XCB_CONFIG_WINDOW_WIDTH
			| XCB_CONFIG_WINDOW_HEIGHT,
			platform->saved_window_geometry);

	error_xcb =
		xcb_request_check(
			platform->conn,
			cookie_configure);

	if (error_xcb != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_CONFIGURE);
		return;
	}

	xcb_flush(platform->conn);
	globox_error_ok(error);
}

//TODO
void set_state_event(
	struct globox* context,
	struct wayland_platform* platform,
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

	xcb_generic_error_t* error_xcb =
		xcb_request_check(
			platform->conn,
			cookie);

	if (error_xcb != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EVENT_SEND);
		return;
	}

	globox_error_ok(error);
}

//TODO
void set_state_atoms(
	struct globox* context,
	struct wayland_platform* platform,
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

//TODO
void set_state_hidden(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	xcb_generic_error_t* error_xcb;

	if (platform->atoms[X11_ATOM_CHANGE_STATE] != XCB_NONE)
	{
		// iconify with the ICCCM method
		xcb_client_message_event_t event =
		{
			.response_type = XCB_CLIENT_MESSAGE,
			.type = platform->atoms[X11_ATOM_CHANGE_STATE],
			.format = 32,
			.window = platform->win,
			.data =
			{
				.data32 =
				{
					XCB_ICCCM_WM_STATE_ICONIC,
				},
			},
		};

		uint32_t mask =
			XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
			| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;

		xcb_void_cookie_t cookie =
			xcb_send_event_checked(
				platform->conn,
				0,
				platform->root_win,
				mask,
				(const char*)(&event));

		error_xcb =
			xcb_request_check(
				platform->conn,
				cookie);

		if (error_xcb != NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_EVENT_SEND);
			return;
		}
	}

	set_state_event(
		context,
		platform,
		platform->atoms[X11_ATOM_STATE_HIDDEN],
		1,
		error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

//TODO
// there is a bug in ewmh that prevents fullscreen from working properly
// since keeping xcb-ewmh around only for initialization would be kind
// of silly we removed the dependency and used raw xcb all the way
void wayland_helpers_set_state(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	xcb_void_cookie_t cookie;
	xcb_generic_error_t* error_xcb;

	switch (context->feature_state->state)
	{
		case GLOBOX_STATE_REGULAR:
		{
			cookie =
				xcb_map_window_checked(
					platform->conn,
					platform->win);

			error_xcb =
				xcb_request_check(
					platform->conn,
					cookie);

			if (error_xcb != NULL)
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
			set_state_hidden(context, platform, error);

			if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
			{
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

			error_xcb =
				xcb_request_check(
					platform->conn,
					cookie);

			if (error_xcb != NULL)
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

			error_xcb =
				xcb_request_check(
					platform->conn,
					cookie);

			if (error_xcb != NULL)
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

void wayland_helpers_set_title(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	xdg_toplevel_set_title(
		platform->xdg_toplevel,
		context->feature_title->title);

	// always ok
	globox_error_ok(error);
}

void wayland_helpers_set_icon(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// never ok
	globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_ICON);
}

void wayland_helpers_set_frame(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// Wayland clients are expected to render their own decorations by default,
	// but we can try to use the decorations negociation protocol to try and
	// have the compositor render them for us if it is able and willing to.

	switch (platform->decoration_mode)
	{
		case ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE:
		{
			if (context->feature_frame->frame == true)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_DECORATIONS_UNAVAILABLE);
				context->feature_frame->frame = false;
				return;
			}

			break;
		}
		case ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE:
		{
			if (context->feature_frame->frame == false)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_DECORATIONS_FORCED);
				context->feature_frame->frame = true;
				return;
			}

			break;
		}
	}

	globox_error_ok(error);
}

void wayland_helpers_set_background(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// The Wayland specification requires alpha support so transparency is
	// always available, however background blur requires the dedicated
	// KDE protocol so we have to check for this.

	if ((platform->kde_blur_manager == NULL)
	&& (context->feature_background->background == GLOBOX_BACKGROUND_BLURRED))
	{
		context->feature_background->background = GLOBOX_BACKGROUND_TRANSPARENT;

		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_BACKGROUND_BLUR);
		return;
	}

	globox_error_ok(error);
}

void wayland_helpers_set_vsync(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// always ok
	globox_error_ok(error);
}

// wayland-specific
// TODO
