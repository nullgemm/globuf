#ifndef H_GLOBOX_PLATFORM
#define H_GLOBOX_PLATFORM

#include "globox.h"

// stdlib
#include <stdint.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-client-protocol.h"
#include "kde-blur-client-protocol.h"
#include "zwp-relative-pointer-protocol.h"
#include "zwp-pointer-constraints-protocol.h"

// include the graphic context structures and functions
// this will also have the effect of exposing the graphic context functions
#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "wayland/software/globox_wayland_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "wayland/vulkan/globox_wayland_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "wayland/egl/globox_wayland_egl.h"
#endif

// constants
#define GLOBOX_CONST_MAX_WAYLAND_EVENTS 1000

// forward-declaration
struct globox;

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
	struct globox_wayland_software globox_wayland_software;
#elif defined(GLOBOX_CONTEXT_VULKAN)
	struct globox_wayland_vulkan globox_wayland_vulkan;
#elif defined(GLOBOX_CONTEXT_EGL)
	struct globox_wayland_egl globox_wayland_egl;
#endif

	int globox_wayland_epoll;
	struct epoll_event globox_wayland_epoll_event[GLOBOX_CONST_MAX_WAYLAND_EVENTS];

	uint32_t* globox_wayland_icon;
	uint32_t globox_wayland_icon_len;

	uint32_t globox_wayland_screen_width;
	uint32_t globox_wayland_screen_height;

	void (*globox_wayland_unminimize_start)(struct globox* globox);
	void (*globox_wayland_unminimize_finish)(struct globox* globox);
	void (*globox_wayland_callback_xdg_surface_configure)(
		struct globox* globox);
	void (*globox_wayland_callback_xdg_toplevel_configure)(
		struct globox* globox,
		int32_t width,
		int32_t height);

	struct wl_display* globox_wayland_display;
	struct wl_registry* globox_wayland_registry;

	struct wl_shm* globox_wayland_shm;
	struct wl_compositor* globox_wayland_compositor;
	struct wl_output* globox_wayland_output;
	struct wl_seat* globox_wayland_seat;

	struct xdg_wm_base* globox_wayland_xdg_wm_base;
	struct xdg_toplevel* globox_wayland_xdg_toplevel;
	struct xdg_surface* globox_wayland_xdg_surface;
	struct wl_surface* globox_wayland_surface;

	struct wl_output_listener globox_wayland_output_listener;
	struct wl_registry_listener globox_wayland_registry_listener;
	struct xdg_wm_base_listener globox_wayland_xdg_wm_base_listener;
	struct xdg_toplevel_listener globox_wayland_xdg_toplevel_listener;
	struct xdg_surface_listener globox_wayland_xdg_surface_listener;

	struct zxdg_decoration_manager_v1* globox_wayland_xdg_decoration_manager;
	struct zxdg_toplevel_decoration_v1* globox_wayland_xdg_decoration;
	struct zxdg_toplevel_decoration_v1_listener globox_wayland_xdg_decoration_listener;

	struct org_kde_kwin_blur_manager* globox_wayland_kde_blur_manager;
	struct org_kde_kwin_blur* globox_wayland_kde_blur;

	uint32_t globox_wayland_saved_serial;
};

enum globox_error
{
	// special error code to signal everything is fine
	GLOBOX_ERROR_OK = 0,

	// error codes describing the nature of the problem
	GLOBOX_ERROR_FD,
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
	GLOBOX_ERROR_WAYLAND_EGL_FAIL,
	GLOBOX_ERROR_WAYLAND_EPOLL_CREATE,
	GLOBOX_ERROR_WAYLAND_EPOLL_CTL,
	GLOBOX_ERROR_WAYLAND_EPOLL_WAIT,
	GLOBOX_ERROR_WAYLAND_DISPLAY,
	GLOBOX_ERROR_WAYLAND_ROUNDTRIP,
	GLOBOX_ERROR_WAYLAND_DISPATCH,
	GLOBOX_ERROR_WAYLAND_MMAP,
	GLOBOX_ERROR_WAYLAND_MUNMAP,
	GLOBOX_ERROR_WAYLAND_REQUEST,
	GLOBOX_ERROR_WAYLAND_LISTENER,

	// special value used to get the total number of error codes
	GLOBOX_ERROR_SIZE,
};

#endif
