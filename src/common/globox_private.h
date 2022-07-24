#ifndef H_GLOBOX_PRIVATE
#define H_GLOBOX_PRIVATE

#include "include/globox.h"
#include "common/globox_error.h"

struct globox
{
	// common
	struct globox_config_features features;

	// not common but neutral in signature
	void* backend_data;
	struct globox_config_events event_callbacks;
	struct globox_config_backend backend_callbacks;
	struct globox_feature_data feature_data[GLOBOX_FEATURE_COUNT];

	// error handling
	enum globox_error error;
	char* error_messages[GLOBOX_ERROR_COUNT];
};

#endif
