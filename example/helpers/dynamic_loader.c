#if defined(GLOBUF_SHARED)

#include "globuf.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"
#include "dynamic_loader.h"

#if !defined(GLOBUF_EXAMPLE_WIN)
#include <dlfcn.h>
#else
#include <libloaderapi.h>
#endif

#include <stdbool.h>
#include <stdio.h>

#if defined(GLOBUF_EXAMPLE_X11)
#include <xcb/xcb.h>
#endif

// pointers declaration
#if defined(GLOBUF_EXAMPLE_X11)
xcb_connection_t* (*globuf_get_x11_conn)(struct globuf* context);
xcb_window_t (*globuf_get_x11_window)(struct globuf* context);
xcb_window_t (*globuf_get_x11_root)(struct globuf* context);
xcb_screen_t* (*globuf_get_x11_screen)(struct globuf* context);
void (*cursoryx_prepare_init_x11)(struct cursoryx_config_backend* config);
void (*willis_prepare_init_x11)(struct willis_config_backend* config);
void (*dpishit_prepare_init_x11)(struct dpishit_config_backend* config);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
void (*globuf_prepare_init_x11_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_GLX)
void (*globuf_prepare_init_x11_glx)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_EGL)
void (*globuf_prepare_init_x11_egl)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
void (*globuf_prepare_init_x11_vulkan)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#endif
#elif defined(GLOBUF_EXAMPLE_APPKIT)
void (*cursoryx_prepare_init_appkit)(struct cursoryx_config_backend* config);
void (*willis_prepare_init_appkit)(struct willis_config_backend* config);
void (*dpishit_prepare_init_appkit)(struct dpishit_config_backend* config);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
void (*globuf_prepare_init_appkit_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_EGL)
double (*globuf_appkit_egl_get_scale)(
	struct globuf* context,
	struct globuf_error_info* error);
void (*globuf_prepare_init_appkit_egl)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
void (*globuf_prepare_init_appkit_vulkan)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#endif
#elif defined(GLOBUF_EXAMPLE_WIN)
void* (*globuf_get_win_surface)(struct globuf* context);
void (*cursoryx_prepare_init_win)(struct cursoryx_config_backend* config);
void (*willis_prepare_init_win)(struct willis_config_backend* config);
void (*dpishit_prepare_init_win)(struct dpishit_config_backend* config);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
void (*globuf_prepare_init_win_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_WGL)
void (*globuf_prepare_init_win_wgl)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
void (*globuf_prepare_init_win_vulkan)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#endif
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
bool (*globuf_add_wayland_capabilities_handler)(
	void* data,
	void (*capabilities_handler)(
		void* data,
		void* seat,
		uint32_t capabilities),
	void* capabilities_handler_data);
bool (*globuf_add_wayland_registry_handler)(
	void* data,
	void (*registry_handler)(
		void* data,
		void* registry,
		uint32_t name,
		const char* interface,
		uint32_t version),
	void* registry_handler_data);
bool (*globuf_add_wayland_registry_remover)(
	void* data,
	void (*registry_remover)(
		void* data,
		void* registry,
		uint32_t name),
	void* registry_remover_data);
void* (*globuf_get_wayland_surface)(
	struct globuf* context);
void (*cursoryx_prepare_init_wayland)(struct cursoryx_config_backend* config);
void (*willis_prepare_init_wayland)(struct willis_config_backend* config);
void (*dpishit_prepare_init_wayland)(struct dpishit_config_backend* config);
void (*dpishit_set_wayland_surface)(
	struct dpishit* context,
	void* surface,
	struct dpishit_error_info* error);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
void (*globuf_prepare_init_wayland_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_EGL)
void (*globuf_prepare_init_wayland_egl)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
void (*globuf_prepare_init_wayland_vulkan)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#endif
#endif

// symbol table
struct link
{
	void (**func)();
	char* sym;
};

static struct link table_globuf[] =
{
	// let's load only required symbols for the example in use
#if defined(GLOBUF_EXAMPLE_X11)
	{(void(**)()) &globuf_get_x11_conn, "globuf_get_x11_conn"},
	{(void(**)()) &globuf_get_x11_window, "globuf_get_x11_window"},
	{(void(**)()) &globuf_get_x11_root, "globuf_get_x11_root"},
	{(void(**)()) &globuf_get_x11_screen, "globuf_get_x11_screen"},
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
	{(void(**)()) &globuf_prepare_init_x11_software, "globuf_prepare_init_x11_software"},
	#elif defined(GLOBUF_EXAMPLE_GLX)
	{(void(**)()) &globuf_prepare_init_x11_glx, "globuf_prepare_init_x11_glx"},
	#elif defined(GLOBUF_EXAMPLE_EGL)
	{(void(**)()) &globuf_prepare_init_x11_egl, "globuf_prepare_init_x11_egl"},
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
	{(void(**)()) &globuf_prepare_init_x11_vulkan, "globuf_prepare_init_x11_vulkan"},
	#endif
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
	{(void(**)()) &globuf_prepare_init_appkit_software, "globuf_prepare_init_appkit_software"},
	#elif defined(GLOBUF_EXAMPLE_EGL)
	{(void(**)()) &globuf_appkit_egl_get_scale, "globuf_appkit_egl_get_scale"},
	{(void(**)()) &globuf_prepare_init_appkit_egl, "globuf_prepare_init_appkit_egl"},
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
	{(void(**)()) &globuf_prepare_init_appkit_vulkan, "globuf_prepare_init_appkit_vulkan"},
	#endif
#elif defined(GLOBUF_EXAMPLE_WIN)
	{(void(**)()) &globuf_get_win_surface, "globuf_get_win_surface"},
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
	{(void(**)()) &globuf_prepare_init_win_software, "globuf_prepare_init_win_software"},
	#elif defined(GLOBUF_EXAMPLE_WGL)
	{(void(**)()) &globuf_prepare_init_win_wgl, "globuf_prepare_init_win_wgl"},
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
	{(void(**)()) &globuf_prepare_init_win_vulkan, "globuf_prepare_init_win_vulkan"},
	#endif
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	{(void(**)()) &globuf_add_wayland_capabilities_handler, "globuf_add_wayland_capabilities_handler"},
	{(void(**)()) &globuf_add_wayland_registry_handler, "globuf_add_wayland_registry_handler"},
	{(void(**)()) &globuf_add_wayland_registry_remover, "globuf_add_wayland_registry_remover"},
	{(void(**)()) &globuf_get_wayland_surface, "globuf_get_wayland_surface"},
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
	{(void(**)()) &globuf_prepare_init_wayland_software, "globuf_prepare_init_wayland_software"},
	#elif defined(GLOBUF_EXAMPLE_EGL)
	{(void(**)()) &globuf_prepare_init_wayland_egl, "globuf_prepare_init_wayland_egl"},
	#elif defined(GLOBUF_EXAMPLE_VULKAN)
	{(void(**)()) &globuf_prepare_init_wayland_vulkan, "globuf_prepare_init_wayland_vulkan"},
	#endif
#endif
	// list terminator
	{NULL, NULL},
};

static struct link table_cursoryx[] =
{
#if defined(GLOBUF_EXAMPLE_X11)
	{(void(**)()) &cursoryx_prepare_init_x11, "cursoryx_prepare_init_x11"},
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	{(void(**)()) &cursoryx_prepare_init_wayland, "cursoryx_prepare_init_wayland"},
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	{(void(**)()) &cursoryx_prepare_init_appkit, "cursoryx_prepare_init_appkit"},
#elif defined(GLOBUF_EXAMPLE_WIN)
	{(void(**)()) &cursoryx_prepare_init_win, "cursoryx_prepare_init_win"},
#endif
	// list terminator
	{NULL, NULL},
};

static struct link table_willis[] =
{
#if defined(GLOBUF_EXAMPLE_X11)
	{(void(**)()) &willis_prepare_init_x11, "willis_prepare_init_x11"},
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	{(void(**)()) &willis_prepare_init_wayland, "willis_prepare_init_wayland"},
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	{(void(**)()) &willis_prepare_init_appkit, "willis_prepare_init_appkit"},
#elif defined(GLOBUF_EXAMPLE_WIN)
	{(void(**)()) &willis_prepare_init_win, "willis_prepare_init_win"},
#endif
	// list terminator
	{NULL, NULL},
};

static struct link table_dpishit[] =
{
#if defined(GLOBUF_EXAMPLE_X11)
	{(void(**)()) &dpishit_prepare_init_x11, "dpishit_prepare_init_x11"},
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	{(void(**)()) &dpishit_prepare_init_wayland, "dpishit_prepare_init_wayland"},
	{(void(**)()) &dpishit_set_wayland_surface, "dpishit_set_wayland_surface"},
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	{(void(**)()) &dpishit_prepare_init_appkit, "dpishit_prepare_init_appkit"},
#elif defined(GLOBUF_EXAMPLE_WIN)
	{(void(**)()) &dpishit_prepare_init_win, "dpishit_prepare_init_win"},
#endif
	// list terminator
	{NULL, NULL},
};

// loader implementation
static bool dynamic_loader(char* path_globuf_lib, struct link* table)
{
#if !defined(GLOBUF_EXAMPLE_WIN)
	void* globuf_lib = dlopen(path_globuf_lib, RTLD_NOW);
#else
	HMODULE globuf_lib = LoadLibraryExA(path_globuf_lib, NULL, 0);
#endif

	if (globuf_lib == NULL)
	{
#ifndef GLOBUF_ERROR_SKIP
		fprintf(stderr, "could not load object %s\n", path_globuf_lib);
#if !defined(GLOBUF_EXAMPLE_WIN)
		fprintf(stderr, "dlopen error: %s\n", dlerror());
#endif
#endif
		return false;
	}

	size_t i = 0;

	while ((table[i].func != NULL) && (table[i].sym != NULL))
	{
#if !defined(GLOBUF_EXAMPLE_WIN)
		void(*lol)() = dlsym(globuf_lib, table[i].sym);
#else
		void(*lol)() = (void(*)()) GetProcAddress(globuf_lib, table[i].sym);
#endif

		if (lol == NULL)
		{
#ifndef GLOBUF_ERROR_SKIP
			fprintf(stderr, "could not load symbol %s in object %s\n", table[i].sym, path_globuf_lib);
#if !defined(GLOBUF_EXAMPLE_WIN)
			fprintf(stderr, "dlsym error: %s\n", dlerror());
#endif
#endif
			return false;
		}

		*(table[i].func) = lol;
		++i;
	}

	return true;
}

bool dynamic_loader_globuf(char* path)
{
	return dynamic_loader(path, table_globuf);
}

bool dynamic_loader_cursoryx(char* path)
{
	return dynamic_loader(path, table_cursoryx);
}

bool dynamic_loader_willis(char* path)
{
	return dynamic_loader(path, table_willis);
}

bool dynamic_loader_dpishit(char* path)
{
	return dynamic_loader(path, table_dpishit);
}

#endif
