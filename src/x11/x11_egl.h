#ifndef H_GLOBOX_INTERNAL_X11_EGL
#define H_GLOBOX_INTERNAL_X11_EGL

#include "include/globox.h"
#include <stddef.h>

// # main API (globox.h)
void globox_x11_egl_init(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_egl_clean(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_egl_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error);

void globox_x11_egl_window_destroy(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_egl_window_confirm(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_egl_window_start(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_egl_window_block(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_egl_window_stop(
	struct globox* context,
	struct globox_error_info* error);


void globox_x11_egl_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error);

void globox_x11_egl_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error);

enum globox_event globox_x11_egl_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error);


struct globox_config_features* globox_x11_egl_init_features(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_egl_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error);

void globox_x11_egl_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error);

void globox_x11_egl_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error);

void globox_x11_egl_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error);


unsigned globox_x11_egl_get_width(
	struct globox* context,
	struct globox_error_info* error);

unsigned globox_x11_egl_get_height(
	struct globox* context,
	struct globox_error_info* error);

struct globox_rect globox_x11_egl_get_expose(
	struct globox* context,
	struct globox_error_info* error);


// for this backend, `data` is NULL
void globox_x11_egl_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error);

#endif
