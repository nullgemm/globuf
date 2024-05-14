#include "include/globuf.h"
#include "common/globuf_private.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct globuf* globuf_init(
	struct globuf_config_backend* config,
	struct globuf_error_info* error)
{
	// We allocate the context here and on the heap to avoid having to implement
	// a complex synchronization system just to reset a user-supplied structure
	// (this way this function is naturally thread-safe and reentrant).
	struct globuf* context = malloc(sizeof (struct globuf));

	// If the context allocation failed, we can't initialize the error system
	// and must therefore return NULL to communicate something went wrong.
	if (context == NULL)
	{
		return NULL;
	}

	// zero-initialize the context (don't worry, this is optimized by compilers)
	struct globuf zero = {0};
	*context = zero;

	struct globuf_config_features features =
	{
		.list = NULL,
		.count = 0,
	};
	context->features = features;

	context->backend_data = NULL;

	struct globuf_config_render render_callback =
	{
		.data = NULL,
		.callback = NULL,
	};
	context->render_callback = render_callback;

	struct globuf_config_events event_callbacks =
	{
		.data = NULL,
		.handler = NULL,
	};
	context->event_callbacks = event_callbacks;

	context->backend_callbacks = *config;

	struct globuf_rect expose =
	{
		.x = 0,
		.y = 0,
		.width = 0,
		.height = 0,
	};
	context->expose = expose;

	// initialize the error system
	globuf_error_init(context);

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

void globuf_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	context->backend_callbacks.clean(context, error);
	free(context);

	// error always set
}

void globuf_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	context->backend_callbacks.window_create(context, configs, count, callback, data, error);

	// error always set
}

void globuf_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	context->backend_callbacks.window_destroy(context, error);

	// error always set
}

void globuf_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	context->backend_callbacks.window_confirm(context, error);

	// error always set
}

void globuf_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
{
	context->backend_callbacks.window_start(context, error);

	// error always set
}

void globuf_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	context->backend_callbacks.window_block(context, error);

	// error always set
}

void globuf_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	context->backend_callbacks.window_stop(context, error);

	// error always set
}


void globuf_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	context->backend_callbacks.init_render(context, config, error);

	// error always set
}

void globuf_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	context->backend_callbacks.init_events(context, config, error);

	// error always set
}

enum globuf_event globuf_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	// error always set
	return context->backend_callbacks.handle_events(context, event, error);
}


struct globuf_config_features* globuf_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// error always set
	return context->backend_callbacks.init_features(context, error);
}

void globuf_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	context->backend_callbacks.feature_set_interaction(context, config, error);

	// error always set
}

void globuf_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	context->backend_callbacks.feature_set_state(context, config, error);

	// error always set
}

void globuf_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	context->backend_callbacks.feature_set_title(context, config, error);

	// error always set
}

void globuf_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	context->backend_callbacks.feature_set_icon(context, config, error);

	// error always set
}


// always available
unsigned globuf_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// error always set
	return context->backend_callbacks.get_width(context, error);
}

// always available
unsigned globuf_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// error always set
	return context->backend_callbacks.get_height(context, error);
}

// always available
struct globuf_rect globuf_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// error always set
	return context->backend_callbacks.get_expose(context, error);
}


void globuf_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	context->backend_callbacks.update_content(context, data, error);

	// error always set
}
