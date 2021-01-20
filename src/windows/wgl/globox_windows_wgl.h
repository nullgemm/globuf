#ifndef H_GLOBOX_WINDOWS_WGL
#define H_GLOBOX_WINDOWS_WGL

#include "globox.h"
#include "windows/globox_windows.h"
// windows includes
#include <windows.h>

struct globox_windows_wgl
{
	int globox_wgl_version_major;
	int globox_wgl_version_minor;
	PIXELFORMATDESCRIPTOR globox_wgl_pfd;
	HGLRC globox_wgl_context;
};

#endif
