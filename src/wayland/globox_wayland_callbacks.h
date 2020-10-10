#ifndef H_GLOBOX_WAYLAND_CALLBACKS
#define H_GLOBOX_WAYLAND_CALLBACKS

#include "globox.h"
#include "globox_error.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <wayland-client.h>

#include "wayland/globox_wayland.h"

void callback_xdg_wm_base_ping(
	void* data,
	struct xdg_wm_base* xdg_wm_base,
	uint32_t serial);
void callback_output_geometry(
	void* data,
	struct wl_output* wl_output,
	int32_t x,
	int32_t y,
	int32_t physical_width,
	int32_t physical_height,
	int32_t subpixel,
	const char* make,
	const char* model,
	int32_t output_transform);
void callback_output_mode(
	void* data,
	struct wl_output* wl_output,
	uint32_t flags,
	int32_t width,
	int32_t height,
	int32_t refresh);
void callback_output_done(
	void* data,
	struct wl_output* wl_output);
void callback_output_scale(
	void* data,
	struct wl_output* wl_output,
	int32_t scale);
void callback_registry_global(
	void* data,
	struct wl_registry* wl_registry,
	uint32_t name,
	const char* interface,
	uint32_t version);
void callback_registry_global_remove(
	void* data,
	struct wl_registry* wl_registry,
	uint32_t name);
void callback_xdg_toplevel_configure(
	void* data,
	struct xdg_toplevel* xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array* states);
void callback_xdg_toplevel_close(
	void* data,
	struct xdg_toplevel* xdg_toplevel);
void callback_xdg_surface_configure(
	void* data,
	struct xdg_surface* xdg_surface,
	uint32_t serial);
void callback_xdg_decoration_configure(
	void* data,
	struct zxdg_toplevel_decoration_v1* xdg_decoration,
	uint32_t mode);

#endif
