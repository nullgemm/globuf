/// this file implements platform-independent functions

#define _XOPEN_SOURCE 700

#include "globox.h"
#include "globox_error.h"
// system includes
#include <stdlib.h>
#include <string.h>

#if defined(GLOBOX_PLATFORM_WAYLAND)
	#include "wayland/globox_wayland.h"
#elif defined(GLOBOX_PLATFORM_X11)
	#include "x11/globox_x11.h"
#elif defined(GLOBOX_PLATFORM_WINDOWS)
	#include "windows/globox_windows.h"
#elif defined(GLOBOX_PLATFORM_MACOS)
	#include "macos/globox_macos.h"
#endif

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
	void* callback_data)
{
	char** log = globox->globox_log;

	// in this case this *is* a valid way to initialize the string array
	log[GLOBOX_ERROR_OK] = "out-of-bounds log message";
	log[GLOBOX_ERROR_NULL] = "null pointer";
	log[GLOBOX_ERROR_ALLOC] = "failed memory allocation";
	log[GLOBOX_ERROR_BOUNDS] = "out-of-bounds index";
	log[GLOBOX_ERROR_DOMAIN] = "invalid domain";

#if defined(GLOBOX_PLATFORM_WAYLAND)
	log[GLOBOX_ERROR_WAYLAND_EPOLL_CREATE] =
		"could not create new epoll interface";
	log[GLOBOX_ERROR_WAYLAND_EPOLL_CTL] =
		"could not send epoll control operation";
	log[GLOBOX_ERROR_WAYLAND_EPOLL_WAIT] =
		"error while waiting for epoll events";
	log[GLOBOX_ERROR_WAYLAND_DISPLAY] =
		"could not connect to display";
	log[GLOBOX_ERROR_WAYLAND_ROUNDTRIP] =
		"could not perform display roundtrip";
	log[GLOBOX_ERROR_WAYLAND_DISPATCH] =
		"could not dispatch";
	log[GLOBOX_ERROR_WAYLAND_MMAP] =
		"could not map SHM";
	log[GLOBOX_ERROR_WAYLAND_MUNMAP] =
		"could not unmap SHM";
	log[GLOBOX_ERROR_WAYLAND_REQUEST] =
		"could not perform request";
	log[GLOBOX_ERROR_WAYLAND_LISTENER] =
		"could not add listener";
	log[GLOBOX_ERROR_WAYLAND_EGL_FAIL] =
		"error while dealing with EGL";
#elif defined(GLOBOX_PLATFORM_X11)
	log[GLOBOX_ERROR_X11_CONN] =
		"could not connect to X server";
	log[GLOBOX_ERROR_X11_FLUSH] =
		"could not flush connection";
	log[GLOBOX_ERROR_X11_WIN] =
		"could not create window";
	log[GLOBOX_ERROR_X11_MAP] =
		"could not map window";
	log[GLOBOX_ERROR_X11_GC] =
		"could not create graphics context";
	log[GLOBOX_ERROR_X11_VISUAL_NOT_COMPATIBLE] =
		"could not find compatible visual format";
	log[GLOBOX_ERROR_X11_VISUAL_NOT_FOUND] =
		"could not find any visual format";
	log[GLOBOX_ERROR_X11_PIXMAP] =
		"could not create pixmap";
	log[GLOBOX_ERROR_X11_SHM_VERSION_REPLY] =
		"could not get XCB SHM version";
	log[GLOBOX_ERROR_X11_SHMID] =
		"could not get SHM ID";
	log[GLOBOX_ERROR_X11_SHMADDR] =
		"could not create new SHM";
	log[GLOBOX_ERROR_X11_SHM_ATTACH] =
		"could not attach SHM";
	log[GLOBOX_ERROR_X11_SHMCTL] =
		"could not send SHM control operation";
	log[GLOBOX_ERROR_X11_SHM_PIXMAP] =
		"could not create SHM pixmap";
	log[GLOBOX_ERROR_X11_SHM_DETACH] =
		"could not detach SHM";
	log[GLOBOX_ERROR_X11_SHMDT] =
		"could not release SHM memory";
	log[GLOBOX_ERROR_X11_EPOLL_CREATE] =
		"could not create new epoll interface";
	log[GLOBOX_ERROR_X11_EPOLL_CTL] =
		"could not send epoll control operation";
	log[GLOBOX_ERROR_X11_EPOLL_WAIT] =
		"error while waiting for epoll events";
	log[GLOBOX_ERROR_X11_WIN_ATTR] =
		"could not change window attribute";
	log[GLOBOX_ERROR_X11_ATOMS] =
		"could not change atom";
	log[GLOBOX_ERROR_X11_ICON] =
		"could not set window icon";
	log[GLOBOX_ERROR_X11_TITLE] =
		"could not set window title";
	log[GLOBOX_ERROR_X11_IMAGE] =
		"could not transmit image";
	log[GLOBOX_ERROR_X11_COPY] =
		"could not copy area";
	log[GLOBOX_ERROR_X11_SCREEN_INFO] =
		"could not get screen info";
	log[GLOBOX_ERROR_X11_WIN_INFO] =
		"could not get window geometry info";
	log[GLOBOX_ERROR_X11_STATE] =
		"could not update window property";
	log[GLOBOX_ERROR_X11_INTERACTIVE] =
		"could not handle interactive move and resize";
	log[GLOBOX_ERROR_X11_EGL_FAIL] =
		"error while dealing with EGL";
	log[GLOBOX_ERROR_X11_GLX_FAIL] =
		"error while dealing with GLX";
#elif defined(GLOBOX_PLATFORM_WINDOWS)
	log[GLOBOX_ERROR_WINDOWS_SYM] =
		"could not find symbol";
	log[GLOBOX_ERROR_WINDOWS_MODULE_APP] =
		"could not get module handle";
	log[GLOBOX_ERROR_WINDOWS_MODULE_USER32] =
		"could not find user32.dll";
	log[GLOBOX_ERROR_WINDOWS_DELETE] =
		"could not delete win32 object";
	log[GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET] =
		"could not get device context";
	log[GLOBOX_ERROR_WINDOWS_MESSAGE_GET] =
		"could not get event message";
	log[GLOBOX_ERROR_WINDOWS_CLASS_REGISTER] =
		"could not register extended window class";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_CREATE] =
		"could not create window";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_ADJUST] =
		"could not adjust window rectangle";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_UPDATE] =
		"could not update window";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_RECT_GET] =
		"could not get window rectangle";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_POS_SET] =
		"could not set window position";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET] =
		"could not get window style";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET] =
		"could not set window style";
	log[GLOBOX_ERROR_WINDOWS_PLACEMENT_GET] =
		"could not save window placement";
	log[GLOBOX_ERROR_WINDOWS_PLACEMENT_SET] =
		"could not set window placement";
	log[GLOBOX_ERROR_WINDOWS_CLIENT_RECT] =
		"could not get window client rectangle";
	log[GLOBOX_ERROR_WINDOWS_CLIENT_POS] =
		"could not get window client position";
	log[GLOBOX_ERROR_WINDOWS_TRANSPARENCY_REGION] =
		"could not create transparency region";
	log[GLOBOX_ERROR_WINDOWS_TRANSPARENCY_DWM] =
		"could not initialize transparency";
	log[GLOBOX_ERROR_WINDOWS_COMP_ATTR] =
		"could not set window composition attribute";
	log[GLOBOX_ERROR_WINDOWS_INPUT_SEND] =
		"could not synthetize input event";
	log[GLOBOX_ERROR_WINDOWS_CAPTURE_RELEASE] =
		"could not release mouse capture";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_LOAD] =
		"could not load mouse cursor";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_POS_GET] =
		"could not get mouse cursor position";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_POS_SET] =
		"could not set mouse cursor position";
	log[GLOBOX_ERROR_WINDOWS_BMP_MASK_CREATE] =
		"could not create mask bitmap";
	log[GLOBOX_ERROR_WINDOWS_BMP_COLOR_CREATE] =
		"could not create color bitmap";
	log[GLOBOX_ERROR_WINDOWS_ICON_CREATE] =
		"could not create icon information";
	log[GLOBOX_ERROR_WINDOWS_ICON_SMALL] =
		"could not create small icon";
	log[GLOBOX_ERROR_WINDOWS_ICON_BIG] =
		"could not create big icon";
	log[GLOBOX_ERROR_WINDOWS_UTF8] =
		"could not translate UTF-8";
	log[GLOBOX_ERROR_WINDOWS_TITLE] =
		"could not update window title";
	log[GLOBOX_ERROR_WINDOWS_GDI_DAMAGE] =
		"could not damage GDI region";
	log[GLOBOX_ERROR_WINDOWS_GDI_PAINT] =
		"could not start GDI rendering";
	log[GLOBOX_ERROR_WINDOWS_GDI_BITBLT] =
		"could not copy GDI buffer to window";
	log[GLOBOX_ERROR_WINDOWS_GDI_DIB_CREATE] =
		"could not create GDI device-independent bitmap";
	log[GLOBOX_ERROR_WINDOWS_GDI_BITMAP_SELECT] =
		"could not select GDI bitmap";
	log[GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_CREATE] =
		"could not create GDI device context";
	log[GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_DELETE] =
		"could not delete GDI device context";
	log[GLOBOX_ERROR_WINDOWS_EGL_INIT] =
		"could not init EGL";
	log[GLOBOX_ERROR_WINDOWS_EGL_API_BIND] =
		"could not set EGL target graphics API";
	log[GLOBOX_ERROR_WINDOWS_EGL_DISPLAY_GET] =
		"could not get EGL display";
	log[GLOBOX_ERROR_WINDOWS_EGL_CONFIG_CHOOSE] =
		"could not choose EGL config";
	log[GLOBOX_ERROR_WINDOWS_EGL_CONFIG_ATTRIB_GET] =
		"could not get EGL visual ID";
	log[GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_CREATE] =
		"could not create EGL context";
	log[GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_DESTROY] =
		"could not destroy EGL context";
	log[GLOBOX_ERROR_WINDOWS_EGL_SURFACE_CREATE] =
		"could not create EGL surface";
	log[GLOBOX_ERROR_WINDOWS_EGL_SURFACE_DESTROY] =
		"could not destroy EGL surface";
	log[GLOBOX_ERROR_WINDOWS_EGL_MAKE_CURRENT] =
		"could not make EGL surface current";
	log[GLOBOX_ERROR_WINDOWS_EGL_TERMINATE] =
		"could not terminate EGL";
	log[GLOBOX_ERROR_WINDOWS_EGL_SWAP] =
		"could not swap EGL buffers";
	log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE] =
		"could not choose WGL pixel format";
	log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET] =
		"could not set WGL pixel format";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE] =
		"could not create WGL context";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_DELETE] =
		"could not delete WGL context";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET] =
		"could not set WGL context";
	log[GLOBOX_ERROR_WINDOWS_WGL_SWAP] =
		"could not swap WGL buffers";
#elif defined(GLOBOX_PLATFORM_MACOS)
	log[GLOBOX_ERROR_MACOS_CLASS_GET] =
		"could not get class definition";
	log[GLOBOX_ERROR_MACOS_CLASS_ALLOC] =
		"could not create class pair";
	log[GLOBOX_ERROR_MACOS_CLASS_ADDVAR] =
		"could not add variable to class";
	log[GLOBOX_ERROR_MACOS_CLASS_ADDMETHOD] =
		"could not add method to class";
	log[GLOBOX_ERROR_MACOS_OBJ_INIT] =
		"could not initialize object instance";
	log[GLOBOX_ERROR_MACOS_OBJ_NIL] =
		"the object returned is Nil";
	log[GLOBOX_ERROR_MACOS_EGL_FAIL] =
		"EGL error";
#endif

	// error system initialization
	globox_error_init(globox);

	// common variables initialization
	globox->globox_x = x;
	globox->globox_y = y;
	globox->globox_width = width;
	globox->globox_height = height;
	globox->globox_title = strdup(title); // should be freed
	globox->globox_state = state;
	globox->globox_event_callback = callback;
	globox->globox_event_callback_data = callback_data;
	globox->globox_closed = false;
	globox->globox_interactive_mode = GLOBOX_INTERACTIVE_STOP;

	globox->globox_transparent = false;
	globox->globox_frameless = false;
	globox->globox_blurred = false;
}

void globox_close(struct globox* globox)
{
	free(globox->globox_title);
}

// getters

int32_t globox_get_x(struct globox* globox)
{
	return globox->globox_x;
}

int32_t globox_get_y(struct globox* globox)
{
	return globox->globox_y;
}

uint32_t globox_get_width(struct globox* globox)
{
	return globox->globox_width;
}

uint32_t globox_get_height(struct globox* globox)
{
	return globox->globox_height;
}

char* globox_get_title(struct globox* globox)
{
	return globox->globox_title;
}

bool globox_get_closed(struct globox* globox)
{
	return globox->globox_closed;
}

bool globox_get_redraw(struct globox* globox)
{
	return globox->globox_redraw;
}

enum globox_state globox_get_state(struct globox* globox)
{
	return globox->globox_state;
}

enum globox_interactive_mode
	globox_get_interactive_mode(struct globox* globox)
{
	return globox->globox_interactive_mode;
}

bool globox_get_transparent(struct globox* globox)
{
	return globox->globox_transparent;
}

bool globox_get_frameless(struct globox* globox)
{
	return globox->globox_frameless;
}

bool globox_get_blurred(struct globox* globox)
{
	return globox->globox_blurred;
}

void* globox_get_event_callback_data(struct globox* globox)
{
	return globox->globox_event_callback_data;
}

void (*globox_get_event_callback(struct globox* globox))(void* event, void* data)
{
	return globox->globox_event_callback;
}
