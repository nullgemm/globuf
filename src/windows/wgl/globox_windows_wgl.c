#include "globox.h"
#include "globox_error.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include <GL/gl.h>

#if defined(GLOBOX_COMPATIBILITY_WINE) && defined(GLOBOX_ERROR_LOG_BASIC)
#include <stdio.h>
#endif

#include "windows/globox_windows.h"
#include "windows/wgl/globox_windows_wgl.h"

#undef WGL_WGLEXT_PROTOTYPES
#include <GL/wglext.h>

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

void dummy(struct globox* globox)
{
	// not needed
}

void resize(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;

	if (globox->globox_transparent == false)
	{
		return;
	}

	platform->globox_windows_dwm_transparency_callback(globox);
}

void globox_context_wgl_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_wgl* context = &(platform->globox_windows_wgl);

	platform->globox_windows_resize_callback = resize;
	platform->globox_windows_dcomp_callback = dummy;

	context->globox_wgl_version_major = version_major;
	context->globox_wgl_version_minor = version_minor;
}

void globox_context_wgl_create(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_wgl* context = &(platform->globox_windows_wgl);

	BOOL ok;

	HDC hdc = GetDC(platform->globox_platform_event_handle);

	if (hdc == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET);
		return;
	}

	int pixel_format;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof (PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW
			| PFD_SUPPORT_OPENGL
			| PFD_SUPPORT_COMPOSITION
			| PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0,
	};

	pixel_format = ChoosePixelFormat(hdc, &pfd);

	if (pixel_format == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE);
		return;
	}

	ok = SetPixelFormat(hdc, pixel_format, &pfd);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET);
		return;
	}

	context->globox_wgl_context = wglCreateContext(hdc);

	if (context->globox_wgl_context == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE);
		return;
	}

	ok = wglMakeCurrent(hdc, context->globox_wgl_context);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET);
		return;
	}

	context->globox_wgl_pfd = pfd;

	wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)
			wglGetProcAddress("wglCreateContextAttribsARB");

	wglChoosePixelFormatARB = 
		(PFNWGLCHOOSEPIXELFORMATARBPROC)
			wglGetProcAddress("wglChoosePixelFormatARB");

	wglMakeCurrent(hdc, NULL);
	wglDeleteContext(context->globox_wgl_context);

	int attr_wgl[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 16,
		WGL_STENCIL_BITS_ARB, 0,
		0
	};

	int formats[1];
	UINT formats_count;

	ok = wglChoosePixelFormatARB(
		hdc,
		attr_wgl,
		NULL,
		1,
		formats,
		&formats_count);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE);
		return;
	}

	ok = SetPixelFormat(hdc, *formats, &(context->globox_wgl_pfd));

	if (ok == 0)
	{
#if !defined(GLOBOX_COMPATIBILITY_WINE)
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET);
		return;
#elif defined(GLOBOX_ERROR_LOG_BASIC)
		fprintf(
			stderr,
			"Wine compatibility mode; skipping the following error:\n%s\n",
			globox->globox_log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET]);
#endif
	}

	int attr_gl[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, context->globox_wgl_version_major,
		WGL_CONTEXT_MINOR_VERSION_ARB, context->globox_wgl_version_minor,
		0
	};

	context->globox_wgl_context =
		wglCreateContextAttribsARB(
			hdc,
			NULL,
			attr_gl);

	if (context->globox_wgl_context == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE);
		return;
	}

	ok = wglMakeCurrent(hdc, context->globox_wgl_context);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET);
		return;
	}

	ReleaseDC(platform->globox_platform_event_handle, hdc);
}

void globox_context_wgl_shrink(struct globox* globox)
{
	// not needed
}

void globox_context_wgl_free(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_wgl* context = &(platform->globox_windows_wgl);

	BOOL ok = wglDeleteContext(context->globox_wgl_context);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_DELETE);
		return;
	}
}

void globox_context_wgl_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height)
{
	struct globox_platform* platform = globox->globox_platform;

	HDC hdc = GetDC(platform->globox_platform_event_handle);

	if (hdc == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET);
		return;
	}

	BOOL ok = wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_SWAP);
		return;
	}

	ReleaseDC(platform->globox_platform_event_handle, hdc);

	globox->globox_redraw = false;
}
