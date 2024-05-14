#include "include/globuf.h"
#include "include/globuf_opengl.h"
#include "include/globuf_win_wgl.h"

#include "common/globuf_private.h"
#include "win/win_common.h"
#include "win/win_common_helpers.h"
#include "win/win_wgl.h"
#include "win/win_wgl_helpers.h"

#include <stdlib.h>
#include <wingdi.h>

#include <GL/gl.h>
#undef WGL_WGLEXT_PROTOTYPES
#include <GL/wglext.h>

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

void globuf_win_wgl_init(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// allocate the backend
	struct win_wgl_backend* backend = malloc(sizeof (struct win_wgl_backend));

	if (backend == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct win_wgl_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize the platform
	struct win_platform* platform = &(backend->platform);
	globuf_win_common_init(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// initialize backend
	backend->config = NULL;

	backend->wgl = NULL;
	backend->device_context = NULL;

	PIXELFORMATDESCRIPTOR format_descriptor = {0};
	backend->format_descriptor = format_descriptor;

	// error always set
}

void globuf_win_wgl_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// clean the platform
	globuf_win_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globuf_win_wgl_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// configure features here
	globuf_win_helpers_features_init(context, platform, configs, count, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// run common win32 helper
	globuf_win_common_window_create(
		context,
		platform,
		configs,
		count,
		callback,
		data,
		error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_win_wgl_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// run common win32 helper
	globuf_win_common_window_destroy(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_win_wgl_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_window_confirm(context, platform, error);

	// error always set
}

void globuf_win_wgl_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;
	BOOL ok;

	// set win32 render helper
	platform->render_init_callback = win_helpers_wgl_render;

	// run common win32 helper
	globuf_win_common_window_start(context, platform, error);

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// get wgl context
	HDC device_context = GetDC(platform->event_handle);

	if (device_context == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_DEVICE_CONTEXT_GET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// choose pixel format
	PIXELFORMATDESCRIPTOR format_descriptor =
	{
		.nSize = sizeof (PIXELFORMATDESCRIPTOR),
		.nVersion = 1,
		.dwFlags =
			PFD_DRAW_TO_WINDOW
			| PFD_SUPPORT_OPENGL
			| PFD_SUPPORT_COMPOSITION
			| PFD_DOUBLEBUFFER,
		.iPixelType = PFD_TYPE_RGBA,
		.cColorBits = 32,
		.cRedBits = 0,
		.cRedShift = 0,
		.cGreenBits = 0,
		.cGreenShift = 0,
		.cBlueBits = 0,
		.cBlueShift = 0,
		.cAlphaBits = 0,
		.cAlphaShift = 0,
		.cAccumBits = 0,
		.cAccumRedBits = 0,
		.cAccumGreenBits = 0,
		.cAccumBlueBits = 0,
		.cAccumAlphaBits = 0,
		.cDepthBits = 16,
		.cStencilBits = 0,
		.cAuxBuffers = 0,
		.iLayerType = PFD_MAIN_PLANE,
		.bReserved = 0,
		.dwLayerMask = 0,
		.dwVisibleMask = 0,
		.dwDamageMask = 0,
	};

	backend->format_descriptor = format_descriptor;

	int pixel_format = ChoosePixelFormat(device_context, &format_descriptor);

	if (pixel_format == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_PIXEL_FORMAT_CHOOSE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// set pixel format
	ok = SetPixelFormat(device_context, pixel_format, &format_descriptor);

	if (ok == FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_PIXEL_FORMAT_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// create WGL context
	backend->wgl = wglCreateContext(device_context);

	if (backend->wgl == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_CONTEXT_CREATE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// set WGL context
	ok = wglMakeCurrent(device_context, backend->wgl);

	if (ok == FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_CONTEXT_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// load WGL extensions
	wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)
			wglGetProcAddress("wglCreateContextAttribsARB");

	if (wglCreateContextAttribsARB == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_FUNC_LOAD);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	wglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)
			wglGetProcAddress("wglChoosePixelFormatARB");

	if (wglChoosePixelFormatARB == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_FUNC_LOAD);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// destroy WGL context
	ok = wglMakeCurrent(device_context, NULL);

	if (ok == FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_CONTEXT_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	ok = wglDeleteContext(backend->wgl);

	if (ok == FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_CONTEXT_DESTROY);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// choose new pixel format with extra attributes
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

	int pixel_formats[1];
	UINT pixel_formats_count;

	ok =
		wglChoosePixelFormatARB(
			device_context,
			attr_wgl,
			NULL,
			1,
			pixel_formats,
			&pixel_formats_count);

	if (ok == FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_PIXEL_FORMAT_CHOOSE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// set new pixel format
	ok = SetPixelFormat(device_context, pixel_formats[0], &format_descriptor);

	if (ok == FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_PIXEL_FORMAT_SET);
#if defined(GLOBUF_COMPAT_WINE)
		globuf_error_ok(error);
#else
		ReleaseMutex(platform->mutex_main);
		return;
#endif
	}

	// create new context with extra attributes
	int attr_gl[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, backend->config->major_version,
		WGL_CONTEXT_MINOR_VERSION_ARB, backend->config->minor_version,
		0,
	};

	backend->wgl =
		wglCreateContextAttribsARB(
			device_context,
			NULL,
			attr_gl);

	if (backend->wgl == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_CONTEXT_CREATE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	backend->device_context = device_context;

	platform->render = true;
	WakeConditionVariable(&(platform->cond_render));

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globuf_win_wgl_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper (mutex locked when unblocked)
	globuf_win_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_win_wgl_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// destroy WGL context
	BOOL ok = wglDeleteContext(backend->wgl);

	if (ok == FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_CONTEXT_DESTROY);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// run common win32 helper
	globuf_win_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	// error always set
}


void globuf_win_wgl_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_win_wgl_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globuf_event globuf_win_wgl_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	enum globuf_event out =
		globuf_win_common_handle_events(
			context,
			platform,
			event,
			error);

	// error always set
	return out;
}


struct globuf_config_features* globuf_win_wgl_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	struct globuf_config_features* features =
		globuf_win_common_init_features(context, platform, error);

	// return the newly created features info structure
	// error always set
	return features;
}

void globuf_win_wgl_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globuf_win_wgl_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globuf_win_wgl_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globuf_win_wgl_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globuf_win_wgl_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globuf_win_common_get_width(context, platform, error);
}

unsigned globuf_win_wgl_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globuf_win_common_get_height(context, platform, error);
}

struct globuf_rect globuf_win_wgl_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globuf_win_common_get_expose(context, platform, error);
}


void globuf_win_wgl_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// get device context
	HDC device_context = GetDC(platform->event_handle);

	if (device_context == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_DEVICE_CONTEXT_GET);
		return;
	}

	// swap buffers
	BOOL ok = wglSwapLayerBuffers(device_context, WGL_SWAP_MAIN_PLANE);

	if (ok == FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_WGL_SWAP);
		return;
	}

	// release device context
	ReleaseDC(platform->event_handle, device_context);
	globuf_error_ok(error);
}

void* globuf_win_wgl_callback(
	struct globuf* context)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	return platform;
}


// OpenGL configuration setter
void globuf_init_win_wgl(
	struct globuf* context,
	struct globuf_config_opengl* config,
	struct globuf_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;

	backend->config = config;

	globuf_error_ok(error);
}


void globuf_prepare_init_win_wgl(
	struct globuf_config_backend* config,
	struct globuf_error_info* error)
{
	struct globuf_calls_opengl* opengl =
		malloc(sizeof (struct globuf_calls_opengl));

	if (opengl == NULL)
	{
		error->code = GLOBUF_ERROR_ALLOC;
		error->file = __FILE__;
		error->line = __LINE__;
		return;
	}

	opengl->init = globuf_init_win_wgl;

	config->data = opengl;
	config->callback = globuf_win_wgl_callback;
	config->init = globuf_win_wgl_init;
	config->clean = globuf_win_wgl_clean;
	config->window_create = globuf_win_wgl_window_create;
	config->window_destroy = globuf_win_wgl_window_destroy;
	config->window_start = globuf_win_wgl_window_start;
	config->window_confirm = globuf_win_wgl_window_confirm;
	config->window_block = globuf_win_wgl_window_block;
	config->window_stop = globuf_win_wgl_window_stop;
	config->init_render = globuf_win_wgl_init_render;
	config->init_events = globuf_win_wgl_init_events;
	config->handle_events = globuf_win_wgl_handle_events;
	config->init_features = globuf_win_wgl_init_features;
	config->feature_set_interaction = globuf_win_wgl_feature_set_interaction;
	config->feature_set_state = globuf_win_wgl_feature_set_state;
	config->feature_set_title = globuf_win_wgl_feature_set_title;
	config->feature_set_icon = globuf_win_wgl_feature_set_icon;
	config->get_width = globuf_win_wgl_get_width;
	config->get_height = globuf_win_wgl_get_height;
	config->get_expose = globuf_win_wgl_get_expose;
	config->update_content = globuf_win_wgl_update_content;

	globuf_error_ok(error);
}
