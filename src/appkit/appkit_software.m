#include "include/globox.h"
#include "include/globox_software.h"
#include "include/globox_appkit_software.h"

#include "common/globox_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"
#include "appkit/appkit_software.h"
#include "appkit/appkit_software_helpers.h"

#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

void globox_appkit_software_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct appkit_software_backend* backend = malloc(sizeof (struct appkit_software_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct appkit_software_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize the platform
	struct appkit_platform* platform = &(backend->platform);
	globox_appkit_common_init(context, platform, error);

	// error always set
}

void globox_appkit_software_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_clean(context, platform, error);

	// error always set
}

void globox_appkit_software_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
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

	// create a new layer
	platform->layer = [CALayer new];
	[platform->layer setDelegate: platform->layer_delegate];

	// run common AppKit helper
	globox_appkit_common_window_create(
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
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globox_appkit_software_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_window_destroy(context, platform, error);

	// error always set
}

void globox_appkit_software_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
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

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globox_appkit_software_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_window_block(context, platform, error);

	// error always set
}

void globox_appkit_software_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_window_stop(context, platform, error);

	// error always set
}


void globox_appkit_software_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_init_render(context, platform, config, error);

	// error always set
}

void globox_appkit_software_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_init_events(context, platform, config, error);

	// error always set
}

enum globox_event globox_appkit_software_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	return globox_appkit_common_handle_events(context, platform, event, error);
}


struct globox_config_features* globox_appkit_software_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_init_features(context, platform, error);
}

void globox_appkit_software_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_appkit_software_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_appkit_software_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_appkit_software_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_appkit_software_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_width(context, platform, error);
}

unsigned globox_appkit_software_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_height(context, platform, error);
}

struct globox_rect globox_appkit_software_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_expose(context, platform, error);
}


void globox_appkit_software_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	struct globox_update_software* update = data;

	// create colorspace to convert buffer automatically
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();

	if (colorspace == Nil)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_OBJ_NIL);
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
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	// create image from bitmap
	CGImageRef image = CGBitmapContextCreateImage(bitmap);

	if (image == Nil)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_MACOS_OBJ_NIL);
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

	globox_error_ok(error);
}


void globox_prepare_init_appkit_software(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	config->data = NULL;
	config->init = globox_appkit_software_init;
	config->clean = globox_appkit_software_clean;
	config->window_create = globox_appkit_software_window_create;
	config->window_destroy = globox_appkit_software_window_destroy;
	config->window_start = globox_appkit_software_window_start;
	config->window_block = globox_appkit_software_window_block;
	config->window_stop = globox_appkit_software_window_stop;
	config->init_render = globox_appkit_software_init_render;
	config->init_events = globox_appkit_software_init_events;
	config->handle_events = globox_appkit_software_handle_events;
	config->init_features = globox_appkit_software_init_features;
	config->feature_set_interaction = globox_appkit_software_feature_set_interaction;
	config->feature_set_state = globox_appkit_software_feature_set_state;
	config->feature_set_title = globox_appkit_software_feature_set_title;
	config->feature_set_icon = globox_appkit_software_feature_set_icon;
	config->get_width = globox_appkit_software_get_width;
	config->get_height = globox_appkit_software_get_height;
	config->get_expose = globox_appkit_software_get_expose;
	config->update_content = globox_appkit_software_update_content;

	globox_error_ok(error);
}

// simple allocator we provide so developers don't try to recycle buffers
// (it would not be thread-safe and break this multi-threaded version of globox)
uint32_t* globox_buffer_alloc_appkit_software(
	struct globox* context,
	unsigned width,
	unsigned height,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	uint32_t* argb = NULL;
	size_t len = 4 * width * height;

	argb = malloc(len);

	if (argb == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	globox_error_ok(error);
	return argb;
}

void globox_buffer_free_appkit_software(
	struct globox* context,
	uint32_t* buffer,
	struct globox_error_info* error)
{
	struct appkit_software_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	free(buffer);
	globox_error_ok(error);
}
