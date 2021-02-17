#ifndef H_GLOBOX_PLATFORM
#define H_GLOBOX_PLATFORM

/// this file provides the platform substructure for the main globox context

// stdlib
#include <stdint.h>
#include <stdbool.h>
// include the macos structures and functions
#include "macos/globox_macos_types.h"
// include the graphic context structures and functions
// this will also have the effect of exposing the graphic context functions
#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "software/globox_macos_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "vulkan/globox_macos_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "egl/globox_macos_egl.h"
#endif

// platform structure
struct globox_platform
{
	// event handle that can be used to get window events notifications
	// this is available on all supported platforms and can be accessed
	// with `globox_get_platform_handle()`
	id globox_platform_event_handle;
	uint32_t* globox_platform_argb;

	// graphic contexts
#if defined(GLOBOX_CONTEXT_SOFTWARE)
	struct globox_macos_software globox_macos_software;
#elif defined(GLOBOX_CONTEXT_VULKAN)
	struct globox_macos_vulkan globox_macos_vulkan;
#elif defined(GLOBOX_CONTEXT_EGL)
	struct globox_macos_egl globox_macos_egl;
#endif

	Class globox_macos_class_view;
	Class globox_macos_class_appdelegate;
	Class globox_macos_class_cursor;

	id globox_macos_obj_view;
	id globox_macos_obj_appdelegate;
	id globox_macos_obj_cursor_a;
	id globox_macos_obj_cursor_b;
	id globox_macos_obj_window;
	id globox_macos_obj_masterview;
	id globox_macos_obj_blur;

	struct macos_size globox_macos_window_min_size;
	struct macos_rect globox_macos_buttons[3];

	int16_t globox_macos_titlebar_height;
	int16_t globox_macos_interactive_x;
	int16_t globox_macos_interactive_y;

	bool globox_macos_fullscreen;
	bool globox_macos_inhibit_resize;

	struct macos_rect globox_macos_old_window_frame;
	unsigned int globox_macos_state_old;
	id* globox_macos_obj_cursor_old;
	bool globox_macos_cursor_use_a;
	long globox_macos_cursor;
	unsigned int globox_macos_cursor_hover;

	void (*globox_macos_appdelegate_context_callback)(
		struct macos_appdelegate* appdelegate,
		SEL cmd,
		id msg);
};

#endif
