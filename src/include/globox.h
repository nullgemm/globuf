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
	GLOBOX_FEATURE_SIZE,
	GLOBOX_FEATURE_POS,
	GLOBOX_FEATURE_FRAME,
	GLOBOX_FEATURE_BACKGROUND,
	GLOBOX_FEATURE_VSYNC_CALLBACK,
	// special value used to get the total number of features
	GLOBOX_FEATURE_COUNT,
};

// didn't necessarily happen programmatically
enum globox_event
{
	// special value used when errors occur while processing events
	GLOBOX_EVENT_INVALID = 0,
	// window state updates
	GLOBOX_EVENT_RESTORED,
	GLOBOX_EVENT_MINIMIZED,
	GLOBOX_EVENT_MAXIMIZED,
	GLOBOX_EVENT_FULLSCREEN,
	GLOBOX_EVENT_CLOSED,
	// window size & position updates
	GLOBOX_EVENT_MOVED,
	GLOBOX_EVENT_RESIZED_N,
	GLOBOX_EVENT_RESIZED_NW,
	GLOBOX_EVENT_RESIZED_W,
	GLOBOX_EVENT_RESIZED_SW,
	GLOBOX_EVENT_RESIZED_S,
	GLOBOX_EVENT_RESIZED_SE,
	GLOBOX_EVENT_RESIZED_E,
	GLOBOX_EVENT_RESIZED_NE,
	// lower-level system updates
	GLOBOX_EVENT_CONTENT_DAMAGED, // need to render a part of the content again
	GLOBOX_EVENT_DISPLAY_CHANGED, // need to render the whole content if dpi-aware
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
	GLOBOX_ERROR_FEATURE_UNAVAILABLE,
	GLOBOX_ERROR_FEATURE_STATE_INVALID,
	// posix
	GLOBOX_ERROR_POSIX_MUTEX_INIT = 500,
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
	GLOBOX_ERROR_POSIX_COND_ATTR_SETTYPE,
	GLOBOX_ERROR_POSIX_THREAD_CREATE,
	GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT,
	GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY,
	GLOBOX_ERROR_POSIX_THREAD_ATTR_DETACH,
	// wayland
	// x11
	GLOBOX_ERROR_X11_CONN = 1500,
	GLOBOX_ERROR_X11_FLUSH,
	GLOBOX_ERROR_X11_ATOM_GET,
	GLOBOX_ERROR_X11_WIN_CREATE,
	GLOBOX_ERROR_X11_WIN_DESTROY,
	GLOBOX_ERROR_X11_WIN_MAP,
	GLOBOX_ERROR_X11_WIN_UNMAP,
	GLOBOX_ERROR_X11_EVENT_WAIT,
	GLOBOX_ERROR_X11_EVENT_SEND,
	GLOBOX_ERROR_X11_PROP_CHANGE,
	GLOBOX_ERROR_X11_ATTR_CHANGE,
	GLOBOX_ERROR_X11_GC_CREATE,
	GLOBOX_ERROR_X11_SHM_VERSION_REPLY,
	GLOBOX_ERROR_X11_PIXMAP,
	GLOBOX_ERROR_X11_SHM_PIXMAP,
	GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE,
	// windows
	// macos
	// special
	GLOBOX_ERROR_COUNT,
};

struct globox_error_info
{
	enum globox_error code;
	const char* file;
	unsigned line;
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

struct globox_feature_vsync_callback
{
	void* data;
	void (*callback)(void* data);
};

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
		void** features,
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
	// feature registry
	struct globox_config_features* (*init_features)(
		struct globox* context,
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
	void (*feature_set_size)(
		struct globox* context,
		struct globox_feature_size* config,
		struct globox_error_info* error);
	void (*feature_set_pos)(
		struct globox* context,
		struct globox_feature_pos* config,
		struct globox_error_info* error);
	void (*feature_set_frame)(
		struct globox* context,
		struct globox_feature_frame* config,
		struct globox_error_info* error);
	void (*feature_set_background)(
		struct globox* context,
		struct globox_feature_background* config,
		struct globox_error_info* error);
	void (*feature_set_vsync_callback)(
		struct globox* context,
		struct globox_feature_vsync_callback* config,
		struct globox_error_info* error);

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
	void** features,
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

// ## configuration (can always be called)
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

void globox_feature_set_size(
	struct globox* context,
	struct globox_feature_size* config,
	struct globox_error_info* error);

void globox_feature_set_pos(
	struct globox* context,
	struct globox_feature_pos* config,
	struct globox_error_info* error);

void globox_feature_set_frame(
	struct globox* context,
	struct globox_feature_frame* config,
	struct globox_error_info* error);

void globox_feature_set_background(
	struct globox* context,
	struct globox_feature_background* config,
	struct globox_error_info* error);

void globox_feature_set_vsync_callback(
	struct globox* context,
	struct globox_feature_vsync_callback* config,
	struct globox_error_info* error);

// # content updater (backend-specific but still cross-platform)
void globox_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error);

// # getters
unsigned globox_get_width(
	struct globox* context,
	struct globox_error_info* error);

unsigned globox_get_height(
	struct globox* context,
	struct globox_error_info* error);

#endif
