#ifndef H_GLOBUF_INTERNAL_WIN_SOFTWARE_HELPERS
#define H_GLOBUF_INTERNAL_WIN_SOFTWARE_HELPERS

#include "include/globuf.h"
#include "win/win_common.h"
#include <wingdi.h>

struct win_software_backend
{
	struct win_platform platform;

	BITMAPINFO bmp_info;
	HBITMAP bmp_handle;
};

#endif
