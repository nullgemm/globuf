#include "include/globox.h"
#include "include/globox_software.h"
#include "include/globox_win_software.h"

#include "common/globox_private.h"
#include "win/win_common.h"
#include "win/win_common_helpers.h"
#include "win/win_software.h"
#include "win/win_software_helpers.h"

#include <stdint.h>
#include <winuser.h>
#include <stdlib.h>

void globox_win_software_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct win_software_backend* backend = malloc(sizeof (struct win_software_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct win_software_backend zero = {0};
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
	BITMAPINFO bmp_info = {0};
	backend->bmp_info = bmp_info;
	backend->bmp_handle = NULL;

	// error always set
}

void globox_win_software_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// clean the platform
	globox_win_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globox_win_software_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
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
	win_helpers_features_init(context, platform, configs, count, error);

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

void globox_win_software_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
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

void globox_win_software_window_confirm(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_window_confirm(context, platform, error);

	// error always set
}

void globox_win_software_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// run common win32 helper
	globox_win_common_window_start(context, platform, error);

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	platform->render = true;
	WakeConditionVariable(&(platform->cond_render));

	// create software context
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
	backend->bmp_info = bmp_info;

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globox_win_software_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper (mutex locked when unblocked)
	globox_win_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_win_software_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
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


void globox_win_software_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globox_win_software_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globox_event globox_win_software_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	enum globox_event out =
		globox_win_common_handle_events(
			context,
			platform,
			event,
			error);

	return out;
}


struct globox_config_features* globox_win_software_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	struct globox_config_features* features =
		globox_win_common_init_features(context, platform, error);

	return features;
}

void globox_win_software_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_win_software_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_win_software_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_win_software_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globox_win_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_win_software_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globox_win_common_get_width(context, platform, error);
}

unsigned globox_win_software_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globox_win_common_get_height(context, platform, error);
}

struct globox_rect globox_win_software_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globox_win_common_get_expose(context, platform, error);
}


void globox_win_software_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	struct globox_update_software* update = data;
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// proceed
	BOOL ok;

	// damage region
	RECT region =
	{
		.left = update->x,
		.top = update->y,
		.right = update->x + update->width,
		.bottom = update->y + update->height,
	};

	ok = InvalidateRect(platform->event_handle, &region, TRUE);

	if (ok == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_GDI_DAMAGE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// get device context
	HDC device_context_win = GetDC(platform->event_handle);

	if (device_context_win == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_DEVICE_CONTEXT_GET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// create compatibility device context
	HDC device_context_compat = CreateCompatibleDC(device_context_win);

	if (device_context_compat == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_DEVICE_CONTEXT_CREATE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// create compatibility bitmap
	HBITMAP bmp_compat_old =
		(HBITMAP)
			SelectObject(
				device_context_compat,
				backend->bmp_handle);

	if (bmp_compat_old == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_BMP_CREATE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// copy buffer
	ok = BitBlt(
		device_context_win,
		update->x,
		update->y,
		update->width,
		update->height,
		device_context_compat,
		update->x,
		update->y,
		SRCCOPY);

	if (ok == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_GDI_BITBLT);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// get compatibility bitmap
	HBITMAP bmp_old =
		(HBITMAP)
			SelectObject(device_context_compat, bmp_compat_old);

	if (bmp_old == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_BMP_GET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// delete compat device context
	ok = DeleteDC(device_context_compat);

	if (ok == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_DEVICE_CONTEXT_DELETE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	ReleaseDC(platform->event_handle, device_context_win);
	GdiFlush();

	// tell windows we updated the damaged region
	ok = ValidateRect(platform->event_handle, &region);

	if (ok == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_PAINT_VALIDATE);
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


void globox_prepare_init_win_software(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	config->data = NULL;
	config->init = globox_win_software_init;
	config->clean = globox_win_software_clean;
	config->window_create = globox_win_software_window_create;
	config->window_destroy = globox_win_software_window_destroy;
	config->window_confirm = globox_win_software_window_confirm;
	config->window_start = globox_win_software_window_start;
	config->window_block = globox_win_software_window_block;
	config->window_stop = globox_win_software_window_stop;
	config->init_render = globox_win_software_init_render;
	config->init_events = globox_win_software_init_events;
	config->handle_events = globox_win_software_handle_events;
	config->init_features = globox_win_software_init_features;
	config->feature_set_interaction = globox_win_software_feature_set_interaction;
	config->feature_set_state = globox_win_software_feature_set_state;
	config->feature_set_title = globox_win_software_feature_set_title;
	config->feature_set_icon = globox_win_software_feature_set_icon;
	config->get_width = globox_win_software_get_width;
	config->get_height = globox_win_software_get_height;
	config->get_expose = globox_win_software_get_expose;
	config->update_content = globox_win_software_update_content;

	globox_error_ok(error);
}


// simple allocator we provide so developers don't try to recycle buffers
// (it would not be thread-safe and break this multi-threaded version of globox)
uint32_t* globox_buffer_alloc_win_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		return NULL;
	}

	// update bitmap info
	backend->bmp_info.bmiHeader.biWidth = (LONG) width;
	backend->bmp_info.bmiHeader.biHeight = (LONG) height;

	// get device context
	HDC device_context = GetDC(platform->event_handle);

	if (device_context == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_DEVICE_CONTEXT_GET);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// get device independent bitmap
	uint32_t* argb = NULL;

	backend->bmp_handle =
		CreateDIBSection(
			device_context,
			&(backend->bmp_info),
			DIB_RGB_COLORS,
			(void**) &argb,
			NULL,
			0);

	if ((backend->bmp_handle == NULL) || (argb == NULL))
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_DIB_CREATE);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// release device context
	ReleaseDC(platform->event_handle, device_context);

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		return NULL;
	}

	globox_error_ok(error);
	return argb;
}

void globox_buffer_free_win_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error)
{
	struct win_software_backend* backend = context->backend_data;
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

	// proceed
	if (backend->bmp_handle != NULL)
	{
		BOOL ok = DeleteObject(backend->bmp_handle);

		if (ok == 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WIN_OBJECT_DELETE);
			ReleaseMutex(platform->mutex_main);
			return;
		}
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
