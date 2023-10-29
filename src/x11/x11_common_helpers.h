#ifndef H_GLOBOX_INTERNAL_X11_COMMON_HELPERS
#define H_GLOBOX_INTERNAL_X11_COMMON_HELPERS

#include "include/globox.h"
#include "x11/x11_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcb/xcb.h>

void* globox_x11_helpers_render_loop(
	void* data);

void* globox_x11_helpers_event_loop(
	void* data);

void globox_x11_helpers_features_init(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error);

void globox_x11_helpers_handle_interaction(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

void globox_x11_helpers_set_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

void globox_x11_helpers_set_title(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

void globox_x11_helpers_set_icon(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

void globox_x11_helpers_set_frame(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

void globox_x11_helpers_set_background(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

void globox_x11_helpers_set_vsync(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

enum globox_event globox_x11_helpers_get_state(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

void globox_x11_helpers_get_title(
	struct globox* context,
	struct x11_platform* platform,
	struct globox_error_info* error);

#ifdef GLOBOX_ERROR_HELPER_XCB
void globox_x11_helpers_xcb_error_log(
	struct globox* context,
	struct x11_platform* platform,
	xcb_generic_error_t* error);
#endif

#endif
