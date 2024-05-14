#ifndef H_GLOBUF_INTERNAL_WAYLAND_SOFTWARE_HELPERS
#define H_GLOBUF_INTERNAL_WAYLAND_SOFTWARE_HELPERS

#include "include/globuf.h"
#include "wayland/wayland_common.h"

#include <wayland-client.h>

struct wayland_software_backend
{
	struct wayland_platform platform;

	// software structures
	struct wl_shm* shm;
	struct wl_buffer* buffer;

	// mmap'd buffer size
	size_t buffer_len;

	// wayland buffer listener
	struct wl_buffer_listener listener_buffer;
};

void wayland_helpers_callback_registry_shm(
	void* data,
	struct wl_registry* registry,
	uint32_t name,
	const char* interface,
	uint32_t version);

void wayland_helpers_buffer_release(
	void* data,
	struct wl_buffer* wl_buffer);

#endif
