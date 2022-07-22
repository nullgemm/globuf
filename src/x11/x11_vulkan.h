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

struct globox_config_features* globox_x11_vulkan_init_features(
	struct globox* context);

void globox_x11_vulkan_init_events(
	struct globox* context,
	void (*handler)(void* data, void* event));

enum globox_event globox_x11_vulkan_handle_events(
	struct globox* context,
	void* event);

void globox_x11_vulkan_set_feature(
	struct globox* context,
	struct globox_feature_request* request);

// for this backend, `data` is unused
void globox_x11_vulkan_update_content(
	struct globox* context,
	void* data);

#endif
