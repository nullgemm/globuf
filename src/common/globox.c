#include "include/globox.h"
#include "common/globox_private.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct globox* globox_init(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	// We allocate the context here and on the heap to avoid having to implement
	// a complex synchronization system just to reset a user-supplied structure
	// (this way this function is naturally thread-safe and reentrant).
	struct globox* context = malloc(sizeof (struct globox));

	// If the context allocation failed, we can't initialize the error system
	// and must therefore return NULL to communicate something went wrong.
	if (context == NULL)
	{
		return NULL;
	}

	// zero-initialize the context (don't worry, this is optimized by compilers)
	struct globox zero = {0};
	*context = zero;

	struct globox_config_features features =
	{
		.list = NULL,
		.count = 0,
	};
	context->features = features;

	context->backend_data = NULL;

	struct globox_config_render render_callback =
	{
		.data = NULL,
		.callback = NULL,
	};
	context->render_callback = render_callback;

	struct globox_config_events event_callbacks =
	{
		.data = NULL,
		.handler = NULL,
	};
	context->event_callbacks = event_callbacks;

	context->backend_callbacks = *config;

	struct globox_rect expose =
	{
		.x = 0,
		.y = 0,
		.width = 0,
		.height = 0,
	};
	context->expose = expose;

	// initialize the error system
	globox_error_init(context);

	// initialize the feature pointers
	context->feature_interaction = NULL;
	context->feature_state = NULL;
	context->feature_title = NULL;
	context->feature_icon = NULL;
	context->feature_size = NULL;
	context->feature_pos = NULL;
	context->feature_frame = NULL;
	context->feature_background = NULL;
	context->feature_vsync = NULL;

	// call the backend's init function
	context->backend_callbacks.init(context, error);

	// error always set
	return context;
}

void globox_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	context->backend_callbacks.clean(context, error);
	free(context);

	// error always set
}

void globox_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	context->backend_callbacks.window_create(context, configs, count, callback, data, error);

	// error always set
}

void globox_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	context->backend_callbacks.window_destroy(context, error);

	// error always set
}

void globox_window_confirm(
	struct globox* context,
	struct globox_error_info* error)
{
	context->backend_callbacks.window_confirm(context, error);

	// error always set
}

void globox_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	context->backend_callbacks.window_start(context, error);

	// error always set
}

void globox_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	context->backend_callbacks.window_block(context, error);

	// error always set
}

void globox_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	context->backend_callbacks.window_stop(context, error);

	// error always set
}


void globox_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	context->backend_callbacks.init_render(context, config, error);

	// error always set
}

void globox_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	context->backend_callbacks.init_events(context, config, error);

	// error always set
}

enum globox_event globox_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	// error always set
	return context->backend_callbacks.handle_events(context, event, error);
}


struct globox_config_features* globox_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	// error always set
	return context->backend_callbacks.init_features(context, error);
}

void globox_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	context->backend_callbacks.feature_set_interaction(context, config, error);

	// error always set
}

void globox_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	context->backend_callbacks.feature_set_state(context, config, error);

	// error always set
}

void globox_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	context->backend_callbacks.feature_set_title(context, config, error);

	// error always set
}

void globox_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	context->backend_callbacks.feature_set_icon(context, config, error);

	// error always set
}


// always available
unsigned globox_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	// error always set
	return context->backend_callbacks.get_width(context, error);
}

// always available
unsigned globox_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	// error always set
	return context->backend_callbacks.get_height(context, error);
}

// always available
struct globox_rect globox_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	// error always set
	return context->backend_callbacks.get_expose(context, error);
}


void globox_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	context->backend_callbacks.update_content(context, data, error);

	// error always set
}
