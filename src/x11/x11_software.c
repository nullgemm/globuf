#include "include/globuf.h"
#include "include/globuf_software.h"
#include "include/globuf_x11_software.h"

#include "common/globuf_private.h"
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

void globuf_x11_software_init(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// allocate the backend
	struct x11_software_backend* backend = malloc(sizeof (struct x11_software_backend));

	if (backend == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct x11_software_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize values that can be initialized explicitly
	backend->shared_pixmaps = false;
	backend->buffer_ptr = NULL;
	backend->buffer_len = 0;

	// open a connection to the X server
	struct x11_platform* platform = &(backend->platform);
	platform->conn = xcb_connect(NULL, &(platform->screen_id));
	int error_posix = xcb_connection_has_error(platform->conn);

	if (error_posix > 0)
	{
		xcb_disconnect(platform->conn);
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_CONN);
		return;
	}

	// initialize the platform
	globuf_x11_common_init(context, platform, error);

	// error always set
}

void globuf_x11_software_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// close the connection to the X server
	xcb_disconnect(platform->conn);

	// clean the platform
	globuf_x11_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globuf_x11_software_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	globuf_x11_helpers_features_init(context, platform, configs, count, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// select visual configuration
	if (context->feature_background->background != GLOBUF_BACKGROUND_OPAQUE)
	{
		x11_helpers_visual_transparent(context, error);

		if (globuf_error_get_code(error) == GLOBUF_ERROR_X11_VISUAL_INCOMPATIBLE)
		{
			x11_helpers_visual_opaque(context, error);
		}
	}
	else
	{
		x11_helpers_visual_opaque(context, error);
	}

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// run common X11 helper
	globuf_x11_common_window_create(context, platform, configs, count, callback, data, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
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
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_GC_CREATE);
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
			globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_SHM_VERSION);
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
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_x11_software_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_window_destroy(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	xcb_free_pixmap(platform->conn, backend->software_pixmap);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_x11_software_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_window_confirm(context, platform, error);

	// error always set
}

void globuf_x11_software_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_x11_software_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper (mutex locked when unblocked)
	globuf_x11_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_x11_software_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globuf_x11_software_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_x11_software_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globuf_event globuf_x11_software_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	enum globuf_event out =
		globuf_x11_common_handle_events(
			context,
			platform,
			event,
			error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return out;
	}

	// process configure event specifically
	xcb_generic_event_t* xcb_event = event;

	// only lock the main mutex when making changes to the context
	switch (xcb_event->response_type & ~0x80)
	{
		case XCB_CONFIGURE_NOTIFY:
		{
			xcb_configure_notify_event_t* configure =
				(xcb_configure_notify_event_t*) xcb_event;

			// lock xsync mutex
			int error_posix = pthread_mutex_lock(&(platform->mutex_xsync));

			if (error_posix != 0)
			{
				globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
				break;
			}

			// safe value updates
			if (platform->xsync_status == GLOBUF_XSYNC_CONFIGURED)
			{
				platform->xsync_status = GLOBUF_XSYNC_ACKNOWLEDGED;
			}

			// unlock xsync mutex
			error_posix = pthread_mutex_unlock(&(platform->mutex_xsync));

			if (error_posix != 0)
			{
				globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
				break;
			}

			out = GLOBUF_EVENT_MOVED_RESIZED;
			break;
		}
	}


	// error always set
	return out;
}


struct globuf_config_features* globuf_x11_software_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	struct globuf_config_features* features =
		globuf_x11_common_init_features(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return features;
	}

	// available if the _NET_SUPPPORTED prop. has the _NET_WM_FRAME_DRAWN atom
	xcb_generic_error_t* error_xcb;
	xcb_atom_t* atoms = platform->atoms;

	xcb_get_property_cookie_t cookie =
		xcb_get_property(
			platform->conn,
			0,
			platform->root_win,
			atoms[X11_ATOM_NET_SUPPORTED],
			XCB_ATOM_ATOM,
			0,
			1024);

	xcb_get_property_reply_t* reply =
		xcb_get_property_reply(
			platform->conn,
			cookie,
			&error_xcb);

	if (error_xcb != NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_PROP_GET);

		return features;
	}

	int net_atoms_count =
		xcb_get_property_value_length(reply) / (sizeof (xcb_atom_t));

	xcb_atom_t* net_atoms =
		xcb_get_property_value(reply);

	int i = 0;

	while (i < net_atoms_count)
	{
		if (net_atoms[i] == platform->atoms[X11_ATOM_FRAME_DRAWN])
		{
			features->list[features->count] = GLOBUF_FEATURE_VSYNC;
			context->feature_vsync =
				malloc(sizeof (struct globuf_feature_vsync));
			features->count += 1;

			if (context->feature_vsync == NULL)
			{
				globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
				return NULL;
			}

			break;
		}

		++i;
	}

	free(reply);

	// return the newly created features info structure
	// error always set
	return features;
}

void globuf_x11_software_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globuf_x11_software_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globuf_x11_software_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globuf_x11_software_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globuf_x11_software_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globuf_x11_common_get_width(context, platform, error);
}

unsigned globuf_x11_software_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globuf_x11_common_get_height(context, platform, error);
}

struct globuf_rect globuf_x11_software_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globuf_x11_common_get_expose(context, platform, error);
}


void globuf_x11_software_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);
	struct globuf_update_software* update = data;

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
			globuf_error_throw(context, error, GLOBUF_ERROR_X11_PIXMAP);
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
					globuf_error_throw(context, error, GLOBUF_ERROR_X11_IMAGE);
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
			globuf_error_throw(context, error, GLOBUF_ERROR_X11_IMAGE);
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
			globuf_error_throw(context, error, GLOBUF_ERROR_X11_PIXMAP);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_COPY);
		return;
	}

	int error_flush = xcb_flush(platform->conn);

	if (error_flush <= 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_FLUSH);
		return;
	}

	xcb_free_pixmap(
		platform->conn,
		backend->software_pixmap);

	globuf_error_ok(error);
}

void* globuf_x11_software_callback(
	struct globuf* context)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);
	return platform;
}


// simple allocator we provide so developers don't try to recycle buffers
// (it would not be thread-safe and break this multi-threaded version of globuf)
uint32_t* globuf_buffer_alloc_x11_software(
	struct globuf* context,
	unsigned width,
	unsigned height,
	struct globuf_error_info* error)
{
	struct x11_software_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	uint32_t* argb = NULL;
	size_t len = 4 * width * height;

	if (len == backend->buffer_len)
	{
		return backend->buffer_ptr;
	}

	if (backend->buffer_ptr != NULL)
	{
		if (backend->shared_pixmaps == false)
		{
			free(backend->buffer_ptr);
			backend->buffer_len = 0;
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

			backend->buffer_len = 0;

			if (error_shm != NULL)
			{
				globuf_error_throw(
					context,
					error,
					GLOBUF_ERROR_X11_SHM_DETACH);

				return NULL;
			}

			int error_shmdt = shmdt(backend->buffer_ptr);

			if (error_shmdt == -1)
			{
				globuf_error_throw(
					context,
					error,
					GLOBUF_ERROR_POSIX_SHMDT);

				return NULL;
			}
		}
	}

	if (backend->shared_pixmaps == false)
	{
		argb = malloc(len);

		if (argb == NULL)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
			return NULL;
		}
	}
	else
	{
		x11_helpers_shm_create(context, len, error);

		if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
		{
			return NULL;
		}

		argb = (uint32_t*) backend->software_shm.shmaddr;
	}

	backend->buffer_ptr = argb;
	backend->buffer_len = len;
	globuf_error_ok(error);
	return argb;
}

void globuf_buffer_free_x11_software(
	struct globuf* context,
	uint32_t* buffer,
	struct globuf_error_info* error)
{
	globuf_error_ok(error);
}


void globuf_prepare_init_x11_software(
	struct globuf_config_backend* config,
	struct globuf_error_info* error)
{
	struct globuf_calls_software* software =
		malloc(sizeof (struct globuf_calls_software));

	if (software == NULL)
	{
		error->code = GLOBUF_ERROR_ALLOC;
		error->file = __FILE__;
		error->line = __LINE__;
		return;
	}

	software->alloc = globuf_buffer_alloc_x11_software;
	software->free = globuf_buffer_free_x11_software;

	config->data = software;
	config->callback = globuf_x11_software_callback;
	config->init = globuf_x11_software_init;
	config->clean = globuf_x11_software_clean;
	config->window_create = globuf_x11_software_window_create;
	config->window_destroy = globuf_x11_software_window_destroy;
	config->window_confirm = globuf_x11_software_window_confirm;
	config->window_start = globuf_x11_software_window_start;
	config->window_block = globuf_x11_software_window_block;
	config->window_stop = globuf_x11_software_window_stop;
	config->init_render = globuf_x11_software_init_render;
	config->init_events = globuf_x11_software_init_events;
	config->handle_events = globuf_x11_software_handle_events;
	config->init_features = globuf_x11_software_init_features;
	config->feature_set_interaction = globuf_x11_software_feature_set_interaction;
	config->feature_set_state = globuf_x11_software_feature_set_state;
	config->feature_set_title = globuf_x11_software_feature_set_title;
	config->feature_set_icon = globuf_x11_software_feature_set_icon;
	config->get_width = globuf_x11_software_get_width;
	config->get_height = globuf_x11_software_get_height;
	config->get_expose = globuf_x11_software_get_expose;
	config->update_content = globuf_x11_software_update_content;

	globuf_error_ok(error);
}
