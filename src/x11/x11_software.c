#include "include/globox.h"
#include "include/globox_software.h"
#include "include/globox_x11_software.h"

#include "common/globox_private.h"
#include "x11/x11_common.h"
#include "x11/x11_common_helpers.h"
#include "x11/x11_software.h"
#include "x11/x11_software_helpers.h"

#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <xcb/shm.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>

void globox_x11_software_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct x11_backend* backend = malloc(sizeof (struct x11_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct x11_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize the platform
	globox_x11_common_init(context, &(backend->platform), error);

	// error always set
}

void globox_x11_software_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// clean the platform
	globox_x11_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globox_x11_software_window_create(
	struct globox* context,
	void** features,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int posix_error = pthread_mutex_lock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// select visual configuration
	if (context->feature_background != GLOBOX_BACKGROUND_OPAQUE)
	{
		x11_helpers_visual_transparent(context, error);

		if (globox_error_get_code(error) == GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE)
		{
			x11_helpers_visual_opaque(context, error);
		}
		else if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			return;
		}
	}
	else
	{
		x11_helpers_visual_opaque(context, error);

		if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			return;
		}
	}

	// run common X11 helper
	globox_x11_common_window_create(context, platform, features, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// create graphics context
	uint32_t values[2] =
	{
		platform->screen_obj->black_pixel,
		0,
	};

	backend->software_gfx =
		xcb_generate_id(
			platform->conn);

	xcb_void_cookie_t cookie_gc =
		xcb_create_gc_checked(
			platform->conn,
			backend->software_gfx,
			platform->win,
			XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES,
			values);

	xcb_generic_error_t* error_gc =
		xcb_request_check(
			platform->conn,
			cookie_gc);

	if (error_gc != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GC_CREATE);
		return;
	}

	// we are not done yet as we wish to bypass the xcb drawing API to
	// write directly to a shared memory buffer (just like CPU wayland)

	// test whether the shm extension is available
	const xcb_query_extension_reply_t* query_reply =
		xcb_get_extension_data(
			platform->conn,
			&xcb_shm_id);

	if (query_reply->present != 0)
	{
		// test whether shared buffers can be used
		xcb_generic_error_t* error_shm;

		xcb_shm_query_version_cookie_t cookie_shm =
			xcb_shm_query_version(
				platform->conn);

		xcb_shm_query_version_reply_t* reply_shm =
			xcb_shm_query_version_reply(
				platform->conn,
				cookie_shm,
				&error_shm);

		if ((error_shm != NULL) || (reply_shm == NULL))
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_SHM_VERSION);
			return;
		}

		backend->shared_pixmaps = reply_shm->shared_pixmaps;
		free(reply_shm);

		if (backend->shared_pixmaps == true)
		{
			backend->software_shm.shmseg =
				xcb_generate_id(
					platform->conn);
		}
	}
	else
	{
		backend->shared_pixmaps = false;
	}

	backend->software_pixmap =
		xcb_generate_id(
			platform->conn);

	// unlock mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_x11_software_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int posix_error = pthread_mutex_lock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_destroy(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	xcb_free_pixmap(platform->conn, backend->software_pixmap);

	// unlock mutex
	posix_error = pthread_mutex_unlock(&(platform->mutex_main));

	if (posix_error != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_x11_software_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_software_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper (mutex locked when unblocked)
	globox_x11_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_software_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globox_x11_software_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_software_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globox_event globox_x11_software_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	enum globox_event out =
		globox_x11_common_handle_events(
			context,
			platform,
			event,
			error);

	// no extra failure check at the moment

	// error always set
	return out;
}


struct globox_config_features* globox_x11_software_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	struct globox_config_features* features =
		globox_x11_common_init_features(context, platform, error);

	// no extra failure check at the moment

	// return the newly created features info structure
	// error always set
	return features;
}

void globox_x11_software_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_x11_software_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_x11_software_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_x11_software_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_icon(context, platform, config, error);

	// error always set
}

void globox_x11_software_feature_set_size(
	struct globox* context,
	struct globox_feature_size* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_size(context, platform, config, error);

	// error always set
}

void globox_x11_software_feature_set_pos(
	struct globox* context,
	struct globox_feature_pos* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_pos(context, platform, config, error);

	// error always set
}

void globox_x11_software_feature_set_frame(
	struct globox* context,
	struct globox_feature_frame* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_frame(context, platform, config, error);

	// error always set
}

void globox_x11_software_feature_set_background(
	struct globox* context,
	struct globox_feature_background* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_background(context, platform, config, error);

	// error always set
}

void globox_x11_software_feature_set_vsync(
	struct globox* context,
	struct globox_feature_vsync* config,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_vsync(context, platform, config, error);

	// error always set
}


void globox_x11_software_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);
	struct globox_update_software* update = data;

	// unlike wayland, X can't automatically copy buffers from cpu to gpu
	// so if the display server is running in DRM we need to do it manually
	// for this we can use xcb_put_image() to transfer the data using a socket
	if (backend->shared_pixmaps == false)
	{
		int y2 = update->y;
		unsigned height2 = update->height;

		size_t len =
			sizeof (xcb_get_image_request_t);

		size_t len_theoric =
			(len + (4 * context->feature_size->width * height2)) >> 2;

		uint64_t len_max =
			xcb_get_maximum_request_length(
				platform->conn);

		xcb_void_cookie_t cookie_pixmap =
			xcb_create_pixmap_checked(
				platform->conn,
				platform->visual_depth,
				backend->software_pixmap,
				platform->win,
				context->feature_size->width,
				context->feature_size->height);

		xcb_generic_error_t* error_pixmap =
			xcb_request_check(
				platform->conn,
				cookie_pixmap);

		if (error_pixmap != NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_PIXMAP);
			return;
		}

		xcb_void_cookie_t cookie_image;
		xcb_generic_error_t* error_image;

		if (len_theoric >= len_max)
		{
			uint64_t rows_batch =
				((len_max << 2) - len)
				/ (4 * context->feature_size->width);

			while (rows_batch <= height2)
			{
				cookie_image =
					xcb_put_image_checked(
						platform->conn,
						XCB_IMAGE_FORMAT_Z_PIXMAP,
						backend->software_pixmap,
						backend->software_gfx,
						context->feature_size->width,
						rows_batch,
						0,
						y2,
						0,
						platform->visual_depth,
						4 * context->feature_size->width * rows_batch,
						(void*)(update->buf + y2*context->feature_size->width));

				error_image =
					xcb_request_check(
						platform->conn,
						cookie_image);

				if (error_image != NULL)
				{
					globox_error_throw(context, error, GLOBOX_ERROR_X11_IMAGE);
					return;
				}

				y2 += rows_batch;
				height2 -= rows_batch;
			}
		}

		cookie_image =
			xcb_put_image_checked(
				platform->conn,
				XCB_IMAGE_FORMAT_Z_PIXMAP,
				backend->software_pixmap,
				backend->software_gfx,
				context->feature_size->width,
				height2,
				0,
				y2,
				0,
				platform->visual_depth,
				4 * context->feature_size->width * height2,
				(void*)(update->buf + y2*context->feature_size->width));

		error_image =
			xcb_request_check(
				platform->conn,
				cookie_image);

		if (error_image != NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_IMAGE);
			return;
		}
	}
	else
	{
		xcb_void_cookie_t cookie_pixmap =
			xcb_shm_create_pixmap_checked(
				platform->conn,
				backend->software_pixmap,
				platform->win,
				context->feature_size->width,
				context->feature_size->height,
				platform->visual_depth,
				backend->software_shm.shmseg,
				0);

		xcb_generic_error_t* error_pixmap =
			xcb_request_check(
				platform->conn,
				cookie_pixmap);

		if (error_pixmap != NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_PIXMAP);
			return;
		}
	}

	xcb_void_cookie_t cookie_copy =
		xcb_copy_area_checked(
			platform->conn,
			backend->software_pixmap,
			platform->win,
			backend->software_gfx,
			update->x,
			update->y,
			update->x,
			update->y,
			update->width,
			update->height);

	xcb_generic_error_t* error_copy =
		xcb_request_check(
			platform->conn,
			cookie_copy);

	if (error_copy != NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_COPY);
		return;
	}

	int error_flush = xcb_flush(platform->conn);

	if (error_flush <= 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_FLUSH);
		return;
	}

	xcb_free_pixmap(
		platform->conn,
		backend->software_pixmap);

	globox_error_ok(error);
}

void globox_prepare_init_x11_software(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	config->data = NULL;
	config->init = globox_x11_software_init;
	config->clean = globox_x11_software_clean;
	config->window_create = globox_x11_software_window_create;
	config->window_destroy = globox_x11_software_window_destroy;
	config->window_start = globox_x11_software_window_start;
	config->window_block = globox_x11_software_window_block;
	config->window_stop = globox_x11_software_window_stop;
	config->init_features = globox_x11_software_init_features;
	config->init_render = globox_x11_software_init_render;
	config->init_events = globox_x11_software_init_events;
	config->handle_events = globox_x11_software_handle_events;
	config->feature_set_interaction = globox_x11_software_feature_set_interaction;
	config->feature_set_state = globox_x11_software_feature_set_state;
	config->feature_set_title = globox_x11_software_feature_set_title;
	config->feature_set_icon = globox_x11_software_feature_set_icon;
	config->feature_set_size = globox_x11_software_feature_set_size;
	config->feature_set_pos = globox_x11_software_feature_set_pos;
	config->feature_set_frame = globox_x11_software_feature_set_frame;
	config->feature_set_background = globox_x11_software_feature_set_background;
	config->feature_set_vsync = globox_x11_software_feature_set_vsync;
	config->update_content = globox_x11_software_update_content;

	globox_error_ok(error);
}

// simple allocator we provide so developers don't try to recycle buffers
// (it would not be thread-safe and break this multi-threaded version of globox)
uint32_t* globox_buffer_alloc_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	uint32_t* argb = NULL;
	size_t len = 4 * width * height;

	if (backend->shared_pixmaps == false)
	{
		argb = malloc(len);

		if (argb == NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
			return NULL;
		}
	}
	else
	{
		x11_helpers_shm_create(context, len, error);

		if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			return NULL;
		}

		argb = (uint32_t*) backend->software_shm.shmaddr;
	}

	globox_error_ok(error);
	return argb;
}

void globox_buffer_free_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	if (backend->shared_pixmaps == false)
	{
		free(buffer);
	}
	else
	{
		xcb_void_cookie_t cookie_shm;
		xcb_generic_error_t* error_shm;

		cookie_shm =
			xcb_shm_detach_checked(
				platform->conn,
				backend->software_shm.shmseg);

		error_shm =
			xcb_request_check(
				platform->conn,
				cookie_shm);

		if (error_shm != NULL)
		{
			globox_error_throw(
				context,
				error,
				GLOBOX_ERROR_X11_SHM_DETACH);

			return;
		}

		int error_shmdt =
			shmdt(
				backend->software_shm.shmaddr);

		if (error_shmdt == -1)
		{
			globox_error_throw(
				context,
				error,
				GLOBOX_ERROR_POSIX_SHMDT);

			return;
		}
	}

	globox_error_ok(error);
}
