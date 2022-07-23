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
	log[GLOBOX_ERROR_POSIX_COND_DESTROY] =
		"failed to destroy posix cond";
	log[GLOBOX_ERROR_POSIX_COND_ATTR_INIT] =
		"failed to init posix cond attributes";
	log[GLOBOX_ERROR_POSIX_COND_ATTR_DESTROY] =
		"failed to destroy posix cond attributes";
	log[GLOBOX_ERROR_POSIX_COND_ATTR_SETTYPE] =
		"failed to set type in posix cond attributes";

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
