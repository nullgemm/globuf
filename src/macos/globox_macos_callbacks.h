#ifndef H_GLOBOX_MACOS_CALLBACKS
#define H_GLOBOX_MACOS_CALLBACKS

#include "macos/globox_macos_types.h"
#include "macos/globox_macos.h"

struct macos_rect callback_window_will_use_standard_frame(
	id window_delegate,
	SEL cmd,
	id* window,
	struct macos_rect rect);
void callback_window_will_move(
	id window_delegate,
	SEL cmd,
	id* notif);
void callback_window_will_miniaturize(
	id window_delegate,
	SEL cmd,
	id* notif);
void callback_window_did_deminiaturize(
	id window_delegate,
	SEL cmd,
	id* notif);
void callback_window_will_enter_fullscreen(
	id window_delegate,
	SEL cmd,
	id* window,
	struct macos_size size);
void callback_window_will_exit_fullscreen(
	id window_delegate,
	SEL cmd,
	id* window,
	struct macos_size size);
void callback_window_will_close(
	id window_delegate,
	SEL cmd,
	id* notif);
struct macos_size callback_window_will_use_fullscreen_content_size(
	id window_delegate,
	SEL cmd,
	id* window,
	struct macos_size size);
unsigned long callback_window_will_use_fullscreen_presentation_options(
	id window_delegate,
	SEL cmd,
	id* window,
	unsigned long options);
BOOL callback_application_did_finish_launching(
	struct macos_appdelegate* appdelegate,
	SEL cmd,
	id msg);
long callback_core_cursor_type(
	id self,
	SEL cmd);

#endif
