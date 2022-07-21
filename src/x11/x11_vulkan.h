#ifndef H_GLOBOX_INTERNAL_X11_VULKAN
#define H_GLOBOX_INTERNAL_X11_VULKAN

#include "include/globox.h"

// # main API (globox.h)
void globox_x11_vulkan_init(
	struct globox* context);

void globox_x11_vulkan_clean(
	struct globox* context);

void globox_x11_vulkan_window_create(
	struct globox* context);

void globox_x11_vulkan_window_destroy(
	struct globox* context);

void globox_x11_vulkan_window_start(
	struct globox* context);

void globox_x11_vulkan_window_block(
	struct globox* context);

void globox_x11_vulkan_window_stop(
	struct globox* context);

void globox_x11_vulkan_init_features(
	struct globox* context,
	struct globox_config_features* config);

void globox_x11_vulkan_init_events(
	struct globox* context,
	void (*handler)(void* data, void* event));

enum globox_event globox_x11_vulkan_handle_events(
	struct globox* context,
	void* event);

void globox_x11_vulkan_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config);

void globox_x11_vulkan_set_state(
	struct globox* context,
	struct globox_feature_state* config);

void globox_x11_vulkan_set_title(
	struct globox* context,
	struct globox_feature_title* config);

void globox_x11_vulkan_set_icon(
	struct globox* context,
	struct globox_feature_icon* config);

void globox_x11_vulkan_set_size(
	struct globox* context,
	struct globox_feature_size* config);

void globox_x11_vulkan_set_pos(
	struct globox* context,
	struct globox_feature_pos* config);

void globox_x11_vulkan_set_frame(
	struct globox* context,
	struct globox_feature_frame* config);

void globox_x11_vulkan_set_background(
	struct globox* context,
	struct globox_feature_background* config);

void globox_x11_vulkan_set_vsync_callback(
	struct globox* context,
	struct globox_feature_vsync_callback* config);

// for this backend, `data` is unused
void globox_x11_vulkan_update_content(
	struct globox* context,
	void* data);

#endif
