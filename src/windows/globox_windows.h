#ifndef H_GLOBOX_WINDOWS
#define H_GLOBOX_WINDOWS

/// this file provides the platform substructure for the main globox context

// stdlib
#include <stdint.h>
#include <stdbool.h>
// include the windows structures and functions

// include the graphic context structures and functions
// this will also have the effect of exposing the graphic context functions
#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "software/globox_windows_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "vulkan/globox_windows_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "egl/globox_windows_egl.h"
#elif defined(GLOBOX_CONTEXT_WGL)
	#include "glx/globox_windows_wgl.h"
#endif

// forward-declaration
struct globox;

// sizemove enum
enum globox_windows_sizemove
{
	GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL = 0, // waiting for the modal loop
	GLOBOX_WINDOWS_SIZEMOVE_WAITEDGES,     // waiting for the edge info
	GLOBOX_WINDOWS_SIZEMOVE_KILLMODAL,     // killing the modal loop
	GLOBOX_WINDOWS_SIZEMOVE_SYNTHDRAG,     // synthetizing a mouse drag
	GLOBOX_WINDOWS_SIZEMOVE_STARTSIZE,     // starting the resize
};

// platform structure
struct globox_platform
{
	// event handle that can be used to get window events notifications
	// this is available on all supported platforms and can be accessed
	// with `globox_get_platform_handle()`
	HWND globox_platform_event_handle;
	uint32_t* globox_platform_argb;

	// graphic contexts
#if defined(GLOBOX_CONTEXT_SOFTWARE)
	struct globox_windows_software globox_windows_software;
#elif defined(GLOBOX_CONTEXT_VULKAN)
	struct globox_windows_vulkan globox_windows_vulkan;
#elif defined(GLOBOX_CONTEXT_EGL)
	struct globox_windows_egl globox_windows_egl;
#elif defined(GLOBOX_CONTEXT_GLX)
	struct globox_windows_glx globox_windows_glx;
#endif

	MSG globox_windows_msg;
	LPWSTR globox_windows_wide_title;
	LPWSTR globox_windows_class_name;
	HMODULE globox_windows_class_module_handle;
	WNDCLASSEX globox_windows_class;

	LONG globox_windows_style_backup;
	LONG globox_windows_exstyle_backup;
	WINDOWPLACEMENT globox_windows_position_backup;

	uint32_t globox_windows_old_outer_x;
	uint32_t globox_windows_old_outer_y;
	uint32_t globox_windows_framediff_x;
	uint32_t globox_windows_framediff_y;

	uint32_t globox_windows_outer_width;
	uint32_t globox_windows_outer_height;
	uint32_t globox_windows_old_outer_width;
	uint32_t globox_windows_old_outer_height;

	int64_t globox_windows_interactive_x;
	int64_t globox_windows_interactive_y;
	enum globox_windows_sizemove globox_windows_sizemove_step;

	void (*globox_windows_resize_callback)(struct globox* globox);
};

enum globox_error
{
	// special error code to signal everything is fine
	GLOBOX_ERROR_OK = 0,

	// error codes describing the nature of the problem
	GLOBOX_ERROR_NULL,
	GLOBOX_ERROR_ALLOC,
	GLOBOX_ERROR_BOUNDS,
	GLOBOX_ERROR_DOMAIN,

	// extra high-level error codes for use by the developer
	// they can be used to throw a more generic error and
	// describe the context instead of the error itself
	GLOBOX_ERROR_FUNC_OPEN,
	GLOBOX_ERROR_FUNC_CLOSE,

	GLOBOX_ERROR_FUNC_PLATFORM_INIT,
	GLOBOX_ERROR_FUNC_PLATFORM_FREE,
	GLOBOX_ERROR_FUNC_PLATFORM_CREATE_WINDOW,
	GLOBOX_ERROR_FUNC_PLATFORM_HOOKS,
	GLOBOX_ERROR_FUNC_PLATFORM_COMMIT,
	GLOBOX_ERROR_FUNC_PLATFORM_PREPOLL,
	GLOBOX_ERROR_FUNC_PLATFORM_EVENTS_POLL,
	GLOBOX_ERROR_FUNC_PLATFORM_EVENTS_WAIT,
	GLOBOX_ERROR_FUNC_PLATFORM_EVENTS_HANDLE,
	GLOBOX_ERROR_FUNC_PLATFORM_SET_ICON,
	GLOBOX_ERROR_FUNC_PLATFORM_SET_TITLE,
	GLOBOX_ERROR_FUNC_PLATFORM_SET_STATE,

	GLOBOX_ERROR_FUNC_SOFTWARE_INIT,
	GLOBOX_ERROR_FUNC_SOFTWARE_FREE,
	GLOBOX_ERROR_FUNC_SOFTWARE_CREATE,
	GLOBOX_ERROR_FUNC_SOFTWARE_SHRINK,
	GLOBOX_ERROR_FUNC_SOFTWARE_RESERVE,
	GLOBOX_ERROR_FUNC_SOFTWARE_EXPOSE,
	GLOBOX_ERROR_FUNC_SOFTWARE_COPY,

	// platform errors
	GLOBOX_ERROR_WINDOWS_UTF8_WCHAR,
	GLOBOX_ERROR_WINDOWS_MODULE_HANDLE,
	GLOBOX_ERROR_WINDOWS_DEFAULT_CURSOR,
	GLOBOX_ERROR_WINDOWS_REGISTER_CLASS,
	GLOBOX_ERROR_WINDOWS_CREATE_WINDOW,
	GLOBOX_ERROR_WINDOWS_UPDATE_WINDOW,
	GLOBOX_ERROR_WINDOWS_DELETE_BMP_HANDLE,
	GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET,
	GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_REMOVE,
	GLOBOX_ERROR_WINDOWS_CREATE_DIB_SECTION,
	GLOBOX_ERROR_WINDOWS_DAMAGE,
	GLOBOX_ERROR_WINDOWS_PAINT,
	GLOBOX_ERROR_WINDOWS_BITBLT,
	GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_CREATE,
	GLOBOX_ERROR_WINDOWS_SELECT_BITMAP_HANDLE,
	GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_DELETE,
	GLOBOX_ERROR_WINDOWS_MASK_CREATE,
	GLOBOX_ERROR_WINDOWS_BITMAP_CREATE,
	GLOBOX_ERROR_WINDOWS_ICON_CREATE,
	GLOBOX_ERROR_WINDOWS_DELETE_MASK,
	GLOBOX_ERROR_WINDOWS_GET_MESSAGE,
	GLOBOX_ERROR_WINDOWS_TITLE,
	GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET,
	GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET,
	GLOBOX_ERROR_WINDOWS_WINDOW_PLACEMENT_SET,
	GLOBOX_ERROR_WINDOWS_WINDOW_PLACEMENT_GET,
	GLOBOX_ERROR_WINDOWS_DESTROY,
	GLOBOX_ERROR_WINDOWS_ADJUST_WINDOW,
	GLOBOX_ERROR_WINDOWS_CLIENT_RECT_GET,
	GLOBOX_ERROR_WINDOWS_GLOBOX_PTR,

	// special value used to get the total number of error codes
	GLOBOX_ERROR_SIZE,
};

#endif
