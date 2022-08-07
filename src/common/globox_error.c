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
	log[GLOBOX_ERROR_POSIX_COND_ATTR_SETTYPE] =
		"failed to set type in posix cond attributes";
	log[GLOBOX_ERROR_POSIX_THREAD_CREATE] =
		"failed to create posix thread";
	log[GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT] =
		"failed to init posix thread attributes";
	log[GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY] =
		"failed to destroy posix thread attributes";
	log[GLOBOX_ERROR_POSIX_THREAD_ATTR_DETACH] =
		"failt to set detach type in posix thread attributes";
	log[GLOBOX_ERROR_POSIX_SHM_VERSION] =
		"failed to get SHM version";
	log[GLOBOX_ERROR_POSIX_SHMDT] =
		"failed to delete SHM";

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
	log[GLOBOX_ERROR_X11_SHM_DETACH] =
		"failed to detach X11 SHM";
#endif
}

void globox_error_log(
	struct globox* context,
	struct globox_error_info* error)
{
#ifdef GLOBOX_ERROR_LOG_BASIC
#ifndef GLOBOX_ERROR_SKIP
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
	#ifdef GLOBOX_ERROR_LOG_BASIC
		#ifdef GLOBOX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				file,
				line);
		#endif

		globox_error_log(context, error);
	#endif
	#endif

	#ifdef GLOBOX_ERROR_ABORT
		abort();
	#endif
#endif
}
