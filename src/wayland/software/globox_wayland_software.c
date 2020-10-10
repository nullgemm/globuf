#define _XOPEN_SOURCE 700

#include "globox.h"
#include "globox_error.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>

#include "wayland/globox_wayland.h"
#include "wayland/software/globox_wayland_software_helpers.h"

void globox_context_software_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	int error;
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_wayland_software* context = &(platform->globox_wayland_software);

	// set buffer real size
	context->globox_software_buffer_width = globox->globox_width;
	context->globox_software_buffer_height = globox->globox_height;

	// set callbacks function pointers
	platform->globox_wayland_unminimize_start =
		globox_software_callback_unminimize_start;
	platform->globox_wayland_unminimize_finish =
		globox_software_callback_unminimize_finish;
	platform->globox_wayland_callback_xdg_toplevel_configure =
		globox_software_callback_resize;
	platform->globox_wayland_callback_xdg_surface_configure =
		globox_software_callback_attach;

	// wayland surface
	platform->globox_wayland_surface =
		wl_compositor_create_surface(
			platform->globox_wayland_compositor);

	if (platform->globox_wayland_surface == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_REQUEST);
		return;
	}

	// get xdg surface
	platform->globox_wayland_xdg_surface =
		xdg_wm_base_get_xdg_surface(
			platform->globox_wayland_xdg_wm_base,
			platform->globox_wayland_surface);

	if (platform->globox_wayland_xdg_surface == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_REQUEST);
		return;
	}

	// create shm, allocate buffer
	globox_software_callback_allocate(globox);

	if (globox_error_catch(globox))
	{
		return;
	}

	// add surface listener
	error =
		xdg_surface_add_listener(
			platform->globox_wayland_xdg_surface,
			&(platform->globox_wayland_xdg_surface_listener),
			globox);

	if (error == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_LISTENER);
		return;
	}

	// get toplevel surface
	platform->globox_wayland_xdg_toplevel =
		xdg_surface_get_toplevel(
			platform->globox_wayland_xdg_surface);

	if (platform->globox_wayland_xdg_toplevel == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_REQUEST);
		return;
	}

	// commit and dispatch
	globox_platform_commit(globox);

	error =
		wl_display_dispatch(
			platform->globox_wayland_display);

	if (error == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_DISPATCH);
	}

	// copy
	globox_context_software_copy(
		globox,
		0,
		0,
		context->globox_software_buffer_width,
		context->globox_software_buffer_height);

	// add toplevel listener
	error =
		xdg_toplevel_add_listener(
			platform->globox_wayland_xdg_toplevel,
			&(platform->globox_wayland_xdg_toplevel_listener),
			globox);

	if (error == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_LISTENER);

		return;
	}

	return;
}

void globox_context_software_create(struct globox* globox)
{
	// not needed
}

void globox_context_software_shrink(struct globox* globox)
{
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_wayland_software* context = &(platform->globox_wayland_software);
	int error;
	int size =
		4
		* context->globox_software_buffer_width
		* context->globox_software_buffer_height;

	wl_shm_pool_destroy(context->globox_software_pool);

	close(context->globox_software_fd);

	error =
		munmap(
			platform->globox_platform_argb,
			size);

	if (error == -1)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WAYLAND_MUNMAP);
	}

	context->globox_software_buffer_width = globox->globox_width;
	context->globox_software_buffer_height = globox->globox_height;

	globox_software_callback_allocate(globox);

	if (globox_error_catch(globox))
	{
		return;
	}

	return;
}

void globox_context_software_free(struct globox* globox)
{
	// TODO

	return;
}

void globox_context_software_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height)
{
	struct globox_platform* platform = &(globox->globox_platform);

	wl_surface_damage_buffer(
		platform->globox_wayland_surface,
		x,
		y,
		width,
		height);

	globox->globox_redraw = false;

	globox_platform_commit(globox);

	return;
}

// getters

// TODO
