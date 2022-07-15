#include "include/globox.h"

#include <stdbool.h>
#include <stddef.h>

struct globox
{
	// common
	struct globox_config_features features;

	// not common but neutral in signature
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

unsigned globox_get_width(struct globox* context)
{
	// TODO mutex
	return context->size.width;
}

unsigned globox_get_height(struct globox* context)
{
	// TODO mutex
	return context->size.height;
}
