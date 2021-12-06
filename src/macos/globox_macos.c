/// this file implements the main globox functions for the macos platform
/// it is completely isolated from the graphic context functions

// include globox structures and functions
#include "globox.h"
#include "globox_error.h"
// system includes
#include <stdlib.h>
#include <string.h>
// macos includes
#include "macos/globox_macos_types.h"
#include "macos/globox_macos_symbols.h"
#include "macos/globox_macos_callbacks.h"

// include platform structures
#include "macos/globox_macos.h"

// initalize the display system
void globox_platform_init(
	struct globox* globox,
	bool transparent,
	bool blurred)
{
	struct globox_platform* platform = malloc(sizeof (struct globox_platform));

	globox->globox_platform = platform;
	globox->globox_redraw = true;
	globox->globox_transparent = transparent;
	globox->globox_blurred = blurred;

	platform->globox_macos_interactive_x = 0;
	platform->globox_macos_interactive_y = 0;
	platform->globox_macos_fullscreen = false;
	platform->globox_macos_inhibit_resize = false;
	platform->globox_macos_state_old = GLOBOX_STATE_REGULAR;

	// re-used variables
	Class class;
	BOOL ok;

	// create the View class
	class = objc_getClass("NSView");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);
		return;
	}

	platform->globox_macos_class_view =
		objc_allocateClassPair(
			class,
			"View",
			0);

	if (platform->globox_macos_class_view == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_ALLOC);
		return;
	}

	// inject the globox pointer
	ok =
		class_addIvar(
			platform->globox_macos_class_view,
			"globox",
			sizeof (void*),
			sizeof (void*),
			"^v");

	if (ok == NO)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_ADDVAR);

		objc_disposeClassPair(platform->globox_macos_class_view);

		return;
	}

	// create the AppDelegate class
	class = objc_getClass("NSObject");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);

		objc_disposeClassPair(platform->globox_macos_class_view);

		return;
	}

	platform->globox_macos_class_appdelegate =
		objc_allocateClassPair(
			class,
			"AppDelegate",
			0);

	if (platform->globox_macos_class_appdelegate == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_ALLOC);

		objc_disposeClassPair(platform->globox_macos_class_view);

		return;
	}

	// inject the globox pointer
	ok =
		class_addIvar(
			platform->globox_macos_class_appdelegate,
			"globox",
			sizeof (void*),
			sizeof (void*),
			"^v");

	if (ok == NO)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_ADDVAR);

		objc_disposeClassPair(platform->globox_macos_class_view);
		objc_disposeClassPair(platform->globox_macos_class_appdelegate);

		return;
	}

	// inject the initialization method
	ok =
		class_addMethod(
			platform->globox_macos_class_appdelegate,
			sel_getUid("applicationWillFinishLaunching:"),
			(IMP) callback_application_will_finish_launching,
			"i@:@");

	if (ok == NO)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_ADDMETHOD);

		objc_disposeClassPair(platform->globox_macos_class_view);
		objc_disposeClassPair(platform->globox_macos_class_appdelegate);

		return;
	}

	// add post-launch event loop killing method
	ok =
		class_addMethod(
			platform->globox_macos_class_appdelegate,
			sel_getUid("applicationWillBecomeActive:"),
			(IMP) callback_application_will_become_active,
			"v@:@");

	if (ok == NO)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_ADDMETHOD);

		objc_disposeClassPair(platform->globox_macos_class_view);
		objc_disposeClassPair(platform->globox_macos_class_appdelegate);

		return;
	}
}

void globox_platform_create_window(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;
	Class class;
	id obj;

	// create the window
	class = objc_getClass("NSApplication");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);
		return;
	}

	// get the Application instance
	// no error check required
	platform->globox_platform_event_handle =
		macos_msg_id_none(
			(id) class,
			sel_getUid("sharedApplication"));

	// create the AppDelegate instance
	// no error check required
	platform->globox_macos_obj_appdelegate =
		macos_msg_id_none(
			(id) platform->globox_macos_class_appdelegate,
			sel_getUid("alloc"));

	// set the instance's globox pointer
	object_setInstanceVariable(
		platform->globox_macos_obj_appdelegate,
		"globox",
		globox);

	// run AppDelegate init
	obj =
		macos_msg_id_none(
			platform->globox_macos_obj_appdelegate,
			sel_getUid("init"));

	if (obj == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_INIT);

		macos_msg_void_none(
			(id) platform->globox_macos_obj_appdelegate,
			sel_getUid("dealloc"));

		return;
	}

	platform->globox_macos_obj_appdelegate = obj;

	// attach the AppDelegate to the NSApp singleton
	macos_msg_void_voidptr(
		platform->globox_platform_event_handle,
		sel_getUid("setDelegate:"),
		platform->globox_macos_obj_appdelegate);

	// finish launching
	macos_msg_void_none(
		platform->globox_platform_event_handle,
		sel_getUid("run"));
	// the `1` below is not clearly defined in Apple's documentation
	macos_msg_void_int(
		platform->globox_platform_event_handle,
		sel_getUid("setActivationPolicy:"),
		1);

	macos_msg_void_id(
		platform->globox_macos_obj_masterview,
		sel_getUid("addSubview:"),
		platform->globox_macos_obj_view);

	macos_msg_subview(
		platform->globox_macos_obj_masterview,
		sel_getUid("addSubview:positioned:relativeTo:"),
		platform->globox_macos_obj_blur,
		-1,
		platform->globox_macos_obj_view);
}

void globox_platform_hooks(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	macos_msg_void_bool(
		platform->globox_macos_obj_view,
		sel_getUid("setOpaque:"),
		NO);

	Class class = objc_getClass("NSColor");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);

		return;
	}

	id color =
		macos_msg_id_none(
			(id) class,
			sel_getUid("clearColor"));

	if (color == Nil)
	{ 
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);

		return;
	}

	macos_msg_void_id(
		platform->globox_macos_obj_view,
		sel_getUid("setBackgroundColor:"),
		color);

	globox_platform_set_title(globox, globox->globox_title);
	globox_platform_set_state(globox, globox->globox_state);
}

void globox_platform_commit(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	macos_msg_void_bool(
		platform->globox_macos_obj_view,
		sel_getUid("setNeedsDisplay:"),
		YES);
}

void globox_platform_prepoll(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	// create a trackingarea object
	Class class = objc_getClass("NSTrackingArea");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);
		return;
	}

	id trackingarea =
		macos_msg_id_none(
			(id) class,
			sel_getUid("alloc"));

	// configure it to automatically cover the view and supply a dummy rect
	struct macos_rect rect = {0};

	platform->globox_macos_obj_trackingarea =
		macos_msg_id_trackingarea(
			trackingarea,
			sel_getUid("initWithRect:options:owner:userInfo:"),
			rect,
			NSTrackingMouseMoved
			| NSTrackingActiveAlways
			| NSTrackingInVisibleRect,
			platform->globox_macos_obj_view,
			Nil);

	// bind it to the view to get mouse move events
	macos_msg_void_id(
		platform->globox_macos_obj_view,
		sel_getUid("addTrackingArea:"),
		platform->globox_macos_obj_trackingarea);
}

void globox_platform_events_poll(struct globox* globox)
{
	// not needed
}

void globox_platform_events_wait(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	id* future =
		macos_msg_idptr_none(
			(id) objc_getClass("NSDate"),
			sel_getUid("distantFuture"));

	// no error check required
	macos_msg_poll(
		platform->globox_platform_event_handle,
		sel_getUid("nextEventMatchingMask:untilDate:inMode:dequeue:"),
		NSEventMaskAny,
		future,
		NSDefaultRunLoopMode,
		NO);
}

static void query_pointer(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	struct macos_point pos =
		macos_msg_point_none(
			platform->globox_macos_obj_window,
			sel_getUid("mouseLocationOutsideOfEventStream"));

	platform->globox_macos_interactive_x = pos.x;
	platform->globox_macos_interactive_y = pos.y;
}

void globox_platform_interactive_mode(struct globox* globox, enum globox_interactive_mode mode)
{
	if ((mode != GLOBOX_INTERACTIVE_STOP)
		&& (globox->globox_interactive_mode != mode))
	{
		query_pointer(globox);

		if (globox_error_catch(globox))
		{
			return;
		}

		globox->globox_interactive_mode = mode;
	}
	else
	{
		globox->globox_interactive_mode = GLOBOX_INTERACTIVE_STOP;
	}
}

static void handle_interactive_mode(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	if (globox->globox_interactive_mode == GLOBOX_INTERACTIVE_STOP)
	{
		return;
	}

	int16_t old_x = platform->globox_macos_interactive_x;
	int16_t old_y = platform->globox_macos_interactive_y;

	query_pointer(globox);

	// get window position
	struct macos_rect frame;
	struct macos_rect frame_old;

	frame =
		macos_msg_rect_none(
			platform->globox_macos_obj_window,
			sel_getUid("frame"));

	frame_old = frame;

	// compute window changes
	switch (globox->globox_interactive_mode)
	{
		case GLOBOX_INTERACTIVE_MOVE:
		{
			frame.origin.x += platform->globox_macos_interactive_x - old_x;
			frame.origin.y += platform->globox_macos_interactive_y - old_y;
			break;
		}
		case GLOBOX_INTERACTIVE_N:
		{
			frame.size.height += platform->globox_macos_interactive_y - old_y;
			break;
		}
		case GLOBOX_INTERACTIVE_NW:
		{
			frame.origin.x += platform->globox_macos_interactive_x - old_x;
			frame.size.width += old_x - platform->globox_macos_interactive_x;
			frame.size.height += platform->globox_macos_interactive_y - old_y;
			break;
		}
		case GLOBOX_INTERACTIVE_W:
		{
			frame.origin.x += platform->globox_macos_interactive_x - old_x;
			frame.size.width += old_x - platform->globox_macos_interactive_x;
			break;
		}
		case GLOBOX_INTERACTIVE_SW:
		{
			frame.origin.x += platform->globox_macos_interactive_x - old_x;
			frame.origin.y += platform->globox_macos_interactive_y - old_y;
			frame.size.width += old_x - platform->globox_macos_interactive_x;
			frame.size.height += old_y - platform->globox_macos_interactive_y;
			break;
		}
		case GLOBOX_INTERACTIVE_S:
		{
			frame.origin.y += platform->globox_macos_interactive_y - old_y;
			frame.size.height += old_y - platform->globox_macos_interactive_y;
			break;
		}
		case GLOBOX_INTERACTIVE_SE:
		{
			frame.origin.y += platform->globox_macos_interactive_y - old_y;
			frame.size.width += platform->globox_macos_interactive_x - old_x;
			frame.size.height += old_y - platform->globox_macos_interactive_y;
			break;
		}
		case GLOBOX_INTERACTIVE_E:
		{
			frame.size.width += platform->globox_macos_interactive_x - old_x;
			break;
		}
		case GLOBOX_INTERACTIVE_NE:
		{
			frame.size.width += platform->globox_macos_interactive_x - old_x;
			frame.size.height += platform->globox_macos_interactive_y - old_y;
			break;
		}
		default:
		{
			break;
		}
	}

	// fix precision bugs
	frame.origin.x = (int16_t) frame.origin.x;
	frame.origin.y = (int16_t) frame.origin.y;
	frame.size.width = (int16_t) frame.size.width;
	frame.size.height = (int16_t) frame.size.height;

	// set window position
	struct macos_size size_view =
	{
		.width = frame.size.width,
		.height = frame.size.height,
	};

	platform->globox_macos_interactive_x += frame_old.origin.x - frame.origin.x;
	platform->globox_macos_interactive_y += frame_old.origin.y - frame.origin.y;

	macos_msg_resize(
		platform->globox_macos_obj_window,
		sel_getUid("setFrame:display:"),
		frame,
		YES);

	macos_msg_id_size(
		platform->globox_macos_obj_blur,
		sel_getUid("setFrameSize:"),
		size_view);

	macos_msg_id_size(
		platform->globox_macos_obj_view,
		sel_getUid("setFrameSize:"),
		size_view);

	globox->globox_redraw = true;
}

static void window_state_update(struct globox* globox, long event_data)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	enum globox_state state_tmp = globox->globox_state;

	switch (event_data)
	{
		case GLOBOX_MACOS_EVENT_WINDOW_MINIMIZE_ON:
		{
			globox->globox_state = GLOBOX_STATE_MINIMIZED;
			break;
		}
		case GLOBOX_MACOS_EVENT_WINDOW_MINIMIZE_OFF:
		{
			globox->globox_state = platform->globox_macos_state_old;
			break;
		}
		case GLOBOX_MACOS_EVENT_WINDOW_FULLSCREEN_ON:
		{
			globox->globox_state = GLOBOX_STATE_FULLSCREEN;
			break;
		}
		case GLOBOX_MACOS_EVENT_WINDOW_FULLSCREEN_OFF:
		{
			globox->globox_state = platform->globox_macos_state_old;
			break;
		}
		case GLOBOX_MACOS_EVENT_WINDOW_MAXIMIZE_ON:
		{
			globox->globox_state = GLOBOX_STATE_MAXIMIZED;
			break;
		}
		case GLOBOX_MACOS_EVENT_WINDOW_MAXIMIZE_OFF:
		default:
		{
			globox->globox_state = GLOBOX_STATE_REGULAR;
			break;
		}
	}

	platform->globox_macos_state_old = state_tmp;
}

void globox_platform_events_handle(
	struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	id* event =
		macos_msg_poll(
			platform->globox_platform_event_handle,
			sel_getUid("nextEventMatchingMask:untilDate:inMode:dequeue:"),
			NSEventMaskAny,
			Nil, // defaults to distantPast
			NSDefaultRunLoopMode,
			YES);

	if (event == Nil)
	{
		return;
	}

	unsigned long event_type =
		macos_msg_event_type(
			event,
			sel_getUid("type"));

	switch (event_type)
	{
		case NSEventTypeApplicationDefined:
		{
			short event_subtype =
				macos_msg_event_subtype(
					event,
					sel_getUid("subtype"));

			switch (event_subtype)
			{
				case GLOBOX_MACOS_EVENT_WINDOW_STATE:
				{
					long event_data =
						macos_msg_event_type(
							event,
							sel_getUid("data1"));

					window_state_update(globox, event_data);

					break;
				}
				case GLOBOX_MACOS_EVENT_WINDOW_CLOSE:
				{
					globox->globox_closed = true;
					break;
				}
				case GLOBOX_MACOS_EVENT_WINDOW_RESIZE:
				{
					globox->globox_redraw = true;
					break;
				}
				default:
				{
					break;
				}
			}

			break;
		}
		case NSEventTypeLeftMouseDown:
		{
			if ((globox->globox_interactive_mode != GLOBOX_INTERACTIVE_STOP)
			|| (platform->globox_macos_inhibit_resize == true))
			{
				break;
			}

			struct macos_point pos =
				macos_msg_point_none(
					platform->globox_macos_obj_window,
					sel_getUid("mouseLocationOutsideOfEventStream"));

			if (globox->globox_interactive_mode != GLOBOX_INTERACTIVE_STOP)
			{
				platform->globox_macos_interactive_x = pos.x;
				platform->globox_macos_interactive_y = pos.y;
			}

			break;
		}
		case NSEventTypeLeftMouseUp:
		{
			globox->globox_interactive_mode = GLOBOX_INTERACTIVE_STOP;

			break;
		}
	}

	handle_interactive_mode(globox);

	if ((globox->globox_event_callback != NULL)
	&& (event_type != NSEventTypeApplicationDefined))
	{
		globox->globox_event_callback(
			event,
			globox->globox_event_callback_data);
	}
}

void globox_platform_prepare_buffer(
	struct globox* globox)
{
	// not needed
}

void globox_platform_free(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	macos_msg_void_none(
		(id) platform->globox_macos_obj_appdelegate,
		sel_getUid("dealloc"));
	objc_disposeClassPair(platform->globox_macos_class_appdelegate);

	free(platform);
}

void globox_platform_set_icon(
	struct globox* globox,
	uint32_t* pixmap,
	uint32_t len)
{
	// not needed
}

void globox_platform_set_title(
	struct globox* globox,
	const char* title)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;
	char* tmp = strdup(title);

	if (globox->globox_title != NULL)
	{
		free(globox->globox_title);
	}

	globox->globox_title = tmp;

	Class class = objc_getClass("NSString");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);
		return;
	}

	id string =
		macos_msg_string(
			(id) class,
			sel_getUid("stringWithCString:encoding:"),
			tmp,
			NSUTF8StringEncoding);

	if (string == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	macos_msg_void_id(
		platform->globox_macos_obj_window,
		sel_getUid("setTitle:"),
		string);
}

void globox_platform_set_state(
	struct globox* globox,
	enum globox_state state)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;

	switch (state)
	{
		case GLOBOX_STATE_REGULAR:
		{
			if (globox->globox_state == GLOBOX_STATE_MINIMIZED)
			{
				macos_msg_void_id(
					platform->globox_macos_obj_window,
					sel_getUid("deminiaturize:"),
					platform->globox_macos_obj_appdelegate);
			}
			else if (globox->globox_state == GLOBOX_STATE_MAXIMIZED)
			{
				macos_msg_void_id(
					platform->globox_macos_obj_window,
					sel_getUid("zoom:"),
					platform->globox_macos_obj_appdelegate);
			}
			else if (globox->globox_state == GLOBOX_STATE_FULLSCREEN)
			{
				macos_msg_void_id(
					platform->globox_macos_obj_window,
					sel_getUid("toggleFullScreen:"),
					platform->globox_macos_obj_appdelegate);
			}

			globox->globox_state = GLOBOX_STATE_REGULAR;

			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			if (globox->globox_state == GLOBOX_STATE_MINIMIZED)
			{
				macos_msg_void_id(
					platform->globox_macos_obj_window,
					sel_getUid("deminiaturize:"),
					platform->globox_macos_obj_appdelegate);
			}
			else if (globox->globox_state == GLOBOX_STATE_FULLSCREEN)
			{
				macos_msg_void_id(
					platform->globox_macos_obj_window,
					sel_getUid("toggleFullScreen:"),
					platform->globox_macos_obj_appdelegate);
			}

			if (globox->globox_state != GLOBOX_STATE_MAXIMIZED)
			{
				globox->globox_state = GLOBOX_STATE_MAXIMIZED;

				macos_msg_void_id(
					platform->globox_macos_obj_window,
					sel_getUid("zoom:"),
					platform->globox_macos_obj_appdelegate);
			}

			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			globox->globox_state = GLOBOX_STATE_MINIMIZED;

			macos_msg_void_id(
				platform->globox_macos_obj_window,
				sel_getUid("miniaturize:"),
				platform->globox_macos_obj_appdelegate);

			break;
		}
		case GLOBOX_STATE_FULLSCREEN:
		{
			if (globox->globox_state != GLOBOX_STATE_FULLSCREEN)
			{
				globox->globox_state = GLOBOX_STATE_FULLSCREEN;

				macos_msg_void_id(
					platform->globox_macos_obj_window,
					sel_getUid("toggleFullScreen:"),
					platform->globox_macos_obj_appdelegate);
			}

			break;
		}
	}

	globox->globox_state = state;
}

// getters
uint32_t* globox_platform_get_argb(struct globox* globox)
{
	return globox->globox_platform->globox_platform_argb;
}

id globox_platform_get_event_handle(struct globox* globox)
{
	return globox->globox_platform->globox_platform_event_handle;
}
