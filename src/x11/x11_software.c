#include "include/globox.h"
#include "include/globox_software.h"
#include "include/globox_x11_software.h"

#include "common/globox_private.h"
#include "x11/x11_software.h"
#include "x11/x11_common.h"

#include <pthread.h>
#include <stdlib.h>
#include <xcb.h>

struct x11_backend
{
	struct x11_platform* x11_platform;

	xcb_connection_t* conn;
};

void globox_x11_software_init(
	struct globox* context)
{
	// reference the backend data in the globox context
	struct x11_backend* backend = malloc(sizeof (struct x11_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_ALLOC);
		return;
	}

	context->backend_data = backend;

	// reference the platform data in the backend data
	struct x11_platform* platform;
	globox_x11_common_init(&platform);

	if (globox_error_catch(context))
	{
		return;
	}

	backend->x11_platform = platform;

	// open a connection to the X server
	backend->conn = xcb_connect(NULL, &(platform->screen_id));
	int error = xcb_connection_has_error(backend->conn);

	if (error > 0)
	{
		xcb_disconnect(backend->conn);
		globox_error_throw(context, GLOBOX_ERROR_X11_CONN);
		return;
	}

	// get the screen obj from the id the dirty way (there is no other option)
	const struct xcb_setup_t* setup = xcb_get_setup(backend->conn);
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);

	for (int i = 0; i < platform->screen_id; ++i)
	{
		xcb_screen_next(&iter);
	}

	platform->screen_obj = iter.data;

	// get the root window from the screen object
	platform->root_win = platform->screen_obj->root;
}

void globox_x11_software_clean(
	struct globox* context)
{
}

void globox_x11_software_window_create(
	struct globox* context)
{
	// TODO 3
}

void globox_x11_software_window_destroy(
	struct globox* context)
{
}

void globox_x11_software_window_start(
	struct globox* context)
{
}

void globox_x11_software_window_block(
	struct globox* context)
{
}

void globox_x11_software_window_stop(
	struct globox* context)
{
}

void globox_x11_software_init_features(
	struct globox* context,
	struct globox_config_features* config)
{
	// TODO 1
}

void globox_x11_software_init_events(
	struct globox* context,
	void (*handler)(void* data, void* event))
{
}

enum globox_event globox_x11_software_handle_events(
	struct globox* context,
	void* event)
{
}

void globox_x11_software_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config)
{
}

void globox_x11_software_set_state(
	struct globox* context,
	struct globox_feature_state* config)
{
}

void globox_x11_software_set_title(
	struct globox* context,
	struct globox_feature_title* config)
{
}

void globox_x11_software_set_icon(
	struct globox* context,
	struct globox_feature_icon* config)
{
}

void globox_x11_software_set_size(
	struct globox* context,
	struct globox_feature_size* config)
{
}

void globox_x11_software_set_pos(
	struct globox* context,
	struct globox_feature_pos* config)
{
}

void globox_x11_software_set_frame(
	struct globox* context,
	struct globox_feature_frame* config)
{
}

void globox_x11_software_set_background(
	struct globox* context,
	struct globox_feature_background* config)
{
	// TODO 2
}

void globox_x11_software_set_vsync_callback(
	struct globox* context,
	struct globox_feature_vsync_callback* config)
{
}

void globox_x11_software_update_content(
	struct globox* context,
	void* data)
{
}

void globox_prepare_init_x11_software(
	struct globox* context,
	struct globox_config_backend* config)
{
	config->data = NULL;
	config->init = globox_x11_software_init;
	config->clean = globox_x11_software_clean;
	config->window_create = globox_x11_software_window_create;
	config->window_destroy = globox_x11_software_window_destroy;
	config->window_start = globox_x11_software_window_start;
	config->window_block = globox_x11_software_window_block;
	config->window_stop = globox_x11_software_window_stop;
	config->init_features = globox_x11_software_init_features;
	config->init_events = globox_x11_software_init_events;
	config->handle_events = globox_x11_software_handle_events;
	config->set_interaction = globox_x11_software_set_interaction;
	config->set_state = globox_x11_software_set_state;
	config->set_title = globox_x11_software_set_title;
	config->set_icon = globox_x11_software_set_icon;
	config->set_size = globox_x11_software_set_size;
	config->set_pos = globox_x11_software_set_pos;
	config->set_frame = globox_x11_software_set_frame;
	config->set_background = globox_x11_software_set_background;
	config->set_vsync_callback = globox_x11_software_set_vsync_callback;
	config->update_content = globox_x11_software_update_content;
}
