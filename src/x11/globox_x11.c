/// this file implements the main globox functions for the x11 platform
/// it is completely isolated from the graphic context functions

#define _XOPEN_SOURCE 700

// include globox structures and functions
#include "globox.h"
#include "globox_error.h"
// system includes
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
// x11 includes
#include <xcb/xcb.h>

#if defined(GLOBOX_CONTEXT_GLX)
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include "x11/glx/globox_x11_glx.h"
#endif

// include platform structures
#include "x11/globox_x11.h"

// initalize the display system
void globox_platform_init(
	struct globox* globox,
	bool transparent,
	bool blurred)
{
	struct globox_platform* platform = malloc(sizeof (struct globox_platform));

	if (platform == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	globox->globox_platform = platform;
	globox->globox_redraw = false;
	globox->globox_transparent = transparent;
	globox->globox_blurred = blurred;

#if defined(GLOBOX_CONTEXT_GLX)
	struct globox_x11_glx* context = &(platform->globox_x11_glx);

	context->globox_glx_display =
		XOpenDisplay(
			NULL);

	if (context->globox_glx_display == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_GLX_FAIL);
		return;
	}

	platform->globox_x11_conn =
		XGetXCBConnection(
			context->globox_glx_display);

	if (platform->globox_x11_conn == NULL)
	{
		XCloseDisplay(context->globox_glx_display);

		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_CONN);
		return;
	}

	XSetEventQueueOwner(
		context->globox_glx_display,
		XCBOwnsEventQueue);

	globox->globox_redraw = true;
#else
	platform->globox_x11_conn =
		xcb_connect(
			NULL,
			&(platform->globox_x11_screen_id));

	globox->globox_redraw = false;
#endif

	// check if the connection was successful
	int error_conn =
		xcb_connection_has_error(
			platform->globox_x11_conn);

	if (error_conn > 0)
	{
		// cleanup
		xcb_disconnect(
			platform->globox_x11_conn);

		// throw error
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_CONN);
		return;
	}

	// get the internal event file descriptor (unchecked)
	platform->globox_platform_event_handle =
		xcb_get_file_descriptor(
			platform->globox_x11_conn);

	// get the screen obj from the id the dirty way (there is no other option)
	const struct xcb_setup_t* setup =
		xcb_get_setup(
			platform->globox_x11_conn);

	xcb_screen_iterator_t iter =
		xcb_setup_roots_iterator(
			setup);

	for (int i = 0; i < platform->globox_x11_screen_id; ++i)
	{
		xcb_screen_next(&iter);
	}

	platform->globox_x11_screen_obj = iter.data;

	// get the root window from the screen object
	platform->globox_x11_root_win = platform->globox_x11_screen_obj->root;

	if (globox->globox_transparent == true)
	{
		platform->globox_x11_attr_mask =
			XCB_CW_BORDER_PIXEL
			| XCB_CW_EVENT_MASK
			| XCB_CW_COLORMAP;

		platform->globox_x11_attr_val[0] =
			0;
	}
	else
	{
		platform->globox_x11_attr_mask =
			XCB_CW_BACK_PIXMAP
			| XCB_CW_EVENT_MASK
			| XCB_CW_COLORMAP;

		platform->globox_x11_attr_val[0] =
			XCB_BACK_PIXMAP_NONE;
	}

	platform->globox_x11_attr_val[1] =
		XCB_EVENT_MASK_EXPOSURE
		| XCB_EVENT_MASK_STRUCTURE_NOTIFY
		| XCB_EVENT_MASK_PROPERTY_CHANGE;
}

// create the window
void globox_platform_create_window(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	// create the window
	platform->globox_x11_win =
		xcb_generate_id(
			platform->globox_x11_conn);
	
	xcb_void_cookie_t cookie_win =
		xcb_create_window(
			platform->globox_x11_conn,
			platform->globox_x11_visual_depth, // force instead of XCB_COPY_FROM_PARENT
			platform->globox_x11_win,
			platform->globox_x11_root_win,
			globox->globox_x,
			globox->globox_y,
			globox->globox_width,
			globox->globox_height,
			0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			platform->globox_x11_visual_id,
			platform->globox_x11_attr_mask,
			platform->globox_x11_attr_val);

	// check if window creation was successful
	xcb_generic_error_t* error_win =
		xcb_request_check(
			platform->globox_x11_conn,
			cookie_win);

	if (error_win != NULL)
	{
		// throw error
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_WIN);
		return;
	}

	// map the created window
	xcb_void_cookie_t cookie_map =
		xcb_map_window_checked(
			platform->globox_x11_conn,
			platform->globox_x11_win);

	// check if mapping was successful
	xcb_generic_error_t* error_map =
		xcb_request_check(
			platform->globox_x11_conn,
			cookie_map);

	if (error_map != NULL)
	{
		// throw error
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_MAP);
		return;
	}

	// flush connection
	int error_flush =
		xcb_flush(
			platform->globox_x11_conn);

	// check if flush was successful
	if (error_flush <= 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_FLUSH);
		return;
	}
}

void globox_platform_hooks(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	// platform update
	globox_platform_set_title(globox, globox->globox_title);
	globox_platform_set_state(globox, globox->globox_state);

	// provide ewmh-dependant functions
	xcb_intern_atom_cookie_t cookie_atom;
	xcb_intern_atom_reply_t* reply_atom;
	xcb_generic_error_t* error_atom;
	uint8_t replace;

	char* atoms_names[GLOBOX_X11_ATOM_COUNT] =
	{
		[GLOBOX_X11_ATOM_STATE_MAXIMIZED_HORIZONTAL] = "_NET_WM_STATE_MAXIMIZED_HORZ",
		[GLOBOX_X11_ATOM_STATE_MAXIMIZED_VERTICAL  ] = "_NET_WM_STATE_MAXIMIZED_VERT",
		[GLOBOX_X11_ATOM_STATE_FULLSCREEN          ] = "_NET_WM_STATE_FULLSCREEN",
		[GLOBOX_X11_ATOM_STATE_HIDDEN              ] = "_NET_WM_STATE_HIDDEN",
		[GLOBOX_X11_ATOM_STATE                     ] = "_NET_WM_STATE",
		[GLOBOX_X11_ATOM_ICON                      ] = "_NET_WM_ICON",
		[GLOBOX_X11_ATOM_FRAMELESS                 ] = "_MOTIF_WM_HINTS",
		[GLOBOX_X11_ATOM_BLUR_KDE                  ] = "_KDE_NET_WM_BLUR_BEHIND_REGION",
		[GLOBOX_X11_ATOM_BLUR_DEEPIN               ] = "_NET_WM_DEEPIN_BLUR_REGION_ROUNDED",
		[GLOBOX_X11_ATOM_PROTOCOLS                 ] = "WM_PROTOCOLS",
		[GLOBOX_X11_ATOM_DELETE_WINDOW             ] = "WM_DELETE_WINDOW",
	};

	for (int i = 0; i < GLOBOX_X11_ATOM_COUNT; ++i)
	{
		if (i == GLOBOX_X11_ATOM_PROTOCOLS)
		{
			replace = 1;
		}
		else
		{
			replace = 0;
		}

		cookie_atom =
			xcb_intern_atom(
				platform->globox_x11_conn,
				replace,
				strlen(atoms_names[i]),
				atoms_names[i]);

		reply_atom =
			xcb_intern_atom_reply(
				platform->globox_x11_conn,
				cookie_atom,
				&error_atom);

		if (error_atom != NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_X11_ATOMS);
			return;
		}

		platform->globox_x11_atom_list[i] = reply_atom->atom;

		free(reply_atom);
	}

	// window deletion protocol
	xcb_void_cookie_t cookie_prop =
		xcb_change_property_checked(
			platform->globox_x11_conn,
			XCB_PROP_MODE_REPLACE,
			platform->globox_x11_win,
			platform->globox_x11_atom_list[GLOBOX_X11_ATOM_PROTOCOLS],
			4,
			32,
			1,
			&(platform->globox_x11_atom_list[GLOBOX_X11_ATOM_DELETE_WINDOW]));

	error_atom = xcb_request_check(
		platform->globox_x11_conn,
		cookie_prop);

	if (error_atom != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_ATOMS);
		return;
	}

	// configure the window as frameless

	// this is supposed to be a struct but whatever, here we set
	//  - flags to 0x02 to configure the decorations
	//  - decorations to 0x00 to remove them
	uint32_t motif_hints[5] =
	{
		2, // flags
		0, // functions
		0, // decorations
		0, // input_mode
		0, // status
	};

	cookie_prop =
		xcb_change_property(
			platform->globox_x11_conn,
			XCB_PROP_MODE_REPLACE,
			platform->globox_x11_win,
			platform->globox_x11_atom_list[GLOBOX_X11_ATOM_FRAMELESS],
			platform->globox_x11_atom_list[GLOBOX_X11_ATOM_FRAMELESS],
			32,
			5,
			motif_hints);

	error_atom = xcb_request_check(
		platform->globox_x11_conn,
		cookie_prop);

	if (error_atom != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_ATOMS);
		return;
	}

	// blurred background
	if (globox->globox_blurred == true)
	{
		// kde blur
		cookie_prop =
			xcb_change_property(
				platform->globox_x11_conn,
				XCB_PROP_MODE_REPLACE,
				platform->globox_x11_win,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_BLUR_KDE],
				XCB_ATOM_CARDINAL,
				32,
				0,
				NULL);

		error_atom = xcb_request_check(
			platform->globox_x11_conn,
			cookie_prop);

		if (error_atom != NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_X11_ATOMS);
			return;
		}

		// deeping blur
		cookie_prop =
			xcb_change_property(
				platform->globox_x11_conn,
				XCB_PROP_MODE_REPLACE,
				platform->globox_x11_win,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_BLUR_DEEPIN],
				XCB_ATOM_CARDINAL,
				32,
				0,
				NULL);

		error_atom = xcb_request_check(
			platform->globox_x11_conn,
			cookie_prop);

		if (error_atom != NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_X11_ATOMS);
			return;
		}
	}

	// epoll initialization
	platform->globox_x11_epoll = epoll_create(1);

	if (platform->globox_x11_epoll == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EPOLL_CREATE);
		return;
	}

	struct epoll_event ev =
	{
		EPOLLIN | EPOLLET,
		{0},
	};

	int error_epoll =
		epoll_ctl(
			platform->globox_x11_epoll,
			EPOLL_CTL_ADD,
			platform->globox_platform_event_handle,
			&ev);

	if (error_epoll == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EPOLL_CTL);
		return;
	}

	// input handling
	if (globox->globox_event_callback != NULL)
	{
		platform->globox_x11_attr_val[1] |=
			XCB_EVENT_MASK_KEY_PRESS
			| XCB_EVENT_MASK_KEY_RELEASE
			| XCB_EVENT_MASK_BUTTON_PRESS
			| XCB_EVENT_MASK_BUTTON_RELEASE
			| XCB_EVENT_MASK_POINTER_MOTION;

		xcb_void_cookie_t cookie_event =
			xcb_change_window_attributes_checked(
				platform->globox_x11_conn,
				platform->globox_x11_win,
				platform->globox_x11_attr_mask,
				platform->globox_x11_attr_val);

		xcb_generic_error_t* error_event =
			xcb_request_check(
				platform->globox_x11_conn,
				cookie_event);

		if (error_event != NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_X11_WIN_ATTR);
			return;
		}
	}
}

void globox_platform_commit(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	// flush connection
	int error_flush =
		xcb_flush(
			platform->globox_x11_conn);

	// check if flush was successful
	if (error_flush <= 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_FLUSH);
		return;
	}
}

void globox_platform_prepoll(struct globox* globox)
{
	// not needed
}

void globox_platform_events_poll(struct globox* globox)
{
	// not needed
}

void globox_platform_events_wait(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	int error_epoll =
		epoll_wait(
			platform->globox_x11_epoll,
			platform->globox_x11_epoll_event,
			GLOBOX_CONST_MAX_X11_EVENTS,
			-1);

	if (error_epoll == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EPOLL_WAIT);
		return;
	}
}

static void query_pointer(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

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
	struct globox_platform* platform = globox->globox_platform;

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

static void handle_title(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	xcb_get_property_cookie_t cookie =
		xcb_get_property(
			platform->globox_x11_conn,
			0,
			platform->globox_x11_win,
			XCB_ATOM_WM_NAME,
			XCB_ATOM_STRING,
			0,
			32);

	xcb_generic_error_t* error;

	xcb_get_property_reply_t* reply =
		xcb_get_property_reply(
			platform->globox_x11_conn,
			cookie,
			&error);

	if (error != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_TITLE);
		return;
	}

	char* value = (char*) xcb_get_property_value(reply);

	if (value == NULL)
	{
		free(reply);

		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_TITLE);
		return;
	}

	globox_platform_set_title(globox, value);

	free(reply);
}

static void handle_state(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	xcb_get_property_cookie_t cookie =
		xcb_get_property(
			platform->globox_x11_conn,
			0,
			platform->globox_x11_win,
			platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE],
			XCB_ATOM_ATOM,
			0,
			32);

	xcb_generic_error_t* error;

	xcb_get_property_reply_t* reply =
		xcb_get_property_reply(
			platform->globox_x11_conn,
			cookie,
			&error);

	if (error != NULL)
	{
		free(reply);

		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_STATE);
		return;
	}

	xcb_atom_t* value = (xcb_atom_t*) xcb_get_property_value(reply);

	if (value == NULL)
	{
		free(reply);

		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_STATE);
		return;
	}

	globox->globox_redraw = true;

	// it is possible valgrind mistakes `value` for an invalid pointer
	// this is normal and stems from the spurious XCB/X11 black magic

	if (*value == platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_FULLSCREEN])
	{
		globox->globox_state = GLOBOX_STATE_FULLSCREEN;
	}
	else if ((*value == platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_VERTICAL])
		|| (*value == platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_HORIZONTAL]))
	{
		globox->globox_state = GLOBOX_STATE_MAXIMIZED;
	}
	else if (*value == platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_HIDDEN])
	{
		globox->globox_state = GLOBOX_STATE_MINIMIZED;
		globox->globox_redraw = false;
	}
	else
	{
		globox->globox_state = GLOBOX_STATE_REGULAR;
	}

	free(reply);
}

void globox_platform_events_handle(
	struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	xcb_configure_notify_event_t* resize = NULL;
	xcb_property_notify_event_t* state = NULL;
	xcb_expose_event_t* show = NULL;
	bool redraw = false;
	int i = 0;

	xcb_generic_event_t* event =
		xcb_poll_for_event(
			platform->globox_x11_conn);

	while (event != NULL)
	{
		switch (event->response_type & ~0x80)
		{
			case XCB_EXPOSE:
			{
				show = (xcb_expose_event_t*) event;

				if (redraw == false)
				{
					if (i >= GLOBOX_CONST_MAX_X11_EXPOSE_QUEUE)
					{
						platform->globox_x11_expose(globox, i);

						if (globox_error_catch(globox))
						{
							return;
						}

						i = 0;
					}

					platform->globox_x11_expose_queue[(4 * i) + 0] = show->x;
					platform->globox_x11_expose_queue[(4 * i) + 1] = show->y;
					platform->globox_x11_expose_queue[(4 * i) + 2] = show->width;
					platform->globox_x11_expose_queue[(4 * i) + 3] = show->height;

					++i;
				}

				free(show);

				break;
			}
			case XCB_CONFIGURE_NOTIFY:
			{
				if (resize != NULL)
				{
					free(resize);
				}

				resize = (xcb_configure_notify_event_t*) event;

				if ((redraw == false)
					&& ((resize->width != globox->globox_width)
						|| (resize->height != globox->globox_height)))
				{
					i = 0;
					redraw = true;
				}

				break;
			}
			case XCB_PROPERTY_NOTIFY:
			{
				if (state == NULL)
				{
					state = (xcb_property_notify_event_t*) event;

					if ((state->atom != platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE])
						&& (state->atom != XCB_ATOM_WM_NAME))
					{
						free(state);
						state = NULL;
					}
				}
				else
				{
					free(event);
				}

				break;
			}
			case XCB_CLIENT_MESSAGE:
			{
				xcb_client_message_event_t* delete = (xcb_client_message_event_t*) event;

				if (delete->data.data32[0] == platform->globox_x11_atom_list[GLOBOX_X11_ATOM_DELETE_WINDOW])
				{
					globox->globox_closed = true;
				}

				free(delete);

				break;
			}
			default:
			{
				if (globox->globox_event_callback != NULL)
				{
					globox->globox_event_callback(event, globox->globox_event_callback_data);
					free(event);
				}

				break;
			}
		}

		event = xcb_poll_for_event(platform->globox_x11_conn);
	}

	if (resize != NULL)
	{
		if (redraw == true)
		{
			globox->globox_width = resize->width;
			globox->globox_height = resize->height;
			platform->globox_x11_reserve(globox);
			globox->globox_redraw = true;
		}

		free(resize);
	}

	if (state != NULL)
	{
		handle_title(globox);
		handle_state(globox);

		free(state);
	}

	if (i > 0)
	{
		platform->globox_x11_expose(globox, i);
	}

	handle_interactive_mode(globox);
}

void globox_platform_prepare_buffer(
	struct globox* globox)
{
	// not needed
}

void globox_platform_free(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	xcb_destroy_window(platform->globox_x11_conn, platform->globox_x11_win);

#if defined(GLOBOX_CONTEXT_GLX)
	XCloseDisplay(platform->globox_x11_glx.globox_glx_display);
#else
	xcb_disconnect(platform->globox_x11_conn);
#endif

	free(platform);
}

void globox_platform_set_icon(
	struct globox* globox,
	uint32_t* pixmap,
	uint32_t len)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	xcb_void_cookie_t cookie_icon =
		xcb_change_property_checked(
			platform->globox_x11_conn,
			XCB_PROP_MODE_REPLACE,
			platform->globox_x11_win,
			platform->globox_x11_atom_list[GLOBOX_X11_ATOM_ICON],
			XCB_ATOM_CARDINAL,
			32,
			len,
			pixmap);

	xcb_generic_error_t* error_icon =
		xcb_request_check(
			platform->globox_x11_conn,
			cookie_icon);

	if (error_icon != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_ICON);
		return;
	}

	globox_platform_commit(globox);
}

void globox_platform_set_title(
	struct globox* globox,
	const char* title)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;
	char* tmp = strdup(title);

	if (tmp == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	if (globox->globox_title != NULL)
	{
		free(globox->globox_title);
	}

	globox->globox_title = tmp;

	xcb_void_cookie_t cookie_title =
		xcb_change_property_checked(
			platform->globox_x11_conn,
			XCB_PROP_MODE_REPLACE,
			platform->globox_x11_win,
			XCB_ATOM_WM_NAME,
			XCB_ATOM_STRING,
			8,
			strlen(tmp) + 1,
			tmp);
	
	xcb_generic_error_t* error_title =
		xcb_request_check(
			platform->globox_x11_conn,
			cookie_title);

	if (error_title != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_TITLE);
	}
}

// ask the server to change the window state
// there is a bug in ewmh that prevents fullscreen from working properly
// since keeping xcb-ewmh around only for initialization would be kind
// of silly we removed the dependency and used raw xcb all the way

static void set_state(
	struct globox* globox,
	xcb_atom_t atom,
	uint32_t action)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	xcb_client_message_event_t event =
	{
		.response_type = XCB_CLIENT_MESSAGE,
		.type = platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE],
		.format = 32,
		.window = platform->globox_x11_win,
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

	xcb_void_cookie_t cookie_event =
		xcb_send_event_checked(
			platform->globox_x11_conn,
			1,
			platform->globox_x11_win,
			XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
			| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
			(const char*)(&event));

	xcb_generic_error_t* error_event =
		xcb_request_check(
			platform->globox_x11_conn,
			cookie_event);

	if (error_event != NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_STATE);
	}
}

void globox_platform_set_state(
	struct globox* globox,
	enum globox_state state)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;
	xcb_void_cookie_t cookie_map;
	xcb_generic_error_t* error_map;

	switch (state)
	{
		case GLOBOX_STATE_REGULAR:
		{
			cookie_map =
				xcb_map_window_checked(
					platform->globox_x11_conn,
					platform->globox_x11_win);

			error_map =
				xcb_request_check(
					platform->globox_x11_conn,
					cookie_map);

			if (error_map != NULL)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_X11_MAP);
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_FULLSCREEN],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_HORIZONTAL],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_VERTICAL],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			cookie_map =
				xcb_map_window_checked(
					platform->globox_x11_conn,
					platform->globox_x11_win);

			error_map =
				xcb_request_check(
					platform->globox_x11_conn,
					cookie_map);

			if (error_map != NULL)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_X11_MAP);
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_FULLSCREEN],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_HORIZONTAL],
				1);

			if (globox_error_catch(globox))
			{
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_VERTICAL],
				1);

			if (globox_error_catch(globox))
			{
				return;
			}

			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_FULLSCREEN],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_HORIZONTAL],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_VERTICAL],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			cookie_map =
				xcb_unmap_window_checked(
					platform->globox_x11_conn,
					platform->globox_x11_win);

			error_map =
				xcb_request_check(
					platform->globox_x11_conn,
					cookie_map);

			if (error_map != NULL)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_X11_MAP);
				return;
			}

			break;
		}
		case GLOBOX_STATE_FULLSCREEN:
		{
			cookie_map =
				xcb_map_window_checked(
					platform->globox_x11_conn,
					platform->globox_x11_win);

			error_map =
				xcb_request_check(
					platform->globox_x11_conn,
					cookie_map);

			if (error_map != NULL)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_X11_MAP);
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_HORIZONTAL],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_MAXIMIZED_VERTICAL],
				0);

			if (globox_error_catch(globox))
			{
				return;
			}

			set_state(
				globox,
				platform->globox_x11_atom_list[GLOBOX_X11_ATOM_STATE_FULLSCREEN],
				1);

			if (globox_error_catch(globox))
			{
				return;
			}

			break;
		}
	}

	globox->globox_state = state;
}

// getters
uint32_t* globox_platform_get_argb(struct globox* globox)
{
	return globox->globox_platform->globox_platform_argb;
}

int globox_platform_get_event_handle(struct globox* globox)
{
	return globox->globox_platform->globox_platform_event_handle;
}
