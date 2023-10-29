#include "include/globox.h"
#include "include/globox_opengl.h"
#include "include/globox_win_wgl.h"

#include "common/globox_private.h"
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

void globox_win_wgl_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct win_wgl_backend* backend = malloc(sizeof (struct win_wgl_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct win_wgl_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize the platform
	struct win_platform* platform = &(backend->platform);
	globox_win_common_init(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
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

void globox_win_wgl_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// clean the platform
	globox_win_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globox_win_wgl_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// configure features here
	globox_win_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// run common win32 helper
	globox_win_common_window_create(
		context,
		platform,
		configs,
		count,
		callback,
		data,
		error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_win_wgl_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// run common win32 helper
	globox_win_common_window_destroy(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_win_wgl_window_confirm(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_window_confirm(context, platform, error);

	// error always set
}

void globox_win_wgl_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;
	BOOL ok;

	// set win32 render helper
	platform->render_init_callback = win_helpers_wgl_render;

	// run common win32 helper
	globox_win_common_window_start(context, platform, error);

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// get wgl context
	HDC device_context = GetDC(platform->event_handle);

	if (device_context == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_DEVICE_CONTEXT_GET);
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
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_PIXEL_FORMAT_CHOOSE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// set pixel format
	ok = SetPixelFormat(device_context, pixel_format, &format_descriptor);

	if (ok == FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_PIXEL_FORMAT_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// create WGL context
	backend->wgl = wglCreateContext(device_context);

	if (backend->wgl == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_CONTEXT_CREATE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// set WGL context
	ok = wglMakeCurrent(device_context, backend->wgl);

	if (ok == FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_CONTEXT_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// load WGL extensions
	wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)
			wglGetProcAddress("wglCreateContextAttribsARB");

	if (wglCreateContextAttribsARB == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_FUNC_LOAD);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	wglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)
			wglGetProcAddress("wglChoosePixelFormatARB");

	if (wglChoosePixelFormatARB == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_FUNC_LOAD);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// destroy WGL context
	ok = wglMakeCurrent(device_context, NULL);

	if (ok == FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_CONTEXT_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	ok = wglDeleteContext(backend->wgl);

	if (ok == FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_CONTEXT_DESTROY);
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
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_PIXEL_FORMAT_CHOOSE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// set new pixel format
	ok = SetPixelFormat(device_context, pixel_formats[0], &format_descriptor);

	if (ok == FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_PIXEL_FORMAT_SET);
#if defined(GLOBOX_COMPAT_WINE)
		globox_error_ok(error);
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
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_CONTEXT_CREATE);
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
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globox_win_wgl_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper (mutex locked when unblocked)
	globox_win_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_win_wgl_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// destroy WGL context
	BOOL ok = wglDeleteContext(backend->wgl);

	if (ok == FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_CONTEXT_DESTROY);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// run common win32 helper
	globox_win_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	// error always set
}


void globox_win_wgl_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globox_win_wgl_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globox_event globox_win_wgl_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	enum globox_event out =
		globox_win_common_handle_events(
			context,
			platform,
			event,
			error);

	// error always set
	return out;
}


struct globox_config_features* globox_win_wgl_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	struct globox_config_features* features =
		globox_win_common_init_features(context, platform, error);

	// return the newly created features info structure
	// error always set
	return features;
}

void globox_win_wgl_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_win_wgl_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_win_wgl_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_win_wgl_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_win_wgl_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globox_win_common_get_width(context, platform, error);
}

unsigned globox_win_wgl_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globox_win_common_get_height(context, platform, error);
}

struct globox_rect globox_win_wgl_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globox_win_common_get_expose(context, platform, error);
}


void globox_win_wgl_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// get device context
	HDC device_context = GetDC(platform->event_handle);

	if (device_context == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_DEVICE_CONTEXT_GET);
		return;
	}

	// swap buffers
	BOOL ok = wglSwapLayerBuffers(device_context, WGL_SWAP_MAIN_PLANE);

	if (ok == FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WGL_SWAP);
		return;
	}

	// release device context
	ReleaseDC(platform->event_handle, device_context);
	globox_error_ok(error);
}

void* globox_win_wgl_callback(
	struct globox* context)
{
	struct win_wgl_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	return platform;
}


// OpenGL configuration setter
void globox_init_win_wgl(
	struct globox* context,
	struct globox_config_opengl* config,
	struct globox_error_info* error)
{
	struct win_wgl_backend* backend = context->backend_data;

	backend->config = config;

	globox_error_ok(error);
}


void globox_prepare_init_win_wgl(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	struct globox_calls_opengl* opengl =
		malloc(sizeof (struct globox_calls_opengl));

	if (opengl == NULL)
	{
		error->code = GLOBOX_ERROR_ALLOC;
		error->file = __FILE__;
		error->line = __LINE__;
		return;
	}

	opengl->init = globox_init_win_wgl;

	config->data = opengl;
	config->callback = globox_win_wgl_callback;
	config->init = globox_win_wgl_init;
	config->clean = globox_win_wgl_clean;
	config->window_create = globox_win_wgl_window_create;
	config->window_destroy = globox_win_wgl_window_destroy;
	config->window_start = globox_win_wgl_window_start;
	config->window_confirm = globox_win_wgl_window_confirm;
	config->window_block = globox_win_wgl_window_block;
	config->window_stop = globox_win_wgl_window_stop;
	config->init_render = globox_win_wgl_init_render;
	config->init_events = globox_win_wgl_init_events;
	config->handle_events = globox_win_wgl_handle_events;
	config->init_features = globox_win_wgl_init_features;
	config->feature_set_interaction = globox_win_wgl_feature_set_interaction;
	config->feature_set_state = globox_win_wgl_feature_set_state;
	config->feature_set_title = globox_win_wgl_feature_set_title;
	config->feature_set_icon = globox_win_wgl_feature_set_icon;
	config->get_width = globox_win_wgl_get_width;
	config->get_height = globox_win_wgl_get_height;
	config->get_expose = globox_win_wgl_get_expose;
	config->update_content = globox_win_wgl_update_content;

	globox_error_ok(error);
}
