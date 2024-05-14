#include "include/globuf.h"
#include "include/globuf_opengl.h"
#include "include/globuf_appkit_egl.h"

#include "common/globuf_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"
#include "appkit/appkit_egl.h"
#include "appkit/appkit_egl_helpers.h"

#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <EGL/egl.h>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

void globuf_appkit_egl_init(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// allocate the backend
	struct appkit_egl_backend* backend = malloc(sizeof (struct appkit_egl_backend));

	if (backend == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct appkit_egl_backend zero = {0};
	*backend = zero;

	// initialize what can be
	backend->egl = EGL_NO_CONTEXT;
	backend->display = EGL_NO_DISPLAY;
	backend->surface = EGL_NO_SURFACE;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize the platform
	struct appkit_platform* platform = &(backend->platform);
	globuf_appkit_common_init(context, platform, error);

	// error always set
}

void globuf_appkit_egl_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_clean(context, platform, error);

	// error always set
}

void globuf_appkit_egl_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	globuf_appkit_helpers_features_init(context, platform, configs, count, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// create a new layer
	platform->layer = [CALayer new];
	[platform->layer setDelegate: platform->layer_delegate];

	// run common AppKit helper
	globuf_appkit_common_window_create(
		context,
		platform,
		configs,
		count,
		callback,
		data,
		error);

	// get EGL display
	backend->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (backend->display == EGL_NO_DISPLAY)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_DISPLAY_GET);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_INIT);
		return;
	}

	// use OpenGL ES
	error_egl = eglBindAPI(EGL_OPENGL_ES_API);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_BIND_API);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_CONFIG);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_CONTEXT_CREATE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globuf_appkit_egl_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

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
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_DESTROY_SURFACE);
		return;
	}

	error_egl = eglDestroyContext(backend->display, backend->egl);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_DESTROY_CONTEXT);
		return;
	}

	error_egl = eglTerminate(backend->display);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_TERMINATE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// run common AppKit helper
	globuf_appkit_common_window_destroy(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

void globuf_appkit_egl_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_window_confirm(context, platform, error);

	// error always set
}

void globuf_appkit_egl_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common AppKit helper
	globuf_appkit_common_window_start(context, platform, error);

	// create EGL surface
	backend->surface =
		eglCreateWindowSurface(
			backend->display,
			backend->attr_config,
			(EGLNativeWindowType) platform->layer,
			NULL);

	if (backend->surface == EGL_NO_SURFACE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_WINDOW_SURFACE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globuf_appkit_egl_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_window_block(context, platform, error);

	// error always set
}

void globuf_appkit_egl_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_window_stop(context, platform, error);

	// error always set
}


void globuf_appkit_egl_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_init_render(context, platform, config, error);

	platform->render_init_callback = appkit_helpers_egl_bind;

	// error always set
}

void globuf_appkit_egl_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_init_events(context, platform, config, error);

	// error always set
}

enum globuf_event globuf_appkit_egl_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	return globuf_appkit_common_handle_events(context, platform, event, error);
}


struct globuf_config_features* globuf_appkit_egl_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globuf_appkit_common_init_features(context, platform, error);
}

void globuf_appkit_egl_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globuf_appkit_egl_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globuf_appkit_egl_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globuf_appkit_egl_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globuf_appkit_egl_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globuf_appkit_common_get_width(context, platform, error);
}

unsigned globuf_appkit_egl_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globuf_appkit_common_get_height(context, platform, error);
}

struct globuf_rect globuf_appkit_egl_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globuf_appkit_common_get_expose(context, platform, error);
}


void globuf_appkit_egl_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	struct globuf_update_egl* update = data;

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// swap buffers
	EGLBoolean error_egl = eglSwapBuffers(backend->display, backend->surface);

	if (error_egl == EGL_FALSE)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_EGL_SWAP);
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

void* globuf_appkit_egl_callback(
	struct globuf* context)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	return platform;
}


double globuf_appkit_egl_get_scale(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return 1.0;
	}

	double scale = [platform->win backingScaleFactor];

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return 1.0;
	}

	return scale;
}


// OpenGL configuration setter
void globuf_init_appkit_egl(
	struct globuf* context,
	struct globuf_config_opengl* config,
	struct globuf_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;

	backend->config = config;

	globuf_error_ok(error);
}


void globuf_prepare_init_appkit_egl(
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

	opengl->init = globuf_init_appkit_egl;

	config->data = opengl;
	config->callback = globuf_appkit_egl_callback;
	config->init = globuf_appkit_egl_init;
	config->clean = globuf_appkit_egl_clean;
	config->window_create = globuf_appkit_egl_window_create;
	config->window_destroy = globuf_appkit_egl_window_destroy;
	config->window_confirm = globuf_appkit_egl_window_confirm;
	config->window_start = globuf_appkit_egl_window_start;
	config->window_block = globuf_appkit_egl_window_block;
	config->window_stop = globuf_appkit_egl_window_stop;
	config->init_render = globuf_appkit_egl_init_render;
	config->init_events = globuf_appkit_egl_init_events;
	config->handle_events = globuf_appkit_egl_handle_events;
	config->init_features = globuf_appkit_egl_init_features;
	config->feature_set_interaction = globuf_appkit_egl_feature_set_interaction;
	config->feature_set_state = globuf_appkit_egl_feature_set_state;
	config->feature_set_title = globuf_appkit_egl_feature_set_title;
	config->feature_set_icon = globuf_appkit_egl_feature_set_icon;
	config->get_width = globuf_appkit_egl_get_width;
	config->get_height = globuf_appkit_egl_get_height;
	config->get_expose = globuf_appkit_egl_get_expose;
	config->update_content = globuf_appkit_egl_update_content;

	globuf_error_ok(error);
}
