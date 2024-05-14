#ifndef H_GLOBUF_X11
#define H_GLOBUF_X11

#include "globuf.h"
#include <xcb/xcb.h>

xcb_connection_t* globuf_get_x11_conn(struct globuf* context);
xcb_window_t globuf_get_x11_window(struct globuf* context);
xcb_window_t globuf_get_x11_root(struct globuf* context);
xcb_screen_t* globuf_get_x11_screen(struct globuf* context);

#endif
