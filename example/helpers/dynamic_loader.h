#ifndef H_GLOBUF_LOADER_X11_SOFTWARE
#define H_GLOBUF_LOADER_X11_SOFTWARE
#if defined(GLOBUF_SHARED)

#include "globuf.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"

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
extern void (*cursoryx_prepare_init_x11)(struct cursoryx_config_backend* config);
extern void (*willis_prepare_init_x11)(struct willis_config_backend* config);
extern void (*dpishit_prepare_init_x11)(struct dpishit_config_backend* config);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
extern void (*globuf_prepare_init_x11_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_GLX)
extern void (*globuf_prepare_init_x11_glx)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_EGL)
extern void (*globuf_prepare_init_x11_egl)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
extern void (*globuf_prepare_init_x11_vulkan)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#endif
#elif defined(GLOBUF_EXAMPLE_APPKIT)
extern void (*cursoryx_prepare_init_appkit)(struct cursoryx_config_backend* config);
extern void (*willis_prepare_init_appkit)(struct willis_config_backend* config);
extern void (*dpishit_prepare_init_appkit)(struct dpishit_config_backend* config);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
extern void (*globuf_prepare_init_appkit_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_EGL)
extern double (*globuf_appkit_egl_get_scale)(
	struct globuf* context,
	struct globuf_error_info* error);
extern void (*globuf_prepare_init_appkit_egl)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
extern void (*globuf_prepare_init_appkit_vulkan)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#endif
#elif defined(GLOBUF_EXAMPLE_WIN)
extern void* (*globuf_get_win_surface)(struct globuf* context);
extern void (*cursoryx_prepare_init_win)(struct cursoryx_config_backend* config);
extern void (*willis_prepare_init_win)(struct willis_config_backend* config);
extern void (*dpishit_prepare_init_win)(struct dpishit_config_backend* config);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
extern void (*globuf_prepare_init_win_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_WGL)
extern void (*globuf_prepare_init_win_wgl)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
extern void (*globuf_prepare_init_win_vulkan)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#endif
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
extern bool (*globuf_add_wayland_capabilities_handler)(
	void* data,
	void (*capabilities_handler)(
		void* data,
		void* seat,
		uint32_t capabilities),
	void* capabilities_handler_data);
extern bool (*globuf_add_wayland_registry_handler)(
	void* data,
	void (*registry_handler)(
		void* data,
		void* registry,
		uint32_t name,
		const char* interface,
		uint32_t version),
	void* registry_handler_data);
extern bool (*globuf_add_wayland_registry_remover)(
	void* data,
	void (*registry_remover)(
		void* data,
		void* registry,
		uint32_t name),
	void* registry_remover_data);
extern void* (*globuf_get_wayland_surface)(
	struct globuf* context);
extern void (*cursoryx_prepare_init_wayland)(struct cursoryx_config_backend* config);
extern void (*willis_prepare_init_wayland)(struct willis_config_backend* config);
extern void (*dpishit_prepare_init_wayland)(struct dpishit_config_backend* config);
extern void (*dpishit_set_wayland_surface)(
	struct dpishit* context,
	void* surface,
	struct dpishit_error_info* error);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
extern void (*globuf_prepare_init_wayland_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_EGL)
extern void (*globuf_prepare_init_wayland_egl)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
extern void (*globuf_prepare_init_wayland_vulkan)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#endif
#endif

// loader declaration
bool dynamic_loader_globuf(char* path);
bool dynamic_loader_cursoryx(char* path);
bool dynamic_loader_willis(char* path);
bool dynamic_loader_dpishit(char* path);

#endif
#endif
