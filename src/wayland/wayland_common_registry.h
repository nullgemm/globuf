#ifndef H_GLOBOX_INTERNAL_WAYLAND_COMMON_REGISTRY
#define H_GLOBOX_INTERNAL_WAYLAND_COMMON_REGISTRY

#include "include/globox.h"
#include "wayland/wayland_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>

void globox_wayland_helpers_callback_registry(
	void* data,
	struct wl_registry* registry,
	uint32_t name,
	const char* interface,
	uint32_t version);

void globox_wayland_helpers_callback_registry_remove(
	void* data,
	struct wl_registry* registry,
	uint32_t name);

void globox_wayland_helpers_surface_frame_done(
	void* data,
	struct wl_callback* callback,
	uint32_t time);

void globox_wayland_helpers_xdg_wm_base_ping(
	void* data,
	struct xdg_wm_base* xdg_wm_base,
	uint32_t serial);

void globox_wayland_helpers_xdg_surface_configure(
	void* data,
	struct xdg_surface* xdg_surface,
	uint32_t serial);

void globox_wayland_helpers_xdg_toplevel_configure(
	void* data,
	struct xdg_toplevel* xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array* states);

void globox_wayland_helpers_xdg_toplevel_close(
	void* data,
	struct xdg_toplevel* xdg_toplevel);

void globox_wayland_helpers_xdg_decoration_configure(
	void* data,
	struct zxdg_toplevel_decoration_v1* xdg_decoration,
	uint32_t mode);

#endif
