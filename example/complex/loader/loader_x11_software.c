#if defined(GLOBUF_SHARED)

#include "globuf.h"
#include "loader_x11_software.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include <xcb/xcb.h>

// pointers implementation
#if defined(GLOBUF_EXAMPLE_X11)
xcb_connection_t* (*globuf_get_x11_conn)(struct globuf* context);
xcb_window_t (*globuf_get_x11_window)(struct globuf* context);
xcb_window_t (*globuf_get_x11_root)(struct globuf* context);
xcb_screen_t* (*globuf_get_x11_screen)(struct globuf* context);

void (*globuf_prepare_init_x11_software)(
	struct globuf_config_backend* config,
	struct globuf_error_info* error);
#elif defined(GLOBUF_EXAMPLE_APPKIT)
#elif defined(GLOBUF_EXAMPLE_WIN)
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
#endif

// symbol table
struct link
{
	void (**func)();
	char* sym;
};

static struct link table[] =
{
#if defined(GLOBUF_EXAMPLE_X11)
	{(void(**)()) &globuf_get_x11_conn, "globuf_get_x11_conn"},
	{(void(**)()) &globuf_get_x11_window, "globuf_get_x11_window"},
	{(void(**)()) &globuf_get_x11_root, "globuf_get_x11_root"},
	{(void(**)()) &globuf_get_x11_screen, "globuf_get_x11_screen"},
	{(void(**)()) &globuf_prepare_init_x11_software, "globuf_prepare_init_x11_software"},
#elif defined(GLOBUF_EXAMPLE_APPKIT)
#elif defined(GLOBUF_EXAMPLE_WIN)
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
#endif
	{NULL, NULL},
};

// loader implementation
bool globuf_loader_x11_software(
	char* path_globuf_lib,
	int options)
{
	void* globuf_lib = dlopen(path_globuf_lib, options);

	if (globuf_lib == NULL)
	{
#ifndef GLOBUF_ERROR_SKIP
		fprintf(stderr, "could not load object %s\n", path_globuf_lib);
		fprintf(stderr, "dlopen error: %s\n", dlerror());
#endif
		return false;
	}

	size_t i = 0;

	while ((table[i].func != NULL) && (table[i].sym != NULL))
	{
		void(*lol)() = dlsym(globuf_lib, table[i].sym);

		if (lol == NULL)
		{
#ifndef GLOBUF_ERROR_SKIP
			fprintf(stderr, "could not load symbol %s in object %s\n", table[i].sym, path_globuf_lib);
			fprintf(stderr, "dlsym error: %s\n", dlerror());
#endif
			return false;
		}

		*(table[i].func) = lol;
		++i;
	}

	return true;
}

#endif
