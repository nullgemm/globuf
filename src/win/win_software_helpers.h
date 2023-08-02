#ifndef H_GLOBOX_INTERNAL_WIN_SOFTWARE_HELPERS
#define H_GLOBOX_INTERNAL_WIN_SOFTWARE_HELPERS

#include "include/globox.h"
#include "win/win_common.h"
#include <wingdi.h>

struct win_software_backend
{
	struct win_platform platform;

	BITMAPINFO bmp_info;
	HBITMAP bmp_handle;
};

#endif
