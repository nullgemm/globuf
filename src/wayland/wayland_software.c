#define _XOPEN_SOURCE 700

#include "include/globuf.h"
#include "include/globuf_software.h"
#include "include/globuf_wayland_software.h"

#include "common/globuf_private.h"
#include "wayland/wayland_common.h"
#include "wayland/wayland_common_helpers.h"
#include "wayland/wayland_common_registry.h"
#include "wayland/wayland_software.h"
#include "wayland/wayland_software_helpers.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>

void globuf_wayland_software_init(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// allocate the backend
	struct wayland_software_backend* backend = malloc(sizeof (struct wayland_software_backend));

	if (backend == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct wayland_software_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize values that can be initialized explicitly
	backend->shm = NULL;
	backend->buffer = NULL;
	backend->buffer_len = 0;

	// buffer listener
	struct wl_buffer_listener listener_buffer =
	{
		.release = wayland_helpers_buffer_release,
	};

	backend->listener_buffer = listener_buffer;

	// initialize the platform
	globuf_wayland_common_init(context, &(backend->platform), error);

	// error always set
}

void globuf_wayland_software_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// clean the platform
	globuf_wayland_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globuf_wayland_software_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	globuf_wayland_helpers_features_init(context, platform, configs, count, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// run common wayland helper
	globuf_wayland_common_window_create(context, platform, configs, count, callback, data, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_software_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_window_destroy(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy shm
	if (backend->shm != NULL)
	{
		wl_shm_destroy(backend->shm);
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_software_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// add backend-specific registry handlers
	bool error_reg =
		globuf_add_wayland_registry_handler(
			context,
			wayland_helpers_callback_registry_shm,
			context);

	if (error_reg == false)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_REGISTRY_CALLBACK);
		return;
	}

	// run common wayland helper
	globuf_wayland_common_window_confirm(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	if (backend->shm == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_SHM_GET);
		return;
	}

	// error always set
}

void globuf_wayland_software_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_wayland_software_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper (mutex locked when unblocked)
	globuf_wayland_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_wayland_software_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globuf_wayland_software_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_wayland_software_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globuf_event globuf_wayland_software_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	enum globuf_event out =
		globuf_wayland_common_handle_events(
			context,
			platform,
			event,
			error);

	// error always set
	return out;
}


struct globuf_config_features* globuf_wayland_software_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	struct globuf_config_features* features =
		globuf_wayland_common_init_features(context, platform, error);

	// return the newly created features info structure
	// error always set
	return features;
}

void globuf_wayland_software_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globuf_wayland_software_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globuf_wayland_software_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globuf_wayland_software_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globuf_wayland_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globuf_wayland_software_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globuf_wayland_common_get_width(context, platform, error);
}

unsigned globuf_wayland_software_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globuf_wayland_common_get_height(context, platform, error);
}

struct globuf_rect globuf_wayland_software_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globuf_wayland_common_get_expose(context, platform, error);
}


void globuf_wayland_software_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);
	struct globuf_update_software* update = data;
	int error_posix;

	// set buffer listener
	error_posix =
		wl_buffer_add_listener(
			backend->buffer,
			&(backend->listener_buffer),
			context);

	if (error_posix == -1)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	// update surface
	wl_surface_attach(platform->surface, backend->buffer, update->x, update->y);
	wl_surface_damage_buffer(platform->surface, 0, 0, INT32_MAX, INT32_MAX);
	wl_surface_commit(platform->surface);

	globuf_error_ok(error);
}

void* globuf_wayland_software_callback(
	struct globuf* context)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);
	return platform;
}


// simple allocator we provide so developers don't try to recycle buffers
// (it would not be thread-safe and break this multi-threaded version of globuf)
uint32_t* globuf_buffer_alloc_wayland_software(
	struct globuf* context,
	unsigned width,
	unsigned height,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	backend->buffer_len = 4 * width * height;

	// create shm - code by sircmpwn
	int retries = 100;
	int fd;

	struct timespec time;
	uint64_t random;
	int i;

	do
	{
		char name[] = "/wl_shm-XXXXXX";

		clock_gettime(CLOCK_REALTIME, &time);
		random = time.tv_nsec;

		for (i = 0; i < 6; ++i)
		{
			name[(sizeof (name)) - 7 + i] =
				'A'
				+ (random & 15)
				+ ((random & 16) * 2);

			random >>= 5;
		}

		fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);

		--retries;

		if (fd >= 0)
		{
			shm_unlink(name);

			break;
		}
	}
	while ((retries > 0) && (errno == EEXIST));

	if (fd < 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_FD);
		return NULL;
	}

	// allocate shm
	int error_posix;

	do
	{
		error_posix = ftruncate(fd, backend->buffer_len);
	}
	while ((error_posix < 0) && (errno == EINTR));

	if (error_posix < 0)
	{
		close(fd);
		globuf_error_throw(context, error, GLOBUF_ERROR_FD);
		return NULL;
	}

	// mmap
	uint32_t* argb =
		mmap(
			NULL,
			backend->buffer_len,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			0);

	if (argb == MAP_FAILED)
	{
		close(fd);
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_MMAP);
		return NULL;
	}

	// create memory pool
	struct wl_shm_pool* software_pool =
		wl_shm_create_pool(
			backend->shm,
			fd,
			backend->buffer_len);

	if (software_pool == NULL)
	{
		munmap(argb, backend->buffer_len);
		close(fd);
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_REQUEST);
		return NULL;
	}

	// create buffer
	uint32_t format;

	if (context->feature_background->background == GLOBUF_BACKGROUND_OPAQUE)
	{
		format = WL_SHM_FORMAT_XRGB8888;
	}
	else
	{
		format = WL_SHM_FORMAT_ARGB8888;
	}

	backend->buffer =
		wl_shm_pool_create_buffer(
			software_pool,
			0,
			width,
			height,
			width * 4,
			format);

	if (backend->buffer == NULL)
	{
		wl_shm_pool_destroy(software_pool);
		munmap(argb, backend->buffer_len);
		close(fd);
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_REQUEST);
		return NULL;
	}

	// clean resources
	wl_shm_pool_destroy(software_pool);
	close(fd);

	// all good
	globuf_error_ok(error);
	return argb;
}

void globuf_buffer_free_wayland_software(
	struct globuf* context,
	uint32_t* buffer,
	struct globuf_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);
	int error_posix = munmap(buffer, backend->buffer_len);

	if (error_posix < 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_MUNMAP);
		return;
	}

	globuf_error_ok(error);
}


void globuf_prepare_init_wayland_software(
	struct globuf_config_backend* config,
	struct globuf_error_info* error)
{
	struct globuf_calls_software* software =
		malloc(sizeof (struct globuf_calls_software));

	if (software == NULL)
	{
		error->code = GLOBUF_ERROR_ALLOC;
		error->file = __FILE__;
		error->line = __LINE__;
		return;
	}

	software->alloc = globuf_buffer_alloc_wayland_software;
	software->free = globuf_buffer_free_wayland_software;

	config->data = software;
	config->callback = globuf_wayland_software_callback;
	config->init = globuf_wayland_software_init;
	config->clean = globuf_wayland_software_clean;
	config->window_create = globuf_wayland_software_window_create;
	config->window_destroy = globuf_wayland_software_window_destroy;
	config->window_confirm = globuf_wayland_software_window_confirm;
	config->window_start = globuf_wayland_software_window_start;
	config->window_block = globuf_wayland_software_window_block;
	config->window_stop = globuf_wayland_software_window_stop;
	config->init_render = globuf_wayland_software_init_render;
	config->init_events = globuf_wayland_software_init_events;
	config->handle_events = globuf_wayland_software_handle_events;
	config->init_features = globuf_wayland_software_init_features;
	config->feature_set_interaction = globuf_wayland_software_feature_set_interaction;
	config->feature_set_state = globuf_wayland_software_feature_set_state;
	config->feature_set_title = globuf_wayland_software_feature_set_title;
	config->feature_set_icon = globuf_wayland_software_feature_set_icon;
	config->get_width = globuf_wayland_software_get_width;
	config->get_height = globuf_wayland_software_get_height;
	config->get_expose = globuf_wayland_software_get_expose;
	config->update_content = globuf_wayland_software_update_content;

	globuf_error_ok(error);
}
