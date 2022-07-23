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
	struct x11_platform platform;
};

void globox_x11_software_init(
	struct globox* context)
{
	// allocate the backend
	struct x11_backend* backend = malloc(sizeof (struct x11_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct x11_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend = backend;

	// initialize the platform
	globox_x11_common_init(&(backend->platform));
}

void globox_x11_software_clean(
	struct globox* context)
{
	struct x11_backend* backend = context->backend;
	struct x11_platform* platform = &(backend->platform);

	// destroy mutex
	int error = pthread_mutex_destroy(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	// clean the platform
	globox_x11_common_clean(platform);

	// free the backend
	free(backend);
}

void globox_x11_software_window_create(
	struct globox* context)
{
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

struct globox_config_features* globox_x11_software_init_features(
	struct globox* context)
{
	struct x11_backend* backend = context->backend;
	struct x11_platform* platform = &(backend->platform);

	xcb_atom_t* atoms = platform->atoms;

	struct globox_config_features* features =
		malloc(sizeof (struct globox_config_features));

	if (features == NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_ALLOC);
		return;
	}

	features->count = 0;

	features->list =
		malloc(GLOBOX_FEATURE_COUNT * (sizeof (enum globox_feature)));

	if (features->list == NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_ALLOC);
		return;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_INTERACTION;
	features->count += 1;

	// available if atom valid
	if (atoms[X11_ATOM_STATE] != XCB_NONE)
	{
		features->list[features->count] = GLOBOX_FEATURE_STATE;
		features->count += 1;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_TITLE;
	features->count += 1;

	// available if atom valid
	if (atoms[X11_ATOM_ICON] != XCB_NONE)
	{
		features->list[features->count] = GLOBOX_FEATURE_ICON;
		features->count += 1;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_SIZE;
	features->count += 1;

	// always available
	features->list[features->count] = GLOBOX_FEATURE_POS;
	features->count += 1;

	// available if atom valid
	if (atoms[X11_ATOM_HINTS_MOTIF] != XCB_NONE)
	{
		features->list[features->count] = GLOBOX_FEATURE_FRAME;
		features->count += 1;
	}

	// transparency is always available since globox requires 32bit X11 visuals
	features->list[features->count] = GLOBOX_FEATURE_BACKGROUND;
	features->count += 1;

	// always available (emulated)
	features->list[features->count] = GLOBOX_FEATURE_VSYNC_CALLBACK;
	features->count += 1;
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

void globox_x11_software_set_feature(
	struct globox* context,
	struct globox_feature_request* request)
{
	switch (request->feature)
	{
		case GLOBOX_FEATURE_INTERACTION:
		{
			globox_x11_common_set_interaction(context, request);
			break;
		}
		case GLOBOX_FEATURE_STATE:
		{
			globox_x11_common_set_state(context, request);
			break;
		}
		case GLOBOX_FEATURE_TITLE:
		{
			globox_x11_common_set_title(context, request);
			break;
		}
		case GLOBOX_FEATURE_ICON:
		{
			globox_x11_common_set_icon(context, request);
			break;
		}
		case GLOBOX_FEATURE_SIZE:
		{
			globox_x11_common_set_size(context, request);
			break;
		}
		case GLOBOX_FEATURE_POS:
		{
			globox_x11_common_set_pos(context, request);
			break;
		}
		case GLOBOX_FEATURE_FRAME:
		{
			globox_x11_common_set_frame(context, request);
			break;
		}
		case GLOBOX_FEATURE_BACKGROUND:
		{
			globox_x11_common_set_background(context, request);
			break;
		}
		case GLOBOX_FEATURE_VSYNC_CALLBACK:
		{
			globox_x11_common_set_vsync_callback(context, request);
			break;
		}
		default:
		{
			break;
		}
	}
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
	config->set_feature = globox_x11_software_set_feature;
	config->update_content = globox_x11_software_update_content;
}
