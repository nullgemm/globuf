#ifndef H_GLOBOX_INTERNAL_X11_SOFTWARE
#define H_GLOBOX_INTERNAL_X11_SOFTWARE

#include "include/globox.h"

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

void globox_x11_software_window_block(
	struct globox* context);

void globox_x11_software_window_stop(
	struct globox* context);


void globox_x11_software_init_events(
	struct globox* context,
	void (*handler)(void* data, void* event));

enum globox_event globox_x11_software_handle_events(
	struct globox* context,
	void* event);

struct globox_config_features* globox_x11_software_init_features(
	struct globox* context);

void globox_x11_software_set_feature(
	struct globox* context,
	struct globox_feature_request* request);

// for this backend, `data` is of type `struct globox_update_software*`
void globox_x11_software_update_content(
	struct globox* context,
	void* data);

#endif
