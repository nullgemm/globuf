#include "include/globox.h"
#include "include/globox_opengl.h"
#include "include/globox_x11_egl.h"

#include "common/globox_private.h"
#include "x11/x11_common.h"
#include "x11/x11_common_helpers.h"
#include "x11/x11_egl.h"
#include "x11/x11_egl_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <EGL/egl.h>

void globox_x11_egl_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct x11_egl_backend* backend = malloc(sizeof (struct x11_egl_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct x11_egl_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize values that can be initialized explicitly
	backend->config = NULL;

	// open a connection to the X server
	struct x11_platform* platform = &(backend->platform);
	platform->conn = xcb_connect(NULL, &(platform->screen_id));
	int error_posix = xcb_connection_has_error(platform->conn);

	if (error_posix > 0)
	{
		xcb_disconnect(platform->conn);
		globox_error_throw(context, error, GLOBOX_ERROR_X11_CONN);
		return;
	}

	// initialize the platform
	globox_x11_common_init(context, platform, error);

	// error always set
}

void globox_x11_egl_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// close the connection to the X server
	xcb_disconnect(platform->conn);

	// clean the platform
	globox_x11_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globox_x11_egl_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	x11_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// run common X11 helper
	globox_x11_common_window_create(context, platform, configs, count, callback, data, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// get display
	backend->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (backend->display == EGL_NO_DISPLAY)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_DISPLAY_GET);
		return;
	}

	// init
	EGLBoolean error_egl;
	EGLint display_version_major;
	EGLint display_version_minor;

	error_egl =
		eglInitialize(
			backend->display,
			&display_version_major,
			&display_version_minor);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_INIT);
		return;
	}

	// use OpenGL
	error_egl = eglBindAPI(EGL_OPENGL_API);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_BIND_API);
		return;
	}

	error_egl =
		eglChooseConfig(
			backend->display,
			backend->config->attributes,
			&(backend->attr_config),
			1,
			&(backend->attr_config_size));

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_CONFIG);
		return;
	}

	EGLint attr_context[] =
	{
		EGL_CONTEXT_MAJOR_VERSION, backend->config->major_version,
		EGL_CONTEXT_MINOR_VERSION, backend->config->minor_version,
		EGL_NONE,
	};

	backend->egl =
		eglCreateContext(
			backend->display,
			backend->attr_config,
			EGL_NO_CONTEXT,
			attr_context);

	if (backend->egl == EGL_NO_CONTEXT)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_CONTEXT_CREATE);
		return;
	}

	// get visual id from EGL
	EGLint visual_id;

	error_egl =
		eglGetConfigAttrib(
			backend->display,
			backend->attr_config,
			EGL_NATIVE_VISUAL_ID,
			&visual_id);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_CONFIG_ATTR);
		return;
	}

	platform->visual_id = visual_id;

	// get visual depth from EGL
	EGLint visual_depth;

	error_egl =
		eglGetConfigAttrib(
			backend->display,
			backend->attr_config,
			EGL_DEPTH_SIZE,
			&visual_depth);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_CONFIG_ATTR);
		return;
	}

	platform->visual_depth = visual_depth;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_x11_egl_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy all structures and terminate EGL
	EGLBoolean error_egl;

	error_egl = eglDestroySurface(backend->display, backend->surface);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_DESTROY_SURFACE);
		return;
	}

	error_egl = eglDestroyContext(backend->display, backend->egl);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_DESTROY_CONTEXT);
		return;
	}

	error_egl = eglTerminate(backend->display);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_TERMINATE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_destroy(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_x11_egl_window_confirm(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_window_confirm(context, platform, error);

	// error always set
}

void globox_x11_egl_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// create EGL surface
	backend->surface =
		eglCreateWindowSurface(
			backend->display,
			backend->attr_config,
			(EGLNativeWindowType) platform->win,
			NULL);

	if (backend->surface == EGL_NO_SURFACE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_WINDOW_SURFACE);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_egl_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper (mutex locked when unblocked)
	globox_x11_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_egl_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globox_x11_egl_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_init_render(context, platform, config, error);

	platform->render_init_callback = x11_helpers_egl_bind;

	// error always set
}

void globox_x11_egl_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globox_event globox_x11_egl_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	enum globox_event out =
		globox_x11_common_handle_events(
			context,
			platform,
			event,
			error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
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
				globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
				break;
			}

			// safe value updates
			if (platform->xsync_status == GLOBOX_XSYNC_CONFIGURED)
			{
				platform->xsync_status = GLOBOX_XSYNC_ACKNOWLEDGED;
			}

			// unlock xsync mutex
			error_posix = pthread_mutex_unlock(&(platform->mutex_xsync));

			if (error_posix != 0)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
				break;
			}

			out = GLOBOX_EVENT_MOVED_RESIZED;
			break;
		}
	}


	// error always set
	return out;
}


struct globox_config_features* globox_x11_egl_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	struct globox_config_features* features =
		globox_x11_common_init_features(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return features;
	}

	// VSync is always available (kind of) with EGL!
	features->list[features->count] = GLOBOX_FEATURE_VSYNC;
	context->feature_vsync =
		malloc(sizeof (struct globox_feature_vsync));
	features->count += 1;

	if (context->feature_vsync == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// return the newly created features info structure
	// error always set
	return features;
}

void globox_x11_egl_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_x11_egl_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_x11_egl_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_x11_egl_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_x11_egl_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_width(context, platform, error);
}

unsigned globox_x11_egl_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_height(context, platform, error);
}

struct globox_rect globox_x11_egl_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_expose(context, platform, error);
}


void globox_x11_egl_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	EGLBoolean error_egl = eglSwapBuffers(backend->display, backend->surface);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_EGL_SWAP);
		return;
	}


	globox_error_ok(error);
}

void* globox_x11_egl_callback(
	struct globox* context)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);
	return platform;
}


// OpenGL configuration setter
void globox_init_x11_egl(
	struct globox* context,
	struct globox_config_opengl* config,
	struct globox_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;

	backend->config = config;

	globox_error_ok(error);
}


void globox_prepare_init_x11_egl(
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

	opengl->init = globox_init_x11_egl;

	config->data = opengl;
	config->callback = globox_x11_egl_callback;
	config->init = globox_x11_egl_init;
	config->clean = globox_x11_egl_clean;
	config->window_create = globox_x11_egl_window_create;
	config->window_destroy = globox_x11_egl_window_destroy;
	config->window_confirm = globox_x11_egl_window_confirm;
	config->window_start = globox_x11_egl_window_start;
	config->window_block = globox_x11_egl_window_block;
	config->window_stop = globox_x11_egl_window_stop;
	config->init_render = globox_x11_egl_init_render;
	config->init_events = globox_x11_egl_init_events;
	config->handle_events = globox_x11_egl_handle_events;
	config->init_features = globox_x11_egl_init_features;
	config->feature_set_interaction = globox_x11_egl_feature_set_interaction;
	config->feature_set_state = globox_x11_egl_feature_set_state;
	config->feature_set_title = globox_x11_egl_feature_set_title;
	config->feature_set_icon = globox_x11_egl_feature_set_icon;
	config->get_width = globox_x11_egl_get_width;
	config->get_height = globox_x11_egl_get_height;
	config->get_expose = globox_x11_egl_get_expose;
	config->update_content = globox_x11_egl_update_content;

	globox_error_ok(error);
}
