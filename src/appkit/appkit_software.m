#include "include/globuf.h"
#include "include/globuf_software.h"
#include "include/globuf_appkit_software.h"

#include "common/globuf_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"
#include "appkit/appkit_software.h"
#include "appkit/appkit_software_helpers.h"

#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

void globuf_appkit_software_init(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// allocate the backend
	struct appkit_software_backend* backend = malloc(sizeof (struct appkit_software_backend));

	if (backend == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct appkit_software_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize the platform
	struct appkit_platform* platform = &(backend->platform);
	globuf_appkit_common_init(context, platform, error);

	// error always set
}

void globuf_appkit_software_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_clean(context, platform, error);

	// error always set
}

void globuf_appkit_software_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
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

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globuf_appkit_software_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_window_destroy(context, platform, error);

	// error always set
}

void globuf_appkit_software_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_window_confirm(context, platform, error);

	// error always set
}

void globuf_appkit_software_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
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

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globuf_appkit_software_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_window_block(context, platform, error);

	// error always set
}

void globuf_appkit_software_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_window_stop(context, platform, error);

	// error always set
}


void globuf_appkit_software_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_init_render(context, platform, config, error);

	// error always set
}

void globuf_appkit_software_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_init_events(context, platform, config, error);

	// error always set
}

enum globuf_event globuf_appkit_software_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	return globuf_appkit_common_handle_events(context, platform, event, error);
}


struct globuf_config_features* globuf_appkit_software_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globuf_appkit_common_init_features(context, platform, error);
}

void globuf_appkit_software_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globuf_appkit_software_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globuf_appkit_software_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globuf_appkit_software_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globuf_appkit_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globuf_appkit_software_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globuf_appkit_common_get_width(context, platform, error);
}

unsigned globuf_appkit_software_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globuf_appkit_common_get_height(context, platform, error);
}

struct globuf_rect globuf_appkit_software_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globuf_appkit_common_get_expose(context, platform, error);
}


void globuf_appkit_software_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	struct globuf_update_software* update = data;

	// create colorspace to convert buffer automatically
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();

	if (colorspace == Nil)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_OBJ_NIL);
		return;
	}

	// create bitmap from buffer
	CGContextRef bitmap =
		CGBitmapContextCreate(
			update->buf,
			update->width,
			update->height,
			8,
			update->width * 4,
			colorspace,
			kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little);

	if (bitmap == Nil)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_OBJ_NIL);
		return;
	}

	// create image from bitmap
	CGImageRef image = CGBitmapContextCreateImage(bitmap);

	if (image == Nil)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_MACOS_OBJ_NIL);
		return;
	}

	// copy image to layer
	dispatch_sync(dispatch_get_main_queue(), ^{
		[platform->layer setContents:(__bridge id)image];
	});

	// free memory
	CGColorSpaceRelease(colorspace);
	CGContextRelease(bitmap);
	CGImageRelease(image);

	globuf_error_ok(error);
}

void* globuf_appkit_software_callback(
	struct globuf* context)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	return platform;
}


// simple allocator we provide so developers don't try to recycle buffers
// (it would not be thread-safe and break this multi-threaded version of globuf)
uint32_t* globuf_buffer_alloc_appkit_software(
	struct globuf* context,
	unsigned width,
	unsigned height,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	uint32_t* argb = NULL;
	size_t len = 4 * width * height;

	argb = malloc(len);

	if (argb == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	globuf_error_ok(error);
	return argb;
}

void globuf_buffer_free_appkit_software(
	struct globuf* context,
	uint32_t* buffer,
	struct globuf_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	free(buffer);
	globuf_error_ok(error);
}


void globuf_prepare_init_appkit_software(
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

	software->alloc = globuf_buffer_alloc_appkit_software;
	software->free = globuf_buffer_free_appkit_software;

	config->data = software;
	config->callback = globuf_appkit_software_callback;
	config->init = globuf_appkit_software_init;
	config->clean = globuf_appkit_software_clean;
	config->window_create = globuf_appkit_software_window_create;
	config->window_destroy = globuf_appkit_software_window_destroy;
	config->window_confirm = globuf_appkit_software_window_confirm;
	config->window_start = globuf_appkit_software_window_start;
	config->window_block = globuf_appkit_software_window_block;
	config->window_stop = globuf_appkit_software_window_stop;
	config->init_render = globuf_appkit_software_init_render;
	config->init_events = globuf_appkit_software_init_events;
	config->handle_events = globuf_appkit_software_handle_events;
	config->init_features = globuf_appkit_software_init_features;
	config->feature_set_interaction = globuf_appkit_software_feature_set_interaction;
	config->feature_set_state = globuf_appkit_software_feature_set_state;
	config->feature_set_title = globuf_appkit_software_feature_set_title;
	config->feature_set_icon = globuf_appkit_software_feature_set_icon;
	config->get_width = globuf_appkit_software_get_width;
	config->get_height = globuf_appkit_software_get_height;
	config->get_expose = globuf_appkit_software_get_expose;
	config->update_content = globuf_appkit_software_update_content;

	globuf_error_ok(error);
}
