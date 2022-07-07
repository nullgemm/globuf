#ifndef H_GLOBOX
#define H_GLOBOX

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// # types
struct globox;

enum globox_feature
{
	GLOBOX_FEATURE_INTERACTION = 0,
	GLOBOX_FEATURE_STATE,
	GLOBOX_FEATURE_TITLE,
	GLOBOX_FEATURE_ICON,
	GLOBOX_FEATURE_INIT_SIZE,
	GLOBOX_FEATURE_INIT_POS,
	GLOBOX_FEATURE_FRAMED,
	GLOBOX_FEATURE_BACKGROUND,
	GLOBOX_FEATURE_VSYNC_CALLBACK,
};

enum globox_interaction
{
	GLOBOX_INTERACTION_STOP = 0,
	GLOBOX_INTERACTION_MOVE,
	GLOBOX_INTERACTION_N,  // North
	GLOBOX_INTERACTION_NW, // North-West
	GLOBOX_INTERACTION_W,  // West
	GLOBOX_INTERACTION_SW, // South-West
	GLOBOX_INTERACTION_S,  // South
	GLOBOX_INTERACTION_SE, // South-East
	GLOBOX_INTERACTION_E,  // East
	GLOBOX_INTERACTION_NE, // North-East
};

enum globox_state
{
	GLOBOX_STATE_REGULAR = 0,
	GLOBOX_STATE_MINIMIZED,
	GLOBOX_STATE_MAXIMIZED,
	GLOBOX_STATE_FULLSCREEN,
	GLOBOX_STATE_CLOSED,
};

enum globox_background
{
	GLOBOX_BACKGROUND_OPAQUE = 0,
	GLOBOX_BACKGROUND_BLURRED,
	GLOBOX_BACKGROUND_TRANSPARENT,
};

enum globox_error
{
	// generic
	GLOBOX_ERROR_OK = 0,
	GLOBOX_ERROR_NULL,
	GLOBOX_ERROR_ALLOC,
	GLOBOX_ERROR_BOUNDS,
	GLOBOX_ERROR_DOMAIN,
	GLOBOX_ERROR_FD,
	// wayland
	GLOBOX_ERROR_WAYLAND_EPOLL_CREATE = 500,
	GLOBOX_ERROR_WAYLAND_EPOLL_CTL,
	GLOBOX_ERROR_WAYLAND_EPOLL_WAIT,
	GLOBOX_ERROR_WAYLAND_DISPLAY,
	GLOBOX_ERROR_WAYLAND_ROUNDTRIP,
	GLOBOX_ERROR_WAYLAND_FLUSH,
	GLOBOX_ERROR_WAYLAND_DISPATCH,
	GLOBOX_ERROR_WAYLAND_MMAP,
	GLOBOX_ERROR_WAYLAND_MUNMAP,
	GLOBOX_ERROR_WAYLAND_REQUEST,
	GLOBOX_ERROR_WAYLAND_LISTENER,
	GLOBOX_ERROR_WAYLAND_EGL_FAIL,
	// x11
	GLOBOX_ERROR_X11_CONN = 1000,
	GLOBOX_ERROR_X11_FLUSH,
	GLOBOX_ERROR_X11_WIN,
	GLOBOX_ERROR_X11_MAP,
	GLOBOX_ERROR_X11_GC,
	GLOBOX_ERROR_X11_VISUAL_NOT_COMPATIBLE,
	GLOBOX_ERROR_X11_VISUAL_NOT_FOUND,
	GLOBOX_ERROR_X11_PIXMAP,
	GLOBOX_ERROR_X11_SHM_VERSION_REPLY,
	GLOBOX_ERROR_X11_SHMID,
	GLOBOX_ERROR_X11_SHMADDR,
	GLOBOX_ERROR_X11_SHM_ATTACH,
	GLOBOX_ERROR_X11_SHMCTL,
	GLOBOX_ERROR_X11_SHM_PIXMAP,
	GLOBOX_ERROR_X11_SHM_DETACH,
	GLOBOX_ERROR_X11_SHMDT,
	GLOBOX_ERROR_X11_EPOLL_CREATE,
	GLOBOX_ERROR_X11_EPOLL_CTL,
	GLOBOX_ERROR_X11_EPOLL_WAIT,
	GLOBOX_ERROR_X11_WIN_ATTR,
	GLOBOX_ERROR_X11_ATOMS,
	GLOBOX_ERROR_X11_ICON,
	GLOBOX_ERROR_X11_TITLE,
	GLOBOX_ERROR_X11_IMAGE,
	GLOBOX_ERROR_X11_COPY,
	GLOBOX_ERROR_X11_SCREEN_INFO,
	GLOBOX_ERROR_X11_WIN_INFO,
	GLOBOX_ERROR_X11_STATE,
	GLOBOX_ERROR_X11_INTERACTIVE,
	GLOBOX_ERROR_X11_EGL_FAIL,
	// windows
	GLOBOX_ERROR_WINDOWS_SYM = 1500,
	GLOBOX_ERROR_WINDOWS_MODULE_APP,
	GLOBOX_ERROR_WINDOWS_MODULE_USER32,
	GLOBOX_ERROR_WINDOWS_DELETE,
	GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET,
	GLOBOX_ERROR_WINDOWS_MESSAGE_GET,
	GLOBOX_ERROR_WINDOWS_CLASS_REGISTER,
	GLOBOX_ERROR_WINDOWS_WINDOW_CREATE,
	GLOBOX_ERROR_WINDOWS_WINDOW_ADJUST,
	GLOBOX_ERROR_WINDOWS_WINDOW_UPDATE,
	GLOBOX_ERROR_WINDOWS_WINDOW_RECT_GET,
	GLOBOX_ERROR_WINDOWS_WINDOW_POS_SET,
	GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET,
	GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET,
	GLOBOX_ERROR_WINDOWS_PLACEMENT_GET,
	GLOBOX_ERROR_WINDOWS_PLACEMENT_SET,
	GLOBOX_ERROR_WINDOWS_CLIENT_RECT,
	GLOBOX_ERROR_WINDOWS_CLIENT_POS,
	GLOBOX_ERROR_WINDOWS_TRANSPARENCY_REGION,
	GLOBOX_ERROR_WINDOWS_TRANSPARENCY_DWM,
	GLOBOX_ERROR_WINDOWS_COMP_ATTR,
	GLOBOX_ERROR_WINDOWS_INPUT_SEND,
	GLOBOX_ERROR_WINDOWS_CAPTURE_RELEASE,
	GLOBOX_ERROR_WINDOWS_CURSOR_LOAD,
	GLOBOX_ERROR_WINDOWS_CURSOR_POS_GET,
	GLOBOX_ERROR_WINDOWS_CURSOR_POS_SET,
	GLOBOX_ERROR_WINDOWS_BMP_MASK_CREATE,
	GLOBOX_ERROR_WINDOWS_BMP_COLOR_CREATE,
	GLOBOX_ERROR_WINDOWS_ICON_CREATE,
	GLOBOX_ERROR_WINDOWS_ICON_SMALL,
	GLOBOX_ERROR_WINDOWS_ICON_BIG,
	GLOBOX_ERROR_WINDOWS_UTF8,
	GLOBOX_ERROR_WINDOWS_TITLE,
	GLOBOX_ERROR_WINDOWS_GDI_DAMAGE,
	GLOBOX_ERROR_WINDOWS_GDI_PAINT,
	GLOBOX_ERROR_WINDOWS_GDI_BITBLT,
	GLOBOX_ERROR_WINDOWS_GDI_DIB_CREATE,
	GLOBOX_ERROR_WINDOWS_GDI_BITMAP_SELECT,
	GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_CREATE,
	GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_DELETE,
	GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE,
	GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET,
	GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE,
	GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_DELETE,
	GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET,
	GLOBOX_ERROR_WINDOWS_WGL_SWAP,
	// macos
	GLOBOX_ERROR_MACOS_CLASS_GET = 2000,
	GLOBOX_ERROR_MACOS_CLASS_ALLOC,
	GLOBOX_ERROR_MACOS_CLASS_ADDVAR,
	GLOBOX_ERROR_MACOS_CLASS_ADDMETHOD,
	GLOBOX_ERROR_MACOS_OBJ_INIT,
	GLOBOX_ERROR_MACOS_OBJ_NIL,
	GLOBOX_ERROR_MACOS_EGL_FAIL,
};

// # cross-platform, cross-backend
// ## lifecycle (N.B.: the event loop is always started on a separate thread)
// allocate base resources and make initial checks
void globox_init(
	struct globox* context);
// free base resources
void globox_clean(
	struct globox* context);

// create the window without displaying it and call all window feature callbacks
void globox_window_create(
	struct globox* context);
// destroy the window object entirely
void globox_window_destroy(
	struct globox* context);

// start displaying the window and running the loop
void globox_window_start(
	struct globox* context);
// close the window if still open and stop the loop
void globox_window_stop(
	struct globox* context);

// ## errors
bool globox_error_catch(
	struct globox* context);
void globox_error_reset(
	struct globox* context);
void globox_error_log(
	struct globox* context);
const char* globox_error_get_message(
	struct globox* context);
enum globox_error globox_error_get_code(
	struct globox* context);

// ## configuration (can always be called)
// backend callbacks
struct globox_config_backend
{
	void* data;
	// function pointers for each cross-platform globox call
	// lifecycle
	void (*init)(
		struct globox* context);
	void (*clean)(
		struct globox* context);
	void (*window_create)(
		struct globox* context);
	void (*window_destroy)(
		struct globox* context);
	void (*window_start)(
		struct globox* context);
	void (*window_stop)(
		struct globox* context);
	// feature registry
	void (*init_features)(
		struct globox* context,
		struct globox_config_features* config);
	// event handlers
	void (*init_events)(
		struct globox* context,
		struct globox_config_events* config);
	// features
	void (*set_interaction)(
		struct globox* context,
		struct globox_feature_interaction* config);
	void (*set_state)(
		struct globox* context,
		struct globox_feature_state* config);
	void globox_set_title(
		struct globox* context,
		struct globox_feature_title* config);
	void globox_set_icon(
		struct globox* context,
		struct globox_feature_icon* config);
	void globox_set_init_size(
		struct globox* context,
		struct globox_feature_init_size* config);
	void globox_set_init_pos(
		struct globox* context,
		struct globox_feature_init_pos* config);
	void globox_set_frame(
		struct globox* context,
		struct globox_feature_frame* config);
	void globox_set_background(
		struct globox* context,
		struct globox_feature_background* config);
	void globox_set_vsync_callback(
		struct globox* context,
		struct globox_feature_vsync_callback* config);
	void globox_update_content(
		struct globox* context,
		void* data);
};

void globox_init_backend(
	struct globox* context,
	struct globox_config_backend* config);

// feature registry
struct globox_config_features
{
	void* data;
	void (*registry)(
		void* data,
		void* context,
		enum globox_feature feature);
};

void globox_init_features(
	struct globox* context,
	struct globox_config_features* config);

// event handlers
struct globox_config_events
{
	size_t event_handler_count;
	void** event_handler_data;
	void (**event_handler)(
		void* data,
		void* event);
};

void globox_init_events(
	struct globox* context,
	struct globox_config_events* config);

// ## features (can only be called if confirmed in the registry callback)
// interaction
struct globox_feature_interaction
{
	enum globox_interaction action;
};

void globox_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config);

// state
struct globox_feature_state
{
	enum globox_state state;
};

void globox_set_state(
	struct globox* context,
	struct globox_feature_state* config);

// title
struct globox_feature_title
{
	const char* title;
};

void globox_set_title(
	struct globox* context,
	struct globox_feature_title* config);

// icon
struct globox_feature_icon
{
	uint32_t* pixmap;
	uint32_t len;
};

void globox_set_icon(
	struct globox* context,
	struct globox_feature_icon* config);

// init size
struct globox_feature_init_size
{
	unsigned width_init;
	unsigned height_init;
};

void globox_set_init_size(
	struct globox* context,
	struct globox_feature_init_size* config);

// init pos
struct globox_feature_init_pos
{
	int x_init;
	int y_init;
};

void globox_set_init_pos(
	struct globox* context,
	struct globox_feature_init_pos* config);

// frame
struct globox_feature_frame
{
	bool frame;
	void* data;
	void (*callback)(void* data, bool frame);
};

void globox_set_frame(
	struct globox* context,
	struct globox_feature_frame* config);

// background
struct globox_feature_background
{
	enum globox_background background;
	void* data;
	void (*callback)(void* data, enum globox_background background);
};

void globox_set_background(
	struct globox* context,
	struct globox_feature_background* config);

// vsync callback
struct globox_feature_vsync_callback
{
	void* data;
	void (*callback)(void* data);
};

void globox_set_vsync_callback(
	struct globox* context,
	struct globox_feature_vsync_callback* config);

// # content updater (backend-specific but still cross-platform)
void globox_update_content(
	struct globox* context,
	void* data);

#endif
