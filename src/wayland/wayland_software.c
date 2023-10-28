#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "include/globox_software.h"
#include "include/globox_wayland_software.h"

#include "common/globox_private.h"
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

void globox_wayland_software_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct wayland_software_backend* backend = malloc(sizeof (struct wayland_software_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
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
		.release = globox_wayland_helpers_buffer_release,
	};

	backend->listener_buffer = listener_buffer;

	// initialize the platform
	globox_wayland_common_init(context, &(backend->platform), error);

	// error always set
}

void globox_wayland_software_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// clean the platform
	globox_wayland_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globox_wayland_software_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	globox_wayland_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// run common wayland helper
	globox_wayland_common_window_create(context, platform, configs, count, callback, data, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_software_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_window_destroy(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
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
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_software_window_confirm(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// add backend-specific registry handlers
	bool error_reg =
		globox_add_wayland_registry_handler(
			context,
			globox_wayland_helpers_callback_registry_shm,
			context);

	if (error_reg == false)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_REGISTRY_CALLBACK);
		return;
	}

	// run common wayland helper
	globox_wayland_common_window_confirm(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	if (backend->shm == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_SHM_GET);
		return;
	}

	// error always set
}

void globox_wayland_software_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_wayland_software_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper (mutex locked when unblocked)
	globox_wayland_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_wayland_software_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globox_wayland_software_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globox_wayland_software_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globox_event globox_wayland_software_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	enum globox_event out =
		globox_wayland_common_handle_events(
			context,
			platform,
			event,
			error);

	// error always set
	return out;
}


struct globox_config_features* globox_wayland_software_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	struct globox_config_features* features =
		globox_wayland_common_init_features(context, platform, error);

	// return the newly created features info structure
	// error always set
	return features;
}

void globox_wayland_software_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_wayland_software_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_wayland_software_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_wayland_software_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common wayland helper
	globox_wayland_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_wayland_software_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globox_wayland_common_get_width(context, platform, error);
}

unsigned globox_wayland_software_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globox_wayland_common_get_height(context, platform, error);
}

struct globox_rect globox_wayland_software_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globox_wayland_common_get_expose(context, platform, error);
}


void globox_wayland_software_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);
	struct globox_update_software* update = data;
	int error_posix;

	// set buffer listener
	error_posix =
		wl_buffer_add_listener(
			backend->buffer,
			&(backend->listener_buffer),
			context);

	if (error_posix == -1)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	// update surface
	wl_surface_attach(platform->surface, backend->buffer, update->x, update->y);
	wl_surface_damage_buffer(platform->surface, 0, 0, INT32_MAX, INT32_MAX);
	wl_surface_commit(platform->surface);

	globox_error_ok(error);
}

void* globox_wayland_software_callback(
	struct globox* context)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);
	return platform;
}


// simple allocator we provide so developers don't try to recycle buffers
// (it would not be thread-safe and break this multi-threaded version of globox)
uint32_t* globox_buffer_alloc_wayland_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error)
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
		globox_error_throw(context, error, GLOBOX_ERROR_FD);
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
		globox_error_throw(context, error, GLOBOX_ERROR_FD);
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
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_MMAP);
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
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_REQUEST);
		return NULL;
	}

	// create buffer
	uint32_t format;

	if (context->feature_background->background == GLOBOX_BACKGROUND_OPAQUE)
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
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_REQUEST);
		return NULL;
	}

	// clean resources
	wl_shm_pool_destroy(software_pool);
	close(fd);

	// all good
	globox_error_ok(error);
	return argb;
}

void globox_buffer_free_wayland_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error)
{
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);
	int error_posix = munmap(buffer, backend->buffer_len);

	if (error_posix < 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_MUNMAP);
		return;
	}

	globox_error_ok(error);
}


void globox_prepare_init_wayland_software(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	struct globox_calls_software* software =
		malloc(sizeof (struct globox_calls_software));

	if (software == NULL)
	{
		error->code = GLOBOX_ERROR_ALLOC;
		error->file = __FILE__;
		error->line = __LINE__;
		return;
	}

	software->alloc = globox_buffer_alloc_wayland_software;
	software->free = globox_buffer_free_wayland_software;

	config->data = software;
	config->callback = globox_wayland_software_callback;
	config->init = globox_wayland_software_init;
	config->clean = globox_wayland_software_clean;
	config->window_create = globox_wayland_software_window_create;
	config->window_destroy = globox_wayland_software_window_destroy;
	config->window_confirm = globox_wayland_software_window_confirm;
	config->window_start = globox_wayland_software_window_start;
	config->window_block = globox_wayland_software_window_block;
	config->window_stop = globox_wayland_software_window_stop;
	config->init_render = globox_wayland_software_init_render;
	config->init_events = globox_wayland_software_init_events;
	config->handle_events = globox_wayland_software_handle_events;
	config->init_features = globox_wayland_software_init_features;
	config->feature_set_interaction = globox_wayland_software_feature_set_interaction;
	config->feature_set_state = globox_wayland_software_feature_set_state;
	config->feature_set_title = globox_wayland_software_feature_set_title;
	config->feature_set_icon = globox_wayland_software_feature_set_icon;
	config->get_width = globox_wayland_software_get_width;
	config->get_height = globox_wayland_software_get_height;
	config->get_expose = globox_wayland_software_get_expose;
	config->update_content = globox_wayland_software_update_content;

	globox_error_ok(error);
}
