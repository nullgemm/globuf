#include "include/globuf.h"
#include "include/globuf_opengl.h"
#include "include/globuf_x11_egl.h"

#include "common/globuf_private.h"
#include "x11/x11_common.h"
#include "x11/x11_common_helpers.h"
#include "x11/x11_egl.h"
#include "x11/x11_egl_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

void globuf_x11_egl_init(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// allocate the backend
	struct x11_egl_backend* backend = malloc(sizeof (struct x11_egl_backend));

	if (backend == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_CONN);
		return;
	}

	// initialize the platform
	globuf_x11_common_init(context, platform, error);

	// error always set
}

void globuf_x11_egl_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// close the connection to the X server
	xcb_disconnect(platform->conn);

	// clean the platform
	globuf_x11_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globuf_x11_egl_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
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

	// get display
	backend->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (backend->display == EGL_NO_DISPLAY)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_DISPLAY_GET);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_INIT);
		return;
	}

	// use OpenGL
	error_egl = eglBindAPI(EGL_OPENGL_API);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_BIND_API);
		return;
	}

	// get egl config count
	EGLint attr_configs_alloc_size = 0;
	EGLint attr_configs_fill_size = 0;
	EGLConfig* attr_configs = NULL;

	error_egl =
		eglChooseConfig(
			backend->display,
			backend->config->attributes,
			NULL,
			0,
			&attr_configs_alloc_size);

	if ((error_egl == EGL_FALSE) || (attr_configs_alloc_size == 0))
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_CONFIG);
		return;
	}

	// allocate config array
	attr_configs = malloc((sizeof (EGLConfig)) * attr_configs_alloc_size);

	if (attr_configs == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	// get egl configs
	error_egl =
		eglChooseConfig(
			backend->display,
			backend->config->attributes,
			attr_configs,
			attr_configs_alloc_size,
			&attr_configs_fill_size);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_CONFIG);
		free(attr_configs);
		return;
	}

	// check if the egl config groups extension is available
	bool group_ext = false;

#ifdef EGL_EXT_config_select_group
	const char* list =
		eglQueryString(
			backend->display,
			EGL_EXTENSIONS);

	group_ext =
		x11_helpers_egl_ext_support(
			list,
			"EGL_EXT_config_select_group");
#endif

	if (group_ext == false)
	{
		context->feature_background->background = GLOBUF_BACKGROUND_OPAQUE;
	}

	// find compatible visual
	EGLint attr_context[] =
	{
		EGL_CONTEXT_MAJOR_VERSION, backend->config->major_version,
		EGL_CONTEXT_MINOR_VERSION, backend->config->minor_version,
		EGL_NONE,
	};

	EGLint i = 0;

	while (i < attr_configs_fill_size)
	{
		// get visual depth from EGL
		EGLint visual_depth;

		error_egl =
			eglGetConfigAttrib(
				backend->display,
				attr_configs[i],
				EGL_DEPTH_SIZE,
				&visual_depth);

		if (error_egl == EGL_FALSE)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_CONFIG_ATTR);
			free(attr_configs);
			return;
		}

		// break if creating an opaque context
		if (context->feature_background->background == GLOBUF_BACKGROUND_OPAQUE)
		{
			platform->visual_depth = visual_depth;
			break;
		}

		// get transparency info from EGL
		EGLint group;

		error_egl =
			eglGetConfigAttrib(
				backend->display,
				attr_configs[i],
				EGL_CONFIG_SELECT_GROUP_EXT,
				&group);

		if (error_egl == EGL_FALSE)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_CONFIG_ATTR);
			free(attr_configs);
			return;
		}

		// break if visual supports transparency
		if ((group == 1) && (visual_depth == 24))
		{
			platform->visual_depth = 32;
			break;
		}

		// continue searching for a compatible context
		++i;
	}

	// since X11 EGL transparency support was only added recently,
	// fall back to using opaque contexts if needed...
	if (i == attr_configs_fill_size)
	{
		context->feature_background->background = GLOBUF_BACKGROUND_OPAQUE;
		i = 0;
	}

	// save config and free array
	backend->attr_config = attr_configs[i];
	free(attr_configs);

	// create context
	backend->egl =
		eglCreateContext(
			backend->display,
			backend->attr_config,
			EGL_NO_CONTEXT,
			attr_context);

	if (backend->egl == EGL_NO_CONTEXT)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_CONTEXT_CREATE);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_CONFIG_ATTR);
		return;
	}

	platform->visual_id = visual_id;

	if (context->feature_background->background != GLOBUF_BACKGROUND_OPAQUE)
	{
		// generate a compatible colormap for the chosen visual id
		xcb_colormap_t colormap =
			xcb_generate_id(
				platform->conn);

		xcb_create_colormap(
			platform->conn,
			XCB_COLORMAP_ALLOC_NONE,
			colormap,
			platform->screen_obj->root,
			platform->visual_id);

		platform->attr_val[2] = colormap;
	}

	// run common X11 helper
	globuf_x11_common_window_create(context, platform, configs, count, callback, data, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_x11_egl_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy all structures and terminate EGL
	EGLBoolean error_egl;

	error_egl = eglDestroySurface(backend->display, backend->surface);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_DESTROY_SURFACE);
		return;
	}

	error_egl = eglDestroyContext(backend->display, backend->egl);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_DESTROY_CONTEXT);
		return;
	}

	error_egl = eglTerminate(backend->display);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_TERMINATE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// run common X11 helper
	globuf_x11_common_window_destroy(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

void globuf_x11_egl_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_window_confirm(context, platform, error);

	// error always set
}

void globuf_x11_egl_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
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
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_WINDOW_SURFACE);
		return;
	}

	// run common X11 helper
	globuf_x11_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_x11_egl_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper (mutex locked when unblocked)
	globuf_x11_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_x11_egl_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globuf_x11_egl_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_init_render(context, platform, config, error);

	platform->render_init_callback = x11_helpers_egl_bind;

	// error always set
}

void globuf_x11_egl_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globuf_event globuf_x11_egl_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
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


struct globuf_config_features* globuf_x11_egl_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	struct globuf_config_features* features =
		globuf_x11_common_init_features(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return features;
	}

	// VSync is always available (kind of) with EGL!
	features->list[features->count] = GLOBUF_FEATURE_VSYNC;
	context->feature_vsync =
		malloc(sizeof (struct globuf_feature_vsync));
	features->count += 1;

	if (context->feature_vsync == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// return the newly created features info structure
	// error always set
	return features;
}

void globuf_x11_egl_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globuf_x11_egl_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globuf_x11_egl_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globuf_x11_egl_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globuf_x11_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globuf_x11_egl_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globuf_x11_common_get_width(context, platform, error);
}

unsigned globuf_x11_egl_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globuf_x11_common_get_height(context, platform, error);
}

struct globuf_rect globuf_x11_egl_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globuf_x11_common_get_expose(context, platform, error);
}


void globuf_x11_egl_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	EGLBoolean error_egl = eglSwapBuffers(backend->display, backend->surface);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_X11_EGL_SWAP);
		return;
	}


	globuf_error_ok(error);
}

void* globuf_x11_egl_callback(
	struct globuf* context)
{
	struct x11_egl_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);
	return platform;
}


// OpenGL configuration setter
void globuf_init_x11_egl(
	struct globuf* context,
	struct globuf_config_opengl* config,
	struct globuf_error_info* error)
{
	struct x11_egl_backend* backend = context->backend_data;

	backend->config = config;

	globuf_error_ok(error);
}


void globuf_prepare_init_x11_egl(
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

	opengl->init = globuf_init_x11_egl;

	config->data = opengl;
	config->callback = globuf_x11_egl_callback;
	config->init = globuf_x11_egl_init;
	config->clean = globuf_x11_egl_clean;
	config->window_create = globuf_x11_egl_window_create;
	config->window_destroy = globuf_x11_egl_window_destroy;
	config->window_confirm = globuf_x11_egl_window_confirm;
	config->window_start = globuf_x11_egl_window_start;
	config->window_block = globuf_x11_egl_window_block;
	config->window_stop = globuf_x11_egl_window_stop;
	config->init_render = globuf_x11_egl_init_render;
	config->init_events = globuf_x11_egl_init_events;
	config->handle_events = globuf_x11_egl_handle_events;
	config->init_features = globuf_x11_egl_init_features;
	config->feature_set_interaction = globuf_x11_egl_feature_set_interaction;
	config->feature_set_state = globuf_x11_egl_feature_set_state;
	config->feature_set_title = globuf_x11_egl_feature_set_title;
	config->feature_set_icon = globuf_x11_egl_feature_set_icon;
	config->get_width = globuf_x11_egl_get_width;
	config->get_height = globuf_x11_egl_get_height;
	config->get_expose = globuf_x11_egl_get_expose;
	config->update_content = globuf_x11_egl_update_content;

	globuf_error_ok(error);
}
