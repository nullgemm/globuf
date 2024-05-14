#ifndef H_GLOBUF_INTERNAL_WAYLAND_COMMON_HELPERS
#define H_GLOBUF_INTERNAL_WAYLAND_COMMON_HELPERS

#include "include/globuf.h"
#include "wayland/wayland_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

void* globuf_wayland_helpers_render_loop(
	void* data);

void* globuf_wayland_helpers_event_loop(
	void* data);

void globuf_wayland_helpers_features_init(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	struct globuf_error_info* error);

void globuf_wayland_helpers_handle_interaction(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_helpers_set_state(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_helpers_set_title(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_helpers_set_icon(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_helpers_set_frame(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_helpers_set_background(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_helpers_set_vsync(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

enum globuf_event globuf_wayland_helpers_get_state(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_helpers_get_title(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

#endif
