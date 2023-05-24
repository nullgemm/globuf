#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "appkit/appkit_egl_helpers.h"
#include "appkit/appkit_egl.h"
#include "appkit/appkit_common.h"
#include <stdbool.h>
#include <string.h>
#include <EGL/egl.h>

void appkit_helpers_egl_bind(struct globox* context, struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;

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
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_MAKE_CURRENT);
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
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_SWAP_INTERVAL);
		return;
	}

	globox_error_ok(error);
}
