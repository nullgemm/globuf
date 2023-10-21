#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "wayland/wayland_common.h"
#include "wayland/wayland_common_registry.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>

#include "xdg-decoration-client-protocol.h"
#include "kde-blur-client-protocol.h"

// registry

void globox_wayland_helpers_callback_registry(
	void* data,
	struct wl_registry* registry,
	uint32_t name,
	const char* interface,
	uint32_t version)
{
	struct wayland_platform* platform = data;
	struct globox* context = platform->globox;

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
			.ping = globox_wayland_helpers_xdg_wm_base_ping,
		};

		platform->listener_xdg_wm_base = listener_xdg_wm_base;

		error_posix =
			xdg_wm_base_add_listener(
				platform->xdg_wm_base,
				&(platform->listener_xdg_wm_base),
				platform);

		if (error_posix == -1)
		{
			globox_error_throw(
				context,
				&error,
				GLOBOX_ERROR_WAYLAND_LISTENER_ADD);
		}
	}
	else if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
	{
		platform->xdg_decoration_manager =
			wl_registry_bind(
				registry,
				name,
				&zxdg_decoration_manager_v1_interface,
				1);

		if (platform->xdg_decoration_manager == NULL)
		{
			globox_error_throw(
				context,
				&error,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}
	}
	else if (strcmp(interface, org_kde_kwin_blur_manager_interface.name) == 0)
	{
		platform->kde_blur_manager =
			wl_registry_bind(
				registry,
				name,
				&org_kde_kwin_blur_manager_interface,
				1);

		if (platform->kde_blur_manager == NULL)
		{
			globox_error_throw(
				context,
				&error,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
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

void globox_wayland_helpers_callback_registry_remove(
	void* data,
	struct wl_registry* registry,
	uint32_t name)
{
	// TODO not needed?
}

// callbacks

void globox_wayland_helpers_surface_frame_done(
	void* data,
	struct wl_callback* callback,
	uint32_t time)
{
	struct wayland_platform* platform = data;
	struct globox* context = platform->globox;
	struct globox_error_info error;

	if (callback != NULL)
	{
		// destroy the current frame callback
		// this is where the surface_frame from last time is destroyed
		wl_callback_destroy(callback);
	}

	if (platform->closed == true)
	{
		return;
	}

	// register a new frame callback
	// valgrind false positive, surface_frame is destroyed in the next callback
	struct wl_callback* surface_frame = wl_surface_frame(platform->surface);

	if (surface_frame == NULL)
	{
		globox_error_throw(context, &error, GLOBOX_ERROR_WAYLAND_SURFACE_FRAME_GET);
		return;
	}

	// set surface frame callback
	struct wl_callback_listener listener_surface_frame =
	{
		.done = globox_wayland_helpers_surface_frame_done,
	};

	platform->listener_surface_frame = listener_surface_frame;

	int error_posix =
		wl_callback_add_listener(
			surface_frame,
			&(platform->listener_surface_frame),
			platform);

	if (error_posix == -1)
	{
		globox_error_throw(context, &error, GLOBOX_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	// run developer callback
	context->render_callback.callback(context->render_callback.data);
}

void globox_wayland_helpers_xdg_wm_base_ping(
	void* data,
	struct xdg_wm_base* xdg_wm_base,
	uint32_t serial)
{
	xdg_wm_base_pong(xdg_wm_base, serial);
}

void globox_wayland_helpers_xdg_surface_configure(
	void* data,
	struct xdg_surface* xdg_surface,
	uint32_t serial)
{
	struct wayland_platform* platform = data;
	struct globox* context = platform->globox;

	xdg_surface_ack_configure(xdg_surface, serial);

	if (context->feature_vsync->vsync == true)
	{
		context->render_callback.callback(context->render_callback.data);
	}
}

void globox_wayland_helpers_xdg_toplevel_configure(
	void* data,
	struct xdg_toplevel* xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array* states)
{
	struct wayland_platform* platform = data;
	struct globox* context = platform->globox;
	struct globox_error_info error;
	int error_posix;

	if ((width == 0) || (height == 0))
	{
		return;
	}

	if (context->feature_vsync->vsync == false)
	{
		// lock main mutex
		error_posix = pthread_mutex_lock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, &error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			return;
		}
	}

	context->feature_size->width = width;
	context->feature_size->height = height;

	if (context->feature_vsync->vsync == false)
	{
		// lock main mutex
		error_posix = pthread_mutex_unlock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, &error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			return;
		}
	}
}

void globox_wayland_helpers_xdg_toplevel_close(
	void* data,
	struct xdg_toplevel* xdg_toplevel)
{
	struct wayland_platform* platform = data;
	struct globox* context = platform->globox;
	struct globox_error_info error;
	int error_posix;

	// make the globox blocking function exit gracefully
	pthread_cond_broadcast(&(platform->cond_main));

	// lock main mutex
	error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, &error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	platform->closed = true;

	// unlock main mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, &error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}
}

void globox_wayland_helpers_xdg_decoration_configure(
	void* data,
	struct zxdg_toplevel_decoration_v1* xdg_decoration,
	uint32_t mode)
{
	struct wayland_platform* platform = data;

	// save the decoration mode actually set by the compositor
	platform->decoration_mode = mode;
}
