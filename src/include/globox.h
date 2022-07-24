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
	// posix
	GLOBOX_ERROR_POSIX_MUTEX_INIT = 500,
	GLOBOX_ERROR_POSIX_MUTEX_DESTROY,
	GLOBOX_ERROR_POSIX_MUTEX_ATTR_INIT,
	GLOBOX_ERROR_POSIX_MUTEX_ATTR_DESTROY,
	GLOBOX_ERROR_POSIX_MUTEX_ATTR_SETTYPE,
	GLOBOX_ERROR_POSIX_MUTEX_LOCK,
	GLOBOX_ERROR_POSIX_MUTEX_UNLOCK,
	GLOBOX_ERROR_POSIX_COND_INIT,
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
	GLOBOX_ERROR_X11_EVENT_WAIT,
	GLOBOX_ERROR_X11_EVENT_SEND,
	GLOBOX_ERROR_X11_PROP_CHANGE,
	GLOBOX_ERROR_X11_ATTR_CHANGE,
	// windows
	// macos
	// special
	GLOBOX_ERROR_COUNT,
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

struct globox_feature_data
{
	enum globox_feature type;
	void* config;

	void (*callback)(void* data, void* config);
	void* data;
};

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
	void (*window_block)(
		struct globox* context);
	void (*window_stop)(
		struct globox* context);
	// feature registry
	struct globox_config_features* (*init_features)(
		struct globox* context);
	// event handler
	void (*init_events)(
		struct globox* context,
		struct globox_config_events* config);
	enum globox_event (*handle_events)(
		struct globox* context,
		void* event);
	// features
	void (*set_feature_data)(
		struct globox* context,
		struct globox_feature_data* feature_data);
	void (*update_content)(
		struct globox* context,
		void* data);
};

// # cross-platform, cross-backend
// ## lifecycle (N.B.: the event loop is always started on a separate thread)
// allocate base resources and make initial checks
struct globox* globox_init(
	struct globox_config_backend* config);
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
// block the caller thread until the window has been closed
void globox_window_block(
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
// event handler
void globox_init_events(
	struct globox* context,
	struct globox_config_events* config);

// window event handler helper provided by globox
// (this is a very special case, other event handlers shouldn't be as simple!)
enum globox_event globox_handle_events(
	struct globox* context,
	void* event);

// ## features (can only be called if confirmed in the registry callback)
// feature registry
struct globox_config_features* globox_init_features(
	struct globox* context);

// feature setters
void globox_set_feature_data(
	struct globox* context,
	struct globox_feature_data* feature_data);

struct globox_feature_interaction
{
	enum globox_interaction action;
};

// feature structures
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

// # content updater (backend-specific but still cross-platform)
void globox_update_content(
	struct globox* context,
	void* data);

// # getters
unsigned globox_get_width(struct globox* context);
unsigned globox_get_height(struct globox* context);

#endif
