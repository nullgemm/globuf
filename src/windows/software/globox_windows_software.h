#ifndef H_GLOBOX_WINDOWS_SOFTWARE
#define H_GLOBOX_WINDOWS_SOFTWARE

#include "globox.h"
#include "windows/globox_windows.h"
#include <windows.h>

struct globox_windows_software
{
	uint32_t globox_software_buffer_width;
	uint32_t globox_software_buffer_height;

	BITMAPINFO globox_software_bmp_info;
	HBITMAP globox_software_bmp_handle;
};

#endif
