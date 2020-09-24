#include "globox.h"
#include "globox_error.h"
#include "x11/globox_x11.h"
#include <EGL/egl.h>

void globox_context_egl_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_x11_egl* context = &(platform->globox_x11_egl);

	// get display
	context->globox_egl_display =
		eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (context->globox_egl_display == EGL_NO_DISPLAY)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EGL_FAIL);
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
			GLOBOX_ERROR_X11_EGL_FAIL);
		return;
	}

	// use OpenGL
	status_egl =
		eglBindAPI(EGL_OPENGL_API);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EGL_FAIL);
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
			GLOBOX_ERROR_X11_EGL_FAIL);
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
			GLOBOX_ERROR_X11_EGL_FAIL);
		return;
	}

	// get visual id from EGL
	EGLint visual_id;

	status_egl =
		eglGetConfigAttrib(
			context->globox_egl_display,
			context->globox_egl_config,
			EGL_NATIVE_VISUAL_ID,
			&visual_id);

	platform->globox_x11_visual_id = visual_id;

	// get visual depth from EGL
	EGLint visual_depth;

	status_egl =
		eglGetConfigAttrib(
			context->globox_egl_display,
			context->globox_egl_config,
			EGL_DEPTH_SIZE,
			&visual_depth);

	platform->globox_x11_visual_depth = visual_depth;

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EGL_FAIL);
		return;
	}

	return;
}

void globox_context_egl_create(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_x11_egl* context = &(platform->globox_x11_egl);

	// create EGL surface
	context->globox_egl_surface =
		eglCreateWindowSurface(
			context->globox_egl_display,
			context->globox_egl_config,
			(EGLNativeWindowType) platform->globox_x11_win,
			NULL);

	if (context->globox_egl_surface == EGL_NO_SURFACE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EGL_FAIL);
		return;
	}

	// bind context to surface
	EGLBoolean status_egl;

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
			GLOBOX_ERROR_X11_EGL_FAIL);
		return;
	}

	// set swap interval
	status_egl =
		eglSwapInterval(
			context->globox_egl_display,
			0);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EGL_FAIL);
		return;
	}
}

void globox_context_egl_shrink(struct globox* globox)
{

}

void globox_context_egl_free(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_x11_egl* context = &(platform->globox_x11_egl);

	EGLBoolean status_egl;

	status_egl =
		eglDestroySurface(
			context->globox_egl_display,
			context->globox_egl_surface);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EGL_FAIL);
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
			GLOBOX_ERROR_X11_EGL_FAIL);
		return;
	}

	status_egl =
		eglTerminate(
			context->globox_egl_display);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EGL_FAIL);
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
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_x11_egl* context = &(platform->globox_x11_egl);

	EGLBoolean status_egl =
		eglSwapBuffers(
			context->globox_egl_display,
			context->globox_egl_surface);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_EGL_FAIL);
		return;
	}

	globox->globox_redraw = false;
}

void globox_context_egl_reserve(struct globox* globox)
{

}

void globox_context_egl_expose(struct globox* globox, int len)
{

}

// getters

EGLDisplay globox_egl_get_display(struct globox* globox)
{
	return globox->globox_platform.globox_x11_egl.globox_egl_display;
}

EGLContext globox_egl_get_context(struct globox* globox)
{
	return globox->globox_platform.globox_x11_egl.globox_egl_context;
}

EGLSurface globox_egl_get_surface(struct globox* globox)
{
	return globox->globox_platform.globox_x11_egl.globox_egl_surface;
}

EGLConfig globox_egl_get_config(struct globox* globox)
{
	return globox->globox_platform.globox_x11_egl.globox_egl_config;
}

EGLint globox_egl_config_get_config_size(struct globox* globox)
{
	return globox->globox_platform.globox_x11_egl.globox_egl_config_size;
}
