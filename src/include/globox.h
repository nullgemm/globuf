#ifndef H_GLOBOX
#define H_GLOBOX

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// types

// private!
struct globox;

enum globox_platform
{
	GLOBOX_PLATFORM_WAYLAND = 0,
	GLOBOX_PLATFORM_X11,
	GLOBOX_PLATFORM_WINDOWS,
	GLOBOX_PLATFORM_MACOS,
};

enum globox_backend
{
	GLOBOX_BACKEND_SOFTWARE = 0,
	GLOBOX_BACKEND_VULKAN,
	GLOBOX_BACKEND_OPENGL,
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

enum globox_window_feature
{
	GLOBOX_WINDOW_FEATURE_X_INIT = 0,
	GLOBOX_WINDOW_FEATURE_Y_INIT,
	GLOBOX_WINDOW_FEATURE_WIDTH_INIT,
	GLOBOX_WINDOW_FEATURE_HEIGHT_INIT,
	GLOBOX_WINDOW_FEATURE_TITLE,
	GLOBOX_WINDOW_FEATURE_FRAMED,
	GLOBOX_WINDOW_FEATURE_BACKGROUND,
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

// structures

// use callback setters to make configuration easier for the user of the library
struct globox_config
{
	enum globox_backend backend;
	void* window_feature_registry_data;
	void (*window_feature_registry)(
		struct globox* context,
		enum globox_window_feature feature,
		void* data);

	size_t backend_callbacks_setter_count;
	void** backend_callbacks_setter_data;
	void (**backend_callbacks_setter)(
		struct globox_backend_callbacks_entry* entry,
		enum globox_platform,
		enum globox_backend,
		void* data);

	size_t event_handler_setter_count;
	void** event_handler_setter_data;
	void (**event_handler_setter)(
		struct globox_event_handlers_entry* entry,
		enum globox_platform,
		enum globox_backend,
		void* data);
};

// use dedicated structures for callback setters to avoid exposing composition
struct globox_event_handlers_entry
{
	void (*event_handler)(void* event, void* data);
};

struct globox_backend_callbacks_entry
{
	void* backend_callback_data;
	void (*backend_callback_create)(struct globox* context, void* data);
	void (*backend_callback_destroy)(struct globox* context, void* data);
};

// functions

// lifecycle
void globox_init(struct globox* context);
void globox_clean(struct globox* context);
void globox_window_create(struct globox* context);
void globox_window_destroy(struct globox* context);

void globox_config(
	struct globox* context,
	struct globox_config* config);

// interactions
void globox_interact(
	struct globox* context,
	enum globox_interaction action);

void globox_set_icon(
	struct globox* context,
	uint32_t* pixmap,
	uint32_t len);

void globox_set_title(
	struct globox* context,
	const char* title); 

void globox_set_state(
	struct globox* context,
	enum globox_state state); 

// errors
bool globox_error_catch(struct globox* context);
void globox_error_reset(struct globox* context);
void globox_error_log(struct globox* context);
const char* globox_error_get_message(struct globox* context);
enum globox_error globox_error_get_code(struct globox* context);

// window feature setters
void globox_window_feature_set_x_init(int x_init);
void globox_window_feature_set_y_init(int y_init);
void globox_window_feature_set_width_init(unsigned width_init);
void globox_window_feature_set_height_init(unsigned height_init);
void globox_window_feature_set_title(const char* title);
void globox_window_feature_set_framed(bool framed);
void globox_window_feature_set_background(enum globox_background background);

// window feature callback setters
void globox_window_feature_callback_set_x_init(void (*callback)(struct globox* context, int x_init));
void globox_window_feature_callback_set_y_init(void (*callback)(struct globox* context, int y_init));
void globox_window_feature_callback_set_width_init(void (*callback)(struct globox* context, unsigned width_init));
void globox_window_feature_callback_set_height_init(void (*callback)(struct globox* context, unsigned height_init));
void globox_window_feature_callback_set_title(void (*callback)(struct globox* context, const char* title));
void globox_window_feature_callback_set_framed(void (*callback)(struct globox* context, bool framed));
void globox_window_feature_callback_set_background(void (*callback)(struct globox* context, enum globox_background background));

#endif
