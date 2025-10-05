#ifndef H_GLOBUF_LOADER_X11_SOFTWARE
#define H_GLOBUF_LOADER_X11_SOFTWARE
#if defined(GLOBUF_SHARED)

#include "globuf.h"
#include <stdint.h>

#if defined(GLOBUF_EXAMPLE_X11)
#include <xcb/xcb.h>
#elif defined(GLOBUF_EXAMPLE_APPKIT)
#elif defined(GLOBUF_EXAMPLE_WIN)
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
#endif

// pointers declaration
#if defined(GLOBUF_EXAMPLE_X11)
extern xcb_connection_t* (*globuf_get_x11_conn)(struct globuf* context);
extern xcb_window_t (*globuf_get_x11_window)(struct globuf* context);
extern xcb_window_t (*globuf_get_x11_root)(struct globuf* context);
extern xcb_screen_t* (*globuf_get_x11_screen)(struct globuf* context);

extern void (*globuf_prepare_init_x11_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
#elif defined(GLOBUF_EXAMPLE_APPKIT)
#elif defined(GLOBUF_EXAMPLE_WIN)
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
#endif

// loader declaration
bool globuf_loader_x11_software(
	char* path_globuf_lib,
	int options);

#endif
#endif
