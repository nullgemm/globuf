#ifndef H_GLOBOX_INTERNAL_WIN_WGL_HELPERS
#define H_GLOBOX_INTERNAL_WIN_WGL_HELPERS

#include "include/globox.h"
#include "include/globox_wgl.h"
#include "win/win_common.h"

#include <wingdi.h>

struct win_wgl_backend
{
	struct win_platform platform;
	struct globox_config_wgl* config;
	HGLRC wgl;
	HDC device_context;
	PIXELFORMATDESCRIPTOR format_descriptor;
};

void win_helpers_wgl_render(struct win_thread_render_loop_data* data);

#endif
