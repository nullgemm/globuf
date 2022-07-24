#include "include/globox.h"
#include "include/globox_software.h"
#include "include/globox_x11_software.h"

#include "common/globox_private.h"
#include "x11/x11_software.h"
#include "x11/x11_common.h"

#include <pthread.h>
#include <stdlib.h>
#include <xcb/xcb.h>

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
	context->backend_data = backend;

	// initialize the platform
	globox_x11_common_init(context, &(backend->platform));
}

void globox_x11_software_clean(
	struct globox* context)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// clean the platform
	globox_x11_common_clean(context, platform);

	// free the backend
	free(backend);
}

void globox_x11_software_window_create(
	struct globox* context)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_create(context, platform);

	// no extra failure check at the moment

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}
}

void globox_x11_software_window_destroy(
	struct globox* context)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_destroy(context, platform);

	// no extra failure check at the moment

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}
}

void globox_x11_software_window_start(
	struct globox* context)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_start(context, platform);

	// no extra failure check at the moment

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}
}

void globox_x11_software_window_block(
	struct globox* context)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock block mutex
	error = pthread_mutex_lock(&(platform->mutex_block));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_block(context, platform);

	// no extra failure check at the moment

	// unlock block mutex
	error = pthread_mutex_unlock(&(platform->mutex_block));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}
}

void globox_x11_software_window_stop(
	struct globox* context)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_stop(context, platform);

	// no extra failure check at the moment

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}
}


// TODO
void globox_x11_software_init_events(
	struct globox* context,
	struct globox_config_events* config)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_init_events(context, platform, config);

	// no extra failure check at the moment

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}
}

// TODO
enum globox_event globox_x11_software_handle_events(
	struct globox* context,
	void* event)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	enum globox_event out =
		globox_x11_common_handle_events(
			context,
			platform,
			event);

	// no extra failure check at the moment

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	return out;
}

struct globox_config_features* globox_x11_software_init_features(
	struct globox* context)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return NULL;
	}

	// run common X11 helper
	struct globox_config_features* features =
		globox_x11_common_init_features(context, platform);

	// no extra failure check at the moment

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return features;
	}

	// return the newly created features info structure
	return features;
}

void globox_x11_software_set_feature_data(
	struct globox* context,
	struct globox_feature_data* feature_data)
{
	int error = 0;
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock main mutex
	error = pthread_mutex_lock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_set_feature_data(context, platform, feature_data);

	// no extra failure check at the moment

	// unlock main mutex
	error = pthread_mutex_unlock(&(platform->mutex_main));

	if (error != 0)
	{
		globox_error_throw(context, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}
}


void globox_x11_software_update_content(
	struct globox* context,
	void* data)
{
}

void globox_prepare_init_x11_software(
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
	config->set_feature_data = globox_x11_software_set_feature_data;
	config->update_content = globox_x11_software_update_content;
}
