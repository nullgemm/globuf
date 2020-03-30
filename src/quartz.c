#include "quartz.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globox.h"
#include "quartz_helpers.h"

// callbacks
void quartz_view_draw_rect_callback(
	id view,
	SEL cmd,
	struct quartz_rect rect)
{
	// retrieve globox context and update window size
	void* out;

	object_getInstanceVariable(
		view,
		"globox",
		&out);

	struct globox* globox = (struct globox*) out;

	// get that sweet memory address
	id ns_ctx = quartz_msg_id(
		(id) objc_getClass("NSGraphicsContext"),
		sel_getUid("currentContext"));

	id cg_ctx = quartz_msg_id(
		ns_ctx,
		sel_getUid("CGContext"));

	// will fail if the context is not a bitmap
	globox->redraw = true;
	globox->argb = (uint32_t*) CGBitmapContextGetData(cg_ctx);
	globox->width = CGBitmapContextGetWidth(cg_ctx);
	globox->height = CGBitmapContextGetHeight(cg_ctx);
	globox->padding = CGBitmapContextGetBytesPerRow(cg_ctx) / 4 - globox->width;
}

// window state event sender
void quartz_window_event(short event_subtype, long event_value)
{
	// create custom internal event
	struct quartz_point point =
	{
		.x = 0.0,
		.y = 0.0,
	};

	id* event = quartz_msg_event(
		(id) objc_getClass("NSEvent"),
		sel_getUid("otherEventWithType:location:modifierFlags:timestamp:windowNumber:context:subtype:data1:data2:"),
		NSEventTypeApplicationDefined,
		point,
		0,
		0,
		0,
		NULL,
		event_subtype,
		event_value,
		0);

	// get app singleton
	id app = quartz_msg_id(
		(id) objc_getClass("NSApplication"),
		sel_getUid("sharedApplication"));

	// send custom event to main thread
	quartz_msg_post(
		app,
		sel_getUid("postEvent:atStart:"),
		event,
		true);
}

// window state event callbacks relaying the info to quartz_window_event
struct quartz_rect quartz_window_event_maximize_on(
	id window_delegate,
	SEL cmd,
	id* window,
	struct quartz_rect rect)
{
	quartz_window_event(
		GLOBOX_QUARTZ_EVENT_WINDOW_STATE,
		GLOBOX_QUARTZ_WINDOW_EVENT_MAXIMIZE_ON);

	return rect;
}

void quartz_window_event_move(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	quartz_window_event(
		GLOBOX_QUARTZ_EVENT_WINDOW_STATE,
		GLOBOX_QUARTZ_WINDOW_EVENT_MAXIMIZE_OFF);
}

void quartz_window_event_minimize_on(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	quartz_window_event(
		GLOBOX_QUARTZ_EVENT_WINDOW_STATE,
		GLOBOX_QUARTZ_WINDOW_EVENT_MINIMIZE_ON);
}

void quartz_window_event_minimize_off(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	quartz_window_event(
		GLOBOX_QUARTZ_EVENT_WINDOW_STATE,
		GLOBOX_QUARTZ_WINDOW_EVENT_MINIMIZE_OFF);
}

void quartz_window_event_fullscreen_on(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	quartz_window_event(
		GLOBOX_QUARTZ_EVENT_WINDOW_STATE,
		GLOBOX_QUARTZ_WINDOW_EVENT_FULLSCREEN_ON);
}

void quartz_window_event_fullscreen_off(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	quartz_window_event(
		GLOBOX_QUARTZ_EVENT_WINDOW_STATE,
		GLOBOX_QUARTZ_WINDOW_EVENT_FULLSCREEN_OFF);
}

void quartz_window_event_close(
	id window_delegate,
	SEL cmd,
	id* notif)
{
	quartz_window_event(GLOBOX_QUARTZ_EVENT_WINDOW_CLOSE, 0);
}

// app delegate window creation callback
bool quartz_app_delegate_init_callback(
	struct quartz_app_delegate* app_delegate,
	SEL cmd,
	id msg)
{
	// retrieve globox context
	void* out;

	object_getInstanceVariable(
		(id) app_delegate,
		"globox",
		&out);

	struct globox* globox = (struct globox*) out;

	// window
	int states =
		NSWindowStyleMaskTitled
		| NSWindowStyleMaskClosable
		| NSWindowStyleMaskMiniaturizable
		| NSWindowStyleMaskResizable;

	struct quartz_rect rect_win =
	{
		.origin.x = globox->init_x,
		.origin.y = globox->init_y,
		.size.width = globox->width,
		.size.height = globox->height,
	};

	app_delegate->window = quartz_msg_id(
		(id) objc_getClass("NSWindow"),
		sel_getUid("alloc"));

	app_delegate->window = quartz_msg_win(
		app_delegate->window, 
		sel_getUid("initWithContentRect:styleMask:backing:defer:"),
		rect_win,
		states,
		NSBackingStoreBuffered,
		false);

	// window delegate
	Class window_delegate_class = objc_allocateClassPair(
		(Class) objc_getClass("NSObject"),
		"WindowDelegate",
		0);

	// inject methods
	class_addMethod(
		window_delegate_class,
		sel_getUid("windowWillUseStandardFrame:defaultFrame:"),
		(IMP) quartz_window_event_maximize_on,
		"v@:^@:@");

	class_addMethod(
		window_delegate_class,
		sel_getUid("windowWillMove:"),
		(IMP) quartz_window_event_move,
		"v@:^@");

	class_addMethod(
		window_delegate_class,
		sel_getUid("windowWillMiniaturize:"),
		(IMP) quartz_window_event_minimize_on,
		"v@:^@");

	class_addMethod(
		window_delegate_class,
		sel_getUid("windowDidDeminiaturize:"),
		(IMP) quartz_window_event_minimize_off,
		"v@:^@");

	class_addMethod(
		window_delegate_class,
		sel_getUid("windowWillEnterFullScreen:"),
		(IMP) quartz_window_event_fullscreen_on,
		"v@:^@");

	class_addMethod(
		window_delegate_class,
		sel_getUid("windowWillExitFullScreen:"),
		(IMP) quartz_window_event_fullscreen_off,
		"v@:^@");

	class_addMethod(
		window_delegate_class,
		sel_getUid("windowWillClose:"),
		(IMP) quartz_window_event_close,
		"v@:^@");

	id window_delegate = quartz_msg_id(
		(id) window_delegate_class,
		sel_getUid("alloc"));

	// register in the application delegate
	quartz_msg_ptr(
		app_delegate->window,
		sel_getUid("setDelegate:"),
		window_delegate);

	// view
	struct quartz_rect rect_view =
	{
		.origin.x = 0,
		.origin.y = 0,
		.size.width = globox->width,
		.size.height = globox->height,
	};

	// instantiate view object
	globox->quartz_view_obj = quartz_msg_id(
		(id) globox->quartz_view_class,
		sel_getUid("alloc"));

	// set instance's globox context pointer
	object_setInstanceVariable(
		globox->quartz_view_obj,
		"globox",
		globox);

	// initialize view frame
	globox->quartz_view_obj = quartz_msg_rect(
		globox->quartz_view_obj,
		sel_getUid("initWithFrame:"),
		rect_view);

	// messages
	quartz_msg_ptr(
		app_delegate->window, 
		sel_getUid("setContentView:"),
		globox->quartz_view_obj);

	quartz_msg_id(
		app_delegate->window, 
		sel_getUid("becomeFirstResponder"));

	quartz_msg_ptr(
		app_delegate->window, 
		sel_getUid("makeKeyAndOrderFront:"),
		app_delegate);

	// escape the pesky default event loop
	quartz_msg_ptr(
		globox->fd.app,
		sel_getUid("stop:"),
		NULL);

	// save a reference to the window instance
	globox->quartz_window_obj = app_delegate->window;

	return true;
}
