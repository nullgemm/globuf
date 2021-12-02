#include "globox.h"
#include "globox_error.h"

#include <EGL/egl.h>
#include <stdlib.h>

#include "macos/globox_macos_types.h"
#include "macos/globox_macos_symbols.h"

#include "macos/globox_macos.h"
#include "macos/egl/globox_macos_egl.h"

void appdelegate_egl_callback(
	struct macos_appdelegate* appdelegate,
	SEL cmd,
	id msg)
{
	void* out = NULL;

	object_getInstanceVariable(
		(id) appdelegate,
		"globox",
		&out);

	if (out == NULL)
	{
		return;
	}

	struct globox* globox = (struct globox*) out;
	struct globox_platform* platform = globox->globox_platform;
	struct globox_macos_egl* context = &(platform->globox_macos_egl);

	// initialize the view object with the view frame
	id tmp;

	struct macos_rect frame =
	{
		.origin.x = 0,
		.origin.y = 0,
		.size.width = globox->globox_width,
		.size.height = globox->globox_height,
	};

	platform->globox_macos_old_window_frame = frame;

	tmp =
		macos_msg_id_rect(
			platform->globox_macos_obj_masterview,
			sel_getUid("initWithFrame:"),
			frame);

	if (tmp == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	platform->globox_macos_obj_masterview = tmp;

	tmp =
		macos_msg_id_rect(
			platform->globox_macos_obj_view,
			sel_getUid("initWithFrame:"),
			frame);

	if (tmp == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	platform->globox_macos_obj_view = tmp;

	macos_msg_void_bool(
		platform->globox_macos_obj_view,
		sel_getUid("setWantsLayer:"),
		YES);

	tmp =
		macos_msg_id_none(
			platform->globox_macos_obj_view,
			sel_getUid("layer"));

	if (tmp == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	context->globox_egl_layer = tmp;

	tmp =
		macos_msg_id_rect(
			platform->globox_macos_obj_blur,
			sel_getUid("initWithFrame:"),
			frame);

	if (tmp == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	platform->globox_macos_obj_blur = tmp;
}

void globox_context_egl_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_macos_egl* context = &(platform->globox_macos_egl);

	platform->globox_macos_appdelegate_context_callback =
		appdelegate_egl_callback;

	// egl
	context->globox_egl_display = EGL_NO_DISPLAY;
	context->globox_egl_context = EGL_NO_CONTEXT;
	context->globox_egl_surface = EGL_NO_SURFACE;

	context->globox_egl_display =
		eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (context->globox_egl_display == EGL_NO_DISPLAY)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

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
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

	status_egl =
		eglBindAPI(
			EGL_OPENGL_ES_API);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

	EGLint egl_config_attrib[] =
	{
		 EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		 EGL_RED_SIZE, 8,
		 EGL_GREEN_SIZE, 8,
		 EGL_BLUE_SIZE, 8,
		 EGL_ALPHA_SIZE, 8,
		 EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
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
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

	EGLint egl_context_attrib[] =
	{
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
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

	return;
}

void globox_context_egl_create(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_macos_egl* context = &(platform->globox_macos_egl);

	context->globox_egl_surface =
		eglCreateWindowSurface(
			context->globox_egl_display,
			context->globox_egl_config,
			(EGLNativeWindowType) context->globox_egl_layer,
			NULL);

	if (context->globox_egl_surface == EGL_NO_SURFACE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

	// egl set-up
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
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

	status_egl =
		eglSwapInterval(
			context->globox_egl_display,
			0);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_EGL_FAIL);
	}
}

void globox_context_egl_shrink(struct globox* globox)
{
	// not needed
}

void globox_context_egl_free(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;
	struct globox_macos_egl* context = &(platform->globox_macos_egl);

	EGLBoolean status_egl;

	status_egl =
		eglDestroySurface(
			context->globox_egl_display,
			context->globox_egl_surface);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_EGL_FAIL);
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
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

	status_egl =
		eglTerminate(
			context->globox_egl_display);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_EGL_FAIL);
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
	struct globox_macos_egl* context = &(platform->globox_macos_egl);

	EGLBoolean status_egl =
		eglSwapBuffers(
			context->globox_egl_display,
			context->globox_egl_surface);

	if (status_egl == EGL_FALSE)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_EGL_FAIL);
		return;
	}

	globox->globox_redraw = false;

	macos_msg_void_none(
		context->globox_egl_layer,
		sel_getUid("setNeedsDisplay"));

	struct macos_rect frame =
		macos_msg_rect_none(
			platform->globox_macos_obj_window,
			sel_getUid("frame"));

	globox->globox_width =
		frame.size.width;

	globox->globox_height =
		frame.size.height;

	globox_platform_commit(globox);
}
