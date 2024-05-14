#define _XOPEN_SOURCE 700

#include "include/globuf.h"
#include "common/globuf_private.h"
#include "x11/x11_egl_helpers.h"
#include "x11/x11_egl.h"
#include "x11/x11_common.h"
#include <stdbool.h>
#include <string.h>
#include <EGL/egl.h>

bool x11_helpers_egl_ext_support(const char *list, const char *extension)
{
	const char* beg = list;
	const char* end;
	const char* cur;

	cur = strstr(beg, extension);

	while (cur != NULL)
	{
		end = cur + strlen(extension);

		// the extension name might be a subset of another one so
		// we must check the surrouding characters to make sure
		if (((cur == beg) || (cur[-1] == ' '))
			&& ((end[0] == '\0') || (end[0] == ' ')))
		{
			return true;
		}

		beg = end;
		cur = strstr(beg, extension);
	}

	return false;
}

void x11_helpers_egl_bind(struct globuf* context, struct globuf_error_info* error)
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
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_MAKE_CURRENT);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_SWAP_INTERVAL);
		return;
	}

	globuf_error_ok(error);
}
