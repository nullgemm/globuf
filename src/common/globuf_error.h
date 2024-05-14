#ifndef H_GLOBUF_ERROR
#define H_GLOBUF_ERROR

#include "include/globuf.h"

#define globuf_error_throw(context, error, code) \
	globuf_error_throw_extra(\
		context,\
		error,\
		code,\
		GLOBUF_ERROR_FILE,\
		GLOBUF_ERROR_LINE)
#define GLOBUF_ERROR_FILE __FILE__
#define GLOBUF_ERROR_LINE __LINE__

void globuf_error_throw_extra(
	struct globuf* context,
	struct globuf_error_info* error,
	enum globuf_error code,
	const char* file,
	unsigned line);

void globuf_error_init(
	struct globuf* context);

#endif
