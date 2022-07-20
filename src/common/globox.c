#include "include/globox.h"
#include "common/globox_private.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

void globox_init(
	struct globox* context)
{
	// TODO mutex
	// zero-initialize the context (don't worry this is optimized by compilers)
	struct globox zero = {0};
	*context = zero;

	// init error system
	globox_error_init(context);

	if (globox_error_catch(context))
	{
		return;
	}

	// the backend init callback is called when setting the
	// backend function pointers in `globox_init_backend`
}

void globox_clean(
	struct globox* context)
{
	// TODO mutex
	context->backend_callbacks->clean(context);
}

void globox_window_create(
	struct globox* context)
{
	// TODO mutex
	context->backend_callbacks->window_create(context);
}

void globox_window_destroy(
	struct globox* context)
{
	// TODO mutex
	context->backend_callbacks->window_destroy(context);
}

void globox_window_start(
	struct globox* context)
{
	// TODO mutex
	context->backend_callbacks->window_start(context);
}

void globox_window_block(
	struct globox* context)
{
	// TODO mutex
	context->backend_callbacks->window_block(context);
}

void globox_window_stop(
	struct globox* context)
{
	// TODO mutex
	context->backend_callbacks->window_stop(context);
}

struct globox_config_features* globox_init_features(
	struct globox* context)
{
	// TODO mutex
	context->backend_callbacks->init_features(context);
}

void globox_init_events(
	struct globox* context,
	struct globox_config_events* config)
{
	// TODO mutex
	context->backend_callbacks->init_events(context, config);
}

enum globox_event globox_handle_events(
	struct globox* context,
	void* event)
{
	// TODO mutex
	context->backend_callbacks->handle_events(context, event);
}

void globox_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config)
{
	// TODO mutex
	context->backend_callbacks->set_interaction(context, config);
}

void globox_set_state(
	struct globox* context,
	struct globox_feature_state* config)
{
	// TODO mutex
	context->backend_callbacks->set_state(context, config);
}

void globox_set_title(
	struct globox* context,
	struct globox_feature_title* config)
{
	// TODO mutex
	context->backend_callbacks->set_title(context, config);
}

void globox_set_icon(
	struct globox* context,
	struct globox_feature_icon* config)
{
	// TODO mutex
	context->backend_callbacks->set_icon(context, config);
}

void globox_set_size(
	struct globox* context,
	struct globox_feature_size* config)
{
	// TODO mutex
	context->backend_callbacks->set_size(context, config);
}

void globox_set_pos(
	struct globox* context,
	struct globox_feature_pos* config)
{
	// TODO mutex
	context->backend_callbacks->set_pos(context, config);
}

void globox_set_frame(
	struct globox* context,
	struct globox_feature_frame* config)
{
	// TODO mutex
	context->backend_callbacks->set_frame(context, config);
}

void globox_set_background(
	struct globox* context,
	struct globox_feature_background* config)
{
	// TODO mutex
	context->backend_callbacks->set_background(context, config);
}

void globox_set_vsync_callback(
	struct globox* context,
	struct globox_feature_vsync_callback* config)
{
	// TODO mutex
	context->backend_callbacks->set_vsync_callback(context, config);
}

void globox_update_content(
	struct globox* context,
	void* data)
{
	// TODO mutex
	context->backend_callbacks->update_content(context, data);
}

void globox_init_backend(
	struct globox* context,
	struct globox_config_backend* config)
{
	// TODO mutex
	// same here, this is optimized by compilers
	context->backend_callbacks = *config;
	// call the backend's init function
	context->backend_callbacks->init(context);
}

unsigned globox_get_width(struct globox* context)
{
	// TODO mutex
	return context->size.width;
}

unsigned globox_get_height(struct globox* context)
{
	// TODO mutex
	return context->size.height;
}
