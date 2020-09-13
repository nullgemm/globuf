/// this file implements all globox error-handling functions

#include "globox.h"
#include "globox_error.h"

#ifdef GLOBOX_ERROR_LOG_BASIC
	#include <stdio.h>
#endif

#ifdef GLOBOX_ERROR_ABORT
	#include <stdlib.h>
#endif

void globox_error_init(struct globox* globox)
{
#ifndef GLOBOX_ERROR_SKIP
	globox->globox_error = GLOBOX_ERROR_OK;
#else
#endif
}

inline void globox_error_reset(struct globox* globox)
{
#ifndef GLOBOX_ERROR_SKIP
	globox->globox_error = GLOBOX_ERROR_OK;
#endif
}

inline void globox_error_basic_log(struct globox* globox)
{
#ifdef GLOBOX_ERROR_LOG_BASIC
#ifndef GLOBOX_ERROR_SKIP
	if (globox->globox_error < GLOBOX_ERROR_SIZE)
	{
		fprintf(stderr, "%s\n", globox->globox_log[globox->globox_error]);
	}
	else
	{
		fprintf(stderr, "%s\n", globox->globox_log[0]);
	}
#endif
#endif
}

inline char* globox_error_output_log(struct globox* globox)
{
	if (globox->globox_error < GLOBOX_ERROR_SIZE)
	{
		return globox->globox_log[globox->globox_error];
	}
	else
	{
		return globox->globox_log[0];
	}
}

enum globox_error globox_error_output_code(struct globox* globox)
{
	return globox->globox_error;
}

#ifdef GLOBOX_ERROR_LOG_DEBUG
inline void globox_error_throw_extra(
	struct globox* globox,
	enum globox_error new_code,
	const char* file,
	unsigned int line)
#else
inline void globox_error_throw(
	struct globox* globox,
	enum globox_error new_code)
#endif
{
#ifndef GLOBOX_ERROR_SKIP
	globox->globox_error = new_code;

	#ifdef GLOBOX_ERROR_LOG_THROW
	#ifdef GLOBOX_ERROR_LOG_BASIC
		#ifdef GLOBOX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				file,
				line);
		#endif

		globox_error_basic_log(globox);
	#endif
	#endif

	#ifdef GLOBOX_ERROR_ABORT
		abort();
	#endif
#endif
}

inline char globox_error_catch(struct globox* globox)
{
#ifndef GLOBOX_ERROR_SKIP
	return (globox->globox_error != GLOBOX_ERROR_OK);
#else
	return 0;
#endif
}
