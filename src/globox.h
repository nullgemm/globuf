#ifndef H_GLOBOX
#define H_GLOBOX

/// this is the main globox header and the only one the developer should include

// common includes
#include <stdint.h>
#include <stdbool.h>

// errors
enum globox_error
{
	// special error code to signal everything is fine
	GLOBOX_ERROR_OK = 0,

	// generic error codes
	GLOBOX_ERROR_NULL,
	GLOBOX_ERROR_ALLOC,
	GLOBOX_ERROR_BOUNDS,
	GLOBOX_ERROR_DOMAIN,
	GLOBOX_ERROR_FD,

#if defined(GLOBOX_PLATFORM_WAYLAND)
	// wayland platform errors
	GLOBOX_ERROR_WAYLAND_EPOLL_CREATE,
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
	// wayland egl errors
	GLOBOX_ERROR_WAYLAND_EGL_FAIL,
#elif defined(GLOBOX_PLATFORM_X11)
	// x11 platform errors
	GLOBOX_ERROR_X11_CONN,
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
	// x11 egl errors
	GLOBOX_ERROR_X11_EGL_FAIL,
	// x11 glx errors
	GLOBOX_ERROR_X11_GLX_FAIL,
#elif defined(GLOBOX_PLATFORM_WINDOWS)
	// windows platform errors
	GLOBOX_ERROR_WINDOWS_SYM,
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
	// windows gdi errors
	GLOBOX_ERROR_WINDOWS_GDI_DAMAGE,
	GLOBOX_ERROR_WINDOWS_GDI_PAINT,
	GLOBOX_ERROR_WINDOWS_GDI_BITBLT,
	GLOBOX_ERROR_WINDOWS_GDI_DIB_CREATE,
	GLOBOX_ERROR_WINDOWS_GDI_BITMAP_SELECT,
	GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_CREATE,
	GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_DELETE,
	// windows egl errors
	GLOBOX_ERROR_WINDOWS_EGL_INIT,
	GLOBOX_ERROR_WINDOWS_EGL_API_BIND,
	GLOBOX_ERROR_WINDOWS_EGL_DISPLAY_GET,
	GLOBOX_ERROR_WINDOWS_EGL_CONFIG_CHOOSE,
	GLOBOX_ERROR_WINDOWS_EGL_CONFIG_ATTRIB_GET,
	GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_CREATE,
	GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_DESTROY,
	GLOBOX_ERROR_WINDOWS_EGL_SURFACE_CREATE,
	GLOBOX_ERROR_WINDOWS_EGL_SURFACE_DESTROY,
	GLOBOX_ERROR_WINDOWS_EGL_MAKE_CURRENT,
	GLOBOX_ERROR_WINDOWS_EGL_TERMINATE,
	GLOBOX_ERROR_WINDOWS_EGL_SWAP,
	// windows wgl errors
	GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE,
	GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET,
	GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE,
	GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_DELETE,
	GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET,
	GLOBOX_ERROR_WINDOWS_WGL_SWAP,
#elif defined(GLOBOX_PLATFORM_MACOS)
	// macos platform errors
	GLOBOX_ERROR_MACOS_CLASS_GET,
	GLOBOX_ERROR_MACOS_CLASS_ALLOC,
	GLOBOX_ERROR_MACOS_CLASS_ADDVAR,
	GLOBOX_ERROR_MACOS_CLASS_ADDMETHOD,
	GLOBOX_ERROR_MACOS_OBJ_INIT,
	GLOBOX_ERROR_MACOS_OBJ_NIL,
	GLOBOX_ERROR_MACOS_EGL_FAIL,
#endif

	// special value used to get the total number of error codes
	GLOBOX_ERROR_SIZE,
};

// helper structures
enum globox_state
{
	GLOBOX_STATE_REGULAR,
	GLOBOX_STATE_MINIMIZED,
	GLOBOX_STATE_MAXIMIZED,
	GLOBOX_STATE_FULLSCREEN,
};

enum globox_interactive_mode
{
	GLOBOX_INTERACTIVE_STOP = 0,
	GLOBOX_INTERACTIVE_MOVE,
	GLOBOX_INTERACTIVE_N,  // North
	GLOBOX_INTERACTIVE_NW, // North-West
	GLOBOX_INTERACTIVE_W,  // West
	GLOBOX_INTERACTIVE_SW, // South-West
	GLOBOX_INTERACTIVE_S,  // South
	GLOBOX_INTERACTIVE_SE, // South-East
	GLOBOX_INTERACTIVE_E,  // East
	GLOBOX_INTERACTIVE_NE, // North-East
};

// globox
struct globox
{
	// window position (signed)
	int32_t globox_x; 
	int32_t globox_y;
	// window size
	uint32_t globox_width;
	uint32_t globox_height;
	// window title (UTF-8)
	char* globox_title;
	bool globox_closed;
	bool globox_redraw;
	// window state (regular, minimized, maximized, fullscreen)
	enum globox_state globox_state;
	// interactive move and resize state
	enum globox_interactive_mode globox_interactive_mode;
	// window style
	bool globox_transparent;
	bool globox_frameless;
	bool globox_blurred;
	// window events callback data (developer-provided)
	void* globox_event_callback_data;
	// window events callback (resize, hover, click, type)
	void (*globox_event_callback)(
		void* event,
		void* data);
	// error handling
	enum globox_error globox_error;
	char* globox_log[GLOBOX_ERROR_SIZE];
	// platform-specific substructure
	struct globox_platform* globox_platform;
};

// cleans the error field
void globox_error_reset(struct globox* globox);
// prints the error message
void globox_error_basic_log(struct globox* globox);
// returns a pointer to the error message
char* globox_error_output_log(struct globox* globox);
// returns the error code
enum globox_error globox_error_output_code(struct globox* globox);
// returns true if an error occurred
char globox_error_catch(struct globox* globox);

// generic functions
void globox_open(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height,
	const char* title,
	enum globox_state state,
	void (*callback)(
		void* event,
		void* data),
	void* callback_data);

void globox_close(struct globox* globox);

void globox_set_closed(struct globox* globox, bool closed);

// platform-dependent functions
void globox_platform_free(struct globox* globox);
void globox_platform_create_window(struct globox* globox);
void globox_platform_hooks(struct globox* globox);
void globox_platform_commit(struct globox* globox);
void globox_platform_prepoll(struct globox* globox);
void globox_platform_events_poll(struct globox* globox);
void globox_platform_events_wait(struct globox* globox);

void globox_platform_interactive_mode(
	struct globox* globox,
	enum globox_interactive_mode mode);

void globox_platform_init(
	struct globox* globox,
	bool transparent,
	bool blurred);

void globox_platform_events_handle(
	struct globox* globox);

void globox_platform_prepare_buffer(
	struct globox* globox);

void globox_platform_set_icon(
	struct globox* globox,
	uint32_t* pixmap,
	uint32_t len);

void globox_platform_set_title(
	struct globox* globox,
	const char* title); 

void globox_platform_set_state(
	struct globox* globox,
	enum globox_state state); 

// context-dependent functions
#if defined(GLOBOX_CONTEXT_SOFTWARE)
void globox_context_software_free(struct globox* globox);
void globox_context_software_create(struct globox* globox);
void globox_context_software_shrink(struct globox* globox);
void globox_context_software_init(
	struct globox* globox,
	int version_major,
	int version_minor);
void globox_context_software_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);
#elif defined(GLOBOX_CONTEXT_VULKAN)
#elif defined(GLOBOX_CONTEXT_EGL)
void globox_context_egl_free(struct globox* globox);
void globox_context_egl_create(struct globox* globox);
void globox_context_egl_shrink(struct globox* globox);
void globox_context_egl_init(
	struct globox* globox,
	int version_major,
	int version_minor);
void globox_context_egl_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);
#elif defined(GLOBOX_CONTEXT_WGL)
void globox_context_wgl_free(struct globox* globox);
void globox_context_wgl_create(struct globox* globox);
void globox_context_wgl_shrink(struct globox* globox);
void globox_context_wgl_init(
	struct globox* globox,
	int version_major,
	int version_minor);
void globox_context_wgl_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);
#elif defined(GLOBOX_CONTEXT_GLX)
void globox_context_glx_free(struct globox* globox);
void globox_context_glx_create(struct globox* globox);
void globox_context_glx_shrink(struct globox* globox);
void globox_context_glx_init(
	struct globox* globox,
	int version_major,
	int version_minor);
void globox_context_glx_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);
#endif

// generic getters 
int32_t globox_get_x(struct globox* globox);
int32_t globox_get_y(struct globox* globox);
uint32_t globox_get_width(struct globox* globox);
uint32_t globox_get_height(struct globox* globox);
char* globox_get_title(struct globox* globox);
bool globox_get_closed(struct globox* globox);
bool globox_get_redraw(struct globox* globox);
enum globox_state globox_get_state(struct globox* globox);
enum globox_interactive_mode
	globox_get_interactive_mode(struct globox* globox);

bool globox_get_transparent(struct globox* globox);
bool globox_get_blurred(struct globox* globox);

// the syntax reaches its limits when it's time to return a function pointer
// here is what is returned: void (*globox_event_callback)(void* event, void* data)
void (*globox_get_event_callback(struct globox* globox))(void* event, void* data);
void* globox_get_event_callback_data(struct globox* globox);

// platform setters
#if defined(GLOBOX_PLATFORM_WAYLAND)
void globox_wayland_save_serial(struct globox* globox, uint32_t serial);
#endif

// platform getters 
uint32_t* globox_platform_get_argb(struct globox* globox);

#endif
