#include "include/globox.h"
#include "include/globox_software.h"
#include "include/globox_appkit_software.h"

#include "common/globox_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"
#include "appkit/appkit_software.h"
#include "appkit/appkit_software_helpers.h"

#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>

void globox_appkit_software_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct appkit_software_backend* backend = malloc(sizeof (struct appkit_software_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct appkit_software_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize values that can be initialized explicitly
#if 0
	backend->shared_pixmaps = false;
#endif

	// initialize the platform
	struct appkit_platform* platform = &(backend->platform);
	globox_appkit_common_init(context, platform, error);

	// error always set
}

void globox_appkit_software_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_clean(context, platform, error);
}

void globox_appkit_software_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// configure features here
	globox_appkit_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_appkit_common_window_create(
		context,
		platform,
		configs,
		count,
		callback,
		data,
		error);
}

void globox_appkit_software_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_window_destroy(context, platform, error);
}

void globox_appkit_software_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_window_start(context, platform, error);
}

void globox_appkit_software_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_window_block(context, platform, error);
}

void globox_appkit_software_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_window_stop(context, platform, error);
}


void globox_appkit_software_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_init_render(context, platform, config, error);
}

void globox_appkit_software_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_init_events(context, platform, config, error);
}

enum globox_event globox_appkit_software_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	return globox_appkit_common_handle_events(context, platform, event, error);
}


struct globox_config_features* globox_appkit_software_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	return globox_appkit_common_init_features(context, platform, error);
}

void globox_appkit_software_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_feature_set_interaction(context, platform, config, error);
}

void globox_appkit_software_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_feature_set_state(context, platform, config, error);
}

void globox_appkit_software_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_feature_set_title(context, platform, config, error);
}

void globox_appkit_software_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	globox_appkit_common_feature_set_icon(context, platform, config, error);
}


unsigned globox_appkit_software_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_width(context, platform, error);
}

unsigned globox_appkit_software_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_height(context, platform, error);
}

struct globox_rect globox_appkit_software_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_expose(context, platform, error);
}


void globox_appkit_software_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	struct globox_update_software* update = data;

	globox_error_ok(error);
}


void globox_prepare_init_appkit_software(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	config->data = NULL;
	config->init = globox_appkit_software_init;
	config->clean = globox_appkit_software_clean;
	config->window_create = globox_appkit_software_window_create;
	config->window_destroy = globox_appkit_software_window_destroy;
	config->window_start = globox_appkit_software_window_start;
	config->window_block = globox_appkit_software_window_block;
	config->window_stop = globox_appkit_software_window_stop;
	config->init_render = globox_appkit_software_init_render;
	config->init_events = globox_appkit_software_init_events;
	config->handle_events = globox_appkit_software_handle_events;
	config->init_features = globox_appkit_software_init_features;
	config->feature_set_interaction = globox_appkit_software_feature_set_interaction;
	config->feature_set_state = globox_appkit_software_feature_set_state;
	config->feature_set_title = globox_appkit_software_feature_set_title;
	config->feature_set_icon = globox_appkit_software_feature_set_icon;
	config->get_width = globox_appkit_software_get_width;
	config->get_height = globox_appkit_software_get_height;
	config->get_expose = globox_appkit_software_get_expose;
	config->update_content = globox_appkit_software_update_content;

	globox_error_ok(error);
}
