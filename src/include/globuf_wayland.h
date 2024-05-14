#ifndef H_GLOBUF_WAYLAND
#define H_GLOBUF_WAYLAND

#include "globuf.h"
#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>

bool globuf_add_wayland_capabilities_handler(
	void* data,
	void (*capabilities_handler)(
		void* data,
		struct wl_seat* seat,
		uint32_t capabilities),
	void* capabilities_handler_data);

bool globuf_add_wayland_registry_handler(
	void* data,
	void (*registry_handler)(
		void* data,
		struct wl_registry* registry,
		uint32_t name,
		const char* interface,
		uint32_t version),
	void* registry_handler_data);

bool globuf_add_wayland_registry_remover(
	void* data,
	void (*registry_remover)(
		void* data,
		struct wl_registry* registry,
		uint32_t name),
	void* registry_remover_data);

struct wl_surface* globuf_get_wayland_surface(
	struct globuf* context);

#endif
