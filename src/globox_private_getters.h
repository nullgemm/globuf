#ifndef H_GLOBOX_PRIVATE_GETTERS
#define H_GLOBOX_PRIVATE_GETTERS

/// this is an extra header which can be useful
/// to get access to the internals of globox

#include "globox.h"

// common includes
#include <stdint.h>
#include <stdbool.h>

// platform-specific includes
#if defined(GLOBOX_PLATFORM_WAYLAND)
	#include <wayland-client.h>
	#include "xdg-shell-client-protocol.h"
	#include "kde-blur-client-protocol.h"
#elif defined(GLOBOX_PLATFORM_X11)
	#include <xcb/xcb.h>
	#if defined(GLOBOX_CONTEXT_SOFTWARE)
		#include <xcb/xcb_image.h>
		#include <xcb/shm.h>
	#elif defined(GLOBOX_CONTEXT_GLX)
		#include <X11/Xlib.h>
		#include <GL/glx.h>
	#endif
#elif defined(GLOBOX_PLATFORM_WINDOWS)
	#include <windows.h>
#elif defined(GLOBOX_PLATFORM_MACOS)
	#include <objc/objc.h>
#endif

#if defined(GLOBOX_CONTEXT_EGL)
	#include <EGL/egl.h>
#endif

// platform-dependent getters
#if defined(GLOBOX_PLATFORM_WAYLAND)
int globox_platform_get_event_handle(
	struct globox* globox);
// no getter for globox_wayland_epoll
// no getter for globox_wayland_epoll_event
// no getter for globox_wayland_icon
// no getter for globox_wayland_icon_len
// no getter for globox_wayland_unminimize_start
// no getter for globox_wayland_unminimize_finish
// no getter for globox_wayland_callback_xdg_surface_configure
// no getter for globox_wayland_callback_xdg_toplevel_configure
struct wl_display* globox_get_wayland_display(
	struct globox* globox);
struct wl_registry* globox_get_wayland_registry(
	struct globox* globox);
struct wl_shm* globox_get_wayland_shm(
	struct globox* globox);
struct wl_compositor* globox_get_wayland_compositor(
	struct globox* globox);
struct wl_output* globox_get_wayland_output(
	struct globox* globox);
struct wl_seat* globox_get_wayland_seat(
	struct globox* globox);
struct xdg_wm_base* globox_get_wayland_xdg_wm_base(
	struct globox* globox);
struct xdg_toplevel* globox_get_wayland_xdg_toplevel(
	struct globox* globox);
struct xdg_surface* globox_get_wayland_xdg_surface(
	struct globox* globox);
struct wl_surface* globox_get_wayland_surface(
	struct globox* globox);
struct wl_output_listener* globox_get_wayland_output_listener(
	struct globox* globox);
// no getter for globox_wayland_registry_listener
// no getter for globox_wayland_xdg_wm_base_listener
// no getter for globox_wayland_xdg_toplevel_listener
// no getter for globox_wayland_xdg_surface_listener
// no getter for globox_wayland_output_registry
// no getter for globox_wayland_output_name
void** globox_get_wayland_output_data(
	struct globox* globox);
struct zwp_relative_pointer_manager_v1* globox_get_wayland_pointer_manager(
	struct globox* globox);
struct zwp_pointer_constraints_v1* globox_get_wayland_pointer_constraints(
	struct globox* globox);
struct org_kde_kwin_blur_manager* globox_get_wayland_kde_blur_manager(
	struct globox* globox);
struct org_kde_kwin_blur* globox_get_wayland_kde_blur(
	struct globox* globox);
uint32_t globox_get_wayland_saved_serial(
	struct globox* globox);

#if defined(GLOBOX_CONTEXT_SOFTWARE)
// no getter for globox_software_buffer_mutex
// no getter for globox_software_buffer_list
// no getter for globox_software_buffer_list_len
// no getter for globox_software_buffer_list_max
// no getter for globox_software_buffer_listener
struct wl_buffer* globox_get_wayland_software_buffer(
	struct globox* globox);
// no getter for globox_software_buffer_len
#elif defined(GLOBOX_CONTEXT_EGL)
struct wl_egl_window* globox_get_wayland_egl_window(
	struct globox* globox);
#endif
#elif defined(GLOBOX_PLATFORM_X11)
int globox_platform_get_event_handle(
	struct globox* globox);
// no getter for globox_x11_expose
// no getter for globox_x11_reserve
xcb_connection_t* globox_get_x11_conn(
	struct globox* globox);
xcb_atom_t* globox_get_x11_atom_list(
	struct globox* globox);
xcb_window_t globox_get_x11_win(
	struct globox* globox);
xcb_window_t globox_get_x11_root_win(
	struct globox* globox);
// no getter for globox_x11_interactive_x
// no getter for globox_x11_interactive_y
int globox_get_x11_screen_id(
	struct globox* globox);
xcb_screen_t* globox_get_x11_screen_obj(
	struct globox* globox);
int globox_get_x11_visual_depth(
	struct globox* globox);
xcb_visualid_t globox_get_x11_visual_id(
	struct globox* globox);
uint32_t globox_get_x11_attr_mask(
	struct globox* globox);
uint32_t* globox_get_x11_attr_val(
	struct globox* globox);
// no getter for globox_get_x11_epoll
// no getter for globox_x11_epoll_event
// no getter for globox_x11_expose_queue

#if defined(GLOBOX_CONTEXT_SOFTWARE)
xcb_shm_segment_info_t globox_get_x11_software_shm(
	struct globox* globox);
xcb_gcontext_t globox_get_x11_software_gfx(
	struct globox* globox);
xcb_pixmap_t globox_get_x11_software_pixmap(
	struct globox* globox);
// no getter for globox_software_pixmap_update
// no getter for globox_software_shared_pixmaps
// no getter for globox_software_buffer_len
#elif defined(GLOBOX_CONTEXT_GLX)
Display* globox_get_x11_glx_display(
	struct globox* globox);
GLXFBConfig globox_get_x11_glx_fb_config(
	struct globox* globox);
GLXContext globox_get_x11_glx_context(
	struct globox* globox);
GLXWindow globox_get_x11_glx_win(
	struct globox* globox);
// no getter for globox_glx_version_major
// no getter for globox_glx_version_minor
#endif
#elif defined(GLOBOX_PLATFORM_WINDOWS)
HWND globox_platform_get_event_handle(
	struct globox* globox);
MSG globox_get_windows_msg(
	struct globox* globox);
LPWSTR globox_get_windows_wide_title(
	struct globox* globox);
LPWSTR globox_get_windows_class_name(
	struct globox* globox);
HMODULE globox_get_windows_class_module_handle(
	struct globox* globox);
WNDCLASSEX globox_get_windows_class(
	struct globox* globox);
LONG globox_get_windows_style_backup(
	struct globox* globox);
// no getter for globox_windows_exstyle_backup;
WINDOWPLACEMENT globox_get_windows_position_backup(
	struct globox* globox);
// no getter for globox_windows_old_outer_x
// no getter for globox_windows_old_outer_y
// no getter for globox_windows_framediff_x
// no getter for globox_windows_framediff_y
// no getter for globox_windows_outer_width
// no getter for globox_windows_outer_height
// no getter for globox_windows_old_outer_width
// no getter for globox_windows_old_outer_height
// no getter for globox_windows_interactive_x
// no getter for globox_windows_interactive_y
// no getter for globox_windows_sizemove_step
// no getter for globox_windows_resize_callback
// no getter for globox_windows_dcomp_callback
// no getter for globox_windows_dwm_transparency_callback

#if defined(GLOBOX_CONTEXT_SOFTWARE)
// no getter for globox_software_buffer_width
// no getter for globox_software_buffer_height
BITMAPINFO globox_get_windows_software_bmp_info(
	struct globox* globox);
HBITMAP globox_get_windows_software_bmp_handle(
	struct globox* globox);
#elif defined(GLOBOX_CONTEXT_WGL)
// no getter for globox_wgl_version_major
// no getter for globox_wgl_version_minor
PIXELFORMATDESCRIPTOR globox_get_windows_wgl_pfd(
	struct globox* globox);
HGLRC globox_get_windows_wgl_context(
	struct globox* globox);
#endif
#elif defined(GLOBOX_PLATFORM_MACOS)
id globox_platform_get_event_handle(
	struct globox* globox);
Class globox_get_macos_class_view(
	struct globox* globox);
Class globox_get_macos_class_appdelegate(
	struct globox* globox);
// no getter for globox_get_macos_class_cursor
id globox_get_macos_obj_view(
	struct globox* globox);
id globox_get_macos_obj_appdelegate(
	struct globox* globox);
// no getter for globox_get_macos_obj_cursor_a
// no getter for globox_get_macos_obj_cursor_b
id globox_get_macos_obj_window(
	struct globox* globox);
id globox_get_macos_obj_masterview(
	struct globox* globox);
id globox_get_macos_obj_blur(
	struct globox* globox);
// no getter for globox_get_macos_window_min_size
// no getter for globox_get_macos_buttons
// no getter for globox_macos_titlebar_height
// no getter for globox_macos_interactive_x
// no getter for globox_macos_interactive_y
// no getter for globox_macos_fullscreen
// no getter for globox_macos_inhibit_resize
// no getter for globox_macos_old_window_frame
// no getter for globox_macos_state_old
// no getter for globox_macos_obj_cursor_old
// no getter for globox_macos_cursor_use_a
// no getter for globox_macos_cursor
// no getter for globox_macos_cursor_hover
// no getter for globox_macos_appdelegate_context_callback

#if defined(GLOBOX_CONTEXT_SOFTWARE)
// no getter for globox_software_buffer_width
// no getter for globox_software_buffer_height
id globox_get_macos_software_layer(
	struct globox* globox);
#elif defined(GLOBOX_CONTEXT_EGL)
id globox_get_macos_egl_layer(
	struct globox* globox);
#endif
#endif

// platform-independent context getters
#if defined(GLOBOX_CONTEXT_EGL)
EGLDisplay globox_get_egl_display(
	struct globox* globox);
EGLContext globox_get_egl_context(
	struct globox* globox);
EGLSurface globox_get_egl_surface(
	struct globox* globox);
EGLConfig globox_get_egl_config(
	struct globox* globox);
EGLint globox_get_egl_config_size(
	struct globox* globox);
#endif

#endif
