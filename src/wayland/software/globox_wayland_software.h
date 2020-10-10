#ifndef H_GLOBOX_WAYLAND_SOFTWARE
#define H_GLOBOX_WAYLAND_SOFTWARE

#include "globox.h"
#include "wayland/globox_wayland.h"

struct globox_wayland_software
{
	uint32_t globox_software_buffer_width;
	uint32_t globox_software_buffer_height;

	int globox_software_fd;
	struct wl_shm_pool* globox_software_pool;
	struct wl_buffer* globox_software_buffer;
};

#endif
