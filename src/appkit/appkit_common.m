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

	// init pthread mutex (main)
	error_posix = pthread_mutex_init(&(platform->mutex_main), &mutex_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_INIT);
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

	// destroy pthread mutex (main)
	error_posix = pthread_mutex_destroy(&(platform->mutex_main));

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
	NSString* title =
		[[NSString alloc] initWithUTF8String:context->feature_title->title];

	NSWindowStyleMask mask =
		NSWindowStyleMaskResizable
		| NSWindowStyleMaskClosable
		| NSWindowStyleMaskMiniaturizable
		| NSWindowStyleMaskTitled;

	NSRect rect =
		NSMakeRect(
			context->feature_pos->x,
			context->feature_pos->y,
			context->feature_size->width,
			context->feature_size->height);

	struct appkit_thread_event_loop_data event_data =
	{
		.globox = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_event_loop_data = event_data;

	struct appkit_window_delegate_data delegate_data =
	{
		.globox = context,
		.platform = platform,
		.error = error,
	};

	platform->window_delegate_data = delegate_data;

	// start function in a new thread

	// create the window (must execute on the main thread)
	dispatch_sync(dispatch_get_main_queue(), ^{
		platform->win_delegate =
			[GloboxWindowDelegate new];

		id delegate = platform->win_delegate;

		[delegate
			setGloboxDelegateData:
				&(platform->window_delegate_data)];

		platform->win =
			[[[GloboxWindow alloc]
				initWithContentRect:rect
				styleMask:mask
				backing:NSBackingStoreBuffered
				defer:NO]
				autorelease];

		id window = platform->win;

		[window
			setGloboxEventData:
				&(platform->thread_event_loop_data)];

		[window
			cascadeTopLeftFromPoint:NSMakePoint(
				context->feature_pos->x,
				context->feature_pos->y)];

		[window setDelegate:delegate];

		[window setAcceptsMouseMovedEvents:YES];

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

		if (context->feature_frame->frame == false)
		{
			[window setTitlebarAppearsTransparent:YES];
		}
	});

	// configure features
	struct globox_config_reply* reply =
		malloc(count * (sizeof (struct globox_config_reply)));

	if (reply == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	for (size_t i = 0; i < count; ++i)
	{
		enum globox_feature feature = configs[i].feature;
		reply[i].feature = feature;

		switch (feature)
		{
			case GLOBOX_FEATURE_INTERACTION:
			case GLOBOX_FEATURE_ICON:
			case GLOBOX_FEATURE_VSYNC:
			{
				globox_error_throw(
					context,
					&reply[i].error,
					GLOBOX_ERROR_FEATURE_UNAVAILABLE);
				break;
			}
			default:
			{
				reply[i].error.code = GLOBOX_ERROR_OK;
				reply[i].error.file = NULL;
				reply[i].error.line = 0;
				break;
			}
		}
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

	// init thread attributes
	int error_posix;
	pthread_attr_t attr;

	error_posix = pthread_attr_init(&attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT);
		return;
	}

	error_posix = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_JOINABLE);
		return;
	}

	// start the render loop in a new thread
	// init thread function data
	struct appkit_thread_render_loop_data render_data =
	{
		.globox = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_render_loop_data = render_data;

	// start function in a new thread
	error_posix =
		pthread_create(
			&(platform->thread_render_loop),
			&attr,
			appkit_helpers_render_loop,
			&(platform->thread_render_loop_data));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_CREATE);
		return;
	}

	// destroy the attributes
	error_posix = pthread_attr_destroy(&attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY);
		return;
	}

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

	error_cond =
		pthread_cond_wait(&(platform->cond_main), &(platform->mutex_block));

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
	context->render_callback = *config;
	globox_error_ok(error);
}

void globox_appkit_common_init_events(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	// set the event callback
	context->event_callbacks = *config;
	globox_error_ok(error);
}

enum globox_event globox_appkit_common_handle_events(
	struct globox* context,
	struct appkit_platform* platform,
	void* event,
	struct globox_error_info* error)
{
	enum globox_event globox_event = GLOBOX_EVENT_UNKNOWN;
	NSEvent* nsevent = (NSEvent*) event;
	NSEventType type = [nsevent type];

	switch (type)
	{
		case NSEventTypeApplicationDefined:
		{
			if ([nsevent subtype] == 0)
			{
				enum globox_event data = [nsevent data1];

				switch (data)
				{
					case GLOBOX_EVENT_RESTORED:
					case GLOBOX_EVENT_MINIMIZED:
					case GLOBOX_EVENT_MAXIMIZED:
					case GLOBOX_EVENT_FULLSCREEN:
					case GLOBOX_EVENT_MOVED_RESIZED:
					case GLOBOX_EVENT_DAMAGED:
					{
						globox_event = data;
						break;
					}
					case GLOBOX_EVENT_CLOSED:
					{
						[platform->win close];
						platform->closed = true;
						globox_event = data;
						break;
					}
					default:
					{
						break;
					}
				}
			}

			break;
		}
		default:
		{
			break;
		}
	}

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
	globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_UNAVAILABLE);
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
	globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_UNAVAILABLE);
}

unsigned globox_appkit_common_get_width(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->width;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return 0;
	}

	// return value
	globox_error_ok(error);
	return value;
}

unsigned globox_appkit_common_get_height(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->height;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return 0;
	}

	// return value
	globox_error_ok(error);
	return value;
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
