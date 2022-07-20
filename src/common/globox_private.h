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

	// common details not directly available prior to window creation
	struct globox_feature_interaction interaction;
	struct globox_feature_state state;

	// common details set using the window feature transaction system
	struct globox_feature_title title;
	struct globox_feature_icon icon;
	struct globox_feature_size size;
	struct globox_feature_pos pos;
	struct globox_feature_frame frame;
	struct globox_feature_background background;
	struct globox_feature_vsync_callback vsync_callback;

	// error handling
	enum globox_error error;
	char* error_messages[GLOBOX_ERROR_COUNT];
};

#endif
