#ifndef H_GLOBUF_INTERNAL_APPKIT_COMMON
#define H_GLOBUF_INTERNAL_APPKIT_COMMON

#include "include/globuf.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#include <AppKit/AppKit.h>

// # private helpers
struct appkit_thread_render_loop_data
{
	struct globuf* globuf;
	struct appkit_platform* platform;
	struct globuf_error_info* error;
};

struct appkit_thread_event_loop_data
{
	struct globuf* globuf;
	struct appkit_platform* platform;
	struct globuf_error_info* error;
};

struct appkit_window_delegate_data
{
	struct globuf* globuf;
	struct appkit_platform* platform;
	struct globuf_error_info* error;
};

struct appkit_layer_delegate_data
{
	struct globuf* globuf;
	struct appkit_platform* platform;
	struct globuf_error_info* error;
};

struct appkit_platform
{
	pthread_mutex_t mutex_main;
	pthread_mutex_t mutex_block;
	pthread_cond_t cond_main;

	bool closed;
	id win_delegate;
	id win;
	NSView* view_master;
	NSVisualEffectView* view_blur;
	NSView* view;
	id layer_delegate;
	id layer;

	// saved action for globuf mouse action
	int32_t old_mouse_pos_x;
	int32_t old_mouse_pos_y;
	int32_t saved_mouse_pos_x;
	int32_t saved_mouse_pos_y;
	bool saved_window;
	NSRect saved_window_geometry;

	// render handling
	pthread_t thread_render_loop;
	struct appkit_thread_render_loop_data thread_render_loop_data;
	struct appkit_thread_event_loop_data thread_event_loop_data;
	struct appkit_window_delegate_data window_delegate_data;
	struct appkit_layer_delegate_data layer_delegate_data;
	void (*render_init_callback)(struct globuf*, struct globuf_error_info*);
};

void globuf_appkit_common_init(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

void globuf_appkit_common_clean(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

void globuf_appkit_common_window_create(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error);

void globuf_appkit_common_window_destroy(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

void globuf_appkit_common_window_confirm(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

void globuf_appkit_common_window_start(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

void globuf_appkit_common_window_block(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

void globuf_appkit_common_window_stop(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);


void globuf_appkit_common_init_render(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_config_render* config,
	struct globuf_error_info* error);

void globuf_appkit_common_init_events(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_config_events* config,
	struct globuf_error_info* error);

enum globuf_event globuf_appkit_common_handle_events(
	struct globuf* context,
	struct appkit_platform* platform,
	void* event,
	struct globuf_error_info* error);


struct globuf_config_features* globuf_appkit_common_init_features(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

void globuf_appkit_common_feature_set_interaction(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error);

void globuf_appkit_common_feature_set_state(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_feature_state* config,
	struct globuf_error_info* error);

void globuf_appkit_common_feature_set_title(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_feature_title* config,
	struct globuf_error_info* error);

void globuf_appkit_common_feature_set_icon(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error);


unsigned globuf_appkit_common_get_width(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

unsigned globuf_appkit_common_get_height(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

struct globuf_rect globuf_appkit_common_get_expose(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

#endif
