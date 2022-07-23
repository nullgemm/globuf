#include "include/globox.h"
#include "common/globox_private.h"
#include "common/globox_error.h"

#include <stdbool.h>

#ifdef GLOBOX_ERROR_LOG_BASIC
	#include <stdio.h>
#endif

#ifdef GLOBOX_ERROR_ABORT
	#include <stdlib.h>
#endif

void globox_error_init(
	struct globox* context)
{
#ifndef GLOBOX_ERROR_SKIP
	context->error = GLOBOX_ERROR_OK;
	char** log = context->error_messages;

	log[GLOBOX_ERROR_OK] =
		"out-of-bound error message";
	log[GLOBOX_ERROR_NULL] =
		"null pointer";
	log[GLOBOX_ERROR_ALLOC] =
		"failed malloc";
	log[GLOBOX_ERROR_BOUNDS] =
		"out-of-bounds index";
	log[GLOBOX_ERROR_DOMAIN] =
		"invalid domain";
	log[GLOBOX_ERROR_FD] =
		"invalid file descriptor";

	log[GLOBOX_ERROR_POSIX_MUTEX_LOCK] =
		"failed to lock posix mutex";
	log[GLOBOX_ERROR_POSIX_MUTEX_UNLOCK] =
		"failed to unlock posix mutex";

	log[GLOBOX_ERROR_WAYLAND_EPOLL_CREATE] =
		"";
	log[GLOBOX_ERROR_WAYLAND_EPOLL_CTL] =
		"";
	log[GLOBOX_ERROR_WAYLAND_EPOLL_WAIT] =
		"";
	log[GLOBOX_ERROR_WAYLAND_DISPLAY] =
		"";
	log[GLOBOX_ERROR_WAYLAND_ROUNDTRIP] =
		"";
	log[GLOBOX_ERROR_WAYLAND_FLUSH] =
		"";
	log[GLOBOX_ERROR_WAYLAND_DISPATCH] =
		"";
	log[GLOBOX_ERROR_WAYLAND_MMAP] =
		"";
	log[GLOBOX_ERROR_WAYLAND_MUNMAP] =
		"";
	log[GLOBOX_ERROR_WAYLAND_REQUEST] =
		"";
	log[GLOBOX_ERROR_WAYLAND_LISTENER] =
		"";
	log[GLOBOX_ERROR_WAYLAND_EGL_FAIL] =
		"";

	log[GLOBOX_ERROR_X11_CONN] =
		"";
	log[GLOBOX_ERROR_X11_FLUSH] =
		"";
	log[GLOBOX_ERROR_X11_WIN] =
		"";
	log[GLOBOX_ERROR_X11_MAP] =
		"";
	log[GLOBOX_ERROR_X11_GC] =
		"";
	log[GLOBOX_ERROR_X11_VISUAL_NOT_COMPATIBLE] =
		"";
	log[GLOBOX_ERROR_X11_VISUAL_NOT_FOUND] =
		"";
	log[GLOBOX_ERROR_X11_PIXMAP] =
		"";
	log[GLOBOX_ERROR_X11_SHM_VERSION_REPLY] =
		"";
	log[GLOBOX_ERROR_X11_SHMID] =
		"";
	log[GLOBOX_ERROR_X11_SHMADDR] =
		"";
	log[GLOBOX_ERROR_X11_SHM_ATTACH] =
		"";
	log[GLOBOX_ERROR_X11_SHMCTL] =
		"";
	log[GLOBOX_ERROR_X11_SHM_PIXMAP] =
		"";
	log[GLOBOX_ERROR_X11_SHM_DETACH] =
		"";
	log[GLOBOX_ERROR_X11_SHMDT] =
		"";
	log[GLOBOX_ERROR_X11_EPOLL_CREATE] =
		"";
	log[GLOBOX_ERROR_X11_EPOLL_CTL] =
		"";
	log[GLOBOX_ERROR_X11_EPOLL_WAIT] =
		"";
	log[GLOBOX_ERROR_X11_WIN_ATTR] =
		"";
	log[GLOBOX_ERROR_X11_ATOMS] =
		"";
	log[GLOBOX_ERROR_X11_ICON] =
		"";
	log[GLOBOX_ERROR_X11_TITLE] =
		"";
	log[GLOBOX_ERROR_X11_IMAGE] =
		"";
	log[GLOBOX_ERROR_X11_COPY] =
		"";
	log[GLOBOX_ERROR_X11_SCREEN_INFO] =
		"";
	log[GLOBOX_ERROR_X11_WIN_INFO] =
		"";
	log[GLOBOX_ERROR_X11_STATE] =
		"";
	log[GLOBOX_ERROR_X11_INTERACTIVE] =
		"";
	log[GLOBOX_ERROR_X11_EGL_FAIL] =
		"";

	log[GLOBOX_ERROR_WINDOWS_SYM] =
		"";
	log[GLOBOX_ERROR_WINDOWS_MODULE_APP] =
		"";
	log[GLOBOX_ERROR_WINDOWS_MODULE_USER32] =
		"";
	log[GLOBOX_ERROR_WINDOWS_DELETE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_MESSAGE_GET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_CLASS_REGISTER] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_CREATE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_ADJUST] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_UPDATE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_RECT_GET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_POS_SET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_PLACEMENT_GET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_PLACEMENT_SET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_CLIENT_RECT] =
		"";
	log[GLOBOX_ERROR_WINDOWS_CLIENT_POS] =
		"";
	log[GLOBOX_ERROR_WINDOWS_TRANSPARENCY_REGION] =
		"";
	log[GLOBOX_ERROR_WINDOWS_TRANSPARENCY_DWM] =
		"";
	log[GLOBOX_ERROR_WINDOWS_COMP_ATTR] =
		"";
	log[GLOBOX_ERROR_WINDOWS_INPUT_SEND] =
		"";
	log[GLOBOX_ERROR_WINDOWS_CAPTURE_RELEASE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_LOAD] =
		"";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_POS_GET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_POS_SET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_BMP_MASK_CREATE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_BMP_COLOR_CREATE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_ICON_CREATE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_ICON_SMALL] =
		"";
	log[GLOBOX_ERROR_WINDOWS_ICON_BIG] =
		"";
	log[GLOBOX_ERROR_WINDOWS_UTF8] =
		"";
	log[GLOBOX_ERROR_WINDOWS_TITLE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_GDI_DAMAGE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_GDI_PAINT] =
		"";
	log[GLOBOX_ERROR_WINDOWS_GDI_BITBLT] =
		"";
	log[GLOBOX_ERROR_WINDOWS_GDI_DIB_CREATE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_GDI_BITMAP_SELECT] =
		"";
	log[GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_CREATE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_DELETE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_DELETE] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET] =
		"";
	log[GLOBOX_ERROR_WINDOWS_WGL_SWAP] =
		"";

	log[GLOBOX_ERROR_MACOS_CLASS_GET] =
		"";
	log[GLOBOX_ERROR_MACOS_CLASS_ALLOC] =
		"";
	log[GLOBOX_ERROR_MACOS_CLASS_ADDVAR] =
		"";
	log[GLOBOX_ERROR_MACOS_CLASS_ADDMETHOD] =
		"";
	log[GLOBOX_ERROR_MACOS_OBJ_INIT] =
		"";
	log[GLOBOX_ERROR_MACOS_OBJ_NIL] =
		"";
	log[GLOBOX_ERROR_MACOS_EGL_FAIL] =
		"";
#else
#endif
}

void globox_error_reset(
	struct globox* context)
{
#ifndef GLOBOX_ERROR_SKIP
	context->error = GLOBOX_ERROR_OK;
#endif
}

void globox_error_log(
	struct globox* context)
{
#ifdef GLOBOX_ERROR_LOG_BASIC
#ifndef GLOBOX_ERROR_SKIP
	if (context->error < GLOBOX_ERROR_COUNT)
	{
		fprintf(stderr, "%s\n", context->error_messages[context->error]);
	}
	else
	{
		fprintf(stderr, "%s\n", context->error_messages[0]);
	}
#endif
#endif
}

const char* globox_error_get_message(
	struct globox* context)
{
	if (context->error < GLOBOX_ERROR_COUNT)
	{
		return context->error_messages[context->error];
	}
	else
	{
		return context->error_messages[0];
	}
}

enum globox_error globox_error_get_code(
	struct globox* context)
{
	return context->error;
}

#ifdef GLOBOX_ERROR_LOG_DEBUG
void globox_error_throw_extra(
	struct globox* context,
	enum globox_error new_code,
	const char* file,
	unsigned int line)
#else
void globox_error_throw(
	struct globox* context,
	enum globox_error new_code)
#endif
{
#ifndef GLOBOX_ERROR_SKIP
	context->error = new_code;

	#ifdef GLOBOX_ERROR_LOG_THROW
	#ifdef GLOBOX_ERROR_LOG_BASIC
		#ifdef GLOBOX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				file,
				line);
		#endif

		globox_error_log(context);
	#endif
	#endif

	#ifdef GLOBOX_ERROR_ABORT
		abort();
	#endif
#endif
}

bool globox_error_catch(
	struct globox* context)
{
#ifndef GLOBOX_ERROR_SKIP
	return (context->error != GLOBOX_ERROR_OK);
#else
	return false;
#endif
}
