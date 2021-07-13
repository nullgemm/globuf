#define _XOPEN_SOURCE 700

#include "globox.h"
#include "globox_error.h"

#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>

#include "wayland/globox_wayland.h"
#include "wayland/software/globox_wayland_software.h"
#include "wayland/software/globox_wayland_software_helpers.h"

void globox_software_callback_buffer_release(
	void* data,
	struct wl_buffer* wl_buffer)
{
	struct globox* globox = data;
	struct globox_platform* platform = globox->globox_platform;
	struct globox_wayland_software* context = &(platform->globox_wayland_software);

	pthread_mutex_lock(&(context->globox_software_buffer_mutex));

	if (context->globox_software_buffer_list_len
		== context->globox_software_buffer_list_max)
	{
		context->globox_software_buffer_list_max += 10;

		context->globox_software_buffer_list =
			realloc(
				context->globox_software_buffer_list,
				context->globox_software_buffer_list_max
					* (sizeof (struct wl_buffer*)));
	}

	context->globox_software_buffer_list
		[context->globox_software_buffer_list_len] =
			wl_buffer;

	context->globox_software_buffer_list_len += 1;

	pthread_mutex_unlock(&(context->globox_software_buffer_mutex));
}

void globox_software_callback_allocate(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_wayland_software* context = &(platform->globox_wayland_software);

	// create shm - code by sircmpwn
	uint8_t retries = 100;
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
		globox_error_throw(
			globox,
			GLOBOX_ERROR_FD);
		return;
	}

	// allocate shm
	int ret;

	do
	{
		ret = ftruncate(fd, context->globox_software_buffer_len);
	}
	while ((ret < 0) && (errno == EINTR));

	if (ret < 0)
	{
		close(fd);
		globox_error_throw(
			globox,
			GLOBOX_ERROR_FD);
		return;
	}

	// mmap
	platform->globox_platform_argb =
		mmap(
			NULL,
			context->globox_software_buffer_len,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			0);

	if (platform->globox_platform_argb == MAP_FAILED)
	{
		close(fd);
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_MMAP);
		return;
	}

	// create memory pool
	struct wl_shm_pool* software_pool =
		wl_shm_create_pool(
			platform->globox_wayland_shm,
			fd,
			context->globox_software_buffer_len);

	if (software_pool == NULL)
	{
		close(fd);
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_REQUEST);
		return;
	}

	// create buffer
	uint32_t format;

	if (globox->globox_transparent == true)
	{
		format = WL_SHM_FORMAT_ARGB8888;
	}
	else
	{
		format = WL_SHM_FORMAT_XRGB8888;
	}

	context->globox_software_buffer =
		wl_shm_pool_create_buffer(
			software_pool,
			0,
			globox->globox_width,
			globox->globox_height,
			globox->globox_width * 4,
			format);

	if (context->globox_software_buffer == NULL)
	{
		close(fd);
		wl_shm_pool_destroy(software_pool);
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_REQUEST);
		return;
	}

	ret = wl_buffer_add_listener(
		context->globox_software_buffer,
		&(context->globox_software_buffer_listener),
		globox);

	if (ret == -1)
	{
		close(fd);
		wl_shm_pool_destroy(software_pool);
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_LISTENER);
		return;
	}

	close(fd);
	wl_shm_pool_destroy(software_pool);

	// clean previous buffers
	pthread_mutex_lock(&(context->globox_software_buffer_mutex));

	if (context->globox_software_buffer_list_len > 0)
	{
		int k = 0;
		int max = context->globox_software_buffer_list_len - 1;

		while (k < max)
		{
			wl_buffer_destroy(context->globox_software_buffer_list[k]);
			++k;
		}

		context->globox_software_buffer_list_len = 1;

		context->globox_software_buffer_list[0] =
			context->globox_software_buffer_list[k];
	}

	pthread_mutex_unlock(&(context->globox_software_buffer_mutex));
}

void globox_software_callback_unminimize_start(struct globox* globox)
{
	// not needed
}

void globox_software_callback_unminimize_finish(struct globox* globox)
{
	globox_context_software_init(
		globox,
		0,
		0);

	return;
}

void globox_software_callback_attach(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_wayland_software* context = &(platform->globox_wayland_software);

	wl_surface_attach(
		platform->globox_wayland_surface,
		context->globox_software_buffer,
		0,
		0);
}

void globox_software_callback_resize(
	struct globox* globox,
	int32_t width,
	int32_t height)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_wayland_software* context = &(platform->globox_wayland_software);

	context->globox_software_buffer_len = 4 * width * height;
}
