#ifndef H_GLOBOX_INTERNAL_X11_SOFTWARE
#define H_GLOBOX_INTERNAL_X11_SOFTWARE

#include "globox.h"

// # main API (globox.h)
void globox_x11_software_init(
	struct globox* context);

void globox_x11_software_clean(
	struct globox* context);

void globox_x11_software_window_create(
	struct globox* context);

void globox_x11_software_window_destroy(
	struct globox* context);

void globox_x11_software_window_start(
	struct globox* context);

void globox_x11_software_window_stop(
	struct globox* context);

void globox_x11_software_init_features(
	struct globox* context,
	struct globox_config_features* config);

void globox_x11_software_init_events(
	struct globox* context,
	void (*handler)(void* data, void* event));

void globox_x11_software_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config);

void globox_x11_software_set_state(
	struct globox* context,
	struct globox_feature_state* config);

void globox_x11_software_set_title(
	struct globox* context,
	struct globox_feature_title* config);

void globox_x11_software_set_icon(
	struct globox* context,
	struct globox_feature_icon* config);

void globox_x11_software_set_init_size(
	struct globox* context,
	struct globox_feature_init_size* config);

void globox_x11_software_set_init_pos(
	struct globox* context,
	struct globox_feature_init_pos* config);

void globox_x11_software_set_frame(
	struct globox* context,
	struct globox_feature_frame* config);

void globox_x11_software_set_background(
	struct globox* context,
	struct globox_feature_background* config);

void globox_x11_software_set_vsync_callback(
	struct globox* context,
	struct globox_feature_vsync_callback* config);

// for this backend, `data` is of type `struct globox_update_software*`
void globox_x11_software_update_content(
	struct globox* context,
	void* data);

#endif
