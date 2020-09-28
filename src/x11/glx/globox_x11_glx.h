#ifndef H_GLOBOX_X11_GLX
#define H_GLOBOX_X11_GLX

#include "globox.h"
#include "x11/globox_x11.h"
// context includes
#include <X11/Xlib.h>
#include <GL/glx.h>

struct globox_x11_glx
{
	// libX11 and GLX
	Display* globox_glx_display;
	GLXFBConfig globox_glx_fb_config;
	GLXContext globox_glx_context;
	GLXWindow globox_glx_win;
	int globox_glx_version_major;
	int globox_glx_version_minor;
};

#endif
