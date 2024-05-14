#define _XOPEN_SOURCE 700

#include "include/globuf.h"
#include "include/globuf_appkit.h"
#include "common/globuf_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#import <AppKit/AppKit.h>

static inline void free_check(const void* ptr)
{
	if (ptr != NULL)
	{
		free((void*) ptr);
	}
}

void globuf_appkit_common_init(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	int error_posix;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;

	// init pthread mutex attributes
	error_posix = pthread_mutexattr_init(&mutex_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_ATTR_INIT);
		return;
	}

	// set pthread mutex type (error checking for now)
	error_posix =
		pthread_mutexattr_settype(
			&mutex_attr,
			PTHREAD_MUTEX_ERRORCHECK);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_ATTR_SETTYPE);
		return;
	}

	// init pthread mutex (main)
	error_posix = pthread_mutex_init(&(platform->mutex_main), &mutex_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// init pthread mutex (block)
	error_posix = pthread_mutex_init(&(platform->mutex_block), &mutex_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// destroy pthread mutex attributes
	error_posix = pthread_mutexattr_destroy(&mutex_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_ATTR_DESTROY);
		return;
	}

	// init pthread cond attributes
	error_posix = pthread_condattr_init(&cond_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_ATTR_INIT);
		return;
	}

	// init pthread cond
	error_posix = pthread_cond_init(&(platform->cond_main), &cond_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_INIT);
		return;
	}

	// destroy pthread cond attributes
	error_posix = pthread_condattr_destroy(&cond_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_ATTR_DESTROY);
		return;
	}

	// initialize saved action
	platform->old_mouse_pos_x = 0;
	platform->old_mouse_pos_y = 0;
	platform->saved_mouse_pos_x = 0;
	platform->saved_mouse_pos_y = 0;
	platform->saved_window = true;
	platform->saved_window_geometry = NSZeroRect;

	// initialize the "closed" boolean
	platform->closed = false;

	globuf_error_ok(error);
}

void globuf_appkit_common_clean(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	int error_posix;
	int error_cond;

	// lock block mutex to be able to destroy the cond
	error_posix = pthread_mutex_lock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy pthread cond
	error_cond = pthread_cond_destroy(&(platform->cond_main));

	// unlock block mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (error_cond != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_DESTROY);
		return;
	}

	// destroy pthread mutex (block)
	error_posix = pthread_mutex_destroy(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	// destroy pthread mutex (main)
	error_posix = pthread_mutex_destroy(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	globuf_error_ok(error);
}

void globuf_appkit_common_window_create(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	NSString* title =
		[[NSString alloc] initWithUTF8String:context->feature_title->title];

	NSWindowStyleMask mask =
		NSWindowStyleMaskResizable
		| NSWindowStyleMaskClosable
		| NSWindowStyleMaskMiniaturizable
		| NSWindowStyleMaskTitled;

	if (context->feature_frame->frame == false)
	{
		mask |= NSWindowStyleMaskFullSizeContentView;
	}

	NSRect rect =
		NSMakeRect(
			context->feature_pos->x,
			context->feature_pos->y,
			context->feature_size->width,
			context->feature_size->height);

	struct appkit_thread_event_loop_data event_data =
	{
		.globuf = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_event_loop_data = event_data;

	struct appkit_window_delegate_data delegate_data =
	{
		.globuf = context,
		.platform = platform,
		.error = error,
	};

	platform->window_delegate_data = delegate_data;

	// create the window (must execute on the main thread)
	platform->win_delegate =
		[GlobufWindowDelegate new];

	id delegate = platform->win_delegate;

	[delegate
		setGlobufDelegateData:
			&(platform->window_delegate_data)];

	// TODO keep only the minimal (ifdef'd) dispatch_sync sections
	__block id window;
	dispatch_sync(dispatch_get_main_queue(), ^{

#if 0
	dispatch_sync(dispatch_get_main_queue(), ^{
#endif
		platform->win =
			[[GlobufWindow alloc]
				initWithContentRect:rect
				styleMask:mask
				backing:NSBackingStoreBuffered
				defer:NO];
#if 0
	});
#endif

	window = platform->win;

	[window
		setGlobufEventData:
			&(platform->thread_event_loop_data)];

	[window
		cascadeTopLeftFromPoint:NSMakePoint(
			context->feature_pos->x,
			context->feature_pos->y)];

	[window setDelegate:delegate];

	// set background mode
	switch (context->feature_background->background)
	{
		case GLOBUF_BACKGROUND_BLURRED:
		case GLOBUF_BACKGROUND_TRANSPARENT:
		{
			[window setOpaque: NO];
			[window setBackgroundColor: [NSColor clearColor]];
			break;
		}
		case GLOBUF_BACKGROUND_OPAQUE:
		{
			break;
		}
	}

	// accept mouse move events
	[window setAcceptsMouseMovedEvents: YES];

	// set title
#if 0
	dispatch_sync(dispatch_get_main_queue(), ^{
#endif
		[window setTitle:title];
#if 0
	});
#endif

	// TODO keep only the minimal (ifdef'd) dispatch_sync sections
	});

	switch (context->feature_state->state)
	{
		case GLOBUF_STATE_MINIMIZED:
		{
			[window miniaturize:nil];
			break;
		}
		case GLOBUF_STATE_MAXIMIZED:
		{
			[window zoom:nil];
			break;
		}
		case GLOBUF_STATE_FULLSCREEN:
		{
			[window toggleFullScreen:nil];
			break;
		}
		default:
		{
			break;
		}
	}

	// set frame type
	if (context->feature_frame->frame == false)
	{
		[window setTitlebarAppearsTransparent: YES];
		[window setTitleVisibility: NSWindowTitleHidden];
		[[window standardWindowButton: NSWindowCloseButton] setHidden: YES];
		[[window standardWindowButton: NSWindowMiniaturizeButton] setHidden: YES];
		[[window standardWindowButton: NSWindowZoomButton] setHidden: YES];
	}

	// create a layer-hosting view
	platform->view = [NSView new];

	// create the custom layer delegate data
	struct appkit_layer_delegate_data layer_delegate_data =
	{
		.globuf = context,
		.platform = platform,
		.error = error,
	};

	platform->layer_delegate_data = layer_delegate_data;

	// create a custom layer delegate
	platform->layer_delegate = [GlobufLayerDelegate new];
	[platform->layer_delegate setGlobufLayerDelegateData: &(platform->layer_delegate_data)];

	// make the view layer-hosting
	[platform->view setLayer: platform->layer];
	[platform->view setWantsLayer: YES];

	// create an effects view if we are using background blur
	if (context->feature_background->background == GLOBUF_BACKGROUND_BLURRED)
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

	// configure features
	struct globuf_config_reply* reply =
		malloc(count * (sizeof (struct globuf_config_reply)));

	if (reply == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	for (size_t i = 0; i < count; ++i)
	{
		enum globuf_feature feature = configs[i].feature;
		reply[i].feature = feature;

		switch (feature)
		{
			case GLOBUF_FEATURE_ICON:
			{
				globuf_error_throw(
					context,
					&reply[i].error,
					GLOBUF_ERROR_FEATURE_UNAVAILABLE);
				break;
			}
			default:
			{
				reply[i].error.code = GLOBUF_ERROR_OK;
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

void globuf_appkit_common_window_destroy(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	// TODO
	globuf_error_ok(error);
}

void globuf_appkit_common_window_confirm(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	globuf_error_ok(error);
}

void globuf_appkit_common_window_start(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	GlobufWindow* win = platform->win;

	[win setAppearance:
		[NSAppearance appearanceNamed:
			NSAppearanceNameVibrantDark]];

	[win makeKeyAndOrderFront: nil];

	// set window content view
	dispatch_sync(dispatch_get_main_queue(), ^{
		[platform->win setContentView: platform->view_master];
	});

	// init thread attributes
	int error_posix;
	pthread_attr_t attr;

	error_posix = pthread_attr_init(&attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_ATTR_INIT);
		return;
	}

	error_posix = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_ATTR_JOINABLE);
		return;
	}

	// start the render loop in a new thread
	// init thread function data
	struct appkit_thread_render_loop_data render_data =
	{
		.globuf = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_render_loop_data = render_data;

	// start function in a new thread
	error_posix =
		pthread_create(
			&(platform->thread_render_loop),
			&attr,
			globuf_appkit_helpers_render_loop,
			&(platform->thread_render_loop_data));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_CREATE);
		return;
	}

	// destroy the attributes
	error_posix = pthread_attr_destroy(&attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_ATTR_DESTROY);
		return;
	}

	globuf_error_ok(error);
}

void globuf_appkit_common_window_block(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	int error_posix;
	int error_cond;

	// lock block mutex
	error_posix = pthread_mutex_lock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	error_cond =
		pthread_cond_wait(&(platform->cond_main), &(platform->mutex_block));

	// unlock block mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (error_cond != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_WAIT);
		return;
	}

	globuf_error_ok(error);
}

void globuf_appkit_common_window_stop(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// close window
	dispatch_sync(dispatch_get_main_queue(), ^{
		[platform->win close];
	});

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}


void globuf_appkit_common_init_render(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	// set the event callback
	context->render_callback = *config;
	globuf_error_ok(error);
}

void globuf_appkit_common_init_events(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	// set the event callback
	context->event_callbacks = *config;
	globuf_error_ok(error);
}

enum globuf_event globuf_appkit_common_handle_events(
	struct globuf* context,
	struct appkit_platform* platform,
	void* event,
	struct globuf_error_info* error)
{
	enum globuf_event globuf_event = GLOBUF_EVENT_UNKNOWN;
	NSEvent* nsevent = (NSEvent*) event;
	NSEventType type = [nsevent type];

	switch (type)
	{
		case NSEventTypeApplicationDefined:
		{
			if ([nsevent subtype] == 0)
			{
				enum globuf_event data = [nsevent data1];

				switch (data)
				{
					case GLOBUF_EVENT_RESTORED:
					{
						context->feature_state->state = GLOBUF_STATE_REGULAR;
						globuf_event = data;
						break;
					}
					case GLOBUF_EVENT_MINIMIZED:
					{
						context->feature_state->state = GLOBUF_STATE_MINIMIZED;
						globuf_event = data;
						break;
					}
					case GLOBUF_EVENT_MAXIMIZED:
					{
						context->feature_state->state = GLOBUF_STATE_MAXIMIZED;
						globuf_event = data;
						break;
					}
					case GLOBUF_EVENT_FULLSCREEN:
					{
						context->feature_state->state = GLOBUF_STATE_FULLSCREEN;
						globuf_event = data;
						break;
					}
					case GLOBUF_EVENT_MOVED_RESIZED:
					{
						globuf_event = data;
						break;
					}
					case GLOBUF_EVENT_DAMAGED:
					{
						NSRect frame = [platform->view frame];
						context->expose.x = NSMinX(frame);
						context->expose.y = NSMinY(frame);
						context->expose.width = NSWidth(frame);
						context->expose.height = NSHeight(frame);
						globuf_event = data;
						break;
					}
					case GLOBUF_EVENT_CLOSED:
					{
						[platform->win close];
						platform->closed = true;
						globuf_event = data;
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
		case NSEventTypeLeftMouseDragged:
		case NSEventTypeRightMouseDragged:
		case NSEventTypeOtherMouseDragged:
		{
			// stop interactive move & resize when in fullscreen
			if (([platform->win styleMask] & NSWindowStyleMaskFullScreen) != 0)
			{
				context->feature_interaction->action = GLOBUF_INTERACTION_STOP;
				break;
			}

			// handle interactive move & resize
			if (context->feature_interaction->action != GLOBUF_INTERACTION_STOP)
			{
				if (platform->saved_window == false)
				{
					platform->saved_window_geometry = [platform->win frame];
					platform->saved_window = true;
				}

				NSScreen* screen = [[NSScreen screens] firstObject];
				NSPoint point = [NSEvent mouseLocation];
				point.y = NSMaxY([screen frame]) - point.y;

				platform->old_mouse_pos_x = platform->saved_mouse_pos_x;
				platform->old_mouse_pos_y = platform->saved_mouse_pos_y;
				platform->saved_mouse_pos_x = point.x;
				platform->saved_mouse_pos_y = point.y;

				globuf_appkit_helpers_handle_interaction(context, platform, error);
			}

			break;
		}
		case NSEventTypeLeftMouseDown:
		case NSEventTypeRightMouseDown:
		case NSEventTypeOtherMouseDown:
		{
			NSScreen* screen = [[NSScreen screens] firstObject];
			NSPoint point = [NSEvent mouseLocation];
			point.y = NSMaxY([screen frame]) - point.y;
			platform->saved_mouse_pos_x = point.x;
			platform->saved_mouse_pos_y = point.y;
			platform->saved_window = false;
			break;
		}
		case NSEventTypeLeftMouseUp:
		case NSEventTypeRightMouseUp:
		case NSEventTypeOtherMouseUp:
		{
			platform->old_mouse_pos_x = 0;
			platform->old_mouse_pos_y = 0;
			platform->saved_mouse_pos_x = 0;
			platform->saved_mouse_pos_y = 0;

			// get current interaction type
			enum globuf_interaction action = context->feature_interaction->action;

			// reset interaction type
			if (action != GLOBUF_INTERACTION_STOP)
			{
				struct globuf_feature_interaction action =
				{
					.action = GLOBUF_INTERACTION_STOP,
				};

				*(context->feature_interaction) = action;
			}

			break;
		}
		default:
		{
			break;
		}
	}

	globuf_error_ok(error);
	return globuf_event;
}

struct globuf_config_features*
	globuf_appkit_common_init_features(
		struct globuf* context,
		struct appkit_platform* platform,
		struct globuf_error_info* error)
{
	struct globuf_config_features* features =
		malloc(sizeof (struct globuf_config_features));

	if (features == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	features->count = 0;
	features->list =
		malloc(GLOBUF_FEATURE_COUNT * (sizeof (enum globuf_feature)));

	if (features->list == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_INTERACTION;
	context->feature_interaction =
		malloc(sizeof (struct globuf_feature_interaction));
	features->count += 1;

	if (context->feature_interaction == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_STATE;
	context->feature_state =
		malloc(sizeof (struct globuf_feature_state));
	features->count += 1;

	if (context->feature_state == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_TITLE;
	context->feature_title =
		malloc(sizeof (struct globuf_feature_title));
	features->count += 1;

	if (context->feature_title == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_SIZE;
	context->feature_size =
		malloc(sizeof (struct globuf_feature_size));
	features->count += 1;

	if (context->feature_size == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_POS;
	context->feature_pos =
		malloc(sizeof (struct globuf_feature_pos));
	features->count += 1;

	if (context->feature_pos == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_FRAME;
	context->feature_frame =
		malloc(sizeof (struct globuf_feature_frame));
	features->count += 1;

	if (context->feature_frame == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_BACKGROUND;
	context->feature_background =
		malloc(sizeof (struct globuf_feature_background));
	features->count += 1;

	if (context->feature_background == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_VSYNC;
	context->feature_vsync =
		malloc(sizeof (struct globuf_feature_vsync));
	features->count += 1;

	if (context->feature_vsync == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	globuf_error_ok(error);
	return features;
}

void globuf_appkit_common_feature_set_interaction(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure if not in fullscreen mode
	if (([platform->win styleMask] & NSWindowStyleMaskFullScreen) == 0)
	{
		*(context->feature_interaction) = *config;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

void globuf_appkit_common_feature_set_state(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	if (config->state != context->feature_state->state)
	{
		globuf_appkit_helpers_set_state(
			context,
			platform->win,
			config,
			error);
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

void globuf_appkit_common_feature_set_title(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	free_check(context->feature_title->title);

	context->feature_title->title = strdup(config->title);

	NSString* title =
		[[NSString alloc] initWithUTF8String:context->feature_title->title];

	dispatch_sync(dispatch_get_main_queue(), ^{
		[platform->win setTitle: title];
	});

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_appkit_common_feature_set_icon(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	globuf_error_throw(context, error, GLOBUF_ERROR_FEATURE_UNAVAILABLE);
}

unsigned globuf_appkit_common_get_width(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->width;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return 0;
	}

	// return value
	globuf_error_ok(error);
	return value;
}

unsigned globuf_appkit_common_get_height(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->height;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return 0;
	}

	// return value
	globuf_error_ok(error);
	return value;
}

struct globuf_rect globuf_appkit_common_get_expose(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	struct globuf_rect dummy =
	{
		.x = 0,
		.y = 0,
		.width = 0,
		.height = 0,
	};

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return dummy;
	}

	// save value
	struct globuf_rect value = context->expose;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return dummy;
	}

	// return value
	globuf_error_ok(error);
	return value;
}
