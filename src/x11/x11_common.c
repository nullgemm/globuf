#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_common.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>

void globox_x11_common_init(
	struct globox* context,
	struct x11_platform* platform)
{
	int error;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;

	// init pthread mutex attributes
	error = pthread_mutexattr_init(&mutex_attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_ATTR_INIT);
		return;
	}

	// set pthread mutex type (error checking for now)
	error =
		pthread_mutexattr_settype(
			&mutex_attr,
			PTHREAD_MUTEX_ERRORCHECK);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_ATTR_SETTYPE);
		return;
	}

	// init pthread mutex (main)
	error = pthread_mutex_init(&(platform->mutex_main), &mutex_attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// init pthread mutex (block)
	error = pthread_mutex_init(&(platform->mutex_block), &mutex_attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// destroy pthread mutex attributes
	error = pthread_mutexattr_destroy(&mutex_attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_ATTR_DESTROY);
		return;
	}

	// init pthread cond attributes
	error = pthread_condattr_init(&cond_attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_COND_ATTR_INIT);
		return;
	}

	// set pthread cond clock
	error =
		pthread_condattr_setclock(
			&cond_attr,
			CLOCK_MONOTONIC);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_COND_ATTR_SETTYPE);
		return;
	}

	// init pthread cond
	error = pthread_cond_init(&(platform->cond_main), &cond_attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_COND_INIT);
		return;
	}

	// destroy pthread cond attributes
	error = pthread_condattr_destroy(&cond_attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_COND_ATTR_DESTROY);
		return;
	}

	// initialize the "closed" boolean
	platform->closed = false;

	// open a connection to the X server
	platform->conn = xcb_connect(NULL, &(platform->screen_id));
	error = xcb_connection_has_error(platform->conn);

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
	xcb_generic_error_t* error_atom;
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
			&error_atom);

		if (error_atom != NULL)
		{
			globox_error_throw(context, GLOBOX_ERROR_X11_ATOM_GET);
			return;
		}

		platform->atoms[i] = reply->atom;
		free(reply);
	}
}

void globox_x11_common_clean(
	struct globox* context,
	struct x11_platform* platform)
{
	int error = 0;

	// close the connection to the X server
	xcb_disconnect(platform->conn);

	// lock block mutex to be able to destroy the cond
	error = pthread_mutex_lock(&(platform->mutex_block));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy pthread cond
	error = pthread_cond_destroy(&(platform->cond_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_COND_DESTROY);
		return;
	}

	// unlock block mutex
	error = pthread_mutex_unlock(&(platform->mutex_block));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// destroy pthread mutex (block)
	error = pthread_mutex_destroy(&(platform->mutex_block));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	// destroy pthread mutex (main)
	error = pthread_mutex_destroy(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}
}

void globox_x11_common_window_create(
	struct globox* context,
	struct x11_platform* platform,
	void** features)
{
	struct globox_feature_background* background =
		context->feature_background;

	// prepare window attributes
	if (background->background != GLOBOX_BACKGROUND_OPAQUE)
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
	struct globox_feature_pos* pos = context->feature_pos;
	struct globox_feature_size* size = context->feature_size;

	platform->win = xcb_generate_id(platform->conn);

	xcb_void_cookie_t cookie =
		xcb_create_window(
			platform->conn,
			platform->visual_depth,
			platform->win,
			platform->root_win,
			pos->x,
			pos->y,
			size->width,
			size->height,
			0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			platform->visual_id,
			platform->attr_mask,
			platform->attr_val);

	xcb_generic_error_t* error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (error != NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_X11_WIN_CREATE);
		return;
	}

	// configure the window
	// support the window deletion protocol
	cookie =
		xcb_change_property_checked(
			platform->conn,
			XCB_PROP_MODE_REPLACE,
			platform->win,
			platform->atoms[X11_ATOM_PROTOCOLS],
			4,
			32,
			1,
			&(platform->atoms[X11_ATOM_DELETE_WINDOW]));

	error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (error != NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_X11_PROP_CHANGE);
		return;
	}

	// select the correct type of window frame
	struct globox_feature_frame* frame =
		context->feature_frame;

	if ((frame != NULL) && (frame->frame == false))
	{
		uint32_t motif_hints[5] =
		{
			2, // flags
			0, // functions
			0, // decorations
			0, // input_mode
			0, // status
		};

		cookie =
			xcb_change_property(
				platform->conn,
				XCB_PROP_MODE_REPLACE,
				platform->win,
				platform->atoms[X11_ATOM_HINTS_MOTIF],
				platform->atoms[X11_ATOM_HINTS_MOTIF],
				32,
				5,
				motif_hints);

		error =
			xcb_request_check(
				platform->conn,
				cookie);

		if (error != NULL)
		{
			globox_error_throw(context, GLOBOX_ERROR_X11_PROP_CHANGE);
			return;
		}
	}

	// select the correct type of window background
	if (background->background == GLOBOX_BACKGROUND_BLURRED)
	{
		// kde blur
		cookie =
			xcb_change_property(
				platform->conn,
				XCB_PROP_MODE_REPLACE,
				platform->win,
				platform->atoms[X11_ATOM_BLUR_KDE],
				XCB_ATOM_CARDINAL,
				32,
				0,
				NULL);

		error =
			xcb_request_check(
				platform->conn,
				cookie);

		if (error != NULL)
		{
			globox_error_throw(context, GLOBOX_ERROR_X11_PROP_CHANGE);
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

		error =
			xcb_request_check(
				platform->conn,
				cookie);

		if (error != NULL)
		{
			globox_error_throw(context, GLOBOX_ERROR_X11_PROP_CHANGE);
			return;
		}
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

	error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (error != NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_X11_ATTR_CHANGE);
		return;
	}
}

void globox_x11_common_window_destroy(
	struct globox* context,
	struct x11_platform* platform)
{
	xcb_void_cookie_t cookie =
		xcb_destroy_window(
			platform->conn,
			platform->win);

	xcb_generic_error_t* error =
		xcb_request_check(
			platform->conn,
			cookie);

	if (error != NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_X11_WIN_DESTROY);
		return;
	}
}

void globox_x11_common_window_start(
	struct globox* context,
	struct x11_platform* platform)
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
		globox_error_throw(context, GLOBOX_ERROR_X11_WIN_MAP);
		return;
	}

	// flush
	int error_flush = xcb_flush(platform->conn);

	if (error_flush <= 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_X11_FLUSH);
		return;
	}
}

void globox_x11_common_window_block(
	struct globox* context,
	struct x11_platform* platform)
{
	int error = pthread_cond_wait(&(platform->cond_main), &(platform->mutex_block));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_COND_WAIT);
		return;
	}
}

void globox_x11_common_window_stop(
	struct globox* context,
	struct x11_platform* platform)
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
		globox_error_throw(context, GLOBOX_ERROR_X11_EVENT_SEND);
		return;
	}

	// flush
	int error_flush = xcb_flush(platform->conn);

	if (error_flush <= 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_X11_FLUSH);
		return;
	}
}


void* x11_event_loop(void* data)
{
	struct x11_thread_event_loop_data* thread_event_loop_data = data;

	struct globox* context = thread_event_loop_data->globox;
	struct x11_platform* platform = thread_event_loop_data->platform;

	int error;

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return NULL;
	}

	xcb_generic_event_t* event;

	while (platform->closed == false)
	{
		// unlock main mutex
		error = pthread_mutex_unlock(&(platform->mutex_main));

		if (error != 0)
		{
			globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			return NULL;
		}

		// block until we get an event
		event = xcb_wait_for_event(platform->conn);

		// lock main mutex
		error = pthread_mutex_lock(&(platform->mutex_main));

		if (error != 0)
		{
			globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			return NULL;
		}

		// IO error
		if (event == NULL)
		{
			globox_error_throw(context, GLOBOX_ERROR_X11_EVENT_WAIT);
			return NULL;
		}

		// unlock main mutex
		error = pthread_mutex_unlock(&(platform->mutex_main));

		if (error != 0)
		{
			globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			return NULL;
		}

		// run developer callback
		context->event_callbacks.handler(
			context->event_callbacks.data,
			event);

		// lock main mutex
		error = pthread_mutex_lock(&(platform->mutex_main));

		if (error != 0)
		{
			globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			return NULL;
		}

		free(event);
	}

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return NULL;
	}

	return NULL;
}

void globox_x11_common_init_events(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_config_events* config)
{
	// set the event callback
	context->event_callbacks = *config;

	// start the event loop in a new thread
	// init thread attributes
	int error;
	pthread_attr_t attr;

	error = pthread_attr_init(&attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT);
		return;
	}

	error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_THREAD_ATTR_DETACH);
		return;
	}

	// init thread function data
	struct x11_thread_event_loop_data data =
	{
		.globox = context,
		.platform = platform,
	};

	platform->thread_event_loop_data = data;

	// start function in a new thread
	error =
		pthread_create(
			&(platform->thread_event_loop),
			&attr,
			x11_event_loop,
			&(platform->thread_event_loop_data));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_THREAD_CREATE);
		return;
	}

	// destroy the attributes
	error = pthread_attr_destroy(&attr);

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY);
		return;
	}
}

enum globox_event globox_x11_common_handle_events(
	struct globox* context,
	struct x11_platform* platform,
	void* event)
{
	// process system events
	enum globox_event globox_event = GLOBOX_EVENT_INVALID;
	xcb_generic_event_t* xcb_event = event;

#if 0
	// TODO handle and return all these
	globox_event = GLOBOX_EVENT_RESTORED;
	globox_event = GLOBOX_EVENT_MINIMIZED;
	globox_event = GLOBOX_EVENT_MAXIMIZED;
	globox_event = GLOBOX_EVENT_FULLSCREEN;
	globox_event = GLOBOX_EVENT_MOVED;
	globox_event = GLOBOX_EVENT_RESIZED_N;
	globox_event = GLOBOX_EVENT_RESIZED_NW;
	globox_event = GLOBOX_EVENT_RESIZED_W;
	globox_event = GLOBOX_EVENT_RESIZED_SW;
	globox_event = GLOBOX_EVENT_RESIZED_S;
	globox_event = GLOBOX_EVENT_RESIZED_SE;
	globox_event = GLOBOX_EVENT_RESIZED_E;
	globox_event = GLOBOX_EVENT_RESIZED_NE;
	// TODO implement these for a first test
	globox_event = GLOBOX_EVENT_CONTENT_DAMAGED;
	globox_event = GLOBOX_EVENT_DISPLAY_CHANGED;
#endif

	switch (xcb_event->response_type & ~0x80)
	{
		case XCB_EXPOSE:
		{
			break;
		}
		case XCB_CONFIGURE_NOTIFY:
		{
			break;
		}
		case XCB_PROPERTY_NOTIFY:
		{
			break;
		}
		case XCB_CLIENT_MESSAGE:
		{
			xcb_client_message_event_t* delete =
				(xcb_client_message_event_t*) xcb_event;

			if (delete->data.data32[0]
				== platform->atoms[X11_ATOM_DELETE_WINDOW])
			{
				// make the globox blocking function exit gracefully
				pthread_cond_broadcast(&(platform->cond_main));
				// make the event loop thread exit gracefully
				platform->closed = true;
				// tell the developer it's the end
				globox_event = GLOBOX_EVENT_CLOSED;
			}

			break;
		}
		default:
		{
			break;
		}
	}

	return globox_event;
}

struct globox_config_features*
	globox_x11_common_init_features(
		struct globox* context,
		struct x11_platform* platform)
{
	xcb_atom_t* atoms = platform->atoms;

	struct globox_config_features* features =
		malloc(sizeof (struct globox_config_features));

	if (features == NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	features->count = 0;
	features->list =
		malloc(GLOBOX_FEATURE_COUNT * (sizeof (enum globox_feature)));

	if (features->list == NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_INTERACTION;
	context->feature_interaction =
		malloc(sizeof (struct globox_feature_interaction));
	features->count += 1;

	// available if atom valid
	if (atoms[X11_ATOM_STATE] != XCB_NONE)
	{
		features->list[features->count] = GLOBOX_FEATURE_STATE;
		context->feature_state =
			malloc(sizeof (struct globox_feature_state));
		features->count += 1;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_TITLE;
	context->feature_title =
		malloc(sizeof (struct globox_feature_title));
	features->count += 1;

	// available if atom valid
	if (atoms[X11_ATOM_ICON] != XCB_NONE)
	{
		features->list[features->count] = GLOBOX_FEATURE_ICON;
		context->feature_icon =
			malloc(sizeof (struct globox_feature_icon));
		features->count += 1;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_SIZE;
	context->feature_size =
		malloc(sizeof (struct globox_feature_size));
	features->count += 1;

	// always available
	features->list[features->count] = GLOBOX_FEATURE_POS;
	context->feature_pos =
		malloc(sizeof (struct globox_feature_pos));
	features->count += 1;

	// available if atom valid
	if (atoms[X11_ATOM_HINTS_MOTIF] != XCB_NONE)
	{
		features->list[features->count] = GLOBOX_FEATURE_FRAME;
		context->feature_frame =
			malloc(sizeof (struct globox_feature_frame));
		features->count += 1;
	}

	// transparency is always available since globox requires 32bit X11 visuals
	features->list[features->count] = GLOBOX_FEATURE_BACKGROUND;
	context->feature_background =
		malloc(sizeof (struct globox_feature_background));
	features->count += 1;

	// always available (emulated)
	features->list[features->count] = GLOBOX_FEATURE_VSYNC_CALLBACK;
	context->feature_vsync_callback =
		malloc(sizeof (struct globox_feature_vsync_callback));
	features->count += 1;

	return features;
}

// TODO
// initialize when appropriate, set when appropriate
// return an error if the configuration is refused
void globox_x11_common_feature_set_interaction(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_interaction* config)
{
	if (context->feature_interaction == NULL)
	{
		return;
	}
}

void globox_x11_common_feature_set_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_state* config)
{
	if (context->feature_state == NULL)
	{
		return;
	}
}

void globox_x11_common_feature_set_title(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_title* config)
{
	if (context->feature_title == NULL)
	{
		return;
	}
}

void globox_x11_common_feature_set_icon(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_icon* config)
{
	if (context->feature_icon == NULL)
	{
		return;
	}
}

void globox_x11_common_feature_set_size(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_size* config)
{
	if (context->feature_size == NULL)
	{
		return;
	}
}

void globox_x11_common_feature_set_pos(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_pos* config)
{
	if (context->feature_pos == NULL)
	{
		return;
	}
}

void globox_x11_common_feature_set_frame(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_frame* config)
{
	if (context->feature_frame == NULL)
	{
		return;
	}
}

void globox_x11_common_feature_set_background(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_background* config)
{
	if (context->feature_background == NULL)
	{
		return;
	}
}

void globox_x11_common_feature_set_vsync_callback(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_feature_vsync_callback* config)
{
	if (context->feature_vsync_callback == NULL)
	{
		return;
	}
}
