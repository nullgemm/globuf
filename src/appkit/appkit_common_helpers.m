#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"

#include <stdlib.h>
#include <string.h>

#import <AppKit/AppKit.h>

@implementation GloboxWindow

@synthesize globoxEventData;

- (void) sendEvent: (NSEvent*) event
{
	struct globox* context = globoxEventData->globox;
	struct appkit_platform* platform = globoxEventData->platform;
	struct globox_error_info* error = globoxEventData->error;

	// run developer callback
	context->event_callbacks.handler(context->event_callbacks.data, event);

	// run actual NSWindow sendEvent
	[super sendEvent: event];
}

- (void) zoom: (id) sender
{
	struct globox* context = globoxEventData->globox;
	struct appkit_platform* platform = globoxEventData->platform;
	BOOL maximized = [platform->win isZoomed];
	enum globox_event event;

	// inverted logic because this is the function doing the max/unmax
	if (maximized == YES)
	{
		event = GLOBOX_EVENT_RESTORED;
	}
	else
	{
		event = GLOBOX_EVENT_MAXIMIZED;
	}

	appkit_helpers_send_app_event(context, platform, event);

	[super zoom: sender];
}

@end

@implementation GloboxWindowDelegate

@synthesize globoxDelegateData;

- (void) windowDidResize: (NSNotification*) notification
{
	appkit_helpers_send_app_event(
		globoxDelegateData->globox,
		globoxDelegateData->platform,
		GLOBOX_EVENT_MOVED_RESIZED);
}

- (void) windowDidMiniaturize: (NSNotification*) notification
{
	appkit_helpers_send_app_event(
		globoxDelegateData->globox,
		globoxDelegateData->platform,
		GLOBOX_EVENT_MINIMIZED);
}

- (void) windowDidDeminiaturize: (NSNotification*) notification
{
	struct appkit_platform* platform = globoxDelegateData->platform;
	BOOL maximized = [platform->win isZoomed];
	enum globox_event event;

	if (maximized == YES)
	{
		event = GLOBOX_EVENT_MAXIMIZED;
	}
	else
	{
		event = GLOBOX_EVENT_RESTORED;
	}

	appkit_helpers_send_app_event(
		globoxDelegateData->globox,
		globoxDelegateData->platform,
		event);
}

- (void) windowDidEnterFullScreen: (NSNotification*) notification
{
	appkit_helpers_send_app_event(
		globoxDelegateData->globox,
		globoxDelegateData->platform,
		GLOBOX_EVENT_FULLSCREEN);
}

- (void) windowDidExitFullScreen: (NSNotification*) notification
{
	struct appkit_platform* platform = globoxDelegateData->platform;
	BOOL maximized = [platform->win isZoomed];
	enum globox_event event;

	if (maximized == YES)
	{
		event = GLOBOX_EVENT_MAXIMIZED;
	}
	else
	{
		event = GLOBOX_EVENT_RESTORED;
	}

	appkit_helpers_send_app_event(
		globoxDelegateData->globox,
		globoxDelegateData->platform,
		event);
}

- (void) windowDidMove: (NSNotification*) notification
{
	appkit_helpers_send_app_event(
		globoxDelegateData->globox,
		globoxDelegateData->platform,
		GLOBOX_EVENT_MOVED_RESIZED);
}

- (BOOL) windowShouldClose: (NSWindow*) sender
{
	appkit_helpers_send_app_event(
		globoxDelegateData->globox,
		globoxDelegateData->platform,
		GLOBOX_EVENT_CLOSED);

	return NO;
}

@end

@implementation GloboxLayerDelegate

@synthesize globoxLayerDelegateData;

- (void) displayLayer: (CALayer*) layer
{
	appkit_helpers_send_app_event(
		globoxLayerDelegateData->globox,
		globoxLayerDelegateData->platform,
		GLOBOX_EVENT_DAMAGED);
}

@end

void* appkit_helpers_render_loop(void* data)
{
	struct appkit_thread_render_loop_data* thread_render_loop_data = data;

	struct globox* context = thread_render_loop_data->globox;
	struct appkit_platform* platform = thread_render_loop_data->platform;
	struct globox_error_info* error = thread_render_loop_data->error;

#if 1
	// lock main mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return NULL;
	}
#endif

	bool closed = platform->closed;

#if 1
	// unlock main mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return NULL;
	}
#endif

	// thread init callback
	if (platform->render_init_callback != NULL)
	{
		platform->render_init_callback(context, error);

		if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			return NULL;
		}
	}

	while (closed == false)
	{
#if 1
		// lock main mutex
		error_posix = pthread_mutex_lock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			break;
		}
#endif

		// save accessible size values
		NSRect frame = [platform->view frame];
		context->feature_size->width = NSWidth(frame);
		context->feature_size->height = NSHeight(frame);

#if 1
		// unlock main mutex
		error_posix = pthread_mutex_unlock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			break;
		}
#endif

		// run developer callback
		context->render_callback.callback(context->render_callback.data);

#if 1
		// lock main mutex
		error_posix = pthread_mutex_lock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			break;
		}
#endif

		closed = platform->closed;

#if 1
		// unlock main mutex
		error_posix = pthread_mutex_unlock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			break;
		}
#endif
	}

	pthread_exit(NULL);
	return NULL;
}

void appkit_helpers_features_init(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error)
{
	for (size_t i = 0; i < count; ++i)
	{
		switch (configs[i].feature)
		{
			case GLOBOX_FEATURE_STATE:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_state) =
						*((struct globox_feature_state*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					struct globox_feature_title* tmp = configs[i].config;
					context->feature_title->title = strdup(tmp->title);
				}

				break;
			}
			case GLOBOX_FEATURE_SIZE:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_size) =
						*((struct globox_feature_size*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_POS:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_pos) =
						*((struct globox_feature_pos*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_frame) =
						*((struct globox_feature_frame*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_background) =
						*((struct globox_feature_background*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_VSYNC:
			{
				if (configs[i].config != NULL)
				{
					// VSync is always on and there's nothing we can do about it
					struct globox_feature_vsync* vsync = configs[i].config;
					vsync->vsync = true;
					*(context->feature_vsync) = *vsync;
				}

				break;
			}
			default:
			{
				globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_INVALID);
				return;
			}
		}
	}
}

void appkit_helpers_send_app_event(
	struct globox* context,
	struct appkit_platform* platform,
	enum globox_event event)
{
	NSEvent* nsevent =
		[NSEvent
			otherEventWithType:NSEventTypeApplicationDefined
			location:NSMakePoint(0.0f, 0.0f)
			modifierFlags:0
			timestamp:CACurrentMediaTime()
			windowNumber:[platform->win windowNumber]
			context:nil
			subtype:0
			data1:event
			data2:0];

	[NSApp
		postEvent:nsevent
		atStart:NO];
}

void appkit_helpers_set_state(
	struct globox* context,
	id window,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	switch (config->state)
	{
		case GLOBOX_STATE_REGULAR:
		{
			if ([window isMiniaturized] == YES)
			{
				[window deminiaturize:nil];
			}

			if (([window styleMask] & NSWindowStyleMaskFullScreen) != 0)
			{
				[window toggleFullScreen:nil];
			}

			if ([window isZoomed] == YES)
			{
				[window zoom:nil];
			}

			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			// On macOS minimizing a fullscreen window is impossible. We can't
			// work around this properly since it stems from an actual technical
			// limit and the only acceptable solution would be to wait for the
			// window to exit fullscreen mode. Instead, we just error out.
			if (([window styleMask] & NSWindowStyleMaskFullScreen) != 0)
			{
				globox_error_throw(
					context,
					error,
					GLOBOX_ERROR_FEATURE_STATE_INVALID);
			}
			else
			{
				[window miniaturize:nil];
			}

			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			if ([window isMiniaturized] == YES)
			{
				[window deminiaturize:nil];
			}
			else if (([window styleMask] & NSWindowStyleMaskFullScreen) != 0)
			{
				[window toggleFullScreen:nil];
			}

			if ([window isZoomed] == NO)
			{
				[window zoom:nil];
			}

			break;
		}
		case GLOBOX_STATE_FULLSCREEN:
		{
			if ([window isMiniaturized] == YES)
			{
				[window deminiaturize:nil];
			}

			[window toggleFullScreen:nil];
			break;
		}
		default:
		{
			globox_error_throw(
				context,
				error,
				GLOBOX_ERROR_FEATURE_STATE_INVALID);
		}
	}

	globox_error_ok(error);
}

void appkit_helpers_handle_interaction(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	NSRect frame = platform->saved_window_geometry;

	// compute window changes
	switch (context->feature_interaction->action)
	{
		case GLOBOX_INTERACTION_MOVE:
		{
			frame =
				NSOffsetRect(
					frame,
					platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					platform->old_mouse_pos_y - platform->saved_mouse_pos_y);
			break;
		}
		case GLOBOX_INTERACTION_N:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame),
					NSWidth(frame),
					NSHeight(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y);
			break;
		}
		case GLOBOX_INTERACTION_NW:
		{
			frame =
				NSMakeRect(
					NSMinX(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSMinY(frame),
					NSWidth(frame) + platform->old_mouse_pos_x - platform->saved_mouse_pos_x,
					NSHeight(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y);
			break;
		}
		case GLOBOX_INTERACTION_W:
		{
			frame =
				NSMakeRect(
					NSMinX(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSMinY(frame),
					NSWidth(frame) + platform->old_mouse_pos_x - platform->saved_mouse_pos_x,
					NSHeight(frame));
			break;
		}
		case GLOBOX_INTERACTION_SW:
		{
			frame =
				NSMakeRect(
					NSMinX(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSMinY(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y,
					NSWidth(frame) + platform->old_mouse_pos_x - platform->saved_mouse_pos_x,
					NSHeight(frame) + platform->saved_mouse_pos_y - platform->old_mouse_pos_y);
			break;
		}
		case GLOBOX_INTERACTION_S:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y,
					NSWidth(frame),
					NSHeight(frame) + platform->saved_mouse_pos_y - platform->old_mouse_pos_y);
			break;
		}
		case GLOBOX_INTERACTION_SE:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y,
					NSWidth(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSHeight(frame) + platform->saved_mouse_pos_y - platform->old_mouse_pos_y);
			break;
		}
		case GLOBOX_INTERACTION_E:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame),
					NSWidth(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSHeight(frame));
			break;
		}
		case GLOBOX_INTERACTION_NE:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame),
					NSWidth(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSHeight(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y);
			break;
		}
		default:
		{
			break;
		}
	}

	// set window position
	[platform->win setFrame: frame display: YES];
	platform->saved_window_geometry = frame;

	globox_error_ok(error);
}
