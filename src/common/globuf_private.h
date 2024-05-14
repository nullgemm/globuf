#ifndef H_GLOBUF_PRIVATE
#define H_GLOBUF_PRIVATE

#include "include/globuf.h"
#include "common/globuf_error.h"

struct globuf
{
	// common
	struct globuf_config_features features;

	// not common but neutral in signature
	void* backend_data;
	struct globuf_config_render render_callback;
	struct globuf_config_events event_callbacks;
	struct globuf_config_backend backend_callbacks;
	struct globuf_rect expose;

	// error handling
	char* error_messages[GLOBUF_ERROR_COUNT];

	// features
	struct globuf_feature_interaction* feature_interaction;
	struct globuf_feature_state* feature_state;
	struct globuf_feature_title* feature_title;
	struct globuf_feature_icon* feature_icon;
	struct globuf_feature_size* feature_size;
	struct globuf_feature_pos* feature_pos;
	struct globuf_feature_frame* feature_frame;
	struct globuf_feature_background* feature_background;
	struct globuf_feature_vsync* feature_vsync;
};

#endif
