#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "wayland/wayland_common.h"
#include "wayland/wayland_common_registry.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// registry

void globox_wayland_helpers_callback_registry(
	void* data,
	struct wl_registry* registry,
	uint32_t name,
	const char* interface,
	uint32_t version)
{
	struct globox* context = data;
	struct wayland_software_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	int error_posix;
	struct globox_error_info error;

	if (strcmp(interface, wl_compositor_interface.name) == 0)
	{
		platform->compositor =
			wl_registry_bind(
				registry,
				name,
				&wl_compositor_interface,
				4);

		if (platform->compositor == NULL)
		{
			globox_error_throw(
				context,
				&error,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}
	}
	else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
	{
		platform->xdg_wm_base =
			wl_registry_bind(
				registry,
				name,
				&xdg_wm_base_interface,
				1);

		if (platform->xdg_wm_base == NULL)
		{
			globox_error_throw(
				context,
				&error,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}

		struct xdg_wm_base_listener listener_xdg_wm_base =
		{
			.ping = ; // TODO
		};

		error_posix =
			xdg_wm_base_add_listener(
				platform->xdg_wm_base,
				&listener_xdg_wm_base,
				context);

		if (error_posix == -1)
		{
			globox_error_throw(
				context,
				&error,
				GLOBOX_ERROR_WAYLAND_LISTENER_ADD);
		}
	}

	// run external capabilities handler callbacks
	struct wayland_registry_handler_node* handler = platform->registry_handlers;

	while (handler != NULL)
	{
		handler->registry_handler(
			handler->registry_handler_data,
			registry,
			name,
			interface,
			version);

		handler = handler->next;
	}
}

// callbacks
// TODO move somewhere else?
