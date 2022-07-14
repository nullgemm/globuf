#include "globox.h"
#include "globox_egl.h"
#include "globox_x11_egl.h"
#include "x11_egl.h"

// # main API (globox.h)
void globox_x11_egl_init(
	struct globox* context)
{
}

void globox_x11_egl_clean(
	struct globox* context)
{
}

void globox_x11_egl_window_create(
	struct globox* context)
{
}

void globox_x11_egl_window_destroy(
	struct globox* context)
{
}

void globox_x11_egl_window_start(
	struct globox* context)
{
}

void globox_x11_egl_window_block(
	struct globox* context)
{
}

void globox_x11_egl_window_stop(
	struct globox* context)
{
}

void globox_x11_egl_init_features(
	struct globox* context,
	struct globox_config_features* config)
{
}

void globox_x11_egl_init_events(
	struct globox* context,
	void (*handler)(void* data, void* event))
{
}

enum globox_event globox_x11_egl_handle_events(
	struct globox* context,
	void* event)
{
}

void globox_x11_egl_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config)
{
}

void globox_x11_egl_set_state(
	struct globox* context,
	struct globox_feature_state* config)
{
}

void globox_x11_egl_set_title(
	struct globox* context,
	struct globox_feature_title* config)
{
}

void globox_x11_egl_set_icon(
	struct globox* context,
	struct globox_feature_icon* config)
{
}

void globox_x11_egl_set_init_size(
	struct globox* context,
	struct globox_feature_init_size* config)
{
}

void globox_x11_egl_set_init_pos(
	struct globox* context,
	struct globox_feature_init_pos* config)
{
}

void globox_x11_egl_set_frame(
	struct globox* context,
	struct globox_feature_frame* config)
{
}

void globox_x11_egl_set_background(
	struct globox* context,
	struct globox_feature_background* config)
{
}

void globox_x11_egl_set_vsync_callback(
	struct globox* context,
	struct globox_feature_vsync_callback* config)
{
}

void globox_x11_egl_update_content(
	struct globox* context,
	void* data)
{
}

// # platform-specific backend-specific API (globox_x11_egl.h)
void globox_prepare_init_x11_egl(
	struct globox* context,
	struct globox_config_backend* config)
{
}

// # platform-agnostic backend-specific API (globox_egl.h)
void globox_init_egl(
	struct globox* context,
	struct globox_config_egl* config)
{
}
