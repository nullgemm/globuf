#include "globox_private_getters.h"

#if defined(GLOBOX_PLATFORM_WAYLAND)
	#include "wayland/globox_wayland.h"
#elif defined(GLOBOX_PLATFORM_X11)
	#include "x11/globox_x11.h"
#elif defined(GLOBOX_PLATFORM_WINDOWS)
	#include "windows/globox_windows.h"
#elif defined(GLOBOX_PLATFORM_MACOS)
	#include "macos/globox_macos.h"
#endif

// platform-dependent getters
#if defined(GLOBOX_PLATFORM_WAYLAND)
// no getter for globox_wayland_epoll
// no getter for globox_wayland_epoll_event
// no getter for globox_wayland_icon
// no getter for globox_wayland_icon_len
// no getter for globox_wayland_unminimize_start
// no getter for globox_wayland_unminimize_finish
// no getter for globox_wayland_callback_xdg_surface_configure
// no getter for globox_wayland_callback_xdg_toplevel_configure
struct wl_display* globox_get_wayland_display(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_display;
}
struct wl_registry* globox_get_wayland_registry(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_registry;
}
struct wl_shm* globox_get_wayland_shm(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_shm;
}
struct wl_compositor* globox_get_wayland_compositor(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_compositor;
}
struct wl_output* globox_get_wayland_output(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_output;
}
struct wl_seat* globox_get_wayland_seat(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_seat;
}
struct xdg_wm_base* globox_get_wayland_xdg_wm_base(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_xdg_wm_base;
}
struct xdg_toplevel* globox_get_wayland_xdg_toplevel(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_xdg_toplevel;
}
struct xdg_surface* globox_get_wayland_xdg_surface(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_xdg_surface;
}
struct wl_surface* globox_get_wayland_surface(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_surface;
}
// no getter for globox_wayland_output_listener
// no getter for globox_wayland_registry_listener
// no getter for globox_wayland_xdg_wm_base_listener
// no getter for globox_wayland_xdg_toplevel_listener
// no getter for globox_wayland_xdg_surface_listener
struct zxdg_decoration_manager_v1* globox_get_wayland_xdg_decoration_manager(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_xdg_decoration_manager;
}
struct zxdg_toplevel_decoration_v1* globox_get_wayland_xdg_decoration(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_xdg_decoration;
}
struct zxdg_toplevel_decoration_v1_listener globox_get_wayland_xdg_decoration_listener(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_xdg_decoration_listener;
}
struct org_kde_kwin_blur_manager* globox_get_wayland_kde_blur_manager(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_kde_blur_manager;
}
struct org_kde_kwin_blur* globox_get_wayland_kde_blur(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_kde_blur;
}
uint32_t globox_get_wayland_saved_serial(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_saved_serial;
}

#if defined(GLOBOX_CONTEXT_SOFTWARE)
// no getter for globox_software_fd
struct wl_shm_pool* globox_get_wayland_software_pool(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_software.globox_software_pool;
}
struct wl_buffer* globox_get_wayland_software_buffer(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_software.globox_software_buffer;
}
// no getter for globox_software_buffer_len
#elif defined(GLOBOX_CONTEXT_EGL)
EGLDisplay globox_get_egl_display(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_egl.globox_egl_display;
}
EGLContext globox_get_egl_context(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_egl.globox_egl_context;
}
EGLSurface globox_get_egl_surface(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_egl.globox_egl_surface;
}
EGLConfig globox_get_egl_config(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_egl.globox_egl_config;
}
EGLint globox_get_egl_config_size(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_egl.globox_egl_config_size;
}
struct wl_egl_window* globox_get_wayland_egl_window(
	struct globox* globox)
{
	return globox->globox_platform->globox_wayland_egl.globox_egl_window;
}
#endif
#elif defined(GLOBOX_PLATFORM_X11)
// no getter for globox_x11_expose
// no getter for globox_x11_reserve
xcb_connection_t* globox_get_x11_conn(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_conn;
}
xcb_atom_t* globox_get_x11_atom_list(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_atom_list;
}
xcb_window_t globox_get_x11_win(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_win;
}
xcb_window_t globox_get_x11_root_win(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_root_win;
}
// no getter for globox_x11_interactive_x
// no getter for globox_x11_interactive_y
int globox_get_x11_screen_id(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_screen_id;
}
xcb_screen_t* globox_get_x11_screen_obj(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_screen_obj;
}
int globox_get_x11_visual_depth(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_visual_depth;
}
xcb_visualid_t globox_get_x11_visual_id(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_visual_id;
}
uint32_t globox_get_x11_attr_mask(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_attr_mask;
}
uint32_t* globox_get_x11_attr_val(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_attr_val;
}
// no getter for globox_get_x11_epoll
// no getter for globox_x11_epoll_event
// no getter for globox_x11_expose_queue

#if defined(GLOBOX_CONTEXT_SOFTWARE)
xcb_shm_segment_info_t globox_get_x11_software_shm(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_software.globox_software_shm;
}
xcb_gcontext_t globox_get_x11_software_gfx(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_software.globox_software_gfx;
}
xcb_pixmap_t globox_get_x11_software_pixmap(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_software.globox_software_pixmap;
}
// no getter for globox_software_pixmap_update
// no getter for globox_software_shared_pixmaps
// no getter for globox_software_buffer_len
#elif defined(GLOBOX_CONTEXT_EGL)
EGLDisplay globox_get_egl_display(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_egl.globox_egl_display;
}
EGLContext globox_get_egl_context(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_egl.globox_egl_context;
}
EGLSurface globox_get_egl_surface(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_egl.globox_egl_surface;
}
EGLConfig globox_get_egl_config(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_egl.globox_egl_config;
}
EGLint globox_get_egl_config_size(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_egl.globox_egl_config_size;
}
#elif defined(GLOBOX_CONTEXT_GLX)
Display* globox_get_x11_glx_display(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_glx.globox_glx_display;
}
GLXFBConfig globox_get_x11_glx_fb_config(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_glx.globox_glx_fb_config;
}
GLXContext globox_get_x11_glx_context(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_glx.globox_glx_context;
}
GLXWindow globox_get_x11_glx_win(
	struct globox* globox)
{
	return globox->globox_platform->globox_x11_glx.globox_glx_win;
}
// no getter for globox_glx_version_major
// no getter for globox_glx_version_minor
#endif
#elif defined(GLOBOX_PLATFORM_WINDOWS)
MSG globox_get_windows_msg(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_msg;
}
LPWSTR globox_get_windows_wide_title(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_wide_title;
}
LPWSTR globox_get_windows_class_name(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_class_name;
}
HMODULE globox_get_windows_class_module_handle(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_class_module_handle;
}
WNDCLASSEX globox_get_windows_class(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_class;
}
LONG globox_get_windows_style_backup(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_style_backup;
}
// no getter for globox_windows_exstyle_backup;
WINDOWPLACEMENT globox_get_windows_position_backup(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_position_backup;
}
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
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_software.globox_software_bmp_info;
}
HBITMAP globox_get_windows_software_bmp_handle(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_software.globox_software_bmp_handle;
}
#elif defined(GLOBOX_CONTEXT_EGL)
EGLDisplay globox_get_egl_display(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_egl.globox_egl_display;
}
EGLContext globox_get_egl_context(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_egl.globox_egl_context;
}
EGLSurface globox_get_egl_surface(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_egl.globox_egl_surface;
}
EGLConfig globox_get_egl_config(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_egl.globox_egl_config;
}
EGLint globox_get_egl_config_size(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_egl.globox_egl_config_size;
}
#elif defined(GLOBOX_CONTEXT_WGL)
// no getter for globox_wgl_version_major
// no getter for globox_wgl_version_minor
PIXELFORMATDESCRIPTOR globox_get_windows_wgl_pfd(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_wgl.globox_wgl_pfd;
}
HGLRC globox_get_windows_wgl_context(
	struct globox* globox)
{
	return globox->globox_platform->globox_windows_wgl.globox_wgl_context;
}
#endif
#elif defined(GLOBOX_PLATFORM_MACOS)
Class globox_get_macos_class_view(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_class_view;
}
Class globox_get_macos_class_appdelegate(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_class_appdelegate;
}
// no getter for globox_get_macos_class_cursor
id globox_get_macos_obj_view(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_obj_view;
}
id globox_get_macos_obj_appdelegate(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_obj_appdelegate;
}
// no getter for globox_get_macos_obj_cursor_a
// no getter for globox_get_macos_obj_cursor_b
id globox_get_macos_obj_window(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_obj_window;
}
id globox_get_macos_obj_masterview(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_obj_masterview;
}
id globox_get_macos_obj_blur(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_obj_blur;
}
struct macos_size globox_get_macos_window_min_size(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_window_min_size;
}
struct macos_rect* globox_get_macos_buttons(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_buttons;
}
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
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_software.globox_software_layer;
}
#elif defined(GLOBOX_CONTEXT_EGL)
EGLDisplay globox_get_egl_display(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_egl.globox_egl_display;
}
EGLContext globox_get_egl_context(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_egl.globox_egl_context;
}
EGLSurface globox_get_egl_surface(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_egl.globox_egl_surface;
}
EGLConfig globox_get_egl_config(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_egl.globox_egl_config;
}
EGLint globox_get_egl_config_size(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_egl.globox_egl_config_size;
}
id globox_get_macos_egl_layer(
	struct globox* globox)
{
	return globox->globox_platform->globox_macos_egl.globox_egl_layer;
}
#endif
#endif
