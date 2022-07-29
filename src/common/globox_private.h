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

	// error handling
	char* error_messages[GLOBOX_ERROR_COUNT];

	// features
	struct globox_feature_interaction* feature_interaction;
	struct globox_feature_state* feature_state;
	struct globox_feature_title* feature_title;
	struct globox_feature_icon* feature_icon;
	struct globox_feature_size* feature_size;
	struct globox_feature_pos* feature_pos;
	struct globox_feature_frame* feature_frame;
	struct globox_feature_background* feature_background;
	struct globox_feature_vsync_callback* feature_vsync_callback;
};

#endif
