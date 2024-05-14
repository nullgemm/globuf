#include "include/globuf.h"
#include "common/globuf_private.h"
#include "common/globuf_error.h"

#include <stdbool.h>

#if defined(GLOBUF_ERROR_LOG_MANUAL) || defined(GLOBUF_ERROR_LOG_THROW)
	#include <stdio.h>
#endif

#ifdef GLOBUF_ERROR_ABORT
	#include <stdlib.h>
#endif

void globuf_error_init(
	struct globuf* context)
{
#ifndef GLOBUF_ERROR_SKIP
	char** log = context->error_messages;

	log[GLOBUF_ERROR_OK] =
		"out-of-bound error message";
	log[GLOBUF_ERROR_NULL] =
		"null pointer";
	log[GLOBUF_ERROR_ALLOC] =
		"failed malloc";
	log[GLOBUF_ERROR_BOUNDS] =
		"out-of-bounds index";
	log[GLOBUF_ERROR_DOMAIN] =
		"invalid domain";
	log[GLOBUF_ERROR_FD] =
		"invalid file descriptor";
	log[GLOBUF_ERROR_FEATURE_INVALID] =
		"invalid feature init request";
	log[GLOBUF_ERROR_FEATURE_UNAVAILABLE] =
		"failed to set one or more features";
	log[GLOBUF_ERROR_FEATURE_STATE_INVALID] =
		"failed to set state because the given value is invalid";

	log[GLOBUF_ERROR_POSIX_MUTEX_INIT] =
		"failed to init posix mutex";
	log[GLOBUF_ERROR_POSIX_MUTEX_DESTROY] =
		"failed to destroy posix mutex";
	log[GLOBUF_ERROR_POSIX_MUTEX_ATTR_INIT] =
		"failed to init posix mutex attributes";
	log[GLOBUF_ERROR_POSIX_MUTEX_ATTR_DESTROY] =
		"failed to destroy posix mutex attributes";
	log[GLOBUF_ERROR_POSIX_MUTEX_ATTR_SETTYPE] =
		"failed to set type in posix mutex attributes";
	log[GLOBUF_ERROR_POSIX_MUTEX_LOCK] =
		"failed to lock posix mutex";
	log[GLOBUF_ERROR_POSIX_MUTEX_UNLOCK] =
		"failed to unlock posix mutex";
	log[GLOBUF_ERROR_POSIX_COND_INIT] =
		"failed to init posix cond";
	log[GLOBUF_ERROR_POSIX_COND_WAIT] =
		"failed to unblock a posix cond";
	log[GLOBUF_ERROR_POSIX_COND_DESTROY] =
		"failed to destroy posix cond";
	log[GLOBUF_ERROR_POSIX_COND_ATTR_INIT] =
		"failed to init posix cond attributes";
	log[GLOBUF_ERROR_POSIX_COND_ATTR_DESTROY] =
		"failed to destroy posix cond attributes";
	log[GLOBUF_ERROR_POSIX_COND_ATTR_SETCLOCK] =
		"failed to set clock in posix cond attributes";
	log[GLOBUF_ERROR_POSIX_THREAD_CREATE] =
		"failed to create posix thread";
	log[GLOBUF_ERROR_POSIX_THREAD_ATTR_INIT] =
		"failed to init posix thread attributes";
	log[GLOBUF_ERROR_POSIX_THREAD_ATTR_DESTROY] =
		"failed to destroy posix thread attributes";
	log[GLOBUF_ERROR_POSIX_THREAD_ATTR_JOINABLE] =
		"failed to set joinable type in posix thread attributes";
	log[GLOBUF_ERROR_POSIX_THREAD_JOIN] =
		"failed to join posix thread";
	log[GLOBUF_ERROR_POSIX_SHM_VERSION] =
		"failed to get SHM version";
	log[GLOBUF_ERROR_POSIX_SHMDT] =
		"failed to delete SHM";
	log[GLOBUF_ERROR_POSIX_SHMID] =
		"failed to get SHM ID";
	log[GLOBUF_ERROR_POSIX_SHMCTL] =
		"failed to ctl SHM";
	log[GLOBUF_ERROR_POSIX_SHMADDR] =
		"failed to get SHM addr";

	log[GLOBUF_ERROR_X11_CONN] =
		"failed to connect to X11 server";
	log[GLOBUF_ERROR_X11_FLUSH] =
		"failed to flush X11 connection";
	log[GLOBUF_ERROR_X11_ATOM_GET] =
		"failed to get X11 atom";
	log[GLOBUF_ERROR_X11_WIN_CREATE] =
		"failed to create X11 window";
	log[GLOBUF_ERROR_X11_WIN_DESTROY] =
		"failed to destroy X11 window";
	log[GLOBUF_ERROR_X11_WIN_MAP] =
		"failed to map X11 window";
	log[GLOBUF_ERROR_X11_WIN_UNMAP] =
		"failed to unmap X11 window";
	log[GLOBUF_ERROR_X11_EVENT_WAIT] =
		"failed to get an X11 event";
	log[GLOBUF_ERROR_X11_EVENT_SEND] =
		"failed to send an X11 event";
	log[GLOBUF_ERROR_X11_PROP_CHANGE] =
		"failed to change an X11 property";
	log[GLOBUF_ERROR_X11_PROP_GET] =
		"failed to get an X11 property";
	log[GLOBUF_ERROR_X11_PROP_VALUE_GET] =
		"failed to get the value of an X11 property";
	log[GLOBUF_ERROR_X11_ATTR_CHANGE] =
		"failed to change an X11 attribute";
	log[GLOBUF_ERROR_X11_GC_CREATE] =
		"failed to create X11 graphical context";
	log[GLOBUF_ERROR_X11_PIXMAP] =
		"failed to get X11 pixmap";
	log[GLOBUF_ERROR_X11_SHM_PIXMAP] =
		"failed to get X11 SHM pixmap";
	log[GLOBUF_ERROR_X11_VISUAL_INCOMPATIBLE] =
		"failed to get a compatible X11 visual configuration";
	log[GLOBUF_ERROR_X11_VISUAL_MISSING] =
		"failed to find the requested X11 visual configuration";
	log[GLOBUF_ERROR_X11_SHM_DETACH] =
		"failed to attach X11 SHM";
	log[GLOBUF_ERROR_X11_SHM_DETACH] =
		"failed to detach X11 SHM";
	log[GLOBUF_ERROR_X11_IMAGE] =
		"failed to create X11 image";
	log[GLOBUF_ERROR_X11_COPY] =
		"failed to copy X11 buffer";
	log[GLOBUF_ERROR_X11_EVENT_INVALID] =
		"received invalid X11 event";
	log[GLOBUF_ERROR_X11_SYNC_COUNTER_CREATE] =
		"failed to create the X11 sync counter";
	log[GLOBUF_ERROR_X11_SYNC_COUNTER_SET] =
		"failed to set the X11 sync counter";
	log[GLOBUF_ERROR_X11_SYNC_COUNTER_DESTROY] =
		"failed to destroy the X11 sync counter";
	log[GLOBUF_ERROR_X11_GEOMETRY_GET] =
		"failed to get X11 window geometry";
	log[GLOBUF_ERROR_X11_TRANSLATE_COORDS] =
		"failed to translate X11 coords";
	log[GLOBUF_ERROR_X11_CONFIGURE] =
		"failed to configure X11 window";
	log[GLOBUF_ERROR_X11_QUERY_POINTER] =
		"failed to query X11 mouse pointer position";
	log[GLOBUF_ERROR_X11_VSYNC] =
		"frame callback VSync is not available for X11";
	log[GLOBUF_ERROR_X11_GLX] =
		"failed to get GLX extension for X11";
	log[GLOBUF_ERROR_X11_GLX_FB_CONF_LIST] =
		"failed to get X11 GLX framebuffer configurations list";
	log[GLOBUF_ERROR_X11_GLX_FB_INVALID] =
		"failed to use X11 GLX framebuffer configuration";
	log[GLOBUF_ERROR_X11_GLX_FB_CONF_ATTR] =
		"failed to get X11 GLX framebuffer configuration attributes";
	log[GLOBUF_ERROR_X11_GLX_ATTR_ARB] =
		"failed to get X11 GLX Attributes ARB extension function pointer";
	log[GLOBUF_ERROR_X11_GLX_CONTEXT] =
		"failed to create a X11 GLX context";
	log[GLOBUF_ERROR_X11_GLX_WINDOW] =
		"failed to create X11 GLX window";
	log[GLOBUF_ERROR_X11_GLX_MAKE_CURRENT] =
		"failed to make X11 GLX context current";
	log[GLOBUF_ERROR_X11_GLX_DISPLAY_OPEN] =
		"failed to open X11 display with Xlib";
	log[GLOBUF_ERROR_X11_GLX_VSYNC] =
		"failed to get required X11 GLX VSync extension function pointer";
	log[GLOBUF_ERROR_X11_EGL_SWAP] =
		"failed to swap X11 EGL buffers";
	log[GLOBUF_ERROR_X11_EGL_DESTROY_SURFACE] =
		"failed to destroy X11 EGL surface";
	log[GLOBUF_ERROR_X11_EGL_DESTROY_CONTEXT] =
		"failed to destroy X11 EGL context";
	log[GLOBUF_ERROR_X11_EGL_TERMINATE] =
		"failed to terminate X11 EGL";
	log[GLOBUF_ERROR_X11_EGL_DISPLAY_GET] =
		"failed to get X11 EGL display";
	log[GLOBUF_ERROR_X11_EGL_INIT] =
		"failed to init X11 EGL";
	log[GLOBUF_ERROR_X11_EGL_BIND_API] =
		"failed to bind X11 EGL API";
	log[GLOBUF_ERROR_X11_EGL_CONFIG] =
		"failed to get X11 EGL configuration";
	log[GLOBUF_ERROR_X11_EGL_CONTEXT_CREATE] =
		"failed to create X11 EGL context";
	log[GLOBUF_ERROR_X11_EGL_CONFIG_ATTR] =
		"failed to create get X11 EGL configuration attributes";
	log[GLOBUF_ERROR_X11_EGL_WINDOW_SURFACE] =
		"failed to create X11 EGL window surface";
	log[GLOBUF_ERROR_X11_EGL_MAKE_CURRENT] =
		"failed to make X11 EGL context current";
	log[GLOBUF_ERROR_X11_EGL_SWAP_INTERVAL] =
		"failed to set X11 EGL swap interval";
	log[GLOBUF_ERROR_X11_VULKAN_SURFACE_CREATE] =
		"failed to create X11 Vulkan surface";
	log[GLOBUF_ERROR_X11_VULKAN_EXTENSIONS_LIST] =
		"failed to list Vulkan extensions";
	log[GLOBUF_ERROR_X11_VULKAN_EXTENSION_UNAVAILABLE] =
		"could not find the XCB Vulkan extension";

	log[GLOBUF_ERROR_WAYLAND_DISPLAY_GET] =
		"could not get Wayland display";
	log[GLOBUF_ERROR_WAYLAND_REGISTRY_GET] =
		"could not get Wayland registry";
	log[GLOBUF_ERROR_WAYLAND_LISTENER_ADD] =
		"could not add Wayland listener";
	log[GLOBUF_ERROR_WAYLAND_ROUNDTRIP] =
		"could not perform Wayland roundtrip";
	log[GLOBUF_ERROR_WAYLAND_POINTER_GET] =
		"could not get Wayland pointer";
	log[GLOBUF_ERROR_WAYLAND_SURFACE_CREATE] =
		"could not create Wayland surface";
	log[GLOBUF_ERROR_WAYLAND_XDG_SURFACE_CREATE] =
		"could not create XDG surface";
	log[GLOBUF_ERROR_WAYLAND_XDG_TOPLEVEL_GET] =
		"could not get XDG toplevel";
	log[GLOBUF_ERROR_WAYLAND_XDG_DECORATION_GET] =
		"could not get XDG decoration";
	log[GLOBUF_ERROR_WAYLAND_SURFACE_FRAME_GET] =
		"could not get Wayland surface frame";
	log[GLOBUF_ERROR_WAYLAND_SHM_GET] =
		"could not get Wayland SHM";
	log[GLOBUF_ERROR_WAYLAND_REQUEST] =
		"could not perform Wayland request";
	log[GLOBUF_ERROR_WAYLAND_COMPOSITOR_MISSING] =
		"could not register Wayland compositor";
	log[GLOBUF_ERROR_WAYLAND_XDG_WM_BASE_MISSSING] =
		"could not register XDG WM base";
	log[GLOBUF_ERROR_WAYLAND_XDG_DECORATION_MANAGER_MISSSING] =
		"could not register XDG decoration manager";
	log[GLOBUF_ERROR_WAYLAND_MMAP] =
		"could not mmap shared Wayland memory";
	log[GLOBUF_ERROR_WAYLAND_MUNMAP] =
		"could not munmap shared Wayland memory";
	log[GLOBUF_ERROR_WAYLAND_REGISTRY_CALLBACK] =
		"could not add Wayland registry callback";
	log[GLOBUF_ERROR_WAYLAND_EVENT_WAIT] =
		"failed to get a Wayland event";
	log[GLOBUF_ERROR_WAYLAND_ICON] =
		"this Wayland compositor does not support application icons";
	log[GLOBUF_ERROR_WAYLAND_BACKGROUND_BLUR] =
		"this Wayland compositor does not support background blur";
	log[GLOBUF_ERROR_WAYLAND_DECORATIONS_FORCED] =
		"this Wayland compositor does not support hiding window decorations";
	log[GLOBUF_ERROR_WAYLAND_DECORATIONS_UNAVAILABLE] =
		"this Wayland compositor does not support displaying window decorations";
	log[GLOBUF_ERROR_WAYLAND_EGL_MAKE_CURRENT] =
		"failed to make Wayland EGL context current";
	log[GLOBUF_ERROR_WAYLAND_EGL_SWAP_INTERVAL] =
		"failed to set Wayland EGL swap interval";
	log[GLOBUF_ERROR_WAYLAND_EGL_DISPLAY_GET] =
		"failed to get Wayland EGL display";
	log[GLOBUF_ERROR_WAYLAND_EGL_INIT] =
		"failed to init Wayland EGL";
	log[GLOBUF_ERROR_WAYLAND_EGL_BIND_API] =
		"failed to bind Wayland EGL API";
	log[GLOBUF_ERROR_WAYLAND_EGL_CONFIG] =
		"failed to get Wayland EGL configuration";
	log[GLOBUF_ERROR_WAYLAND_EGL_CONTEXT_CREATE] =
		"failed to create Wayland EGL context";
	log[GLOBUF_ERROR_WAYLAND_EGL_DESTROY_SURFACE] =
		"failed to destroy Wayland EGL window surface";
	log[GLOBUF_ERROR_WAYLAND_EGL_DESTROY_CONTEXT] =
		"failed to destroy Wayland EGL context";
	log[GLOBUF_ERROR_WAYLAND_EGL_TERMINATE] =
		"failed to terminate Wayland EGL";
	log[GLOBUF_ERROR_WAYLAND_EGL_WINDOW_CREATE] =
		"failed to create Wayland EGL window";
	log[GLOBUF_ERROR_WAYLAND_EGL_WINDOW_SURFACE] =
		"failed to create Wayland EGL window surface";
	log[GLOBUF_ERROR_WAYLAND_EGL_SWAP] =
		"failed to swap Wayland EGL buffers";
	log[GLOBUF_ERROR_WAYLAND_VULKAN_EXTENSIONS_LIST] =
		"failed to create Wayland Vulkan surface";
	log[GLOBUF_ERROR_WAYLAND_VULKAN_EXTENSION_UNAVAILABLE] =
		"failed to list Vulkan extensions";
	log[GLOBUF_ERROR_WAYLAND_VULKAN_SURFACE_CREATE] =
		"could not find the Wayland Vulkan extension";

	log[GLOBUF_ERROR_WIN_MUTEX_CREATE] =
		"could not create win32 mutex";
	log[GLOBUF_ERROR_WIN_MUTEX_DESTROY] =
		"could not destroy win32 mutex";
	log[GLOBUF_ERROR_WIN_MUTEX_LOCK] =
		"could not lock mutex";
	log[GLOBUF_ERROR_WIN_MUTEX_UNLOCK] =
		"could not unlock mutex";
	log[GLOBUF_ERROR_WIN_MODULE_GET] =
		"could not get win32 window class module";
	log[GLOBUF_ERROR_WIN_CLASS_CREATE] =
		"could not create win32 window class";
	log[GLOBUF_ERROR_WIN_NAME_SET] =
		"could not set win32 window name";
	log[GLOBUF_ERROR_WIN_COND_WAIT] =
		"could not wait for win32 cond";
	log[GLOBUF_ERROR_WIN_THREAD_WAIT] =
		"could not wait for win32 thread";
	log[GLOBUF_ERROR_WIN_THREAD_EVENT_START] =
		"could not start win32 window event thread";
	log[GLOBUF_ERROR_WIN_THREAD_EVENT_CLOSE] =
		"could not close win32 window event thread";
	log[GLOBUF_ERROR_WIN_THREAD_RENDER_START] =
		"could not start win32 window render thread";
	log[GLOBUF_ERROR_WIN_THREAD_RENDER_CLOSE] =
		"could not close win32 window render thread";
	log[GLOBUF_ERROR_WIN_CURSOR_LOAD] =
		"could not load win32 window default cursor";
	log[GLOBUF_ERROR_WIN_PAINT_VALIDATE] =
		"could not validate paint operation";
	log[GLOBUF_ERROR_WIN_MONITOR_GET] =
		"could not get monitor";
	log[GLOBUF_ERROR_WIN_MONITOR_INFO_GET] =
		"could not get monitor info";
	log[GLOBUF_ERROR_WIN_WINDOW_POSITION_SET] =
		"could not set win32 window position";
	log[GLOBUF_ERROR_WIN_PLACEMENT_SET] =
		"could not set win32 window placement";
	log[GLOBUF_ERROR_WIN_STYLE_SET] =
		"could not set win32 window style";
	log[GLOBUF_ERROR_WIN_INTERACTION_SET] =
		"could not set win32 window interaction";
	log[GLOBUF_ERROR_WIN_STATE_SET] =
		"could not set win32 window state";
	log[GLOBUF_ERROR_WIN_TITLE_SET] =
		"could not set win32 window title";
	log[GLOBUF_ERROR_WIN_WINDOW_CREATE] =
		"could not create win32 window";
	log[GLOBUF_ERROR_WIN_USERDATA_SET] =
		"could not set win32 window user data";
	log[GLOBUF_ERROR_WIN_BMP_MASK_CREATE] =
		"could not create win32 bitmap mask";
	log[GLOBUF_ERROR_WIN_BMP_COLOR_CREATE] =
		"could not create win32 bitmap color map";
	log[GLOBUF_ERROR_WIN_ICON_CREATE] =
		"could not create win32 icon";
	log[GLOBUF_ERROR_WIN_ICON_SMALL] =
		"could not set win32 window small icon";
	log[GLOBUF_ERROR_WIN_ICON_BIG] =
		"could not set win32 window big icon";
	log[GLOBUF_ERROR_WIN_MSG_GET] =
		"could not get win32 window message";
	log[GLOBUF_ERROR_WIN_DEVICE_CONTEXT_GET] =
		"could not get win32 device context";
	log[GLOBUF_ERROR_WIN_DEVICE_CONTEXT_CREATE] =
		"could not create win32 device context";
	log[GLOBUF_ERROR_WIN_DEVICE_CONTEXT_DELETE] =
		"could not delete win32 device context";
	log[GLOBUF_ERROR_WIN_OBJECT_DELETE] =
		"could not delete win32 object";
	log[GLOBUF_ERROR_WIN_DWM_ENABLE] =
		"could not enable win32 DWM compositing";
	log[GLOBUF_ERROR_WIN_BMP_GET] =
		"could not get win32 bitmap";
	log[GLOBUF_ERROR_WIN_BMP_CREATE] =
		"could not create win32 bitmap";
	log[GLOBUF_ERROR_WIN_DIB_CREATE] =
		"could not create win32 device independent bitmap";
	log[GLOBUF_ERROR_WIN_BACKGROUND_BLUR] =
		"background blur was requested but is not supported";
	log[GLOBUF_ERROR_WIN_GDI_PAINT] =
		"could not create GDI paint struct";
	log[GLOBUF_ERROR_WIN_GDI_DAMAGE] =
		"could not damage GDI surface";
	log[GLOBUF_ERROR_WIN_GDI_BITBLT] =
		"could not blit GDI surface";
	log[GLOBUF_ERROR_WIN_PIXEL_FORMAT_SET] =
		"could not set win32 pixel format";
	log[GLOBUF_ERROR_WIN_PIXEL_FORMAT_CHOOSE] =
		"could not choose win32 pixel format";
	log[GLOBUF_ERROR_WIN_WGL_CONTEXT_SET] =
		"could not set win32 WGL context";
	log[GLOBUF_ERROR_WIN_WGL_CONTEXT_CREATE] =
		"could not create win32 WGL context";
	log[GLOBUF_ERROR_WIN_WGL_CONTEXT_DESTROY] =
		"could not destroy win32 WGL context";
	log[GLOBUF_ERROR_WIN_WGL_SWAP] =
		"could not swap WGL buffers";
	log[GLOBUF_ERROR_WIN_WGL_FUNC_LOAD] =
		"could not load WGL function pointers";
	log[GLOBUF_ERROR_WIN_VULKAN_EXTENSIONS_LIST] =
		"could not list Vulkan extensions";
	log[GLOBUF_ERROR_WIN_VULKAN_EXTENSION_UNAVAILABLE] =
		"could not find the Windows Vulkan extension";
	log[GLOBUF_ERROR_WIN_VULKAN_SURFACE_CREATE] =
		"could not create win32 Vulkan surface";

	log[GLOBUF_ERROR_MACOS_OBJ_NIL] =
		"could not get a valid objective-c object";
	log[GLOBUF_ERROR_MACOS_VULKAN_SURFACE_CREATE] =
		"failed to create Metal Vulkan surface";
	log[GLOBUF_ERROR_MACOS_VULKAN_EXTENSIONS_LIST] =
		"failed to list Vulkan extensions";
	log[GLOBUF_ERROR_MACOS_VULKAN_EXTENSION_UNAVAILABLE] =
		"could not find the Metal Vulkan extension";
	log[GLOBUF_ERROR_MACOS_EGL_SWAP] =
		"failed to swap AppKit EGL buffers";
	log[GLOBUF_ERROR_MACOS_EGL_DESTROY_SURFACE] =
		"failed to destroy AppKit EGL surface";
	log[GLOBUF_ERROR_MACOS_EGL_DESTROY_CONTEXT] =
		"failed to destroy AppKit EGL context";
	log[GLOBUF_ERROR_MACOS_EGL_TERMINATE] =
		"failed to terminate AppKit EGL";
	log[GLOBUF_ERROR_MACOS_EGL_DISPLAY_GET] =
		"failed to get AppKit EGL display";
	log[GLOBUF_ERROR_MACOS_EGL_INIT] =
		"failed to init AppKit EGL";
	log[GLOBUF_ERROR_MACOS_EGL_BIND_API] =
		"failed to bind AppKit EGL API";
	log[GLOBUF_ERROR_MACOS_EGL_CONFIG] =
		"failed to get AppKit EGL configuration";
	log[GLOBUF_ERROR_MACOS_EGL_CONTEXT_CREATE] =
		"failed to create AppKit EGL context";
	log[GLOBUF_ERROR_MACOS_EGL_CONFIG_ATTR] =
		"failed to create get AppKit EGL configuration attributes";
	log[GLOBUF_ERROR_MACOS_EGL_WINDOW_SURFACE] =
		"failed to create AppKit EGL window surface";
	log[GLOBUF_ERROR_MACOS_EGL_MAKE_CURRENT] =
		"failed to make AppKit EGL context current";
	log[GLOBUF_ERROR_MACOS_EGL_SWAP_INTERVAL] =
		"failed to set AppKit EGL swap interval";
#endif
}

void globuf_error_log(
	struct globuf* context,
	struct globuf_error_info* error)
{
#ifndef GLOBUF_ERROR_SKIP
	#ifdef GLOBUF_ERROR_LOG_MANUAL
		#ifdef GLOBUF_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				error->file,
				error->line);
		#endif

		if (error->code < GLOBUF_ERROR_COUNT)
		{
			fprintf(stderr, "%s\n", context->error_messages[error->code]);
		}
		else
		{
			fprintf(stderr, "%s\n", context->error_messages[0]);
		}
	#endif
#endif
}

const char* globuf_error_get_msg(
	struct globuf* context,
	struct globuf_error_info* error)
{
	if (error->code < GLOBUF_ERROR_COUNT)
	{
		return context->error_messages[error->code];
	}
	else
	{
		return context->error_messages[0];
	}
}

enum globuf_error globuf_error_get_code(
	struct globuf_error_info* error)
{
	return error->code;
}

const char* globuf_error_get_file(
	struct globuf_error_info* error)
{
	return error->file;
}

unsigned globuf_error_get_line(
	struct globuf_error_info* error)
{
	return error->line;
}

void globuf_error_ok(
	struct globuf_error_info* error)
{
	error->code = GLOBUF_ERROR_OK;
	error->file = "";
	error->line = 0;
}

void globuf_error_throw_extra(
	struct globuf* context,
	struct globuf_error_info* error,
	enum globuf_error code,
	const char* file,
	unsigned line)
{
#ifndef GLOBUF_ERROR_SKIP
	error->code = code;
	error->file = file;
	error->line = line;

	#ifdef GLOBUF_ERROR_LOG_THROW
		#ifdef GLOBUF_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				file,
				line);
		#endif

		if (error->code < GLOBUF_ERROR_COUNT)
		{
			fprintf(stderr, "%s\n", context->error_messages[error->code]);
		}
		else
		{
			fprintf(stderr, "%s\n", context->error_messages[0]);
		}
	#endif

	#ifdef GLOBUF_ERROR_ABORT
		abort();
	#endif
#endif
}
