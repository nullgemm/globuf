#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_common.h"
#include "x11/x11_common_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/sync.h>
#include <xcb/xcb.h>
#include <xcb/present.h>

void globox_x11_common_init(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	int posix_error;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;

	// init pthread mutex attributes
	posix_error = pthread_mutexattr_init(&mutex_attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_INIT);
		return;
	}

	// set pthread mutex type (error checking for now)
	posix_error =
		pthread_mutexattr_settype(
			&mutex_attr,
			PTHREAD_MUTEX_ERRORCHECK);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_SETTYPE);
		return;
	}

	// init pthread mutex (main)
	posix_error = pthread_mutex_init(&(platform->mutex_main), &mutex_attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// init pthread mutex (block)
	posix_error = pthread_mutex_init(&(platform->mutex_block), &mutex_attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// init pthread mutex (xsync)
	posix_error = pthread_mutex_init(&(platform->mutex_xsync), &mutex_attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// destroy pthread mutex attributes
	posix_error = pthread_mutexattr_destroy(&mutex_attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_DESTROY);
		return;
	}

	// init pthread cond attributes
	posix_error = pthread_condattr_init(&cond_attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_ATTR_INIT);
		return;
	}

	// set pthread cond clock
	posix_error =
		pthread_condattr_setclock(
			&cond_attr,
			CLOCK_MONOTONIC);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_ATTR_SETTYPE);
		return;
	}

	// init pthread cond
	posix_error = pthread_cond_init(&(platform->cond_main), &cond_attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_INIT);
		return;
	}

	// destroy pthread cond attributes
	posix_error = pthread_condattr_destroy(&cond_attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_ATTR_DESTROY);
		return;
	}

	// initialize the "closed" boolean
	platform->closed = false;

	// open a connection to the X server
	platform->conn = xcb_connect(NULL, &(platform->screen_id));
	posix_error = xcb_connection_has_error(platform->conn);

	if (posix_error > 0)
	{
		xcb_disconnect(platform->conn);
		globox_error_throw(context, error, GLOBOX_ERROR_X11_CONN);
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
	xcb_generic_error_t* xcb_error;
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
		[X11_ATOM_SYNC_REQUEST] =
			"_NET_WM_SYNC_REQUEST",
		[X11_ATOM_SYNC_REQUEST_COUNTER] =
			"_NET_WM_SYNC_REQUEST_COUNTER",
		[X11_ATOM_NET_SUPPORTED] =
			"_NET_SUPPORTED",
		[X11_ATOM_FRAME_DRAWN] =
			"_NET_WM_FRAME_DRAWN",
		[X11_ATOM_MOVERESIZE] =
			"_NET_WM_MOVERESIZE",
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
			&xcb_error);

		if (xcb_error != NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_ATOM_GET);
			return;
		}

		platform->atoms[i] = reply->atom;
		free(reply);
	}

	// lock xsync mutex
	posix_error = pthread_mutex_lock(&(platform->mutex_xsync));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	platform->xsync_configure = true;

	// unlock xsync mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_xsync));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	platform->saved_mouse_press_x = 0;
	platform->saved_mouse_press_y = 0;
	platform->saved_mouse_press_button = XCB_BUTTON_INDEX_ANY;

	globox_error_ok(error);
}

void globox_x11_common_clean(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	int posix_error;
	int posix_cond_error;

	// close the connection to the X server
	xcb_disconnect(platform->conn);

	// lock block mutex to be able to destroy the cond
	posix_error = pthread_mutex_lock(&(platform->mutex_block));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy pthread cond
	posix_cond_error = pthread_cond_destroy(&(platform->cond_main));

	// unlock block mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_block));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (posix_cond_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_DESTROY);
		return;
	}

	// destroy pthread mutex (xsync)
	posix_error = pthread_mutex_destroy(&(platform->mutex_xsync));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	// destroy pthread mutex (block)
	posix_error = pthread_mutex_destroy(&(platform->mutex_block));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	// destroy pthread mutex (main)
	posix_error = pthread_mutex_destroy(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	if ((context->feature_title != NULL) && (context->feature_title->title != NULL))
	{
		free((void*) context->feature_title->title);
	}

	if ((context->feature_icon != NULL) && (context->feature_icon->pixmap != NULL))
	{
		free((void*) context->feature_icon->pixmap);
	}

	globox_error_ok(error);
}

void globox_x11_common_window_create(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	x11_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// prepare window attributes
	if ((context->feature_background != NULL)
		&& (context->feature_background->background != GLOBOX_BACKGROUND_OPAQUE))
	{
		platform->attr_mask =
			XCB_CW_BORDER_PIXEL
			| XCB_CW_EVENT_MASK
			| XCB_CW_COLORMAP;

		platform->attr_val[0] =
			0;
	}
	else
	{
		platform->attr_mask =
			XCB_CW_BACK_PIXMAP
			| XCB_CW_EVENT_MASK
			| XCB_CW_COLORMAP;

		platform->attr_val[0] =
			XCB_BACK_PIXMAP_NONE;
	}

	platform->attr_val[1] =
		XCB_EVENT_MASK_EXPOSURE
		| XCB_EVENT_MASK_STRUCTURE_NOTIFY
		| XCB_EVENT_MASK_PROPERTY_CHANGE;

	// create the window
	platform->win = xcb_generate_id(platform->conn);

	xcb_void_cookie_t cookie =
		xcb_create_window(
			platform->conn,
			platform->visual_depth,
			platform->win,
			platform->root_win,
			context->feature_pos->x,
			context->feature_pos->y,
			context->feature_size->width,
			context->feature_size->height,
			0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			platform->visual_id,
			platform->attr_mask,
			platform->attr_val);

	xcb_generic_error_t* xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_WIN_CREATE);
		return;
	}

	// support the window deletion protocol
	xcb_atom_t supported[2] =
	{
		platform->atoms[X11_ATOM_DELETE_WINDOW],
		platform->atoms[X11_ATOM_SYNC_REQUEST],
	};

	cookie =
		xcb_change_property_checked(
			platform->conn,
			XCB_PROP_MODE_REPLACE,
			platform->win,
			platform->atoms[X11_ATOM_PROTOCOLS],
			XCB_ATOM_ATOM,
			32,
			2,
			supported);

	xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		return;
	}

	// select the supported input event categories
	platform->attr_val[1] |=
		XCB_EVENT_MASK_KEY_PRESS
		| XCB_EVENT_MASK_KEY_RELEASE
		| XCB_EVENT_MASK_BUTTON_PRESS
		| XCB_EVENT_MASK_BUTTON_RELEASE
		| XCB_EVENT_MASK_POINTER_MOTION;

	cookie =
		xcb_change_window_attributes_checked(
			platform->conn,
			platform->win,
			platform->attr_mask,
			platform->attr_val);

	xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_ATTR_CHANGE);
		return;
	}

	// create the xsync counter
	// lock xsync mutex
	int posix_error = pthread_mutex_lock(&(platform->mutex_xsync));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	xcb_sync_int64_t value =
	{
		.hi = 0,
		.lo = 0,
	};

	cookie =
		xcb_sync_create_counter(
			platform->conn,
			platform->xsync_counter,
			value);

	xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_SYNC_COUNTER_CREATE);
		return;
	}

	// set the xsync counters
	cookie =
		xcb_change_property_checked(
			platform->conn,
			XCB_PROP_MODE_REPLACE,
			platform->win,
			platform->atoms[X11_ATOM_SYNC_REQUEST_COUNTER],
			XCB_ATOM_CARDINAL,
			32,
			1,
			&(platform->xsync_counter));

	xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_CHANGE);
		return;
	}

	// unlock xsync mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_xsync));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// configure features
	struct globox_config_reply* reply = malloc(count * (sizeof (struct globox_config_reply)));

	if (reply == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	for (size_t i = 0; i < count; ++i)
	{
		enum globox_feature feature = configs[i].feature;
		reply[i].feature = feature;

		switch (feature)
		{
			case GLOBOX_FEATURE_STATE:
			{
				x11_helpers_set_state(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				x11_helpers_set_title(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				x11_helpers_set_icon(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				x11_helpers_set_frame(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				x11_helpers_set_background(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_VSYNC:
			{
				x11_helpers_set_vsync(context, platform, &reply[i].error);
				break;
			}
			default:
			{
				reply[i].error.code = GLOBOX_ERROR_OK;
				reply[i].error.file = NULL;
				reply[i].error.line = 0;
				break;
			}
		}
	}

	callback(reply, count, data);
	free(reply);

	// error always set
}

void globox_x11_common_window_destroy(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	// destroy the xsync counter
	// lock xsync mutex
	int posix_error = pthread_mutex_lock(&(platform->mutex_xsync));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	xcb_generic_error_t* xcb_error;
	xcb_void_cookie_t cookie;

	cookie =
		xcb_sync_destroy_counter(
			platform->conn,
			platform->xsync_counter);

	xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_SYNC_COUNTER_DESTROY);
		return;
	}

	// unlock xsync mutex
	posix_error = pthread_mutex_lock(&(platform->mutex_xsync));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// destroy the window
	cookie =
		xcb_destroy_window(
			platform->conn,
			platform->win);

	xcb_error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_WIN_DESTROY);
		return;
	}

	globox_error_ok(error);
}

void globox_x11_common_window_start(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	// map
	xcb_void_cookie_t cookie =
		xcb_map_window_checked(
			platform->conn,
			platform->win);

	xcb_generic_error_t* error_map =
		xcb_request_check(
			platform->conn,
			cookie);

	if (error_map != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_WIN_MAP);
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

void globox_x11_common_window_block(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	int posix_error;
	int posix_cond_error;

	// lock block mutex
	posix_error = pthread_mutex_lock(&(platform->mutex_block));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	posix_cond_error = pthread_cond_wait(&(platform->cond_main), &(platform->mutex_block));

	// unlock block mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_block));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (posix_cond_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_WAIT);
	}

	globox_error_ok(error);
}

void globox_x11_common_window_stop(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error)
{
	// create the close event
	xcb_client_message_event_t event =
	{
		.response_type = XCB_CLIENT_MESSAGE,
		.format = 32,
		.sequence = 0,
		.window = platform->win,
		.type = platform->atoms[X11_ATOM_PROTOCOLS],
		.data.data32[0] = platform->atoms[X11_ATOM_DELETE_WINDOW],
		.data.data32[1] = XCB_CURRENT_TIME,
	};

	// send the event
	xcb_void_cookie_t cookie =
		xcb_send_event(
			platform->conn,
			false,
			platform->win,
			XCB_EVENT_MASK_NO_EVENT,
			(const char*) &event);

	xcb_generic_error_t* error_event =
		xcb_request_check(
			platform->conn,
			cookie);

	if (error_event != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EVENT_SEND);
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


void globox_x11_common_init_render(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	// set the event callback
	context->render_callback = *config;

	// start the render loop in a new thread
	// init thread attributes
	int posix_error;
	pthread_attr_t attr;

	posix_error = pthread_attr_init(&attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT);
		return;
	}

	posix_error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_DETACH);
		return;
	}

	// init thread function data
	struct x11_thread_render_loop_data data =
	{
		.globox = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_render_loop_data = data;

	// start function in a new thread
	posix_error =
		pthread_create(
			&(platform->thread_render_loop),
			&attr,
			x11_helpers_render_loop,
			&(platform->thread_render_loop_data));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_CREATE);
		return;
	}

	// destroy the attributes
	posix_error = pthread_attr_destroy(&attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY);
		return;
	}

	globox_error_ok(error);
}

void globox_x11_common_init_events(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	// set the event callback
	context->event_callbacks = *config;

	// start the event loop in a new thread
	// init thread attributes
	int posix_error;
	pthread_attr_t attr;

	posix_error = pthread_attr_init(&attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT);
		return;
	}

	posix_error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_DETACH);
		return;
	}

	// init thread function data
	struct x11_thread_event_loop_data data =
	{
		.globox = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_event_loop_data = data;

	// start function in a new thread
	posix_error =
		pthread_create(
			&(platform->thread_event_loop),
			&attr,
			x11_helpers_event_loop,
			&(platform->thread_event_loop_data));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_CREATE);
		return;
	}

	// destroy the attributes
	posix_error = pthread_attr_destroy(&attr);

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY);
		return;
	}

	globox_error_ok(error);
}

enum globox_event globox_x11_common_handle_events(
	struct globox* context,
	struct x11_platform* platform,
	void* event,
	struct globox_error_info* error)
{
	// process system events
	enum globox_event globox_event = GLOBOX_EVENT_UNKNOWN;
	xcb_generic_event_t* xcb_event = event;

	switch (xcb_event->response_type & ~0x80)
	{
		case XCB_NONE:
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_EVENT_INVALID);
			return GLOBOX_EVENT_INVALID;
		}
		case XCB_EXPOSE:
		{
			// TODO fill & process expose queue
			globox_event = GLOBOX_EVENT_DAMAGED;
			break;
		}
		case XCB_CONFIGURE_NOTIFY:
		{
			xcb_configure_notify_event_t* configure =
				(xcb_configure_notify_event_t*) xcb_event;

			// lock xsync mutex
			int posix_error = pthread_mutex_lock(&(platform->mutex_xsync));

			if (posix_error != 0)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
				break;
			}

			// safe value updates
			platform->xsync_width = configure->width;
			platform->xsync_height = configure->height;
			platform->xsync_configure = true;

			// unlock xsync mutex
			posix_error = pthread_mutex_unlock(&(platform->mutex_xsync));

			if (posix_error != 0)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
				break;
			}

			// TODO
			// moved events are to trigger screen info (size, density, etc.) updates
			// (using dpishit in the example)
			#if 0
			globox_event = GLOBOX_EVENT_MOVED;
			globox_event = GLOBOX_EVENT_RESIZED_N;
			globox_event = GLOBOX_EVENT_RESIZED_NW;
			globox_event = GLOBOX_EVENT_RESIZED_W;
			globox_event = GLOBOX_EVENT_RESIZED_SW;
			globox_event = GLOBOX_EVENT_RESIZED_S;
			globox_event = GLOBOX_EVENT_RESIZED_SE;
			globox_event = GLOBOX_EVENT_RESIZED_E;
			globox_event = GLOBOX_EVENT_RESIZED_NE;
			#endif
			break;
		}
		case XCB_PROPERTY_NOTIFY:
		{
			// TODO handle title & state changes
			#if 0
			globox_event = GLOBOX_EVENT_RESTORED;
			globox_event = GLOBOX_EVENT_MINIMIZED;
			globox_event = GLOBOX_EVENT_MAXIMIZED;
			globox_event = GLOBOX_EVENT_FULLSCREEN;
			#endif
			break;
		}
		case XCB_CLIENT_MESSAGE:
		{
			xcb_client_message_event_t* message =
				(xcb_client_message_event_t*) xcb_event;

			if (message->type
				== platform->atoms[X11_ATOM_PROTOCOLS])
			{
				if (message->data.data32[0]
					== platform->atoms[X11_ATOM_DELETE_WINDOW])
				{
					// make the globox blocking function exit gracefully
					pthread_cond_broadcast(&(platform->cond_main));

					// make the event loop thread exit gracefully
					int posix_error = pthread_mutex_lock(&(platform->mutex_block));

					if (posix_error != 0)
					{
						globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
						break;
					}

					platform->closed = true;

					posix_error = pthread_mutex_unlock(&(platform->mutex_block));

					if (posix_error != 0)
					{
						globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
						break;
					}

					// tell the developer it's the end
					globox_event = GLOBOX_EVENT_CLOSED;
					break;
				}

				if (message->data.data32[0]
					== platform->atoms[X11_ATOM_SYNC_REQUEST])
				{
					// lock xsync mutex
					int posix_error = pthread_mutex_lock(&(platform->mutex_xsync));

					if (posix_error != 0)
					{
						globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
						break;
					}

					// save the last xsync value
					platform->xsync_value.hi = message->data.data32[3];
					platform->xsync_value.lo = message->data.data32[2];
					platform->xsync_configure = false;

					// unlock xsync mutex
					posix_error = pthread_mutex_unlock(&(platform->mutex_xsync));

					if (posix_error != 0)
					{
						globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
						break;
					}


					break;
				}
			}

			break;
		}
		case XCB_BUTTON_PRESS:
		{
			xcb_button_press_event_t* button_press =
				(xcb_button_press_event_t*) xcb_event;

			platform->saved_mouse_press_x = button_press->root_x;
			platform->saved_mouse_press_y = button_press->root_y;
			platform->saved_mouse_press_button = button_press->detail;

			break;
		}
		case XCB_BUTTON_RELEASE:
		{
			platform->saved_mouse_press_x = 0;
			platform->saved_mouse_press_y = 0;
			platform->saved_mouse_press_button = XCB_BUTTON_INDEX_ANY;

			break;
		}
		default:
		{
			break;
		}
	}

	// TODO handle non-programmatic window configuration
	// title & state changes
	// interactive mode
	// exposition

	globox_error_ok(error);
	return globox_event;
}

struct globox_config_features*
	globox_x11_common_init_features(
		struct globox* context,
		struct x11_platform* platform,
		struct globox_error_info* error)
{
	xcb_atom_t* atoms = platform->atoms;

	struct globox_config_features* features =
		malloc(sizeof (struct globox_config_features));

	if (features == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	features->count = 0;
	features->list =
		malloc(GLOBOX_FEATURE_COUNT * (sizeof (enum globox_feature)));

	if (features->list == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_INTERACTION;
	context->feature_interaction =
		malloc(sizeof (struct globox_feature_interaction));
	features->count += 1;

	if (context->feature_interaction == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// available if atoms valid
	if ((atoms[X11_ATOM_STATE] != XCB_NONE)
		&& (atoms[X11_ATOM_STATE_MAXIMIZED_HORIZONTAL] != XCB_NONE)
		&& (atoms[X11_ATOM_STATE_MAXIMIZED_VERTICAL] != XCB_NONE)
		&& (atoms[X11_ATOM_STATE_FULLSCREEN] != XCB_NONE)
		&& (atoms[X11_ATOM_STATE_HIDDEN] != XCB_NONE))
	{
		features->list[features->count] = GLOBOX_FEATURE_STATE;
		context->feature_state =
			malloc(sizeof (struct globox_feature_state));
		features->count += 1;

		if (context->feature_state == NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
			return NULL;
		}
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_TITLE;
	context->feature_title =
		malloc(sizeof (struct globox_feature_title));
	features->count += 1;

	if (context->feature_title == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// available if atom valid
	if (atoms[X11_ATOM_ICON] != XCB_NONE)
	{
		features->list[features->count] = GLOBOX_FEATURE_ICON;
		context->feature_icon =
			malloc(sizeof (struct globox_feature_icon));
		features->count += 1;

		if (context->feature_icon == NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
			return NULL;
		}
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_SIZE;
	context->feature_size =
		malloc(sizeof (struct globox_feature_size));
	features->count += 1;

	if (context->feature_size == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_POS;
	context->feature_pos =
		malloc(sizeof (struct globox_feature_pos));
	features->count += 1;

	if (context->feature_pos == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// available if atom valid
	if (atoms[X11_ATOM_HINTS_MOTIF] != XCB_NONE)
	{
		features->list[features->count] = GLOBOX_FEATURE_FRAME;
		context->feature_frame =
			malloc(sizeof (struct globox_feature_frame));
		features->count += 1;

		if (context->feature_frame == NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
			return NULL;
		}
	}

	// transparency is always available since globox requires 32-bit visuals
	features->list[features->count] = GLOBOX_FEATURE_BACKGROUND;
	context->feature_background =
		malloc(sizeof (struct globox_feature_background));
	features->count += 1;

	if (context->feature_background == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// available if the _NET_SUPPPORTED prop. has the _NET_WM_FRAME_DRAWN atom
	xcb_generic_error_t* xcb_error;

	xcb_get_property_cookie_t cookie =
		xcb_get_property(
			platform->conn,
			0,
			platform->root_win,
			atoms[X11_ATOM_NET_SUPPORTED],
			XCB_ATOM_ATOM,
			0,
			1024);

	xcb_get_property_reply_t* reply =
		xcb_get_property_reply(
			platform->conn,
			cookie,
			&xcb_error);

	if (xcb_error != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_PROP_GET);

		return features;
	}

	int net_atoms_count =
		xcb_get_property_value_length(reply);

	xcb_atom_t* net_atoms =
		xcb_get_property_value(reply);

	int i = 0;

	while (i < net_atoms_count)
	{
		if (net_atoms[i] == platform->atoms[X11_ATOM_FRAME_DRAWN])
		{
			features->list[features->count] = GLOBOX_FEATURE_VSYNC;
			context->feature_vsync =
				malloc(sizeof (struct globox_feature_vsync));
			features->count += 1;

			if (context->feature_vsync == NULL)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
				return NULL;
			}

			break;
		}

		++i;
	}

	free(reply);

	globox_error_ok(error);
	return features;
}

void globox_x11_common_feature_set_interaction(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	// lock mutex
	int posix_error = pthread_mutex_lock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	*(context->feature_interaction) = *config;
	x11_helpers_set_interaction(context, platform, error);

	// unlock mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_x11_common_feature_set_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	// lock mutex
	int posix_error = pthread_mutex_lock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	*(context->feature_state) = *config;
	x11_helpers_set_state(context, platform, error);

	// unlock mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_x11_common_feature_set_title(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	// lock mutex
	int posix_error = pthread_mutex_lock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	if (context->feature_title->title != NULL)
	{
		free((void*) context->feature_title->title);
	}

	context->feature_title->title = strdup(config->title);
	x11_helpers_set_title(context, platform, error);

	// unlock mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_x11_common_feature_set_icon(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	// lock mutex
	int posix_error = pthread_mutex_lock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	if (context->feature_icon->pixmap != NULL)
	{
		free(context->feature_icon->pixmap);
	}

	context->feature_icon->pixmap = malloc(config->len * 4);

	if (context->feature_icon->pixmap != NULL)
	{
		memcpy(context->feature_icon->pixmap, config->pixmap, config->len * 4);
		context->feature_icon->len = config->len;
	}
	else
	{
		context->feature_icon->len = 0;
	}

	x11_helpers_set_icon(context, platform, error);

	// unlock mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}
