#ifndef H_GLOBOX_INTERNAL_WAYLAND_COMMON_REGISTRY
#define H_GLOBOX_INTERNAL_WAYLAND_COMMON_REGISTRY

#include "include/globox.h"
#include "wayland/wayland_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>

// registry callbacks
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

// seat listener
void globox_wayland_helpers_seat_capabilities(
	void* data,
	struct wl_seat* seat,
	uint32_t capabilities);

void globox_wayland_helpers_seat_name(
	void* data,
	struct wl_seat* seat,
	const char* name);

// pointer listener
void globox_wayland_helpers_pointer_enter(
	void* data,
	struct wl_pointer* wl_pointer,
	uint32_t serial,
	struct wl_surface* surface,
	wl_fixed_t surface_x,
	wl_fixed_t surface_y);

void globox_wayland_helpers_pointer_leave(
	void* data,
	struct wl_pointer* wl_pointer,
	uint32_t serial,
	struct wl_surface* surface);

void globox_wayland_helpers_pointer_motion(
	void* data,
	struct wl_pointer* wl_pointer,
	uint32_t time,
	wl_fixed_t surface_x,
	wl_fixed_t surface_y);

void globox_wayland_helpers_pointer_button(
	void* data,
	struct wl_pointer* wl_pointer,
	uint32_t serial,
	uint32_t time,
	uint32_t button,
	uint32_t state);

void globox_wayland_helpers_pointer_axis(
	void* data,
	struct wl_pointer* wl_pointer,
	uint32_t time,
	uint32_t axis,
	wl_fixed_t value);

void globox_wayland_helpers_pointer_frame(
	void* data,
	struct wl_pointer* wl_pointer);

void globox_wayland_helpers_pointer_axis_source(
	void* data,
	struct wl_pointer* wl_pointer,
	uint32_t axis_source);

void globox_wayland_helpers_pointer_axis_stop(
	void* data,
	struct wl_pointer* wl_pointer,
	uint32_t time,
	uint32_t axis);

void globox_wayland_helpers_pointer_axis_discrete(
	void* data,
	struct wl_pointer* wl_pointer,
	uint32_t axis,
	int32_t discrete);

// frame callback listener
void globox_wayland_helpers_surface_frame_done(
	void* data,
	struct wl_callback* callback,
	uint32_t time);

// XDG WM base listener
void globox_wayland_helpers_xdg_wm_base_ping(
	void* data,
	struct xdg_wm_base* xdg_wm_base,
	uint32_t serial);

// XDG surface listener
void globox_wayland_helpers_xdg_surface_configure(
	void* data,
	struct xdg_surface* xdg_surface,
	uint32_t serial);

// XDG toplevel listener
void globox_wayland_helpers_xdg_toplevel_configure(
	void* data,
	struct xdg_toplevel* xdg_toplevel,
	int32_t width,
	int32_t height,
	struct wl_array* states);

void globox_wayland_helpers_xdg_toplevel_close(
	void* data,
	struct xdg_toplevel* xdg_toplevel);

// XDG decoration listener
void globox_wayland_helpers_xdg_decoration_configure(
	void* data,
	struct zxdg_toplevel_decoration_v1* xdg_decoration,
	uint32_t mode);

#endif
