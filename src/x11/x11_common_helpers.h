#ifndef H_GLOBUF_INTERNAL_X11_COMMON_HELPERS
#define H_GLOBUF_INTERNAL_X11_COMMON_HELPERS

#include "include/globuf.h"
#include "x11/x11_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcb/xcb.h>

void* globuf_x11_helpers_render_loop(
	void* data);

void* globuf_x11_helpers_event_loop(
	void* data);

void globuf_x11_helpers_features_init(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	struct globuf_error_info* error);

void globuf_x11_helpers_handle_interaction(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_helpers_set_state(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_helpers_set_title(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_helpers_set_icon(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_helpers_set_frame(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_helpers_set_background(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_helpers_set_vsync(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

enum globuf_event globuf_x11_helpers_get_state(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_helpers_get_title(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

#ifdef GLOBUF_ERROR_HELPER_XCB
void globuf_x11_helpers_xcb_error_log(
	struct globuf* context,
	struct x11_platform* platform,
	xcb_generic_error_t* error);
#endif

#endif
