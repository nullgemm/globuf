#define _XOPEN_SOURCE 700

#include "include/globuf.h"
#include "common/globuf_private.h"
#include "wayland/wayland_software_helpers.h"
#include "wayland/wayland_software.h"
#include "wayland/wayland_common.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wayland-client.h>

void wayland_helpers_callback_registry_shm(
	void* data,
	void* registry,
	uint32_t name,
	const char* interface,
	uint32_t version)
{
	struct globuf* context = data;
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	int error_posix;
	struct globuf_error_info error;

	if (strcmp(interface, wl_shm_interface.name) == 0)
	{
		backend->shm =
			wl_registry_bind(
				registry,
				name,
				&wl_shm_interface,
				1);

		if (backend->shm == NULL)
		{
			globuf_error_throw(
				context,
				&error,
				GLOBUF_ERROR_WAYLAND_REQUEST);
			return;
		}
	}
}

void wayland_helpers_buffer_release(
	void* data,
	struct wl_buffer* buffer)
{
	wl_buffer_destroy(buffer);
}
