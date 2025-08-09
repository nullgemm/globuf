#ifndef H_GLOBUF_INTERNAL_WAYLAND_SOFTWARE_HELPERS
#define H_GLOBUF_INTERNAL_WAYLAND_SOFTWARE_HELPERS

#include "include/globuf.h"
#include "wayland/wayland_common.h"

#include <stdint.h>
#include <wayland-client.h>

struct wayland_software_backend
{
	struct wayland_platform platform;

	// software structures
	struct wl_shm* shm;
	struct wl_buffer* buffer;

	// mmap'd buffer
	size_t buffer_len;
	uint32_t* buffer_ptr;
	unsigned buffer_width;
	unsigned buffer_height;

	// wayland buffer listener
	struct wl_buffer_listener listener_buffer;
	struct wl_shm_pool* software_pool;
};

void wayland_helpers_callback_registry_shm(
	void* data,
	void* registry,
	uint32_t name,
	const char* interface,
	uint32_t version);

void wayland_helpers_buffer_release(
	void* data,
	struct wl_buffer* wl_buffer);

#endif
