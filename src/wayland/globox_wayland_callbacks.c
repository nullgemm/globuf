#define _XOPEN_SOURCE 700

#include "globox.h"
#include "globox_error.h"

#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>

#include "wayland/globox_wayland.h"
#include "wayland/globox_wayland_callbacks.h"

void callback_xdg_wm_base_ping(
	void* data,
	struct xdg_wm_base* xdg_wm_base,
	uint32_t serial)
{
	xdg_wm_base_pong(
		xdg_wm_base,
		serial);
}

void callback_output_geometry(
	void* data,
	struct wl_output* wl_output,
	int32_t x,
	int32_t y,
	int32_t physical_width,
	int32_t physical_height,
	int32_t subpixel,
	const char* make,
	const char* model,
	int32_t output_transform)
{
	// TODO
}

void callback_output_mode(
	void* data,
	struct wl_output* wl_output,
	uint32_t flags,
	int32_t width,
	int32_t height,
	int32_t refresh)
{
	// TODO
}

void callback_output_done(
	void* data,
	struct wl_output* wl_output)
{
	// not needed
}

void callback_output_scale(
	void* data,
	struct wl_output* wl_output,
	int32_t scale)
{
	// TODO
}

void callback_registry_global(
	void* data,
	struct wl_registry* wl_registry,
	uint32_t name,
	const char* interface,
	uint32_t version)
{
	int error;
	struct globox* globox = data;
	struct globox_platform* platform = globox->globox_platform;

	if (strcmp(interface, wl_shm_interface.name) == 0)
	{
		platform->globox_wayland_shm =
			wl_registry_bind(
				wl_registry,
				name,
				&wl_shm_interface,
				1);

		if (platform->globox_wayland_shm == NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}
	}
	else if (strcmp(interface, wl_compositor_interface.name) == 0)
	{
		platform->globox_wayland_compositor =
			wl_registry_bind(
				wl_registry,
				name,
				&wl_compositor_interface,
				4);

		if (platform->globox_wayland_compositor == NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}
	}
	else if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
	{
		platform->globox_wayland_xdg_decoration_manager =
			wl_registry_bind(
				wl_registry,
				name,
				&zxdg_decoration_manager_v1_interface,
				1);

		if (platform->globox_wayland_xdg_decoration_manager == NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}
	}
	else if ((strcmp(interface, org_kde_kwin_blur_manager_interface.name) == 0)
		&& (globox->globox_transparent == true)
		&& (globox->globox_blurred == true))
	{
		platform->globox_wayland_kde_blur_manager =
			wl_registry_bind(
				wl_registry,
				name,
				&org_kde_kwin_blur_manager_interface,
				1);

		if (platform->globox_wayland_kde_blur_manager == NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}
	}
	else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
	{
		platform->globox_wayland_xdg_wm_base =
			wl_registry_bind(
				wl_registry,
				name,
				&xdg_wm_base_interface,
				1);

		if (platform->globox_wayland_xdg_wm_base == NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}

		error =
			xdg_wm_base_add_listener(
				platform->globox_wayland_xdg_wm_base,
				&(platform->globox_wayland_xdg_wm_base_listener),
				globox);

		if (error == -1)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WAYLAND_LISTENER);
		}
	}
	else if (strcmp(interface, wl_output_interface.name) == 0)
	{
		platform->globox_wayland_output =
			wl_registry_bind(
				wl_registry,
				name,
				&wl_output_interface,
				1);

		if (platform->globox_wayland_output == NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}

		error =
			wl_output_add_listener(
				platform->globox_wayland_output,
				&(platform->globox_wayland_output_listener),
				globox);

		if (error == -1)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WAYLAND_LISTENER);
		}
	}
	else if ((strcmp(interface, wl_seat_interface.name) == 0)
		&& (globox->globox_event_callback != NULL))
	{
		platform->globox_wayland_seat =
			wl_registry_bind(
				wl_registry,
				name,
				&wl_seat_interface,
				7);

		// add listener
		globox->globox_event_callback(
			platform->globox_wayland_seat,
			globox->globox_event_callback_data);
	}
	else if (strcmp(interface, zwp_relative_pointer_manager_v1_interface.name) == 0)
	{
		platform->globox_wayland_pointer_manager =
			wl_registry_bind(
				wl_registry,
				name,
				&zwp_relative_pointer_manager_v1_interface,
				1);
	}
	else if (strcmp(interface, zwp_pointer_constraints_v1_interface.name) == 0)
	{
		platform->globox_wayland_pointer_constraints =
			wl_registry_bind(
				wl_registry,
				name,
				&zwp_pointer_constraints_v1_interface,
				1);
	}
}

void callback_registry_global_remove(
	void* data,
	struct wl_registry* wl_registry,
	uint32_t name)
{
	// not needed
}

void callback_xdg_toplevel_configure(
	void* data,
	struct xdg_toplevel* xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array* states)
{
	struct globox* globox = data;
	struct globox_platform* platform = globox->globox_platform;

	if ((width == 0) || (height == 0))
	{
		return;
	}

	globox->globox_width = width;
	globox->globox_height = height;
	globox->globox_redraw = true;

	platform->globox_wayland_callback_xdg_toplevel_configure(
		globox,
		width,
		height);
}

void callback_xdg_toplevel_close(
	void* data,
	struct xdg_toplevel* xdg_toplevel)
{
	struct globox* globox = data;

	globox->globox_closed = true;
}

void callback_xdg_surface_configure(
	void* data,
	struct xdg_surface* xdg_surface,
	uint32_t serial)
{
	struct globox* globox = data;
	struct globox_platform* platform = globox->globox_platform;

	xdg_surface_ack_configure(
		xdg_surface,
		serial);

	platform->globox_wayland_callback_xdg_surface_configure(
		globox);
}

void callback_xdg_decoration_configure(
	void* data,
	struct zxdg_toplevel_decoration_v1* xdg_decoration,
	uint32_t mode)
{
	struct globox* globox = data;

	if (mode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE)
	{
		globox->globox_frameless = false;
	}
	else
	{
		globox->globox_frameless = true;
	}
}
