#ifndef H_GLOBOX_INTERNAL_APPKIT_COMMON_HELPERS
#define H_GLOBOX_INTERNAL_APPKIT_COMMON_HELPERS

#include "include/globox.h"
#include "appkit/appkit_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

void globox_appkit_helpers_features_init(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error);

#endif
