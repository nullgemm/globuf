#ifndef H_GLOBOX_ERROR
#define H_GLOBOX_ERROR

#include "include/globox.h"

#define globox_error_throw(context, error, code) \
	globox_error_throw_extra(\
		context,\
		error,\
		code,\
		GLOBOX_ERROR_FILE,\
		GLOBOX_ERROR_LINE)
#define GLOBOX_ERROR_FILE __FILE__
#define GLOBOX_ERROR_LINE __LINE__

void globox_error_throw_extra(
	struct globox* context,
	struct globox_error_info* error,
	enum globox_error code,
	const char* file,
	unsigned line);

void globox_error_init(
	struct globox* context);

#endif
