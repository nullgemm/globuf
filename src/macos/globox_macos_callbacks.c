#include "globox.h"
#include "globox_error.h"
// macos includes
#include "macos/globox_macos_types.h"
#include "macos/globox_macos_symbols.h"

// include platform structures
#include "macos/globox_macos.h"
#include "macos/globox_macos_callbacks.h"

#define WINDOW_DELEGATE_METHOD_COUNT 9

struct globox_macos_method
{
	Class class;
	const char* name;
	void (*callback)();
	const char* signature;
};

static void globox_macos_get_globox(void** out)
{
	*out = NULL;

	Class class = objc_getClass("NSApplication");

	if (class == Nil)
	{
		return;
	}

	id nsapplication =
		macos_msg_id_none(
			(id) class,
			sel_getUid("sharedApplication"));

	if (nsapplication == Nil)
	{
		return;
	}

	id appdelegate =
		macos_msg_id_none(
			nsapplication,
			sel_getUid("delegate"));

	if (appdelegate == Nil)
	{
		return;
	}

	object_getInstanceVariable(
		appdelegate,
		"globox",
		out);
}

static void event_window_state(
	short event_subtype,
	long event_value)
{
	Class class;
	void* out;

	globox_macos_get_globox(&out);

	if (out == NULL)
	{
		return;
	}

	struct globox* globox = (struct globox*) out;

	// build a custom event
	struct macos_point point =
	{
		.x = 0.0,
		.y = 0.0,
	};

	class = objc_getClass("NSEvent");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);
		return;
	}

	id* event =
		macos_msg_event(
			(id) class,
			sel_getUid(
				"otherEventWithType:"
				"location:"
				"modifierFlags:"
				"timestamp:"
				"windowNumber:"
				"context:"
				"subtype:"
				"data1:"
				"data2:"),
			NSEventTypeApplicationDefined,
			point,
			0,
			0,
			0,
			Nil,
			event_subtype,
			event_value,
			0);

	if (event == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_NULL);
		return;
	}

	// get the nsapplication singleton
	class = objc_getClass("NSApplication");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);
		return;
	}

	id nsapplication =
		macos_msg_id_none(
			(id) class,
			sel_getUid("sharedApplication"));

	// send the custom event to the main thread
	macos_msg_post(
		nsapplication,
		sel_getUid("postEvent:atStart:"),
		event,
		NO);
}

struct macos_rect callback_window_will_use_standard_frame(
	id window_delegate,
	SEL cmd,
	id* window,
	struct macos_rect rect)
{
	event_window_state(
		GLOBOX_MACOS_EVENT_WINDOW_STATE,
		GLOBOX_MACOS_EVENT_WINDOW_MAXIMIZE_ON);

	return rect;
}

void callback_window_will_move(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	event_window_state(
		GLOBOX_MACOS_EVENT_WINDOW_STATE,
		GLOBOX_MACOS_EVENT_WINDOW_MAXIMIZE_OFF);
}

void callback_window_will_miniaturize(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	event_window_state(
		GLOBOX_MACOS_EVENT_WINDOW_STATE,
		GLOBOX_MACOS_EVENT_WINDOW_MINIMIZE_ON);
}

void callback_window_did_deminiaturize(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	event_window_state(
		GLOBOX_MACOS_EVENT_WINDOW_STATE,
		GLOBOX_MACOS_EVENT_WINDOW_MINIMIZE_OFF);
}

void callback_window_will_enter_fullscreen(
	id window_delegate,
	SEL cmd,
	id* window,
	struct macos_size size)
{
	// register window state
	event_window_state(
		GLOBOX_MACOS_EVENT_WINDOW_STATE,
		GLOBOX_MACOS_EVENT_WINDOW_FULLSCREEN_ON);

	// configure window accordingly
	void* out;

	globox_macos_get_globox(&out);

	if (out == NULL)
	{
		return;
	}

	struct globox* globox = (struct globox*) out;
	struct globox_platform* platform = globox->globox_platform;

	globox->globox_interactive_mode = GLOBOX_INTERACTIVE_STOP;
	platform->globox_macos_fullscreen = true;

	platform->globox_macos_old_window_frame =
		macos_msg_rect_none(
			platform->globox_macos_obj_window,
			sel_getUid("frame"));

	id screen =
		macos_msg_id_none(
			platform->globox_macos_obj_window,
			sel_getUid("deepestScreen"));

	if (screen == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	struct macos_rect frame =
		macos_msg_rect_none(
			screen,
			sel_getUid("frame"));

	macos_msg_void_size(
		platform->globox_macos_obj_view,
		sel_getUid("setFrameSize:"),
		frame.size);

	macos_msg_void_size(
		platform->globox_macos_obj_blur,
		sel_getUid("setFrameSize:"),
		frame.size);
}

void callback_window_will_exit_fullscreen(
	id window_delegate,
	SEL cmd,
	id* window,
	struct macos_size size)
{
	event_window_state(
		GLOBOX_MACOS_EVENT_WINDOW_STATE,
		GLOBOX_MACOS_EVENT_WINDOW_FULLSCREEN_OFF);

	void* out;

	globox_macos_get_globox(&out);

	if (out == NULL)
	{
		return;
	}

	struct globox* globox = (struct globox*) out;
	struct globox_platform* platform = globox->globox_platform;

	globox->globox_interactive_mode = GLOBOX_INTERACTIVE_STOP;
	platform->globox_macos_fullscreen = false;

	struct macos_size view_size = platform->globox_macos_old_window_frame.size;
	view_size.height -= platform->globox_macos_titlebar_height;

	macos_msg_resize(
		platform->globox_macos_obj_window,
		sel_getUid("setFrame:display:"),
		platform->globox_macos_old_window_frame,
		YES);

	macos_msg_void_size(
		platform->globox_macos_obj_view,
		sel_getUid("setFrameSize:"),
		view_size);

	macos_msg_void_size(
		platform->globox_macos_obj_blur,
		sel_getUid("setFrameSize:"),
		view_size);
}

void callback_window_will_close(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	event_window_state(
		GLOBOX_MACOS_EVENT_WINDOW_CLOSE,
		0);
}

struct macos_size callback_window_will_use_fullscreen_content_size(
	id window_delegate,
	SEL cmd,
	id* window,
	struct macos_size size)
{
	void* out;

	globox_macos_get_globox(&out);

	if (out == NULL)
	{
		return size;
	}

	struct globox* globox = (struct globox*) out;
	struct globox_platform* platform = globox->globox_platform;

	macos_msg_void_size(
		platform->globox_macos_obj_view,
		sel_getUid("setFrameSize:"),
		size);

	macos_msg_void_size(
		platform->globox_macos_obj_blur,
		sel_getUid("setFrameSize:"),
		size);

	event_window_state(
		GLOBOX_MACOS_EVENT_WINDOW_RESIZE,
		0);

	return size;
}

unsigned long callback_window_will_use_fullscreen_presentation_options(
	id window_delegate,
	SEL cmd,
	id* window,
	unsigned long options)
{
	return NSApplicationPresentationHideDock
		| NSApplicationPresentationHideMenuBar
		| NSApplicationPresentationDisableAppleMenu
		| NSApplicationPresentationFullScreen;
}

BOOL callback_application_will_finish_launching(
	struct macos_appdelegate* appdelegate,
	SEL cmd,
	id msg)
{
	void* out = NULL;

	object_getInstanceVariable(
		(id) appdelegate,
		"globox",
		&out);

	if (out == NULL)
	{
		return NO;
	}

	struct globox* globox = (struct globox*) out;
	struct globox_platform* platform = globox->globox_platform;

	Class class;
	BOOL ok;

	// allocate the window
	class = objc_getClass("NSWindow");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);

		return NO;
	}

	// initialize parameters
	int stylemask = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;

	if (globox->globox_frameless == false)
	{
		stylemask |= NSWindowStyleMaskTitled;
	}

	struct macos_rect rect_win =
	{
		.origin.x = globox->globox_x,
		.origin.y = globox->globox_y,
		.size.width = globox->globox_width,
		.size.height = globox->globox_height,
	};

	// initialize the window
	appdelegate->window =
		macos_msg_id_none(
			(id) class,
			sel_getUid("alloc"));

	id window_initialized =
		macos_msg_win(
			appdelegate->window,
			sel_getUid("initWithContentRect:styleMask:backing:defer:"),
			rect_win,
			stylemask,
			NSBackingStoreBuffered,
			NO);

	if (window_initialized == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);

		macos_msg_void_none(
			appdelegate->window,
			sel_getUid("dealloc"));

		return NO;
	}

	appdelegate->window = window_initialized;

	// declare a window delegate class
	class = objc_getClass("NSObject");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);

		macos_msg_void_none(
			appdelegate->window,
			sel_getUid("dealloc"));

		return NO;
	}

	Class window_delegate_class =
		objc_allocateClassPair(
			class,
			"WindowDelegate",
			0);

	if (window_delegate_class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_ALLOC);

		macos_msg_void_none(
			appdelegate->window,
			sel_getUid("dealloc"));

		return NO;
	}

	// inject some methods in the window delegate class
	struct globox_macos_method methods[WINDOW_DELEGATE_METHOD_COUNT] =
	{
		{
			window_delegate_class,
			"windowWillUseStandardFrame:defaultFrame:",
			(void (*)()) callback_window_will_use_standard_frame,
			"v@:^@:@",
		},
		{
			window_delegate_class,
			"windowWillMove:",
			(void (*)()) callback_window_will_move,
			"v@:^@",
		},
		{
			window_delegate_class,
			"windowWillMiniaturize:",
			(void (*)()) callback_window_will_miniaturize,
			"v@:^@",
		},
		{
			window_delegate_class,
			"windowDidDeminiaturize:",
			(void (*)()) callback_window_did_deminiaturize,
			"v@:^@"
		},
		{
			window_delegate_class,
			"windowWillEnterFullScreen:",
			(void (*)()) callback_window_will_enter_fullscreen,
			"v@:^@"
		},
		{
			window_delegate_class,
			"windowWillExitFullScreen:",
			(void (*)()) callback_window_will_exit_fullscreen,
			"v@:^@"
		},
		{
			window_delegate_class,
			"windowWillClose:",
			(void (*)()) callback_window_will_close,
			"v@:^@"
		},
		{
			window_delegate_class,
			"window:willUseFullScreenContentSize:",
			(void (*)()) callback_window_will_use_fullscreen_content_size,
			"@:^@:@"
		},
		{
			window_delegate_class,
			"window:willUseFullScreenPresentationOptions:",
			(void (*)()) callback_window_will_use_fullscreen_presentation_options,
			"@:^@:@"
		},
	};

	int i = 0;

	do
	{
		ok =
			class_addMethod(
				methods[i].class,
				sel_getUid(methods[i].name),
				(IMP) methods[i].callback,
				methods[i].signature);

		if (ok == NO)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_MACOS_CLASS_ADDMETHOD);

			macos_msg_void_none(
				appdelegate->window,
				sel_getUid("dealloc"));

			objc_disposeClassPair(window_delegate_class);

			return NO;
		}
		
		++i;
	}
	while (i < WINDOW_DELEGATE_METHOD_COUNT);

	// instantiate the window delegate object
	id window_delegate_obj =
		macos_msg_id_none(
			(id) window_delegate_class,
			sel_getUid("alloc"));

	// link the window delegate to the application delegate
	macos_msg_void_id(
		appdelegate->window,
		sel_getUid("setDelegate:"),
		window_delegate_obj);

	// instantiate the view objects
	class = objc_getClass("NSView");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);

		macos_msg_void_none(
			window_delegate_obj,
			sel_getUid("dealloc"));

		macos_msg_void_none(
			appdelegate->window,
			sel_getUid("dealloc"));

		objc_disposeClassPair(window_delegate_class);

		return NO;
	}

	platform->globox_macos_obj_masterview =
		macos_msg_id_none(
			(id) class,
			sel_getUid("alloc"));

	class = objc_getClass("NSVisualEffectView");

	if (class == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_CLASS_GET);

		macos_msg_void_none(
			platform->globox_macos_obj_masterview,
			sel_getUid("dealloc"));

		macos_msg_void_none(
			window_delegate_obj,
			sel_getUid("dealloc"));

		macos_msg_void_none(
			appdelegate->window,
			sel_getUid("dealloc"));

		objc_disposeClassPair(window_delegate_class);

		return NO;
	}

	platform->globox_macos_obj_blur =
		macos_msg_id_none(
			(id) class,
			sel_getUid("alloc"));

	platform->globox_macos_obj_view =
		macos_msg_id_none(
			(id) platform->globox_macos_class_view,
			sel_getUid("alloc"));

	// set the view object's globox context pointer
	object_setInstanceVariable(
		platform->globox_macos_obj_view,
		"globox",
		globox);

	// call the context-specific callback
	platform->globox_macos_appdelegate_context_callback(
		appdelegate,
		cmd,
		msg);

	if (globox_error_catch(globox))
	{
		return NO;
	}

	// build the window's content layout
	macos_msg_void_voidptr(
		appdelegate->window,
		sel_getUid("setContentView:"),
		platform->globox_macos_obj_masterview);

	macos_msg_void_none(
		appdelegate->window,
		sel_getUid("becomeFirstResponder"));

	macos_msg_void_voidptr(
		appdelegate->window,
		sel_getUid("makeKeyAndOrderFront:"),
		appdelegate);

	// save a reference to the window instance
	platform->globox_macos_obj_window = appdelegate->window;

	// HACK
	// escape the default window resizing mechanism
	macos_msg_void_bool(
		platform->globox_macos_obj_window,
		sel_getUid("setMovable:"),
		NO);

	macos_msg_void_ulong(
		platform->globox_macos_obj_window,
		sel_getUid("setCollectionBehavior:"),
		NSWindowCollectionBehaviorParticipatesInCycle
		| NSWindowCollectionBehaviorFullScreenPrimary
		| NSWindowCollectionBehaviorFullScreenAllowsTiling);

	id* button;
	int k = 0;

	while ((globox->globox_frameless == false) && (k < 3))
	{
		button =
			macos_msg_idptr_int(
				platform->globox_macos_obj_window,
				sel_getUid("standardWindowButton:"),
				k);

		if (button == Nil)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_MACOS_OBJ_NIL);

			macos_msg_void_none(
				platform->globox_macos_obj_view,
				sel_getUid("dealloc"));

			macos_msg_void_none(
				platform->globox_macos_obj_blur,
				sel_getUid("dealloc"));

			macos_msg_void_none(
				platform->globox_macos_obj_masterview,
				sel_getUid("dealloc"));

			macos_msg_void_none(
				window_delegate_obj,
				sel_getUid("dealloc"));

			macos_msg_void_none(
				appdelegate->window,
				sel_getUid("dealloc"));

			objc_disposeClassPair(window_delegate_class);

			return NO;
		}

		platform->globox_macos_buttons[k] =
			macos_msg_rect_none(
				(id) button,
				sel_getUid("frame"));

		macos_msg_void_bool(
			(id) button,
			sel_getUid("setEnabled:"),
			YES);

		++k;
	}

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

	macos_msg_void_bool(
		platform->globox_platform_event_handle,
		sel_getUid("activateIgnoringOtherApps:"),
		YES);

	return YES;
}

void callback_application_will_become_active(
	struct macos_appdelegate* appdelegate,
	SEL cmd,
	id msg)
{
	void* out = NULL;

	object_getInstanceVariable(
		(id) appdelegate,
		"globox",
		&out);

	if (out == NULL)
	{
		return;
	}

	struct globox* globox = (struct globox*) out;
	struct globox_platform* platform = globox->globox_platform;

	// HACK
	// escape the default event loop
	macos_msg_void_voidptr(
		platform->globox_platform_event_handle,
		sel_getUid("stop:"),
		NULL);
}

long callback_core_cursor_type(
	id self,
	SEL cmd)
{
	void* out;

	globox_macos_get_globox(&out);

	if (out == NULL)
	{
		return 0;
	}

	struct globox* globox = (struct globox*) out;
	struct globox_platform* platform = globox->globox_platform;

	return platform->globox_macos_cursor;
}
