#ifndef H_GLOBOX_INTERNAL_X11_SOFTWARE
#define H_GLOBOX_INTERNAL_X11_SOFTWARE

#include "include/globox.h"

// # main API (globox.h)
void globox_x11_software_init(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_software_clean(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_software_window_create(
	struct globox* context,
	void** features,
	struct globox_error_info* error);

void globox_x11_software_window_destroy(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_software_window_start(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_software_window_block(
	struct globox* context,
	struct globox_error_info* error);

void globox_x11_software_window_stop(
	struct globox* context,
	struct globox_error_info* error);


void globox_x11_software_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error);

enum globox_event globox_x11_software_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error);


struct globox_config_features* globox_x11_software_init_features(
	struct globox* context,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_size(
	struct globox* context,
	struct globox_feature_size* config,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_pos(
	struct globox* context,
	struct globox_feature_pos* config,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_frame(
	struct globox* context,
	struct globox_feature_frame* config,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_background(
	struct globox* context,
	struct globox_feature_background* config,
	struct globox_error_info* error);

bool globox_x11_software_feature_set_vsync_callback(
	struct globox* context,
	struct globox_feature_vsync_callback* config,
	struct globox_error_info* error);

// for this backend, `data` is of type `struct globox_update_software*`
void globox_x11_software_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error);

#endif
