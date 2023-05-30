#include "include/globox.h"
#include "include/globox_egl.h"
#include "include/globox_appkit_egl.h"

#include "common/globox_private.h"
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

void globox_appkit_egl_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct appkit_egl_backend* backend = malloc(sizeof (struct appkit_egl_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct appkit_egl_backend zero = {0};
	*backend = zero;

	// initialize what can be
	backend->scale = 1.0;
	backend->egl = EGL_NO_CONTEXT;
	backend->display = EGL_NO_DISPLAY;
	backend->surface = EGL_NO_SURFACE;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize the platform
	struct appkit_platform* platform = &(backend->platform);
	globox_appkit_common_init(context, platform, error);

	// error always set
}

void globox_appkit_egl_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_clean(context, platform, error);

	// error always set
}

void globox_appkit_egl_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	appkit_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// run common AppKit helper
	globox_appkit_common_window_create(
		context,
		platform,
		configs,
		count,
		callback,
		data,
		error);

	// create a new layer
	platform->layer = [CALayer new];
	[platform->layer setDelegate: platform->layer_delegate];

	// make the view layer-hosting
	[platform->view setLayer: platform->layer];
	[platform->view setWantsLayer: YES];

	// create an effects view if we are using background blur
	if (context->feature_background->background == GLOBOX_BACKGROUND_BLURRED)
	{
		// create the blur view
		platform->view_blur = [NSVisualEffectView new];
		[platform->view_blur setBlendingMode: NSVisualEffectBlendingModeBehindWindow];

		// configure views to be automatically resized by the content view
		[platform->view setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
		[platform->view_blur setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];

		// build view hierarchy
		platform->view_master = [NSView new];
		[platform->view_master addSubview: platform->view];
		[platform->view_master addSubview: platform->view_blur positioned: NSWindowBelow relativeTo: platform->view];
	}
	else
	{
		platform->view_master = platform->view;
	}

	// get EGL display
	backend->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (backend->display == EGL_NO_DISPLAY)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_DISPLAY_GET);
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
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_INIT);
		return;
	}

	// use OpenGL ES
	error_egl = eglBindAPI(EGL_OPENGL_ES_API);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_BIND_API);
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
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_CONFIG);
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
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_CONTEXT_CREATE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globox_appkit_egl_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

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
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_DESTROY_SURFACE);
		return;
	}

	error_egl = eglDestroyContext(backend->display, backend->egl);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_DESTROY_CONTEXT);
		return;
	}

	error_egl = eglTerminate(backend->display);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_TERMINATE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// run common AppKit helper
	globox_appkit_common_window_destroy(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_appkit_egl_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common AppKit helper
	globox_appkit_common_window_start(context, platform, error);

	// set window content view
	dispatch_sync(dispatch_get_main_queue(), ^{
		[platform->win setContentView: platform->view_master];
	});

	// create EGL surface
	backend->surface =
		eglCreateWindowSurface(
			backend->display,
			backend->attr_config,
			(EGLNativeWindowType) platform->layer,
			NULL);

	if (backend->surface == EGL_NO_SURFACE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_WINDOW_SURFACE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globox_appkit_egl_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_window_block(context, platform, error);

	// error always set
}

void globox_appkit_egl_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_window_stop(context, platform, error);

	// error always set
}


void globox_appkit_egl_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_init_render(context, platform, config, error);

	platform->render_init_callback = appkit_helpers_egl_bind;

	// error always set
}

void globox_appkit_egl_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_init_events(context, platform, config, error);

	// error always set
}

enum globox_event globox_appkit_egl_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	return globox_appkit_common_handle_events(context, platform, event, error);
}


struct globox_config_features* globox_appkit_egl_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_init_features(context, platform, error);
}

void globox_appkit_egl_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_appkit_egl_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_appkit_egl_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_appkit_egl_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_appkit_egl_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_width(context, platform, error);
}

unsigned globox_appkit_egl_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_height(context, platform, error);
}

struct globox_rect globox_appkit_egl_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_expose(context, platform, error);
}


void globox_appkit_egl_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	struct globox_update_egl* update = data;

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// swap buffers
	EGLBoolean error_egl = eglSwapBuffers(backend->display, backend->surface);

	if (error_egl == EGL_FALSE)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_EGL_SWAP);
		return;
	}

	backend->scale = [platform->win backingScaleFactor];

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

double globox_appkit_egl_get_scale(
	struct globox* context)
{
	struct appkit_egl_backend* backend = context->backend_data;

	return backend->scale;
}


void globox_prepare_init_appkit_egl(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	config->data = NULL;
	config->init = globox_appkit_egl_init;
	config->clean = globox_appkit_egl_clean;
	config->window_create = globox_appkit_egl_window_create;
	config->window_destroy = globox_appkit_egl_window_destroy;
	config->window_start = globox_appkit_egl_window_start;
	config->window_block = globox_appkit_egl_window_block;
	config->window_stop = globox_appkit_egl_window_stop;
	config->init_render = globox_appkit_egl_init_render;
	config->init_events = globox_appkit_egl_init_events;
	config->handle_events = globox_appkit_egl_handle_events;
	config->init_features = globox_appkit_egl_init_features;
	config->feature_set_interaction = globox_appkit_egl_feature_set_interaction;
	config->feature_set_state = globox_appkit_egl_feature_set_state;
	config->feature_set_title = globox_appkit_egl_feature_set_title;
	config->feature_set_icon = globox_appkit_egl_feature_set_icon;
	config->get_width = globox_appkit_egl_get_width;
	config->get_height = globox_appkit_egl_get_height;
	config->get_expose = globox_appkit_egl_get_expose;
	config->update_content = globox_appkit_egl_update_content;

	globox_error_ok(error);
}


// OpenGL configuration setter
void globox_init_appkit_egl(
	struct globox* context,
	struct globox_config_egl* config,
	struct globox_error_info* error)
{
	struct appkit_egl_backend* backend = context->backend_data;

	backend->config = config;

	globox_error_ok(error);
}
