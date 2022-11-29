#include "include/globox.h"
#include "common/globox_private.h"
#include "common/globox_error.h"

#include <stdbool.h>

#if defined(GLOBOX_ERROR_LOG_MANUAL) || defined(GLOBOX_ERROR_LOG_THROW)
	#include <stdio.h>
#endif

#ifdef GLOBOX_ERROR_ABORT
	#include <stdlib.h>
#endif

void globox_error_init(
	struct globox* context)
{
#ifndef GLOBOX_ERROR_SKIP
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
	log[GLOBOX_ERROR_FEATURE_INVALID] =
		"invalid feature init request";
	log[GLOBOX_ERROR_FEATURE_UNAVAILABLE] =
		"failed to set one or more features";
	log[GLOBOX_ERROR_FEATURE_STATE_INVALID] =
		"failed to set state because the given value is invalid";

	log[GLOBOX_ERROR_POSIX_MUTEX_INIT] =
		"failed to init posix mutex";
	log[GLOBOX_ERROR_POSIX_MUTEX_DESTROY] =
		"failed to destroy posix mutex";
	log[GLOBOX_ERROR_POSIX_MUTEX_ATTR_INIT] =
		"failed to init posix mutex attributes";
	log[GLOBOX_ERROR_POSIX_MUTEX_ATTR_DESTROY] =
		"failed to destroy posix mutex attributes";
	log[GLOBOX_ERROR_POSIX_MUTEX_ATTR_SETTYPE] =
		"failed to set type in posix mutex attributes";
	log[GLOBOX_ERROR_POSIX_MUTEX_LOCK] =
		"failed to lock posix mutex";
	log[GLOBOX_ERROR_POSIX_MUTEX_UNLOCK] =
		"failed to unlock posix mutex";
	log[GLOBOX_ERROR_POSIX_COND_INIT] =
		"failed to init posix cond";
	log[GLOBOX_ERROR_POSIX_COND_WAIT] =
		"failed to unblock a posix cond";
	log[GLOBOX_ERROR_POSIX_COND_DESTROY] =
		"failed to destroy posix cond";
	log[GLOBOX_ERROR_POSIX_COND_ATTR_INIT] =
		"failed to init posix cond attributes";
	log[GLOBOX_ERROR_POSIX_COND_ATTR_DESTROY] =
		"failed to destroy posix cond attributes";
	log[GLOBOX_ERROR_POSIX_COND_ATTR_SETCLOCK] =
		"failed to set clock in posix cond attributes";
	log[GLOBOX_ERROR_POSIX_THREAD_CREATE] =
		"failed to create posix thread";
	log[GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT] =
		"failed to init posix thread attributes";
	log[GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY] =
		"failed to destroy posix thread attributes";
	log[GLOBOX_ERROR_POSIX_THREAD_ATTR_JOINABLE] =
		"failed to set joinable type in posix thread attributes";
	log[GLOBOX_ERROR_POSIX_THREAD_JOIN] =
		"failed to join posix thread";
	log[GLOBOX_ERROR_POSIX_SHM_VERSION] =
		"failed to get SHM version";
	log[GLOBOX_ERROR_POSIX_SHMDT] =
		"failed to delete SHM";
	log[GLOBOX_ERROR_POSIX_SHMID] =
		"failed to get SHM ID";
	log[GLOBOX_ERROR_POSIX_SHMCTL] =
		"failed to ctl SHM";
	log[GLOBOX_ERROR_POSIX_SHMADDR] =
		"failed to get SHM addr";

	log[GLOBOX_ERROR_X11_CONN] =
		"failed to connect to X11 server";
	log[GLOBOX_ERROR_X11_FLUSH] =
		"failed to flush X11 connection";
	log[GLOBOX_ERROR_X11_ATOM_GET] =
		"failed to get X11 atom";
	log[GLOBOX_ERROR_X11_WIN_CREATE] =
		"failed to create X11 window";
	log[GLOBOX_ERROR_X11_WIN_DESTROY] =
		"failed to destroy X11 window";
	log[GLOBOX_ERROR_X11_WIN_MAP] =
		"failed to map X11 window";
	log[GLOBOX_ERROR_X11_WIN_UNMAP] =
		"failed to unmap X11 window";
	log[GLOBOX_ERROR_X11_EVENT_WAIT] =
		"failed to get an X11 event";
	log[GLOBOX_ERROR_X11_EVENT_SEND] =
		"failed to send an X11 event";
	log[GLOBOX_ERROR_X11_PROP_CHANGE] =
		"failed to change an X11 property";
	log[GLOBOX_ERROR_X11_PROP_GET] =
		"failed to get an X11 property";
	log[GLOBOX_ERROR_X11_PROP_VALUE_GET] =
		"failed to get the value of an X11 property";
	log[GLOBOX_ERROR_X11_ATTR_CHANGE] =
		"failed to change an X11 attribute";
	log[GLOBOX_ERROR_X11_GC_CREATE] =
		"failed to create X11 graphical context";
	log[GLOBOX_ERROR_X11_PIXMAP] =
		"failed to get X11 pixmap";
	log[GLOBOX_ERROR_X11_SHM_PIXMAP] =
		"failed to get X11 SHM pixmap";
	log[GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE] =
		"failed to get a compatible X11 visual configuration";
	log[GLOBOX_ERROR_X11_VISUAL_MISSING] =
		"failed to find the requested X11 visual configuration";
	log[GLOBOX_ERROR_X11_SHM_DETACH] =
		"failed to attach X11 SHM";
	log[GLOBOX_ERROR_X11_SHM_DETACH] =
		"failed to detach X11 SHM";
	log[GLOBOX_ERROR_X11_IMAGE] =
		"failed to create X11 image";
	log[GLOBOX_ERROR_X11_COPY] =
		"failed to copy X11 buffer";
	log[GLOBOX_ERROR_X11_EVENT_INVALID] =
		"received invalid X11 event";
	log[GLOBOX_ERROR_X11_SYNC_COUNTER_CREATE] =
		"failed to create the X11 sync counter";
	log[GLOBOX_ERROR_X11_SYNC_COUNTER_SET] =
		"failed to set the X11 sync counter";
	log[GLOBOX_ERROR_X11_SYNC_COUNTER_DESTROY] =
		"failed to destroy the X11 sync counter";
	log[GLOBOX_ERROR_X11_GEOMETRY_GET] =
		"failed to get X11 window geometry";
	log[GLOBOX_ERROR_X11_TRANSLATE_COORDS] =
		"failed to translate X11 coords";
	log[GLOBOX_ERROR_X11_CONFIGURE] =
		"failed to configure X11 window";
	log[GLOBOX_ERROR_X11_QUERY_POINTER] =
		"failed to query X11 mouse pointer position";
	log[GLOBOX_ERROR_X11_GLX_FB_CONF_LIST] =
		"failed to get X11 GLX framebuffer configurations list";
	log[GLOBOX_ERROR_X11_GLX_FB_INVALID] =
		"failed to use X11 GLX framebuffer configuration";
	log[GLOBOX_ERROR_X11_GLX_FB_CONF_ATTR] =
		"failed to get X11 GLX framebuffer configuration attributes";
	log[GLOBOX_ERROR_X11_GLX_ATTR_ARB] =
		"failed to get X11 GLX Attributes ARB extension function pointer";
	log[GLOBOX_ERROR_X11_GLX_CONTEXT] =
		"failed to create a X11 GLX context";
	log[GLOBOX_ERROR_X11_GLX_WINDOW] =
		"failed to create X11 GLX window";
	log[GLOBOX_ERROR_X11_GLX_MAKE_CURRENT] =
		"failed to make X11 GLX context current";
	log[GLOBOX_ERROR_X11_GLX_DISPLAY_OPEN] =
		"failed to open X11 display with Xlib";
	log[GLOBOX_ERROR_X11_GLX_VSYNC] =
		"failed to get required X11 GLX VSync extension function pointer";
	log[GLOBOX_ERROR_X11_EGL_SWAP] =
		"failed to swap X11 EGL buffers";
	log[GLOBOX_ERROR_X11_EGL_DESTROY_SURFACE] =
		"failed to destroy X11 EGL surface";
	log[GLOBOX_ERROR_X11_EGL_DESTROY_CONTEXT] =
		"failed to destroy X11 EGL context";
	log[GLOBOX_ERROR_X11_EGL_TERMINATE] =
		"failed to terminate X11 EGL";
	log[GLOBOX_ERROR_X11_EGL_DISPLAY_GET] =
		"failed to get X11 EGL display";
	log[GLOBOX_ERROR_X11_EGL_INIT] =
		"failed to init X11 EGL";
	log[GLOBOX_ERROR_X11_EGL_BIND_API] =
		"failed to bind X11 EGL API";
	log[GLOBOX_ERROR_X11_EGL_CONFIG] =
		"failed to get X11 EGL configuration";
	log[GLOBOX_ERROR_X11_EGL_CONTEXT_CREATE] =
		"failed to create X11 EGL context";
	log[GLOBOX_ERROR_X11_EGL_CONFIG_ATTR] =
		"failed to create get X11 EGL configuration attributes";
	log[GLOBOX_ERROR_X11_EGL_WINDOW_SURFACE] =
		"failed to create X11 EGL window surface";
	log[GLOBOX_ERROR_X11_EGL_MAKE_CURRENT] =
		"failed to make X11 EGL context current";
	log[GLOBOX_ERROR_X11_EGL_SWAP_INTERVAL] =
		"failed to set X11 EGL swap interval";
#endif
}

void globox_error_log(
	struct globox* context,
	struct globox_error_info* error)
{
#ifndef GLOBOX_ERROR_SKIP
	#ifdef GLOBOX_ERROR_LOG_MANUAL
		#ifdef GLOBOX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				error->file,
				error->line);
		#endif

		if (error->code < GLOBOX_ERROR_COUNT)
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

const char* globox_error_get_msg(
	struct globox* context,
	struct globox_error_info* error)
{
	if (error->code < GLOBOX_ERROR_COUNT)
	{
		return context->error_messages[error->code];
	}
	else
	{
		return context->error_messages[0];
	}
}

enum globox_error globox_error_get_code(
	struct globox_error_info* error)
{
	return error->code;
}

const char* globox_error_get_file(
	struct globox_error_info* error)
{
	return error->file;
}

unsigned globox_error_get_line(
	struct globox_error_info* error)
{
	return error->line;
}

void globox_error_ok(
	struct globox_error_info* error)
{
	error->code = GLOBOX_ERROR_OK;
	error->file = "";
	error->line = 0;
}

void globox_error_throw_extra(
	struct globox* context,
	struct globox_error_info* error,
	enum globox_error code,
	const char* file,
	unsigned line)
{
#ifndef GLOBOX_ERROR_SKIP
	error->code = code;
	error->file = file;
	error->line = line;

	#ifdef GLOBOX_ERROR_LOG_THROW
		#ifdef GLOBOX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				file,
				line);
		#endif

		if (error->code < GLOBOX_ERROR_COUNT)
		{
			fprintf(stderr, "%s\n", context->error_messages[error->code]);
		}
		else
		{
			fprintf(stderr, "%s\n", context->error_messages[0]);
		}
	#endif

	#ifdef GLOBOX_ERROR_ABORT
		abort();
	#endif
#endif
}
