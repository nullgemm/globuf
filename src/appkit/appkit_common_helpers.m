#define _XOPEN_SOURCE 700

#include "include/globuf.h"
#include "common/globuf_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"

#include <stdlib.h>
#include <string.h>

#import <AppKit/AppKit.h>

@implementation GlobufWindow

@synthesize globufEventData;

- (void) sendEvent: (NSEvent*) event
{
	struct globuf* context = globufEventData->globuf;
	struct appkit_platform* platform = globufEventData->platform;
	struct globuf_error_info* error = globufEventData->error;

	// run developer callback
	context->event_callbacks.handler(context->event_callbacks.data, event);

	// run actual NSWindow sendEvent
	[super sendEvent: event];
}

- (void) zoom: (id) sender
{
	struct globuf* context = globufEventData->globuf;
	struct appkit_platform* platform = globufEventData->platform;
	BOOL maximized = [platform->win isZoomed];
	enum globuf_event event;

	// inverted logic because this is the function doing the max/unmax
	if (maximized == YES)
	{
		event = GLOBUF_EVENT_RESTORED;
	}
	else
	{
		event = GLOBUF_EVENT_MAXIMIZED;
	}

	globuf_appkit_helpers_send_app_event(context, platform, event);

	[super zoom: sender];
}

@end

@implementation GlobufWindowDelegate

@synthesize globufDelegateData;

- (void) windowDidResize: (NSNotification*) notification
{
	globuf_appkit_helpers_send_app_event(
		globufDelegateData->globuf,
		globufDelegateData->platform,
		GLOBUF_EVENT_MOVED_RESIZED);
}

- (void) windowDidMiniaturize: (NSNotification*) notification
{
	globuf_appkit_helpers_send_app_event(
		globufDelegateData->globuf,
		globufDelegateData->platform,
		GLOBUF_EVENT_MINIMIZED);
}

- (void) windowDidDeminiaturize: (NSNotification*) notification
{
	struct appkit_platform* platform = globufDelegateData->platform;
	BOOL maximized = [platform->win isZoomed];
	enum globuf_event event;

	if (maximized == YES)
	{
		event = GLOBUF_EVENT_MAXIMIZED;
	}
	else
	{
		event = GLOBUF_EVENT_RESTORED;
	}

	globuf_appkit_helpers_send_app_event(
		globufDelegateData->globuf,
		globufDelegateData->platform,
		event);
}

- (void) windowDidEnterFullScreen: (NSNotification*) notification
{
	globuf_appkit_helpers_send_app_event(
		globufDelegateData->globuf,
		globufDelegateData->platform,
		GLOBUF_EVENT_FULLSCREEN);
}

- (void) windowDidExitFullScreen: (NSNotification*) notification
{
	struct appkit_platform* platform = globufDelegateData->platform;
	BOOL maximized = [platform->win isZoomed];
	enum globuf_event event;

	if (maximized == YES)
	{
		event = GLOBUF_EVENT_MAXIMIZED;
	}
	else
	{
		event = GLOBUF_EVENT_RESTORED;
	}

	globuf_appkit_helpers_send_app_event(
		globufDelegateData->globuf,
		globufDelegateData->platform,
		event);
}

- (void) windowDidMove: (NSNotification*) notification
{
	globuf_appkit_helpers_send_app_event(
		globufDelegateData->globuf,
		globufDelegateData->platform,
		GLOBUF_EVENT_MOVED_RESIZED);
}

- (BOOL) windowShouldClose: (NSWindow*) sender
{
	globuf_appkit_helpers_send_app_event(
		globufDelegateData->globuf,
		globufDelegateData->platform,
		GLOBUF_EVENT_CLOSED);

	return NO;
}

@end

@implementation GlobufLayerDelegate

@synthesize globufLayerDelegateData;

- (void) displayLayer: (CALayer*) layer
{
	globuf_appkit_helpers_send_app_event(
		globufLayerDelegateData->globuf,
		globufLayerDelegateData->platform,
		GLOBUF_EVENT_DAMAGED);
}

@end

void* globuf_appkit_helpers_render_loop(void* data)
{
	struct appkit_thread_render_loop_data* thread_render_loop_data = data;

	struct globuf* context = thread_render_loop_data->globuf;
	struct appkit_platform* platform = thread_render_loop_data->platform;
	struct globuf_error_info* error = thread_render_loop_data->error;

	// lock main mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return NULL;
	}

	bool closed = platform->closed;

	// unlock main mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return NULL;
	}

	// thread init callback
	if (platform->render_init_callback != NULL)
	{
		platform->render_init_callback(context, error);

		if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
		{
			return NULL;
		}
	}

	while (closed == false)
	{
		// lock main mutex
		error_posix = pthread_mutex_lock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
			break;
		}

		// save accessible size values
		NSRect frame = [platform->view frame];
		context->feature_size->width = NSWidth(frame);
		context->feature_size->height = NSHeight(frame);

		// unlock main mutex
		error_posix = pthread_mutex_unlock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
			break;
		}

		// run developer callback
		context->render_callback.callback(context->render_callback.data);

		// lock main mutex
		error_posix = pthread_mutex_lock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
			break;
		}

		closed = platform->closed;

		// unlock main mutex
		error_posix = pthread_mutex_unlock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
			break;
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void globuf_appkit_helpers_features_init(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	struct globuf_error_info* error)
{
	for (size_t i = 0; i < count; ++i)
	{
		switch (configs[i].feature)
		{
			case GLOBUF_FEATURE_STATE:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_state) =
						*((struct globuf_feature_state*)
							configs[i].config);
				}

				break;
			}
			case GLOBUF_FEATURE_TITLE:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					struct globuf_feature_title* tmp = configs[i].config;
					context->feature_title->title = strdup(tmp->title);
				}

				break;
			}
			case GLOBUF_FEATURE_SIZE:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_size) =
						*((struct globuf_feature_size*)
							configs[i].config);
				}

				break;
			}
			case GLOBUF_FEATURE_POS:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_pos) =
						*((struct globuf_feature_pos*)
							configs[i].config);
				}

				break;
			}
			case GLOBUF_FEATURE_FRAME:
			{
				// handled directly in AppKit's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_frame) =
						*((struct globuf_feature_frame*)
							configs[i].config);
				}

				break;
			}
			case GLOBUF_FEATURE_BACKGROUND:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_background) =
						*((struct globuf_feature_background*)
							configs[i].config);
				}

				break;
			}
			case GLOBUF_FEATURE_VSYNC:
			{
				if (configs[i].config != NULL)
				{
					// VSync is always on and there's nothing we can do about it
					struct globuf_feature_vsync* vsync = configs[i].config;
					vsync->vsync = true;
					*(context->feature_vsync) = *vsync;
				}

				break;
			}
			default:
			{
				globuf_error_throw(context, error, GLOBUF_ERROR_FEATURE_INVALID);
				return;
			}
		}
	}
}

void globuf_appkit_helpers_send_app_event(
	struct globuf* context,
	struct appkit_platform* platform,
	enum globuf_event event)
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

void globuf_appkit_helpers_set_state(
	struct globuf* context,
	id window,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	switch (config->state)
	{
		case GLOBUF_STATE_REGULAR:
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
		case GLOBUF_STATE_MINIMIZED:
		{
			// On macOS minimizing a fullscreen window is impossible. We can't
			// work around this properly since it stems from an actual technical
			// limit and the only acceptable solution would be to wait for the
			// window to exit fullscreen mode. Instead, we just error out.
			if (([window styleMask] & NSWindowStyleMaskFullScreen) != 0)
			{
				globuf_error_throw(
					context,
					error,
					GLOBUF_ERROR_FEATURE_STATE_INVALID);
			}
			else
			{
				[window miniaturize:nil];
			}

			break;
		}
		case GLOBUF_STATE_MAXIMIZED:
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
		case GLOBUF_STATE_FULLSCREEN:
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
			globuf_error_throw(
				context,
				error,
				GLOBUF_ERROR_FEATURE_STATE_INVALID);
		}
	}

	globuf_error_ok(error);
}

void globuf_appkit_helpers_handle_interaction(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error)
{
	NSRect frame = platform->saved_window_geometry;

	// compute window changes
	switch (context->feature_interaction->action)
	{
		case GLOBUF_INTERACTION_MOVE:
		{
			frame =
				NSOffsetRect(
					frame,
					platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					platform->old_mouse_pos_y - platform->saved_mouse_pos_y);
			break;
		}
		case GLOBUF_INTERACTION_N:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame),
					NSWidth(frame),
					NSHeight(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y);
			break;
		}
		case GLOBUF_INTERACTION_NW:
		{
			frame =
				NSMakeRect(
					NSMinX(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSMinY(frame),
					NSWidth(frame) + platform->old_mouse_pos_x - platform->saved_mouse_pos_x,
					NSHeight(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y);
			break;
		}
		case GLOBUF_INTERACTION_W:
		{
			frame =
				NSMakeRect(
					NSMinX(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSMinY(frame),
					NSWidth(frame) + platform->old_mouse_pos_x - platform->saved_mouse_pos_x,
					NSHeight(frame));
			break;
		}
		case GLOBUF_INTERACTION_SW:
		{
			frame =
				NSMakeRect(
					NSMinX(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSMinY(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y,
					NSWidth(frame) + platform->old_mouse_pos_x - platform->saved_mouse_pos_x,
					NSHeight(frame) + platform->saved_mouse_pos_y - platform->old_mouse_pos_y);
			break;
		}
		case GLOBUF_INTERACTION_S:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y,
					NSWidth(frame),
					NSHeight(frame) + platform->saved_mouse_pos_y - platform->old_mouse_pos_y);
			break;
		}
		case GLOBUF_INTERACTION_SE:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame) + platform->old_mouse_pos_y - platform->saved_mouse_pos_y,
					NSWidth(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSHeight(frame) + platform->saved_mouse_pos_y - platform->old_mouse_pos_y);
			break;
		}
		case GLOBUF_INTERACTION_E:
		{
			frame =
				NSMakeRect(
					NSMinX(frame),
					NSMinY(frame),
					NSWidth(frame) + platform->saved_mouse_pos_x - platform->old_mouse_pos_x,
					NSHeight(frame));
			break;
		}
		case GLOBUF_INTERACTION_NE:
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

	globuf_error_ok(error);
}
