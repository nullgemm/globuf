#ifndef H_GLOBOX_PLATFORM
#define H_GLOBOX_PLATFORM

/// this file provides the platform substructure for the main globox context

// stdlib
#include <stdint.h>
#include <stdbool.h>
// include the x11 structures and functions
#include <sys/epoll.h>
#include <xcb/xcb.h>
// include the graphic context structures and functions
// this will also have the effect of exposing the graphic context functions
#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "software/globox_x11_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "vulkan/globox_x11_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "egl/globox_x11_egl.h"
#elif defined(GLOBOX_CONTEXT_GLX)
	#include "glx/globox_x11_glx.h"
#endif

// constants
#define GLOBOX_CONST_MAX_X11_EVENTS 1000
#define GLOBOX_CONST_MAX_X11_EXPOSE_QUEUE 1000

// atoms
enum globox_x11_atom_types
{
	GLOBOX_X11_ATOM_STATE_MAXIMIZED_HORIZONTAL = 0,
	GLOBOX_X11_ATOM_STATE_MAXIMIZED_VERTICAL,
	GLOBOX_X11_ATOM_STATE_FULLSCREEN,
	GLOBOX_X11_ATOM_STATE_HIDDEN,
	GLOBOX_X11_ATOM_STATE,
	GLOBOX_X11_ATOM_ICON,
	GLOBOX_X11_ATOM_PROTOCOLS,
	GLOBOX_X11_ATOM_FRAMELESS,
	GLOBOX_X11_ATOM_BLUR_KDE,
	GLOBOX_X11_ATOM_BLUR_DEEPIN,
	GLOBOX_X11_ATOM_DELETE_WINDOW,
	GLOBOX_X11_ATOM_COUNT,
};

// platform structure
struct globox_platform
{
	// event handle that can be used to get window events notifications
	// this is available on all supported platforms and can be accessed
	// with `globox_get_platform_handle()`
	int globox_platform_event_handle;
	uint32_t* globox_platform_argb;

	// graphic contexts
#if defined(GLOBOX_CONTEXT_SOFTWARE)
	struct globox_x11_software globox_x11_software;
#elif defined(GLOBOX_CONTEXT_VULKAN)
	struct globox_x11_vulkan globox_x11_vulkan;
#elif defined(GLOBOX_CONTEXT_EGL)
	struct globox_x11_egl globox_x11_egl;
#elif defined(GLOBOX_CONTEXT_GLX)
	struct globox_x11_glx globox_x11_glx;
#endif

	// setup
	xcb_connection_t* globox_x11_conn;
	xcb_atom_t globox_x11_atom_list[GLOBOX_X11_ATOM_COUNT];

	// windows
	xcb_window_t globox_x11_win;
	xcb_window_t globox_x11_root_win;
	int16_t globox_x11_interactive_x;
	int16_t globox_x11_interactive_y;

	// screens
	int globox_x11_screen_id;
	xcb_screen_t* globox_x11_screen_obj;
	int globox_x11_visual_depth;
	xcb_visualid_t globox_x11_visual_id;

	uint32_t globox_x11_attr_mask;
	uint32_t globox_x11_attr_val[3];

	int globox_x11_epoll;
	struct epoll_event globox_x11_epoll_event[GLOBOX_CONST_MAX_X11_EVENTS];
	uint32_t globox_x11_expose_queue[4 * GLOBOX_CONST_MAX_X11_EXPOSE_QUEUE];
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
	GLOBOX_ERROR_X11_CONN,
	GLOBOX_ERROR_X11_MAP,
	GLOBOX_ERROR_X11_FLUSH,
	GLOBOX_ERROR_X11_WIN,
	GLOBOX_ERROR_X11_GC,
	GLOBOX_ERROR_X11_VISUAL_NOT_COMPATIBLE,
	GLOBOX_ERROR_X11_VISUAL_NOT_FOUND,
	GLOBOX_ERROR_X11_SHM_VERSION,
	GLOBOX_ERROR_X11_SHM_VERSION_REPLY,
	GLOBOX_ERROR_X11_PIXMAP,
	GLOBOX_ERROR_X11_SHMID,
	GLOBOX_ERROR_X11_SHMADDR,
	GLOBOX_ERROR_X11_SHM_ATTACH,
	GLOBOX_ERROR_X11_SHMCTL,
	GLOBOX_ERROR_X11_SHM_PIXMAP,
	GLOBOX_ERROR_X11_EPOLL_CREATE,
	GLOBOX_ERROR_X11_EPOLL_CTL,
	GLOBOX_ERROR_X11_WIN_ATTR,
	GLOBOX_ERROR_X11_ATOMS,
	GLOBOX_ERROR_X11_EPOLL_WAIT,
	GLOBOX_ERROR_X11_ICON,
	GLOBOX_ERROR_X11_TITLE,
	GLOBOX_ERROR_X11_SHM_DETACH,
	GLOBOX_ERROR_X11_SHMDT,
	GLOBOX_ERROR_X11_IMAGE,
	GLOBOX_ERROR_X11_COPY,
	GLOBOX_ERROR_X11_SCREEN_INFO,
	GLOBOX_ERROR_X11_WIN_INFO,
	GLOBOX_ERROR_X11_STATE,
	GLOBOX_ERROR_X11_EGL_FAIL,
	GLOBOX_ERROR_X11_GLX_FAIL,
	GLOBOX_ERROR_X11_INTERACTIVE,

	// special value used to get the total number of error codes
	GLOBOX_ERROR_SIZE,
};

#endif
