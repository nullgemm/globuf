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
	NSString* title = [[NSString alloc] initWithUTF8String:context->feature_title->title];

	NSWindowStyleMask mask =
		NSWindowStyleMaskResizable
		| NSWindowStyleMaskClosable
		| NSWindowStyleMaskMiniaturizable;

	if (context->feature_frame->frame == true)
	{
		mask |= NSWindowStyleMaskTitled;
	}

	NSRect rect =
		NSMakeRect(
			context->feature_pos->x,
			context->feature_pos->y,
			context->feature_size->width,
			context->feature_size->height);

	// create the window (must execute on the main thread)
	dispatch_sync(dispatch_get_main_queue(), ^{
		platform->win = [[[NSWindow alloc] initWithContentRect:rect styleMask:mask backing:NSBackingStoreBuffered defer:NO] autorelease];

		id window = platform->win;
		[window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
		[window setTitle:title];

		switch (context->feature_state->state)
		{
			case GLOBOX_STATE_MINIMIZED:
			{
				[window miniaturize:nil];
				break;
			}
			case GLOBOX_STATE_MAXIMIZED:
			{
				[window zoom:nil];
				break;
			}
			case GLOBOX_STATE_FULLSCREEN:
			{
				[window toggleFullScreen:nil];
				break;
			}
			default:
			{
				break;
			}
		}
	});

	// configure features
	struct globox_config_reply* reply = malloc(count * (sizeof (struct globox_config_reply)));

	if (reply == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	for (size_t i = 0; i < count; ++i)
	{
		enum globox_feature feature = configs[i].feature;
		reply[i].feature = feature;
		reply[i].error.code = GLOBOX_ERROR_OK;
		reply[i].error.file = NULL;
		reply[i].error.line = 0;
	}

	callback(reply, count, data);
	free(reply);

	// error always set
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
	[platform->win makeKeyAndOrderFront:nil];

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
	struct globox_config_features* features =
		malloc(sizeof (struct globox_config_features));

	if (features == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	features->count = 0;
	features->list =
		malloc(GLOBOX_FEATURE_COUNT * (sizeof (enum globox_feature)));

	if (features->list == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_INTERACTION;
	context->feature_interaction =
		malloc(sizeof (struct globox_feature_interaction));
	features->count += 1;

	if (context->feature_interaction == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	context->feature_interaction->action = GLOBOX_INTERACTION_STOP;

	// always available
	features->list[features->count] = GLOBOX_FEATURE_STATE;
	context->feature_state =
		malloc(sizeof (struct globox_feature_state));
	features->count += 1;

	if (context->feature_state == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_TITLE;
	context->feature_title =
		malloc(sizeof (struct globox_feature_title));
	features->count += 1;

	if (context->feature_title == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_SIZE;
	context->feature_size =
		malloc(sizeof (struct globox_feature_size));
	features->count += 1;

	if (context->feature_size == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_POS;
	context->feature_pos =
		malloc(sizeof (struct globox_feature_pos));
	features->count += 1;

	if (context->feature_pos == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_FRAME;
	context->feature_frame =
		malloc(sizeof (struct globox_feature_frame));
	features->count += 1;

	if (context->feature_frame == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_BACKGROUND;
	context->feature_background =
		malloc(sizeof (struct globox_feature_background));
	features->count += 1;

	if (context->feature_background == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	globox_error_ok(error);
	return features;
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
