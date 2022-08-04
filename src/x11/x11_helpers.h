#ifndef H_GLOBOX_INTERNAL_X11_HELPERS
#define H_GLOBOX_INTERNAL_X11_HELPERS

#include "include/globox.h"
#include "x11/x11_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcb/xcb.h>

void* x11_helpers_event_loop(
	void* data);

void x11_helpers_features_init(
	struct globox* context,
	struct x11_platform* platform,
	void** features);

bool x11_helpers_set_interaction(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

bool x11_helpers_set_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

bool x11_helpers_set_title(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

bool x11_helpers_set_icon(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

bool x11_helpers_set_size(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

bool x11_helpers_set_pos(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

bool x11_helpers_set_frame(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

bool x11_helpers_set_background(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

bool x11_helpers_set_vsync_callback(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

#endif
