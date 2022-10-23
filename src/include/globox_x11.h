#ifndef H_GLOBOX_X11
#define H_GLOBOX_X11

#include <xcb/xcb.h>

xcb_connection_t* globox_get_x11_conn(struct globox* context);
xcb_window_t globox_get_x11_window(struct globox* context);
xcb_window_t globox_get_x11_root(struct globox* context);
xcb_screen_t* globox_get_x11_screen(struct globox* context);

#endif
