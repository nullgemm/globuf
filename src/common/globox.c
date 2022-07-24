#include "include/globox.h"
#include "common/globox_private.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct globox* globox_init(
	struct globox_config_backend* config)
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

	// copy the backend function pointers (same here, optimized by compilers)
	context->backend_callbacks = *config;

	// initialize the error system
	globox_error_init(context);

	if (globox_error_catch(context))
	{
		return context;
	}

	// call the backend's init function
	context->backend_callbacks.init(context);

	return context;
}

void globox_clean(
	struct globox* context)
{
	context->backend_callbacks.clean(context);
}

void globox_window_create(
	struct globox* context)
{
	context->backend_callbacks.window_create(context);
}

void globox_window_destroy(
	struct globox* context)
{
	context->backend_callbacks.window_destroy(context);
}

void globox_window_start(
	struct globox* context)
{
	context->backend_callbacks.window_start(context);
}

void globox_window_block(
	struct globox* context)
{
	context->backend_callbacks.window_block(context);
}

void globox_window_stop(
	struct globox* context)
{
	context->backend_callbacks.window_stop(context);
}


void globox_init_events(
	struct globox* context,
	struct globox_config_events* config)
{
	context->backend_callbacks.init_events(context, config);
}

enum globox_event globox_handle_events(
	struct globox* context,
	void* event)
{
	return context->backend_callbacks.handle_events(context, event);
}

struct globox_config_features* globox_init_features(
	struct globox* context)
{
	return context->backend_callbacks.init_features(context);
}

void globox_set_feature_data(
	struct globox* context,
	struct globox_feature_data* feature_data)
{
	context->backend_callbacks.set_feature_data(context, feature_data);
}


void globox_update_content(
	struct globox* context,
	void* data)
{
	context->backend_callbacks.update_content(context, data);
}

unsigned globox_get_width(struct globox* context)
{
	struct globox_feature_size* size =
		context->feature_data[GLOBOX_FEATURE_SIZE].config;

	return size->width;
}

unsigned globox_get_height(struct globox* context)
{
	struct globox_feature_size* size =
		context->feature_data[GLOBOX_FEATURE_SIZE].config;

	return size->height;
}
