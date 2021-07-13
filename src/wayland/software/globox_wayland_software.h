#ifndef H_GLOBOX_WAYLAND_SOFTWARE
#define H_GLOBOX_WAYLAND_SOFTWARE

#include <pthread.h>
#include <stdint.h>
#include <wayland-client.h>

struct globox_wayland_software
{
	pthread_mutex_t globox_software_buffer_mutex;
	struct wl_buffer** globox_software_buffer_list;
	int globox_software_buffer_list_len;
	int globox_software_buffer_list_max;

	struct wl_buffer_listener globox_software_buffer_listener;
	struct wl_buffer* globox_software_buffer;
	uint64_t globox_software_buffer_len;
};

#endif
