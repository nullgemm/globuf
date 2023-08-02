#include "include/globox.h"
#include "common/globox_private.h"
#include "win/win_wgl_helpers.h"
#include "win/win_wgl.h"
#include "win/win_common.h"
#include <stdbool.h>
#include <string.h>

void win_helpers_wgl_render(struct win_thread_render_loop_data* data)
{
	struct globox* context = data->globox;
	struct win_platform* platform = data->platform;
	struct win_wgl_backend* backend = context->backend_data;
	struct globox_error_info* error = data->error;

	// make wgl context current on the calling thread
	BOOL ok = wglMakeCurrent(backend->device_context, backend->wgl);

	if (ok == FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_CONTEXT_SET);
	}
}
