#define _XOPEN_SOURCE 700

#include "globox.h"
#include "globox_error.h"

#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include "zwp-relative-pointer-protocol.h"
#include "zwp-pointer-constraints-protocol.h"

#include "wayland/globox_wayland.h"
#include "wayland/globox_wayland_callbacks.h"

// initalize the display system
void globox_platform_init(
	struct globox* globox,
	bool transparent,
	bool frameless,
	bool blurred)
{
	globox->globox_redraw = true;
	globox->globox_transparent = transparent;
	globox->globox_frameless = frameless;
	globox->globox_blurred = blurred;

	char** log = globox->globox_log;
	log[GLOBOX_ERROR_WAYLAND_REQUEST] = "";

	int error;
	struct globox_platform* platform = &(globox->globox_platform);

	// TODO remove, temporary
	platform->globox_wayland_screen_width = 1920;
	platform->globox_wayland_screen_height = 1080;

	// base
	platform->globox_wayland_xdg_wm_base_listener.ping =
		callback_xdg_wm_base_ping;

	// output
	platform->globox_wayland_output_listener.geometry =
		callback_output_geometry;
	platform->globox_wayland_output_listener.mode =
		callback_output_mode;
	platform->globox_wayland_output_listener.done =
		callback_output_done;
	platform->globox_wayland_output_listener.scale =
		callback_output_scale;

	// registry
	platform->globox_wayland_registry_listener.global =
		callback_registry_global;
	platform->globox_wayland_registry_listener.global_remove =
		callback_registry_global_remove;

	// surfaces
	platform->globox_wayland_xdg_toplevel_listener.configure =
		callback_xdg_toplevel_configure;
	platform->globox_wayland_xdg_toplevel_listener.close =
		callback_xdg_toplevel_close;
	platform->globox_wayland_xdg_surface_listener.configure =
		callback_xdg_surface_configure;

	// connect to display
	platform->globox_wayland_display =
		wl_display_connect(
			NULL);

	if (platform->globox_wayland_display == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_DISPLAY);
		return;
	}

	// get register proxy
	platform->globox_wayland_registry =
		wl_display_get_registry(
			platform->globox_wayland_display);

	if (platform->globox_wayland_registry == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_REQUEST);
		return;
	}

	// get register proxy
	error =
		wl_registry_add_listener(
			platform->globox_wayland_registry,
			&(platform->globox_wayland_registry_listener),
			globox);

	if (error == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_LISTENER);
		return;
	}

	error =
		wl_display_roundtrip(
			platform->globox_wayland_display);

	if (error == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_ROUNDTRIP);
		return;
	}

	// event handle
	platform->globox_platform_event_handle =
		wl_display_get_fd(
			platform->globox_wayland_display);

	return;
}

// create the window
void globox_platform_create_window(struct globox* globox)
{
	// not needed
}

void globox_platform_hooks(struct globox* globox)
{
	struct globox_platform* platform = &(globox->globox_platform);

	// platform update
	globox_platform_set_title(globox, globox->globox_title);
	globox_platform_set_state(globox, globox->globox_state);

	// epoll initialization
	platform->globox_wayland_epoll = epoll_create(1);

	if (platform->globox_wayland_epoll == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_EPOLL_CREATE);
		return;
	}

	struct epoll_event ev =
	{
		EPOLLIN | EPOLLET,
		{0},
	};

	int error_epoll =
		epoll_ctl(
			platform->globox_wayland_epoll,
			EPOLL_CTL_ADD,
			platform->globox_platform_event_handle,
			&ev);

	if (error_epoll == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_EPOLL_CTL);
		return;
	}
}

void globox_platform_commit(struct globox* globox)
{
	struct globox_platform* platform = &(globox->globox_platform);

	wl_surface_commit(
		platform->globox_wayland_surface);
}

void globox_platform_prepoll(struct globox* globox)
{
	struct globox_platform* platform = &(globox->globox_platform);

	int error =
		wl_display_dispatch(
			platform->globox_wayland_display);

	if (error == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_DISPATCH);
	}
}

void globox_platform_events_poll(struct globox* globox)
{
	// not needed
}

void globox_platform_events_wait(struct globox* globox)
{
	// not needed
}

#if 0
static void query_pointer(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	xcb_query_pointer_cookie_t cookie =
		xcb_query_pointer(
			platform->globox_x11_conn,
			platform->globox_x11_win);

	xcb_generic_error_t* error;

	xcb_query_pointer_reply_t* reply =
		xcb_query_pointer_reply(
			platform->globox_x11_conn,
			cookie,
			&error);

	if (error != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_INTERACTIVE);
		return;
	}

	platform->globox_x11_interactive_x = reply->root_x;
	platform->globox_x11_interactive_y = reply->root_y;

	free(reply);
}
#endif

#if 0
void globox_platform_interactive_mode(struct globox* globox, enum globox_interactive_mode mode)
{
	if ((mode != GLOBOX_INTERACTIVE_STOP)
		&& (globox->globox_interactive_mode != mode))
	{
		query_pointer(globox);

		if (globox_error_catch(globox))
		{
			return;
		}

		globox->globox_interactive_mode = mode;
	}
	else
	{
		globox->globox_interactive_mode = GLOBOX_INTERACTIVE_STOP;
	}
}

static void handle_interactive_mode(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	if (globox->globox_interactive_mode == GLOBOX_INTERACTIVE_STOP)
	{
		return;
	}

	int16_t old_x = platform->globox_x11_interactive_x;
	int16_t old_y = platform->globox_x11_interactive_y;

	xcb_generic_error_t* error;
	query_pointer(globox);

	if (globox_error_catch(globox))
	{
		return;
	}

	// get window position
	xcb_get_geometry_cookie_t cookie_geom =
		xcb_get_geometry(
			platform->globox_x11_conn,
			platform->globox_x11_win);

	xcb_get_geometry_reply_t* reply_geom =
		xcb_get_geometry_reply(
			platform->globox_x11_conn,
			cookie_geom,
			&error);

	if (error != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_INTERACTIVE);
		return;
	}

	// translate position in screen coordinates
	xcb_translate_coordinates_cookie_t cookie_translate =
		xcb_translate_coordinates(
			platform->globox_x11_conn,
			platform->globox_x11_win,
			platform->globox_x11_root_win,
			reply_geom->x,
			reply_geom->y);

	xcb_translate_coordinates_reply_t* reply_translate =
		xcb_translate_coordinates_reply(
			platform->globox_x11_conn,
			cookie_translate,
			&error);

	if (error != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_INTERACTIVE);
		return;
	}

	// compute window changes
	uint32_t values[4];

	switch (globox->globox_interactive_mode)
	{
		case GLOBOX_INTERACTIVE_MOVE:
		{
			values[0] = reply_translate->dst_x + platform->globox_x11_interactive_x - old_x;
			values[1] = reply_translate->dst_y + platform->globox_x11_interactive_y - old_y;
			values[2] = reply_geom->width;
			values[3] = reply_geom->height;
			break;
		}
		case GLOBOX_INTERACTIVE_N:
		{
			values[0] = reply_translate->dst_x;
			values[1] = reply_translate->dst_y + platform->globox_x11_interactive_y - old_y;
			values[2] = reply_geom->width;
			values[3] = reply_geom->height + old_y - platform->globox_x11_interactive_y;
			break;
		}
		case GLOBOX_INTERACTIVE_NW:
		{
			values[0] = reply_translate->dst_x + platform->globox_x11_interactive_x - old_x;
			values[1] = reply_translate->dst_y + platform->globox_x11_interactive_y - old_y;
			values[2] = reply_geom->width + old_x - platform->globox_x11_interactive_x;
			values[3] = reply_geom->height + old_y - platform->globox_x11_interactive_y;
			break;
		}
		case GLOBOX_INTERACTIVE_W:
		{
			values[0] = reply_translate->dst_x + platform->globox_x11_interactive_x - old_x;
			values[1] = reply_translate->dst_y;
			values[2] = reply_geom->width + old_x - platform->globox_x11_interactive_x;
			values[3] = reply_geom->height;
			break;
		}
		case GLOBOX_INTERACTIVE_SW:
		{
			values[0] = reply_translate->dst_x + platform->globox_x11_interactive_x - old_x;
			values[1] = reply_translate->dst_y;
			values[2] = reply_geom->width + old_x - platform->globox_x11_interactive_x;
			values[3] = reply_geom->height + platform->globox_x11_interactive_y - old_y ;
			break;
		}
		case GLOBOX_INTERACTIVE_S:
		{
			values[0] = reply_translate->dst_x;
			values[1] = reply_translate->dst_y;
			values[2] = reply_geom->width;
			values[3] = reply_geom->height + platform->globox_x11_interactive_y - old_y;
			break;
		}
		case GLOBOX_INTERACTIVE_SE:
		{
			values[0] = reply_translate->dst_x;
			values[1] = reply_translate->dst_y;
			values[2] = reply_geom->width + platform->globox_x11_interactive_x - old_x;
			values[3] = reply_geom->height + platform->globox_x11_interactive_y - old_y;
			break;
		}
		case GLOBOX_INTERACTIVE_E:
		{
			values[0] = reply_translate->dst_x;
			values[1] = reply_translate->dst_y;
			values[2] = reply_geom->width + platform->globox_x11_interactive_x - old_x;
			values[3] = reply_geom->height;
			break;
		}
		case GLOBOX_INTERACTIVE_NE:
		{
			values[0] = reply_translate->dst_x;
			values[1] = reply_translate->dst_y + platform->globox_x11_interactive_y - old_y;
			values[2] = reply_geom->width + platform->globox_x11_interactive_x - old_x;
			values[3] = reply_geom->height + old_y - platform->globox_x11_interactive_y;
			break;
		}
		default:
		{
			values[0] = reply_translate->dst_x;
			values[1] = reply_translate->dst_y;
			values[2] = reply_geom->width;
			values[3] = reply_geom->height;
			break;
		}
	}

	free(reply_translate);
	free(reply_geom);

	// set window position
	xcb_void_cookie_t cookie_configure =
		xcb_configure_window_checked(
			platform->globox_x11_conn,
			platform->globox_x11_win,
			XCB_CONFIG_WINDOW_X
			| XCB_CONFIG_WINDOW_Y
			| XCB_CONFIG_WINDOW_WIDTH
			| XCB_CONFIG_WINDOW_HEIGHT,
			values);

	error =
		xcb_request_check(
			platform->globox_x11_conn,
			cookie_configure);

	if (error != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_INTERACTIVE);
			return;
	}

	xcb_flush(platform->globox_x11_conn);
}
#endif

void globox_platform_events_handle(
	struct globox* globox)
{
	// not needed
}

void globox_platform_free(struct globox* globox)
{
	// TODO
}

void globox_platform_set_icon(
	struct globox* globox,
	uint32_t* pixmap,
	uint32_t len)
{
	struct globox_platform* platform = &(globox->globox_platform);

	platform->globox_wayland_icon = pixmap;
	platform->globox_wayland_icon_len = len;
}

void globox_platform_set_title(
	struct globox* globox,
	const char* title)
{
	struct globox_platform* platform = &(globox->globox_platform);

	char* tmp = strdup(title);

	if (globox->globox_title != NULL)
	{
		free(globox->globox_title);
	}

	globox->globox_title = tmp;

	xdg_toplevel_set_title(
		platform->globox_wayland_xdg_toplevel,
		title);
}

void globox_platform_set_state(
	struct globox* globox,
	enum globox_state state)
{
	struct globox_platform* platform = &(globox->globox_platform);

	switch (state)
	{
		case GLOBOX_STATE_REGULAR:
		{
			xdg_toplevel_unset_maximized(
				platform->globox_wayland_xdg_toplevel);

			xdg_toplevel_unset_fullscreen(
				platform->globox_wayland_xdg_toplevel);

			if (globox->globox_state == GLOBOX_STATE_MINIMIZED)
			{
				// unmap memory (on software backend)
				platform->globox_wayland_unminimize_start(globox);

				if (globox_error_catch(globox))
				{
					return;
				}

				// destroy surfaces
				wl_surface_destroy(
					platform->globox_wayland_surface);
				xdg_surface_destroy(
					platform->globox_wayland_xdg_surface);
				xdg_toplevel_destroy(
					platform->globox_wayland_xdg_toplevel);

				// remap memory & create surfaces (on software backend)
				platform->globox_wayland_unminimize_finish(globox);

				if (globox_error_catch(globox))
				{
					return;
				}

				// reset title
				globox_platform_set_title(
					globox,
					globox->globox_title);
			}

			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			xdg_toplevel_unset_fullscreen(
				platform->globox_wayland_xdg_toplevel);
			xdg_toplevel_set_maximized(
				platform->globox_wayland_xdg_toplevel);

			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			xdg_toplevel_unset_maximized(
				platform->globox_wayland_xdg_toplevel);
			xdg_toplevel_unset_fullscreen(
				platform->globox_wayland_xdg_toplevel);
			xdg_toplevel_set_minimized(
				platform->globox_wayland_xdg_toplevel);

			break;
		}
		case GLOBOX_STATE_FULLSCREEN:
		{
			xdg_toplevel_unset_maximized(
				platform->globox_wayland_xdg_toplevel);
			xdg_toplevel_set_fullscreen(
				platform->globox_wayland_xdg_toplevel,
				NULL);

			break;
		}
	}

	globox->globox_state = state;
}

// getters

uint32_t* globox_platform_get_argb(struct globox* globox)
{
	return globox->globox_platform.globox_platform_argb;
}

//TODO
