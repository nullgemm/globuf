#ifndef H_GLOBOX_WAYLAND
#define H_GLOBOX_WAYLAND

#include "globox.h"
#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>

bool globox_add_wayland_capabilities_handler(
	void* data,
	void (*capabilities_handler)(
		void* data,
		struct wl_seat* seat,
		uint32_t capabilities),
	void* capabilities_handler_data);

bool globox_add_wayland_registry_handler(
	void* data,
	void (*registry_handler)(
		void* data,
		struct wl_registry* registry,
		uint32_t name,
		const char* interface,
		uint32_t version),
	void* registry_handler_data);

bool globox_add_wayland_registry_remover(
	void* data,
	void (*registry_remover)(
		void* data,
		struct wl_registry* registry,
		uint32_t name),
	void* registry_remover_data);

struct wl_surface* globox_get_wayland_surface(
	struct globox* context);

#endif
