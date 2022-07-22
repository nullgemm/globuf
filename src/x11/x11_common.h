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

#endif
