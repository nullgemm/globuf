#ifndef H_GLOBOX
#define H_GLOBOX

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// # types
// ## general types
struct globox;

enum globox_feature
{
	GLOBOX_FEATURE_INTERACTION = 0,
	GLOBOX_FEATURE_STATE,
	GLOBOX_FEATURE_TITLE,
	GLOBOX_FEATURE_ICON,
	GLOBOX_FEATURE_SIZE,
	GLOBOX_FEATURE_POS,
	GLOBOX_FEATURE_FRAME,
	GLOBOX_FEATURE_BACKGROUND,
	GLOBOX_FEATURE_VSYNC,
	// special value used to get the total number of features
	GLOBOX_FEATURE_COUNT,
};

// didn't necessarily happen programmatically
enum globox_event
{
	// special value used when errors occur while processing events
	GLOBOX_EVENT_INVALID = 0,
	GLOBOX_EVENT_UNKNOWN,
	// window state updates
	GLOBOX_EVENT_RESTORED,
	GLOBOX_EVENT_MINIMIZED,
	GLOBOX_EVENT_MAXIMIZED,
	GLOBOX_EVENT_FULLSCREEN,
	GLOBOX_EVENT_CLOSED,
	// window size & position updates
	GLOBOX_EVENT_MOVED_RESIZED,
	GLOBOX_EVENT_DAMAGED,
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
	GLOBOX_ERROR_FEATURE_INVALID,
	GLOBOX_ERROR_FEATURE_UNAVAILABLE,
	GLOBOX_ERROR_FEATURE_STATE_INVALID,
	// posix
	GLOBOX_ERROR_POSIX_MUTEX_INIT,
	GLOBOX_ERROR_POSIX_MUTEX_DESTROY,
	GLOBOX_ERROR_POSIX_MUTEX_ATTR_INIT,
	GLOBOX_ERROR_POSIX_MUTEX_ATTR_DESTROY,
	GLOBOX_ERROR_POSIX_MUTEX_ATTR_SETTYPE,
	GLOBOX_ERROR_POSIX_MUTEX_LOCK,
	GLOBOX_ERROR_POSIX_MUTEX_UNLOCK,
	GLOBOX_ERROR_POSIX_COND_INIT,
	GLOBOX_ERROR_POSIX_COND_WAIT,
	GLOBOX_ERROR_POSIX_COND_DESTROY,
	GLOBOX_ERROR_POSIX_COND_ATTR_INIT,
	GLOBOX_ERROR_POSIX_COND_ATTR_DESTROY,
	GLOBOX_ERROR_POSIX_COND_ATTR_SETCLOCK,
	GLOBOX_ERROR_POSIX_THREAD_CREATE,
	GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT,
	GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY,
	GLOBOX_ERROR_POSIX_THREAD_ATTR_JOINABLE,
	GLOBOX_ERROR_POSIX_THREAD_JOIN,
	GLOBOX_ERROR_POSIX_SHM_VERSION,
	GLOBOX_ERROR_POSIX_SHMDT,
	GLOBOX_ERROR_POSIX_SHMID,
	GLOBOX_ERROR_POSIX_SHMCTL,
	GLOBOX_ERROR_POSIX_SHMADDR,
	// wayland
	// x11
	GLOBOX_ERROR_X11_CONN,
	GLOBOX_ERROR_X11_FLUSH,
	GLOBOX_ERROR_X11_ATOM_GET,
	GLOBOX_ERROR_X11_WIN_CREATE,
	GLOBOX_ERROR_X11_WIN_DESTROY,
	GLOBOX_ERROR_X11_WIN_MAP,
	GLOBOX_ERROR_X11_WIN_UNMAP,
	GLOBOX_ERROR_X11_EVENT_WAIT,
	GLOBOX_ERROR_X11_EVENT_SEND,
	GLOBOX_ERROR_X11_PROP_CHANGE,
	GLOBOX_ERROR_X11_PROP_GET,
	GLOBOX_ERROR_X11_PROP_VALUE_GET,
	GLOBOX_ERROR_X11_ATTR_CHANGE,
	GLOBOX_ERROR_X11_GC_CREATE,
	GLOBOX_ERROR_X11_PIXMAP,
	GLOBOX_ERROR_X11_SHM_PIXMAP,
	GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE,
	GLOBOX_ERROR_X11_VISUAL_MISSING,
	GLOBOX_ERROR_X11_SHM_ATTACH,
	GLOBOX_ERROR_X11_SHM_DETACH,
	GLOBOX_ERROR_X11_IMAGE,
	GLOBOX_ERROR_X11_COPY,
	GLOBOX_ERROR_X11_EVENT_INVALID,
	GLOBOX_ERROR_X11_SYNC_COUNTER_CREATE,
	GLOBOX_ERROR_X11_SYNC_COUNTER_SET,
	GLOBOX_ERROR_X11_SYNC_COUNTER_DESTROY,
	GLOBOX_ERROR_X11_GEOMETRY_GET,
	GLOBOX_ERROR_X11_TRANSLATE_COORDS,
	GLOBOX_ERROR_X11_CONFIGURE,
	GLOBOX_ERROR_X11_QUERY_POINTER,
	GLOBOX_ERROR_X11_GLX,
	GLOBOX_ERROR_X11_GLX_FB_CONF_LIST,
	GLOBOX_ERROR_X11_GLX_FB_INVALID,
	GLOBOX_ERROR_X11_GLX_FB_CONF_ATTR,
	GLOBOX_ERROR_X11_GLX_ATTR_ARB,
	GLOBOX_ERROR_X11_GLX_CONTEXT,
	GLOBOX_ERROR_X11_GLX_WINDOW,
	GLOBOX_ERROR_X11_GLX_MAKE_CURRENT,
	GLOBOX_ERROR_X11_GLX_DISPLAY_OPEN,
	GLOBOX_ERROR_X11_GLX_VSYNC,
	GLOBOX_ERROR_X11_EGL_SWAP,
	GLOBOX_ERROR_X11_EGL_DESTROY_SURFACE,
	GLOBOX_ERROR_X11_EGL_DESTROY_CONTEXT,
	GLOBOX_ERROR_X11_EGL_TERMINATE,
	GLOBOX_ERROR_X11_EGL_DISPLAY_GET,
	GLOBOX_ERROR_X11_EGL_INIT,
	GLOBOX_ERROR_X11_EGL_BIND_API,
	GLOBOX_ERROR_X11_EGL_CONFIG,
	GLOBOX_ERROR_X11_EGL_CONTEXT_CREATE,
	GLOBOX_ERROR_X11_EGL_CONFIG_ATTR,
	GLOBOX_ERROR_X11_EGL_WINDOW_SURFACE,
	GLOBOX_ERROR_X11_EGL_MAKE_CURRENT,
	GLOBOX_ERROR_X11_EGL_SWAP_INTERVAL,
	GLOBOX_ERROR_X11_VULKAN_SURFACE_CREATE,
	GLOBOX_ERROR_X11_VULKAN_EXTENSIONS_LIST,
	GLOBOX_ERROR_X11_VULKAN_EXTENSION_UNAVAILABLE,
	// windows
	// macos
	GLOBOX_ERROR_MACOS_OBJ_NIL,
	// special
	GLOBOX_ERROR_COUNT,
};

struct globox_error_info
{
	enum globox_error code;
	const char* file;
	unsigned line;
};

struct globox_rect
{
	int x;
	int y;
	unsigned width;
	unsigned height;
};

// ## config types
struct globox_config_render
{
	void* data;
	void (*callback)(void* data);
};

struct globox_config_events
{
	void* data;
	void (*handler)(void* data, void* event);
};

struct globox_config_features
{
	enum globox_feature* list;
	size_t count;
};

struct globox_config_request
{
	enum globox_feature feature;
	void* config;
};

struct globox_config_reply
{
	enum globox_feature feature;
	struct globox_error_info error;
};

// ## feature types
struct globox_feature_interaction
{
	enum globox_interaction action;
};

struct globox_feature_state
{
	enum globox_state state;
};

struct globox_feature_title
{
	const char* title;
};

struct globox_feature_icon
{
	uint32_t* pixmap;
	uint32_t len;
};

struct globox_feature_size
{
	unsigned width;
	unsigned height;
};

struct globox_feature_pos
{
	int x;
	int y;
};

struct globox_feature_frame
{
	bool frame;
};

struct globox_feature_background
{
	enum globox_background background;
};

struct globox_feature_vsync
{
	bool vsync;
};

// ## backend configuration structure
// depends on most of the above
struct globox_config_backend
{
	void* data;
	// function pointers for each cross-platform globox call
	// lifecycle
	void (*init)(
		struct globox* context,
		struct globox_error_info* error);
	void (*clean)(
		struct globox* context,
		struct globox_error_info* error);
	void (*window_create)(
		struct globox* context,
		struct globox_config_request* configs,
		size_t count,
		void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
		void* data,
		struct globox_error_info* error);
	void (*window_destroy)(
		struct globox* context,
		struct globox_error_info* error);
	void (*window_start)(
		struct globox* context,
		struct globox_error_info* error);
	void (*window_block)(
		struct globox* context,
		struct globox_error_info* error);
	void (*window_stop)(
		struct globox* context,
		struct globox_error_info* error);
	// render callback
	void (*init_render)(
		struct globox* context,
		struct globox_config_render* config,
		struct globox_error_info* error);
	// event handler
	void (*init_events)(
		struct globox* context,
		struct globox_config_events* config,
		struct globox_error_info* error);
	enum globox_event (*handle_events)(
		struct globox* context,
		void* event,
		struct globox_error_info* error);
	// feature registry
	struct globox_config_features* (*init_features)(
		struct globox* context,
		struct globox_error_info* error);
	// features
	void (*feature_set_interaction)(
		struct globox* context,
		struct globox_feature_interaction* config,
		struct globox_error_info* error);
	void (*feature_set_state)(
		struct globox* context,
		struct globox_feature_state* config,
		struct globox_error_info* error);
	void (*feature_set_title)(
		struct globox* context,
		struct globox_feature_title* config,
		struct globox_error_info* error);
	void (*feature_set_icon)(
		struct globox* context,
		struct globox_feature_icon* config,
		struct globox_error_info* error);
	// getters
	unsigned (*get_width)(
		struct globox* context,
		struct globox_error_info* error);
	unsigned (*get_height)(
		struct globox* context,
		struct globox_error_info* error);
	struct globox_rect (*get_expose)(
		struct globox* context,
		struct globox_error_info* error);
	// content update function
	void (*update_content)(
		struct globox* context,
		void* data,
		struct globox_error_info* error);
};

// # cross-platform, cross-backend
// ## lifecycle (N.B.: the event loop is always started on a separate thread)
// allocate base resources and make initial checks
struct globox* globox_init(
	struct globox_config_backend* config,
	struct globox_error_info* error);
// free base resources
void globox_clean(
	struct globox* context,
	struct globox_error_info* error);

// create the window without displaying it and call all window feature callbacks
void globox_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error);
// destroy the window object entirely
void globox_window_destroy(
	struct globox* context,
	struct globox_error_info* error);

// start displaying the window and running the loop
void globox_window_start(
	struct globox* context,
	struct globox_error_info* error);
// block the caller thread until the window has been closed
void globox_window_block(
	struct globox* context,
	struct globox_error_info* error);
// close the window if still open and stop the loop
void globox_window_stop(
	struct globox* context,
	struct globox_error_info* error);

// ## configuration (can always be called)
// render callback
void globox_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error);

// event handler
void globox_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error);

// window event handler helper provided by globox
// (this is a very special case, other event handlers shouldn't be as simple!)
enum globox_event globox_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error);

// ## features (can only be called if confirmed in the registry callback)
// feature registry
struct globox_config_features* globox_init_features(
	struct globox* context,
	struct globox_error_info* error);

// feature setters
void globox_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error);

void globox_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error);

void globox_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error);

void globox_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error);

// ## getters
unsigned globox_get_width(
	struct globox* context,
	struct globox_error_info* error);

unsigned globox_get_height(
	struct globox* context,
	struct globox_error_info* error);

struct globox_rect globox_get_expose(
	struct globox* context,
	struct globox_error_info* error);

// ## content updater (backend-specific but still cross-platform)
void globox_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error);

// ## errors
void globox_error_log(
	struct globox* context,
	struct globox_error_info* error);
const char* globox_error_get_msg(
	struct globox* context,
	struct globox_error_info* error);
enum globox_error globox_error_get_code(
	struct globox_error_info* error);
const char* globox_error_get_file(
	struct globox_error_info* error);
unsigned globox_error_get_line(
	struct globox_error_info* error);
void globox_error_ok(
	struct globox_error_info* error);

#endif
