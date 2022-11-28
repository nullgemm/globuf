#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_egl_helpers.h"
#include "x11/x11_egl.h"
#include "x11/x11_common.h"
#include <stdbool.h>
#include <string.h>
#include <EGL/egl.h>

void x11_helpers_egl_bind(struct globox* context, struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;

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
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_MAKE_CURRENT);
		return;
	}

	// set swap interval
	error_egl = eglSwapInterval(backend->display, 0);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_SWAP_INTERVAL);
		return;
	}

	globox_error_ok(error);
}
