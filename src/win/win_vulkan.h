#ifndef H_GLOBUF_INTERNAL_WIN_VULKAN
#define H_GLOBUF_INTERNAL_WIN_VULKAN

#include "include/globuf.h"
#include <stddef.h>

// # main API (globuf.h)
void globuf_win_vulkan_init(
	struct globuf* context,
	struct globuf_error_info* error);

void globuf_win_vulkan_clean(
	struct globuf* context,
	struct globuf_error_info* error);

void globuf_win_vulkan_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error);

void globuf_win_vulkan_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error);

void globuf_win_vulkan_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error);

void globuf_win_vulkan_window_start(
	struct globuf* context,
	struct globuf_error_info* error);

void globuf_win_vulkan_window_block(
	struct globuf* context,
	struct globuf_error_info* error);

void globuf_win_vulkan_window_stop(
	struct globuf* context,
	struct globuf_error_info* error);


void globuf_win_vulkan_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error);

void globuf_win_vulkan_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error);

enum globuf_event globuf_win_vulkan_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error);


struct globuf_config_features* globuf_win_vulkan_init_features(
	struct globuf* context,
	struct globuf_error_info* error);

void globuf_win_vulkan_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error);

void globuf_win_vulkan_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error);

void globuf_win_vulkan_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error);

void globuf_win_vulkan_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error);


unsigned globuf_win_vulkan_get_width(
	struct globuf* context,
	struct globuf_error_info* error);

unsigned globuf_win_vulkan_get_height(
	struct globuf* context,
	struct globuf_error_info* error);

struct globuf_rect globuf_win_vulkan_get_expose(
	struct globuf* context,
	struct globuf_error_info* error);


// for this backend, `data` is NULL
void globuf_win_vulkan_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error);

#endif
