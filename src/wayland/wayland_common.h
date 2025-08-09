#ifndef H_GLOBUF_INTERNAL_WAYLAND_COMMON
#define H_GLOBUF_INTERNAL_WAYLAND_COMMON

#include "include/globuf.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>

#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-client-protocol.h"
#include "kde-blur-client-protocol.h"

// # private helpers
struct wayland_thread_render_loop_data
{
	struct globuf* globuf;
	struct wayland_platform* platform;
	struct globuf_error_info* error;
};

struct wayland_thread_event_loop_data
{
	struct globuf* globuf;
	struct wayland_platform* platform;
	struct globuf_error_info* error;
};

struct wayland_capabilities_handler_node
{
	void (*capabilities_handler)(
		void* data,
		void* seat,
		uint32_t capabilities);
	void* capabilities_handler_data;
	struct wayland_capabilities_handler_node* next;
};

struct wayland_registry_handler_node
{
	void (*registry_handler)(
		void* data,
		void* registry,
		uint32_t name,
		const char* interface,
		uint32_t version);
	void* registry_handler_data;
	struct wayland_registry_handler_node* next;
};

struct wayland_registry_remover_node
{
	void (*registry_remover)(
		void* data,
		void* registry,
		uint32_t name);
	void* registry_remover_data;
	struct wayland_registry_remover_node* next;
};

struct wayland_platform
{
	pthread_mutex_t mutex_main;
	pthread_mutex_t mutex_block;
	pthread_mutex_t mutex_xsync;
	pthread_cond_t cond_main;

	// globuf
	bool init;
	bool closed;
	struct globuf* globuf;

	// core structures
	struct wl_display* display;
	struct wl_registry* registry;
	struct wl_seat* seat;
	struct wl_pointer* pointer;
	struct wl_surface* surface;

	// base interfaces
	struct wl_compositor* compositor;
	struct xdg_wm_base* xdg_wm_base;

	// desktop structures
	struct xdg_surface* xdg_surface;
	struct xdg_toplevel* xdg_toplevel;

	// desktop decorations interface
	struct zxdg_decoration_manager_v1* xdg_decoration_manager;
	struct zxdg_toplevel_decoration_v1* xdg_decoration;

	// kde blur interface
	struct org_kde_kwin_blur_manager* kde_blur_manager;
	struct org_kde_kwin_blur* kde_blur;

	// wayland window status info
	uint32_t decoration_mode;
	uint32_t sizing_edge;

	// wayland listeners
	struct wl_registry_listener listener_registry;
	struct wl_seat_listener listener_seat;
	struct wl_pointer_listener listener_pointer;
	struct wl_callback_listener listener_surface_frame;

	struct xdg_wm_base_listener listener_xdg_wm_base;
	struct xdg_surface_listener listener_xdg_surface;
	struct xdg_toplevel_listener listener_xdg_toplevel;
	struct zxdg_toplevel_decoration_v1_listener listener_xdg_decoration;

	// external wayland negociation callbacks handling
	struct wayland_capabilities_handler_node* capabilities_handlers;
	struct wayland_registry_handler_node* registry_handlers;
	struct wayland_registry_remover_node* registry_removers;

	// globuf feature negociation
	void (*feature_callback)(
		struct globuf_config_reply* replies,
		size_t count,
		void* data);
	void* feature_callback_data;
	struct globuf_config_request* feature_configs;
	size_t feature_count;

	// render handling
	pthread_t thread_render_loop;
	struct wayland_thread_render_loop_data thread_render_loop_data;
	void (*render_init_callback)(struct globuf*, struct globuf_error_info*);

	// event handling
	pthread_t thread_event_loop;
	struct wayland_thread_event_loop_data thread_event_loop_data;
	void (*event_init_callback)(struct globuf*, struct globuf_error_info*);
};

void globuf_wayland_common_init(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_common_clean(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_common_window_create(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error);

void globuf_wayland_common_window_destroy(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_common_window_confirm(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_common_window_start(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_common_window_block(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_common_window_stop(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);


void globuf_wayland_common_init_render(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_config_render* config,
	struct globuf_error_info* error);

void globuf_wayland_common_init_events(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_config_events* config,
	struct globuf_error_info* error);

enum globuf_event globuf_wayland_common_handle_events(
	struct globuf* context,
	struct wayland_platform* platform,
	void* event,
	struct globuf_error_info* error);


struct globuf_config_features* globuf_wayland_common_init_features(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

void globuf_wayland_common_feature_set_interaction(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error);

void globuf_wayland_common_feature_set_state(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_feature_state* config,
	struct globuf_error_info* error);

void globuf_wayland_common_feature_set_title(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_feature_title* config,
	struct globuf_error_info* error);

void globuf_wayland_common_feature_set_icon(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error);


unsigned globuf_wayland_common_get_width(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

unsigned globuf_wayland_common_get_height(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

struct globuf_rect globuf_wayland_common_get_expose(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error);

#endif
