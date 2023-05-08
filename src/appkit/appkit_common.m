#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "include/globox_appkit.h"
#include "common/globox_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#import <AppKit/AppKit.h>

void globox_appkit_common_init(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	int error_posix;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;

	// init pthread mutex attributes
	error_posix = pthread_mutexattr_init(&mutex_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_INIT);
		return;
	}

	// set pthread mutex type (error checking for now)
	error_posix =
		pthread_mutexattr_settype(
			&mutex_attr,
			PTHREAD_MUTEX_ERRORCHECK);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_SETTYPE);
		return;
	}

	// init pthread mutex (block)
	error_posix = pthread_mutex_init(&(platform->mutex_block), &mutex_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// destroy pthread mutex attributes
	error_posix = pthread_mutexattr_destroy(&mutex_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_DESTROY);
		return;
	}

	// init pthread cond attributes
	error_posix = pthread_condattr_init(&cond_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_ATTR_INIT);
		return;
	}

	// init pthread cond
	error_posix = pthread_cond_init(&(platform->cond_main), &cond_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_INIT);
		return;
	}

	// destroy pthread cond attributes
	error_posix = pthread_condattr_destroy(&cond_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_ATTR_DESTROY);
		return;
	}

	// initialize the "closed" boolean
	platform->closed = false;

	globox_error_ok(error);
}

void globox_appkit_common_clean(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	int error_posix;
	int error_cond;

	// lock block mutex to be able to destroy the cond
	error_posix = pthread_mutex_lock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy pthread cond
	error_cond = pthread_cond_destroy(&(platform->cond_main));

	// unlock block mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (error_cond != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_DESTROY);
		return;
	}

	// destroy pthread mutex (block)
	error_posix = pthread_mutex_destroy(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	globox_error_ok(error);
}

void globox_appkit_common_window_create(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	NSString* appName = [[NSProcessInfo processInfo] processName];
	NSWindowStyleMask mask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;

	// create the window (must execute on the main thread)
	dispatch_sync(dispatch_get_main_queue(), ^{
		id window = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200) styleMask:mask backing:NSBackingStoreBuffered defer:NO] autorelease];
		[window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
		[window setTitle:appName];
		[window makeKeyAndOrderFront:nil];
	});
}

void globox_appkit_common_window_destroy(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_window_start(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_window_block(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	int error_posix;
	int error_cond;

	// lock block mutex
	error_posix = pthread_mutex_lock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	error_cond = pthread_cond_wait(&(platform->cond_main), &(platform->mutex_block));

	// unlock block mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (error_cond != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_WAIT);
		return;
	}

	globox_error_ok(error);
}

void globox_appkit_common_window_stop(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}


void globox_appkit_common_init_render(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	// set the event callback
#if 0
	context->render_callback = *config;
#endif
	globox_error_ok(error);
}

void globox_appkit_common_init_events(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	// set the event callback
#if 0
	context->event_callbacks = *config;
#endif
	globox_error_ok(error);
}

enum globox_event globox_appkit_common_handle_events(
	struct globox* context,
	struct appkit_platform* platform,
	void* event,
	struct globox_error_info* error)
{
	// process system events
	enum globox_event globox_event = GLOBOX_EVENT_UNKNOWN;
	globox_error_ok(error);
	return globox_event;
}

struct globox_config_features*
	globox_appkit_common_init_features(
		struct globox* context,
		struct appkit_platform* platform,
		struct globox_error_info* error)
{
	globox_error_ok(error);
	return NULL;
}

void globox_appkit_common_feature_set_interaction(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_feature_set_state(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_feature_set_title(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_feature_set_icon(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

unsigned globox_appkit_common_get_width(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
	return 0;
}

unsigned globox_appkit_common_get_height(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
	return 0;
}

struct globox_rect globox_appkit_common_get_expose(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	struct globox_rect dummy =
	{
		.x = 0,
		.y = 0,
		.width = 0,
		.height = 0,
	};

	globox_error_ok(error);
	return dummy;
}
