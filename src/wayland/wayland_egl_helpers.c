#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "wayland/wayland_egl_helpers.h"
#include "wayland/wayland_egl.h"
#include "wayland/wayland_common.h"
#include <stdbool.h>
#include <string.h>
#include <EGL/egl.h>

void wayland_helpers_egl_bind(struct globox* context, struct globox_error_info* error)
{
	struct wayland_egl_backend* backend = context->backend_data;

	// bind context to surface
	EGLBoolean error_egl;

	error_egl =
		eglMakeCurrent(
			backend->display,
			backend->surface,
			backend->surface,
			backend->egl);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_EGL_MAKE_CURRENT);
		return;
	}

	// set swap interval
	int interval;

	if (context->feature_vsync->vsync == true)
	{
		interval = 1;
	}
	else
	{
		interval = 0;
	}

	error_egl = eglSwapInterval(backend->display, interval);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_EGL_SWAP_INTERVAL);
		return;
	}

	globox_error_ok(error);
}

void wayland_helpers_egl_toplevel_configure(
	void* data,
	struct xdg_toplevel* xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array* states)
{
	struct wayland_platform* platform = data;
	struct globox* context = platform->globox;
	struct wayland_egl_backend* backend = context->backend_data;
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
	wl_egl_window_resize(backend->window, width, height, 0, 0);

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
