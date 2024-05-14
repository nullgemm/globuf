#define _XOPEN_SOURCE 700

#include "include/globuf.h"
#include "common/globuf_private.h"
#include "x11/x11_glx_helpers.h"
#include "x11/x11_glx.h"
#include "x11/x11_common.h"
#include <stdbool.h>
#include <string.h>
#include <GL/glx.h>

bool x11_helpers_glx_ext_support(const char *list, const char *extension)
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

void x11_helpers_glx_bind(struct globuf* context, struct globuf_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;

	Bool error_glx =
		glXMakeContextCurrent(
			backend->display,
			backend->win,
			backend->win,
			backend->glx);

	if (error_glx == False)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_GLX_MAKE_CURRENT);
		return;
	}

	globuf_error_ok(error);
}
