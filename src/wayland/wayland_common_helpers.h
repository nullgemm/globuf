#ifndef H_GLOBOX_INTERNAL_WAYLAND_COMMON_HELPERS
#define H_GLOBOX_INTERNAL_WAYLAND_COMMON_HELPERS

#include "include/globox.h"
#include "wayland/wayland_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcb/xcb.h>

void* wayland_helpers_render_loop(
	void* data);

void* wayland_helpers_event_loop(
	void* data);

void wayland_helpers_features_init(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error);

void wayland_helpers_handle_interaction(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

void wayland_helpers_set_state(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

void wayland_helpers_set_title(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

void wayland_helpers_set_icon(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

void wayland_helpers_set_frame(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

void wayland_helpers_set_background(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

void wayland_helpers_set_vsync(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

enum globox_event wayland_helpers_get_state(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

void wayland_helpers_get_title(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error);

#endif
