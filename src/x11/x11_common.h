#ifndef H_GLOBOX_INTERNAL_X11_COMMON
#define H_GLOBOX_INTERNAL_X11_COMMON

#include "include/globox.h"

#include <xcb.h>

// # private helpers
enum x11_atoms
{
	X11_ATOM_STATE_MAXIMIZED_HORIZONTAL = 0,
	X11_ATOM_STATE_MAXIMIZED_VERTICAL,
	X11_ATOM_STATE_FULLSCREEN,
	X11_ATOM_STATE_HIDDEN,
	X11_ATOM_STATE,
	X11_ATOM_ICON,
	X11_ATOM_HINTS_MOTIF,
	X11_ATOM_BLUR_KDE,
	X11_ATOM_BLUR_DEEPIN,
	X11_ATOM_PROTOCOLS,
	X11_ATOM_DELETE_WINDOW,
	X11_ATOM_COUNT,
};

struct x11_platform
{
	xcb_connection_t* conn;

	int screen_id;
	xcb_screen_t* screen_obj;
	xcb_window_t root_win;

	xcb_atom_t atoms[X11_ATOM_COUNT];
};

void globox_x11_common_init(struct x11_platform* platform);

void globox_x11_common_set_interaction(
	struct globox* context,
	struct globox_feature_request* request);

void globox_x11_common_set_state(
	struct globox* context,
	struct globox_feature_request* request);

void globox_x11_common_set_title(
	struct globox* context,
	struct globox_feature_request* request);

void globox_x11_common_set_icon(
	struct globox* context,
	struct globox_feature_request* request);

void globox_x11_common_set_size(
	struct globox* context,
	struct globox_feature_request* request);

void globox_x11_common_set_pos(
	struct globox* context,
	struct globox_feature_request* request);

void globox_x11_common_set_frame(
	struct globox* context,
	struct globox_feature_request* request);

void globox_x11_common_set_background(
	struct globox* context,
	struct globox_feature_request* request);

void globox_x11_common_set_vsync_callback(
	struct globox* context,
	struct globox_feature_request* request);

#endif
