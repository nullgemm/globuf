#include "globox.h"
#include "globox_error.h"

#include <EGL/egl.h>

#include "windows/globox_windows.h"

void dummy(struct globox* globox)
{
	// not needed
}

void globox_context_egl_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_egl* context = &(platform->globox_windows_egl);

	platform->globox_windows_resize_callback = dummy;
	platform->globox_windows_dcomp_callback = dummy;

	// get display
	context->globox_egl_display =
		eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (context->globox_egl_display == EGL_NO_DISPLAY)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_DISPLAY_GET);
		return;
	}

	// init
	EGLBoolean status_egl;
	EGLint display_version_major;
	EGLint display_version_minor;

	status_egl =
		eglInitialize(
			context->globox_egl_display,
			&display_version_major,
			&display_version_minor);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_INIT);
		return;
	}

	// use OpenGL
	status_egl =
		eglBindAPI(EGL_OPENGL_API);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_API_BIND);
		return;
	}

	// use 8-bit RGBA
	EGLint egl_config_attrib[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_NONE,
	};

	status_egl =
		eglChooseConfig(
			context->globox_egl_display,
			egl_config_attrib,
			&(context->globox_egl_config),
			1,
			&(context->globox_egl_config_size));

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_CONFIG_CHOOSE);
		return;
	}

	EGLint egl_context_attrib[] =
	{
		EGL_CONTEXT_MAJOR_VERSION, version_major,
		EGL_CONTEXT_MINOR_VERSION, version_minor,
		EGL_NONE,
	};

	context->globox_egl_context =
		eglCreateContext(
			context->globox_egl_display,
			context->globox_egl_config,
			EGL_NO_CONTEXT,
			egl_context_attrib);

	if (context->globox_egl_context == EGL_NO_CONTEXT)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_CREATE);
		return;
	}
}

void globox_context_egl_create(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_egl* context = &(platform->globox_windows_egl);

	// get visual id from EGL
	EGLBoolean status_egl;
	EGLint visual_id;

	status_egl =
		eglGetConfigAttrib(
			context->globox_egl_display,
			context->globox_egl_config,
			EGL_NATIVE_VISUAL_ID,
			&visual_id);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_CONFIG_ATTRIB_GET);
		return;
	}

	HDC hdc = GetDC(platform->globox_platform_event_handle);

	if (hdc == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET);
		return;
	}

	PIXELFORMATDESCRIPTOR pfd;
	BOOL ok;

	ok = DescribePixelFormat(
		hdc,
		visual_id,
		sizeof (PIXELFORMATDESCRIPTOR),
		&pfd);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE);
		return;
	}

	ok = SetPixelFormat(hdc, visual_id, &pfd);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET);
		return;
	}

	ReleaseDC(platform->globox_platform_event_handle, hdc);

	// create EGL surface
	context->globox_egl_surface =
		eglCreateWindowSurface(
			context->globox_egl_display,
			context->globox_egl_config,
			(EGLNativeWindowType) platform->globox_platform_event_handle,
			NULL);

	if (context->globox_egl_surface == EGL_NO_SURFACE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_SURFACE_CREATE);
		return;
	}

	// bind context to surface
	status_egl =
		eglMakeCurrent(
			context->globox_egl_display,
			context->globox_egl_surface,
			context->globox_egl_surface,
			context->globox_egl_context);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_MAKE_CURRENT);
		return;
	}
}

void globox_context_egl_shrink(struct globox* globox)
{
	// not needed
}

void globox_context_egl_free(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_egl* context = &(platform->globox_windows_egl);

	EGLBoolean status_egl;

	status_egl =
		eglDestroySurface(
			context->globox_egl_display,
			context->globox_egl_surface);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_SURFACE_DESTROY);
		return;
	}

	status_egl =
		eglDestroyContext(
			context->globox_egl_display,
			context->globox_egl_context);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_DESTROY);
		return;
	}

	status_egl =
		eglTerminate(
			context->globox_egl_display);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_TERMINATE);
		return;
	}
}

void globox_context_egl_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_windows_egl* context = &(platform->globox_windows_egl);

	EGLBoolean status_egl =
		eglSwapBuffers(
			context->globox_egl_display,
			context->globox_egl_surface);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_EGL_SWAP);
		return;
	}

	globox->globox_redraw = false;
}
