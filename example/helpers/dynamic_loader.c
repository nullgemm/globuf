#if defined(GLOBUF_SHARED)

#include "globuf.h"
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
double (*globuf_appkit_egl_get_scale)(
	struct globuf* context,
	struct globuf_error_info* error);
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
void (*globuf_prepare_init_appkit_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
	#elif defined(GLOBUF_EXAMPLE_EGL)
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

static struct link table[] =
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
	{(void(**)()) &globuf_appkit_egl_get_scale, "globuf_appkit_egl_get_scale"},
	#if defined(GLOBUF_EXAMPLE_SOFTWARE)
	{(void(**)()) &globuf_prepare_init_appkit_software, "globuf_prepare_init_appkit_software"},
	#elif defined(GLOBUF_EXAMPLE_EGL)
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

// loader implementation
bool dynamic_loader(char* path_globuf_lib)
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

#endif
