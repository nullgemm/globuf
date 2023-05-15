#ifndef H_GLOBOX_INTERNAL_APPKIT_COMMON
#define H_GLOBOX_INTERNAL_APPKIT_COMMON

#include "include/globox.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

// # private helpers
struct appkit_thread_render_loop_data
{
	struct globox* globox;
	struct appkit_platform* platform;
	struct globox_error_info* error;
};

struct appkit_thread_event_loop_data
{
	struct globox* globox;
	struct appkit_platform* platform;
	struct globox_error_info* error;
};

struct appkit_platform
{
	pthread_mutex_t mutex_main;
	pthread_mutex_t mutex_block;
	pthread_cond_t cond_main;

	bool closed;
	id win;
	id view;
	id layer;

	// render handling
	pthread_t thread_render_loop;
	struct appkit_thread_render_loop_data thread_render_loop_data;
	struct appkit_thread_event_loop_data thread_event_loop_data;
	void (*render_init_callback)(struct globox*, struct globox_error_info*);
};

void globox_appkit_common_init(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

void globox_appkit_common_clean(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

void globox_appkit_common_window_create(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error);

void globox_appkit_common_window_destroy(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

void globox_appkit_common_window_start(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

void globox_appkit_common_window_block(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

void globox_appkit_common_window_stop(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);


void globox_appkit_common_init_render(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_render* config,
	struct globox_error_info* error);

void globox_appkit_common_init_events(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_events* config,
	struct globox_error_info* error);

enum globox_event globox_appkit_common_handle_events(
	struct globox* context,
	struct appkit_platform* platform,
	void* event,
	struct globox_error_info* error);


struct globox_config_features* globox_appkit_common_init_features(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

void globox_appkit_common_feature_set_interaction(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_interaction* config,
	struct globox_error_info* error);

void globox_appkit_common_feature_set_state(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_state* config,
	struct globox_error_info* error);

void globox_appkit_common_feature_set_title(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_title* config,
	struct globox_error_info* error);

void globox_appkit_common_feature_set_icon(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_feature_icon* config,
	struct globox_error_info* error);


unsigned globox_appkit_common_get_width(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

unsigned globox_appkit_common_get_height(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

struct globox_rect globox_appkit_common_get_expose(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_error_info* error);

#endif
