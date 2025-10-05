#ifndef H_GLOBUF_LOADER_X11_SOFTWARE
#define H_GLOBUF_LOADER_X11_SOFTWARE
#if defined(GLOBUF_SHARED)

#include "globuf.h"
#include <stdint.h>
#include <xcb/xcb.h>

// pointers declaration
extern xcb_connection_t* (*globuf_get_x11_conn)(struct globuf* context);
extern xcb_window_t (*globuf_get_x11_window)(struct globuf* context);
extern xcb_window_t (*globuf_get_x11_root)(struct globuf* context);
extern xcb_screen_t* (*globuf_get_x11_screen)(struct globuf* context);

extern void (*globuf_prepare_init_x11_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);

// loader declaration
bool globuf_loader_x11_software(
	char* path_globuf_lib,
	int options);

#endif
#endif
