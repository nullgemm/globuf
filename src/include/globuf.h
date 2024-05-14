#ifndef H_GLOBUF
#define H_GLOBUF

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// # types
// ## general types
struct globuf;

enum globuf_feature
{
	GLOBUF_FEATURE_INTERACTION = 0,
	GLOBUF_FEATURE_STATE,
	GLOBUF_FEATURE_TITLE,
	GLOBUF_FEATURE_ICON,
	GLOBUF_FEATURE_SIZE,
	GLOBUF_FEATURE_POS,
	GLOBUF_FEATURE_FRAME,
	GLOBUF_FEATURE_BACKGROUND,
	GLOBUF_FEATURE_VSYNC,
	// special value used to get the total number of features
	GLOBUF_FEATURE_COUNT,
};

// didn't necessarily happen programmatically
enum globuf_event
{
	// special value used when errors occur while processing events
	GLOBUF_EVENT_INVALID = 0,
	GLOBUF_EVENT_UNKNOWN,
	// window state updates
	GLOBUF_EVENT_RESTORED,
	GLOBUF_EVENT_MINIMIZED,
	GLOBUF_EVENT_MAXIMIZED,
	GLOBUF_EVENT_FULLSCREEN,
	GLOBUF_EVENT_CLOSED,
	// window size & position updates
	GLOBUF_EVENT_MOVED_RESIZED,
	GLOBUF_EVENT_DAMAGED,
};

enum globuf_interaction
{
	GLOBUF_INTERACTION_STOP = 0,
	GLOBUF_INTERACTION_MOVE,
	GLOBUF_INTERACTION_N,  // North
	GLOBUF_INTERACTION_NW, // North-West
	GLOBUF_INTERACTION_W,  // West
	GLOBUF_INTERACTION_SW, // South-West
	GLOBUF_INTERACTION_S,  // South
	GLOBUF_INTERACTION_SE, // South-East
	GLOBUF_INTERACTION_E,  // East
	GLOBUF_INTERACTION_NE, // North-East
};

enum globuf_state
{
	GLOBUF_STATE_REGULAR = 0,
	GLOBUF_STATE_MINIMIZED,
	GLOBUF_STATE_MAXIMIZED,
	GLOBUF_STATE_FULLSCREEN,
	GLOBUF_STATE_CLOSED,
};

enum globuf_background
{
	GLOBUF_BACKGROUND_OPAQUE = 0,
	GLOBUF_BACKGROUND_BLURRED,
	GLOBUF_BACKGROUND_TRANSPARENT,
};

enum globuf_error
{
	// generic
	GLOBUF_ERROR_OK = 0,
	GLOBUF_ERROR_NULL,
	GLOBUF_ERROR_ALLOC,
	GLOBUF_ERROR_BOUNDS,
	GLOBUF_ERROR_DOMAIN,
	GLOBUF_ERROR_FD,
	GLOBUF_ERROR_FEATURE_INVALID,
	GLOBUF_ERROR_FEATURE_UNAVAILABLE,
	GLOBUF_ERROR_FEATURE_STATE_INVALID,
	// posix
	GLOBUF_ERROR_POSIX_MUTEX_INIT,
	GLOBUF_ERROR_POSIX_MUTEX_DESTROY,
	GLOBUF_ERROR_POSIX_MUTEX_ATTR_INIT,
	GLOBUF_ERROR_POSIX_MUTEX_ATTR_DESTROY,
	GLOBUF_ERROR_POSIX_MUTEX_ATTR_SETTYPE,
	GLOBUF_ERROR_POSIX_MUTEX_LOCK,
	GLOBUF_ERROR_POSIX_MUTEX_UNLOCK,
	GLOBUF_ERROR_POSIX_COND_INIT,
	GLOBUF_ERROR_POSIX_COND_WAIT,
	GLOBUF_ERROR_POSIX_COND_DESTROY,
	GLOBUF_ERROR_POSIX_COND_ATTR_INIT,
	GLOBUF_ERROR_POSIX_COND_ATTR_DESTROY,
	GLOBUF_ERROR_POSIX_COND_ATTR_SETCLOCK,
	GLOBUF_ERROR_POSIX_THREAD_CREATE,
	GLOBUF_ERROR_POSIX_THREAD_ATTR_INIT,
	GLOBUF_ERROR_POSIX_THREAD_ATTR_DESTROY,
	GLOBUF_ERROR_POSIX_THREAD_ATTR_JOINABLE,
	GLOBUF_ERROR_POSIX_THREAD_JOIN,
	GLOBUF_ERROR_POSIX_SHM_VERSION,
	GLOBUF_ERROR_POSIX_SHMDT,
	GLOBUF_ERROR_POSIX_SHMID,
	GLOBUF_ERROR_POSIX_SHMCTL,
	GLOBUF_ERROR_POSIX_SHMADDR,
	// wayland
	GLOBUF_ERROR_WAYLAND_DISPLAY_GET,
	GLOBUF_ERROR_WAYLAND_REGISTRY_GET,
	GLOBUF_ERROR_WAYLAND_LISTENER_ADD,
	GLOBUF_ERROR_WAYLAND_ROUNDTRIP,
	GLOBUF_ERROR_WAYLAND_POINTER_GET,
	GLOBUF_ERROR_WAYLAND_SURFACE_CREATE,
	GLOBUF_ERROR_WAYLAND_XDG_SURFACE_CREATE,
	GLOBUF_ERROR_WAYLAND_XDG_TOPLEVEL_GET,
	GLOBUF_ERROR_WAYLAND_XDG_DECORATION_GET,
	GLOBUF_ERROR_WAYLAND_SURFACE_FRAME_GET,
	GLOBUF_ERROR_WAYLAND_SHM_GET,
	GLOBUF_ERROR_WAYLAND_REQUEST,
	GLOBUF_ERROR_WAYLAND_COMPOSITOR_MISSING,
	GLOBUF_ERROR_WAYLAND_XDG_WM_BASE_MISSSING,
	GLOBUF_ERROR_WAYLAND_XDG_DECORATION_MANAGER_MISSSING,
	GLOBUF_ERROR_WAYLAND_MMAP,
	GLOBUF_ERROR_WAYLAND_MUNMAP,
	GLOBUF_ERROR_WAYLAND_REGISTRY_CALLBACK,
	GLOBUF_ERROR_WAYLAND_EVENT_WAIT,
	GLOBUF_ERROR_WAYLAND_ICON,
	GLOBUF_ERROR_WAYLAND_BACKGROUND_BLUR,
	GLOBUF_ERROR_WAYLAND_DECORATIONS_FORCED,
	GLOBUF_ERROR_WAYLAND_DECORATIONS_UNAVAILABLE,
	GLOBUF_ERROR_WAYLAND_EGL_MAKE_CURRENT,
	GLOBUF_ERROR_WAYLAND_EGL_SWAP_INTERVAL,
	GLOBUF_ERROR_WAYLAND_EGL_DISPLAY_GET,
	GLOBUF_ERROR_WAYLAND_EGL_INIT,
	GLOBUF_ERROR_WAYLAND_EGL_BIND_API,
	GLOBUF_ERROR_WAYLAND_EGL_CONFIG,
	GLOBUF_ERROR_WAYLAND_EGL_CONTEXT_CREATE,
	GLOBUF_ERROR_WAYLAND_EGL_DESTROY_SURFACE,
	GLOBUF_ERROR_WAYLAND_EGL_DESTROY_CONTEXT,
	GLOBUF_ERROR_WAYLAND_EGL_TERMINATE,
	GLOBUF_ERROR_WAYLAND_EGL_WINDOW_CREATE,
	GLOBUF_ERROR_WAYLAND_EGL_WINDOW_SURFACE,
	GLOBUF_ERROR_WAYLAND_EGL_SWAP,
	GLOBUF_ERROR_WAYLAND_VULKAN_EXTENSIONS_LIST,
	GLOBUF_ERROR_WAYLAND_VULKAN_EXTENSION_UNAVAILABLE,
	GLOBUF_ERROR_WAYLAND_VULKAN_SURFACE_CREATE,
	// x11
	GLOBUF_ERROR_X11_CONN,
	GLOBUF_ERROR_X11_FLUSH,
	GLOBUF_ERROR_X11_ATOM_GET,
	GLOBUF_ERROR_X11_WIN_CREATE,
	GLOBUF_ERROR_X11_WIN_DESTROY,
	GLOBUF_ERROR_X11_WIN_MAP,
	GLOBUF_ERROR_X11_WIN_UNMAP,
	GLOBUF_ERROR_X11_EVENT_WAIT,
	GLOBUF_ERROR_X11_EVENT_SEND,
	GLOBUF_ERROR_X11_PROP_CHANGE,
	GLOBUF_ERROR_X11_PROP_GET,
	GLOBUF_ERROR_X11_PROP_VALUE_GET,
	GLOBUF_ERROR_X11_ATTR_CHANGE,
	GLOBUF_ERROR_X11_GC_CREATE,
	GLOBUF_ERROR_X11_PIXMAP,
	GLOBUF_ERROR_X11_SHM_PIXMAP,
	GLOBUF_ERROR_X11_VISUAL_INCOMPATIBLE,
	GLOBUF_ERROR_X11_VISUAL_MISSING,
	GLOBUF_ERROR_X11_SHM_ATTACH,
	GLOBUF_ERROR_X11_SHM_DETACH,
	GLOBUF_ERROR_X11_IMAGE,
	GLOBUF_ERROR_X11_COPY,
	GLOBUF_ERROR_X11_EVENT_INVALID,
	GLOBUF_ERROR_X11_SYNC_COUNTER_CREATE,
	GLOBUF_ERROR_X11_SYNC_COUNTER_SET,
	GLOBUF_ERROR_X11_SYNC_COUNTER_DESTROY,
	GLOBUF_ERROR_X11_GEOMETRY_GET,
	GLOBUF_ERROR_X11_TRANSLATE_COORDS,
	GLOBUF_ERROR_X11_CONFIGURE,
	GLOBUF_ERROR_X11_QUERY_POINTER,
	GLOBUF_ERROR_X11_VSYNC,
	GLOBUF_ERROR_X11_GLX,
	GLOBUF_ERROR_X11_GLX_FB_CONF_LIST,
	GLOBUF_ERROR_X11_GLX_FB_INVALID,
	GLOBUF_ERROR_X11_GLX_FB_CONF_ATTR,
	GLOBUF_ERROR_X11_GLX_ATTR_ARB,
	GLOBUF_ERROR_X11_GLX_CONTEXT,
	GLOBUF_ERROR_X11_GLX_WINDOW,
	GLOBUF_ERROR_X11_GLX_MAKE_CURRENT,
	GLOBUF_ERROR_X11_GLX_DISPLAY_OPEN,
	GLOBUF_ERROR_X11_GLX_VSYNC,
	GLOBUF_ERROR_X11_EGL_SWAP,
	GLOBUF_ERROR_X11_EGL_DESTROY_SURFACE,
	GLOBUF_ERROR_X11_EGL_DESTROY_CONTEXT,
	GLOBUF_ERROR_X11_EGL_TERMINATE,
	GLOBUF_ERROR_X11_EGL_DISPLAY_GET,
	GLOBUF_ERROR_X11_EGL_INIT,
	GLOBUF_ERROR_X11_EGL_BIND_API,
	GLOBUF_ERROR_X11_EGL_CONFIG,
	GLOBUF_ERROR_X11_EGL_CONTEXT_CREATE,
	GLOBUF_ERROR_X11_EGL_CONFIG_ATTR,
	GLOBUF_ERROR_X11_EGL_WINDOW_SURFACE,
	GLOBUF_ERROR_X11_EGL_MAKE_CURRENT,
	GLOBUF_ERROR_X11_EGL_SWAP_INTERVAL,
	GLOBUF_ERROR_X11_VULKAN_SURFACE_CREATE,
	GLOBUF_ERROR_X11_VULKAN_EXTENSIONS_LIST,
	GLOBUF_ERROR_X11_VULKAN_EXTENSION_UNAVAILABLE,
	// windows
	GLOBUF_ERROR_WIN_MUTEX_CREATE,
	GLOBUF_ERROR_WIN_MUTEX_DESTROY,
	GLOBUF_ERROR_WIN_MUTEX_LOCK,
	GLOBUF_ERROR_WIN_MUTEX_UNLOCK,
	GLOBUF_ERROR_WIN_MODULE_GET,
	GLOBUF_ERROR_WIN_CLASS_CREATE,
	GLOBUF_ERROR_WIN_NAME_SET,
	GLOBUF_ERROR_WIN_COND_WAIT,
	GLOBUF_ERROR_WIN_THREAD_WAIT,
	GLOBUF_ERROR_WIN_THREAD_EVENT_START,
	GLOBUF_ERROR_WIN_THREAD_EVENT_CLOSE,
	GLOBUF_ERROR_WIN_THREAD_RENDER_START,
	GLOBUF_ERROR_WIN_THREAD_RENDER_CLOSE,
	GLOBUF_ERROR_WIN_CURSOR_LOAD,
	GLOBUF_ERROR_WIN_PAINT_VALIDATE,
	GLOBUF_ERROR_WIN_MONITOR_GET,
	GLOBUF_ERROR_WIN_MONITOR_INFO_GET,
	GLOBUF_ERROR_WIN_WINDOW_POSITION_SET,
	GLOBUF_ERROR_WIN_PLACEMENT_GET,
	GLOBUF_ERROR_WIN_PLACEMENT_SET,
	GLOBUF_ERROR_WIN_STYLE_SET,
	GLOBUF_ERROR_WIN_INTERACTION_SET,
	GLOBUF_ERROR_WIN_STATE_SET,
	GLOBUF_ERROR_WIN_TITLE_SET,
	GLOBUF_ERROR_WIN_WINDOW_CREATE,
	GLOBUF_ERROR_WIN_USERDATA_SET,
	GLOBUF_ERROR_WIN_BMP_MASK_CREATE,
	GLOBUF_ERROR_WIN_BMP_COLOR_CREATE,
	GLOBUF_ERROR_WIN_ICON_CREATE,
	GLOBUF_ERROR_WIN_ICON_SMALL,
	GLOBUF_ERROR_WIN_ICON_BIG,
	GLOBUF_ERROR_WIN_MSG_GET,
	GLOBUF_ERROR_WIN_DEVICE_CONTEXT_GET,
	GLOBUF_ERROR_WIN_DEVICE_CONTEXT_CREATE,
	GLOBUF_ERROR_WIN_DEVICE_CONTEXT_DELETE,
	GLOBUF_ERROR_WIN_OBJECT_DELETE,
	GLOBUF_ERROR_WIN_DWM_ENABLE,
	GLOBUF_ERROR_WIN_BMP_GET,
	GLOBUF_ERROR_WIN_BMP_CREATE,
	GLOBUF_ERROR_WIN_DIB_CREATE,
	GLOBUF_ERROR_WIN_BACKGROUND_BLUR,
	GLOBUF_ERROR_WIN_GDI_PAINT,
	GLOBUF_ERROR_WIN_GDI_DAMAGE,
	GLOBUF_ERROR_WIN_GDI_BITBLT,
	GLOBUF_ERROR_WIN_PIXEL_FORMAT_SET,
	GLOBUF_ERROR_WIN_PIXEL_FORMAT_CHOOSE,
	GLOBUF_ERROR_WIN_WGL_CONTEXT_SET,
	GLOBUF_ERROR_WIN_WGL_CONTEXT_CREATE,
	GLOBUF_ERROR_WIN_WGL_CONTEXT_DESTROY,
	GLOBUF_ERROR_WIN_WGL_SWAP,
	GLOBUF_ERROR_WIN_WGL_FUNC_LOAD,
	GLOBUF_ERROR_WIN_VULKAN_EXTENSIONS_LIST,
	GLOBUF_ERROR_WIN_VULKAN_EXTENSION_UNAVAILABLE,
	GLOBUF_ERROR_WIN_VULKAN_SURFACE_CREATE,
	// macos
	GLOBUF_ERROR_MACOS_OBJ_NIL,
	GLOBUF_ERROR_MACOS_VULKAN_SURFACE_CREATE,
	GLOBUF_ERROR_MACOS_VULKAN_EXTENSIONS_LIST,
	GLOBUF_ERROR_MACOS_VULKAN_EXTENSION_UNAVAILABLE,
	GLOBUF_ERROR_MACOS_EGL_SWAP,
	GLOBUF_ERROR_MACOS_EGL_DESTROY_SURFACE,
	GLOBUF_ERROR_MACOS_EGL_DESTROY_CONTEXT,
	GLOBUF_ERROR_MACOS_EGL_TERMINATE,
	GLOBUF_ERROR_MACOS_EGL_DISPLAY_GET,
	GLOBUF_ERROR_MACOS_EGL_INIT,
	GLOBUF_ERROR_MACOS_EGL_BIND_API,
	GLOBUF_ERROR_MACOS_EGL_CONFIG,
	GLOBUF_ERROR_MACOS_EGL_CONTEXT_CREATE,
	GLOBUF_ERROR_MACOS_EGL_CONFIG_ATTR,
	GLOBUF_ERROR_MACOS_EGL_WINDOW_SURFACE,
	GLOBUF_ERROR_MACOS_EGL_MAKE_CURRENT,
	GLOBUF_ERROR_MACOS_EGL_SWAP_INTERVAL,
	// special
	GLOBUF_ERROR_COUNT,
};

struct globuf_error_info
{
	enum globuf_error code;
	const char* file;
	unsigned line;
};

struct globuf_rect
{
	int x;
	int y;
	unsigned width;
	unsigned height;
};

// ## config types
struct globuf_config_render
{
	void* data;
	void (*callback)(void* data);
};

struct globuf_config_events
{
	void* data;
	void (*handler)(void* data, void* event);
};

struct globuf_config_features
{
	enum globuf_feature* list;
	size_t count;
};

struct globuf_config_request
{
	enum globuf_feature feature;
	void* config;
};

struct globuf_config_reply
{
	enum globuf_feature feature;
	struct globuf_error_info error;
};

// ## feature types
struct globuf_feature_interaction
{
	enum globuf_interaction action;
};

struct globuf_feature_state
{
	enum globuf_state state;
};

struct globuf_feature_title
{
	const char* title;
};

struct globuf_feature_icon
{
	uint32_t* pixmap;
	uint32_t len;
};

struct globuf_feature_size
{
	unsigned width;
	unsigned height;
};

struct globuf_feature_pos
{
	int x;
	int y;
};

struct globuf_feature_frame
{
	bool frame;
};

struct globuf_feature_background
{
	enum globuf_background background;
};

struct globuf_feature_vsync
{
	bool vsync;
};

// ## backend configuration structure
// depends on most of the above
struct globuf_config_backend
{
	// custom data for initialization
	void* data;
	// custom call for internal use
	void* (*callback)(
		struct globuf* context);
	// function pointers for each cross-platform globuf call
	// lifecycle
	void (*init)(
		struct globuf* context,
		struct globuf_error_info* error);
	void (*clean)(
		struct globuf* context,
		struct globuf_error_info* error);
	void (*window_create)(
		struct globuf* context,
		struct globuf_config_request* configs,
		size_t count,
		void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
		void* data,
		struct globuf_error_info* error);
	void (*window_destroy)(
		struct globuf* context,
		struct globuf_error_info* error);
	void (*window_confirm)(
		struct globuf* context,
		struct globuf_error_info* error);
	void (*window_start)(
		struct globuf* context,
		struct globuf_error_info* error);
	void (*window_block)(
		struct globuf* context,
		struct globuf_error_info* error);
	void (*window_stop)(
		struct globuf* context,
		struct globuf_error_info* error);
	// render callback
	void (*init_render)(
		struct globuf* context,
		struct globuf_config_render* config,
		struct globuf_error_info* error);
	// event handler
	void (*init_events)(
		struct globuf* context,
		struct globuf_config_events* config,
		struct globuf_error_info* error);
	enum globuf_event (*handle_events)(
		struct globuf* context,
		void* event,
		struct globuf_error_info* error);
	// feature registry
	struct globuf_config_features* (*init_features)(
		struct globuf* context,
		struct globuf_error_info* error);
	// features
	void (*feature_set_interaction)(
		struct globuf* context,
		struct globuf_feature_interaction* config,
		struct globuf_error_info* error);
	void (*feature_set_state)(
		struct globuf* context,
		struct globuf_feature_state* config,
		struct globuf_error_info* error);
	void (*feature_set_title)(
		struct globuf* context,
		struct globuf_feature_title* config,
		struct globuf_error_info* error);
	void (*feature_set_icon)(
		struct globuf* context,
		struct globuf_feature_icon* config,
		struct globuf_error_info* error);
	// getters
	unsigned (*get_width)(
		struct globuf* context,
		struct globuf_error_info* error);
	unsigned (*get_height)(
		struct globuf* context,
		struct globuf_error_info* error);
	struct globuf_rect (*get_expose)(
		struct globuf* context,
		struct globuf_error_info* error);
	// content update function
	void (*update_content)(
		struct globuf* context,
		void* data,
		struct globuf_error_info* error);
};

// # cross-platform, cross-backend
// ## lifecycle (N.B.: the event loop is always started on a separate thread)
// allocate base resources and make initial checks
struct globuf* globuf_init(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
// free base resources
void globuf_clean(
	struct globuf* context,
	struct globuf_error_info* error);

// create the window without displaying it and call all window feature callbacks
void globuf_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error);
// destroy the window object entirely
void globuf_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error);

// finish setting up the window
void globuf_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error);
// start displaying the window and running the loop
void globuf_window_start(
	struct globuf* context,
	struct globuf_error_info* error);
// block the caller thread until the window has been closed
void globuf_window_block(
	struct globuf* context,
	struct globuf_error_info* error);
// close the window if still open and stop the loop
void globuf_window_stop(
	struct globuf* context,
	struct globuf_error_info* error);

// ## configuration (can always be called)
// render callback
void globuf_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error);

// event handler
void globuf_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error);

// window event handler helper provided by globuf
// (this is a very special case, other event handlers shouldn't be as simple!)
enum globuf_event globuf_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error);

// ## features (can only be called if confirmed in the registry callback)
// feature registry
struct globuf_config_features* globuf_init_features(
	struct globuf* context,
	struct globuf_error_info* error);

// feature setters
void globuf_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error);

void globuf_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error);

void globuf_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error);

void globuf_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error);

// ## getters
unsigned globuf_get_width(
	struct globuf* context,
	struct globuf_error_info* error);

unsigned globuf_get_height(
	struct globuf* context,
	struct globuf_error_info* error);

struct globuf_rect globuf_get_expose(
	struct globuf* context,
	struct globuf_error_info* error);

// ## content updater (backend-specific but still cross-platform)
void globuf_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error);

// ## errors
void globuf_error_log(
	struct globuf* context,
	struct globuf_error_info* error);
const char* globuf_error_get_msg(
	struct globuf* context,
	struct globuf_error_info* error);
enum globuf_error globuf_error_get_code(
	struct globuf_error_info* error);
const char* globuf_error_get_file(
	struct globuf_error_info* error);
unsigned globuf_error_get_line(
	struct globuf_error_info* error);
void globuf_error_ok(
	struct globuf_error_info* error);

#endif
