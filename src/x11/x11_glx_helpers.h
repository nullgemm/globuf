#ifndef H_GLOBUF_INTERNAL_X11_GLX_HELPERS
#define H_GLOBUF_INTERNAL_X11_GLX_HELPERS

#include "include/globuf.h"
#include "include/globuf_opengl.h"
#include "x11/x11_common.h"
#include <GL/glx.h>
#include <X11/Xlib.h>

struct x11_glx_backend
{
	struct x11_platform platform;
	struct globuf_config_opengl* config;
	Display* display;
	GLXFBConfig fb_config;
	GLXContext glx;
	GLXWindow win;
	int error_base;
	int event_base;
};

bool x11_helpers_glx_ext_support(
	const char *list,
	const char *extension);

void x11_helpers_glx_bind(
	struct globuf* context,
	struct globuf_error_info* error);

#endif
