#include "globox.h"
#include "globox_error.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "windows/globox_windows.h"
#include "windows/software/globox_windows_software.h"

void dummy(struct globox* globox)
{
	// not needed
}

void resize(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_software* context = &(platform->globox_windows_software);

	context->globox_software_bmp_info.bmiHeader.biWidth = globox->globox_width;
	context->globox_software_bmp_info.bmiHeader.biHeight = globox->globox_height;

	// destroy old bitmap handle
	if (context->globox_software_bmp_handle != NULL)
	{
		BOOL ok = DeleteObject(context->globox_software_bmp_handle);

		if (ok == 0)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WINDOWS_DELETE);
			return;
		}
	}

	// get a device context handle
	HDC hdc = GetDC(platform->globox_platform_event_handle);

	if (hdc == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET);
		return;
	}

	// create a device-independent bitmap
	context->globox_software_bmp_handle =
		CreateDIBSection(
			hdc,
			&(context->globox_software_bmp_info),
			DIB_RGB_COLORS,
			(void**) &(platform->globox_platform_argb),
			NULL,
			0);

	if (context->globox_software_bmp_handle == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GDI_DIB_CREATE);
		return;
	}

	ReleaseDC(platform->globox_platform_event_handle, hdc);

	if (globox->globox_transparent == false)
	{
		return;
	}

	platform->globox_windows_dwm_transparency_callback(globox);
}

void globox_context_software_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	struct globox_platform* platform = globox->globox_platform;

	platform->globox_windows_resize_callback = resize;
	platform->globox_windows_dcomp_callback = dummy;
}

void globox_context_software_create(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_software* context = &(platform->globox_windows_software);

	// .biWidth and .biHeight are set in resize()
	BITMAPINFOHEADER bmp_info_header =
	{
		.biSize = sizeof (BITMAPINFOHEADER),
		.biPlanes = 1,           // 1 bitmap plane
		.biBitCount = 32,        // 32 bits per pixel
		.biCompression = BI_RGB, // raw bitmap format
		.biSizeImage = 0,        // only paletted bitmaps need this
		.biXPelsPerMeter = 0,    // use a neutral X pixel density
		.biYPelsPerMeter = 0,    // use a neutral Y pixel density
		.biClrUsed = 0,          // all colors must be used
		.biClrImportant = 0,     // all colors are required
	};

	// only paletted bitmaps need bmiColors,
	// so we use this trick to set it to "NULL"
	BITMAPINFO bmp_info = {0};
	bmp_info.bmiHeader = bmp_info_header;

	context->globox_software_bmp_info = bmp_info;
	context->globox_software_bmp_handle = NULL;

	// finish initialization
	resize(globox);
}

void globox_context_software_shrink(struct globox* globox)
{
	// not needed
}

void globox_context_software_free(struct globox* globox)
{
	// TODO
}

void globox_context_software_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_software* context = &(platform->globox_windows_software);

	BOOL ok;

	// damage region
	RECT update =
	{
		.left = x,
		.top = y,
		.right = width,
		.bottom = height,
	};

	ok = InvalidateRect(platform->globox_platform_event_handle, &update, TRUE);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GDI_DAMAGE);
		return;
	}

	// paint
	PAINTSTRUCT paint;

	HDC hdc_window = BeginPaint(platform->globox_platform_event_handle, &paint);

	if (hdc_window == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GDI_PAINT);
		return;
	}

	HDC hdc_compatible = CreateCompatibleDC(hdc_window);

	if (hdc_compatible == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_CREATE);
		return;
	}

	HBITMAP hbm_compatible_old =
		SelectObject(
			hdc_compatible, 
			context->globox_software_bmp_handle);

	if (hbm_compatible_old == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GDI_BITMAP_SELECT);
		return;
	}

	ok = BitBlt(
		hdc_window,
		x,
		y,
		width,
		height,
		hdc_compatible,
		x,
		y,
		SRCCOPY);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GDI_BITBLT);
		return;
	}

	HBITMAP hbm_ret = SelectObject(hdc_compatible, hbm_compatible_old);

	if (hbm_ret == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GDI_BITMAP_SELECT);
		return;
	}

	ok = DeleteDC(hdc_compatible);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_DELETE);
		return;
	}

	// this one can't fail
	EndPaint(platform->globox_platform_event_handle, &paint);

	// commit
	globox->globox_redraw = false;

	GdiFlush();
}

// getters

BITMAPINFO globox_software_get_bmp_info(struct globox* globox)
{
	return globox->globox_platform->globox_windows_software.globox_software_bmp_info;
}

HBITMAP globox_software_get_bmp_handle(struct globox* globox)
{
	return globox->globox_platform->globox_windows_software.globox_software_bmp_handle;
}
