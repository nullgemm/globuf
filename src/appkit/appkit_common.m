#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "include/globox_appkit.h"
#include "common/globox_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void globox_appkit_common_init(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_clean(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_window_create(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
}

void globox_appkit_common_window_destroy(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_window_start(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_window_block(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_window_stop(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}


void globox_appkit_common_init_render(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	// set the event callback
#if 0
	context->render_callback = *config;
#endif
	globox_error_ok(error);
}

void globox_appkit_common_init_events(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	// set the event callback
#if 0
	context->event_callbacks = *config;
#endif
	globox_error_ok(error);
}

enum globox_event globox_appkit_common_handle_events(
	struct globox* context,
	struct appkit_platform* platform,
	void* event,
	struct globox_error_info* error)
{
	// process system events
	enum globox_event globox_event = GLOBOX_EVENT_UNKNOWN;
	globox_error_ok(error);
	return globox_event;
}

struct globox_config_features*
	globox_appkit_common_init_features(
		struct globox* context,
		struct appkit_platform* platform,
		struct globox_error_info* error)
{
	globox_error_ok(error);
	return NULL;
}

void globox_appkit_common_feature_set_interaction(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_feature_set_state(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_feature_set_title(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void globox_appkit_common_feature_set_icon(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

unsigned globox_appkit_common_get_width(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
	return 0;
}

unsigned globox_appkit_common_get_height(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
	return 0;
}

struct globox_rect globox_appkit_common_get_expose(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error)
{
	struct globox_rect dummy =
	{
		.x = 0,
		.y = 0,
		.width = 0,
		.height = 0,
	};

	globox_error_ok(error);
	return dummy;
}
