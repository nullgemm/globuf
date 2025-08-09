#define _XOPEN_SOURCE 700

#include "include/globuf.h"
#include "include/globuf_wayland.h"
#include "common/globuf_private.h"
#include "wayland/wayland_common.h"
#include "wayland/wayland_common_helpers.h"
#include "wayland/wayland_common_registry.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>

#include "xdg-decoration-client-protocol.h"
#include "kde-blur-client-protocol.h"

static inline void free_check(const void* ptr)
{
	if (ptr != NULL)
	{
		free((void*) ptr);
	}
}

void globuf_wayland_common_init(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	int error_posix;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;

	// init pthread mutex attributes
	error_posix = pthread_mutexattr_init(&mutex_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_ATTR_INIT);
		return;
	}

	// set pthread mutex type (error checking for now)
	error_posix =
		pthread_mutexattr_settype(
			&mutex_attr,
			PTHREAD_MUTEX_ERRORCHECK);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_ATTR_SETTYPE);
		return;
	}

	// init pthread mutex (main)
	error_posix = pthread_mutex_init(&(platform->mutex_main), &mutex_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// init pthread mutex (block)
	error_posix = pthread_mutex_init(&(platform->mutex_block), &mutex_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// destroy pthread mutex attributes
	error_posix = pthread_mutexattr_destroy(&mutex_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_ATTR_DESTROY);
		return;
	}

	// init pthread cond attributes
	error_posix = pthread_condattr_init(&cond_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_ATTR_INIT);
		return;
	}

	// set pthread cond clock
#if defined(_POSIX_MONOTONIC_CLOCK)
	clockid_t clock = CLOCK_MONOTONIC;
#else
	clockid_t clock = CLOCK_REALTIME;
#endif

	error_posix =
		pthread_condattr_setclock(
			&cond_attr,
			clock);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_ATTR_SETCLOCK);
		return;
	}

	// init pthread cond
	error_posix = pthread_cond_init(&(platform->cond_main), &cond_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_INIT);
		return;
	}

	// destroy pthread cond attributes
	error_posix = pthread_condattr_destroy(&cond_attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_ATTR_DESTROY);
		return;
	}

	// initialize the context platform reference
	platform->globuf = context;

	// initialize the "closed" boolean
	platform->init = true;
	platform->closed = false;

	// registry listener
	struct wl_registry_listener listener_registry =
	{
		.global = wayland_helpers_callback_registry,
		.global_remove = wayland_helpers_callback_registry_remove,
	};

	platform->listener_registry = listener_registry;

	// seat listener
	struct wl_seat_listener listener_seat =
	{
		.capabilities = wayland_helpers_seat_capabilities,
		.name = wayland_helpers_seat_name,
	};

	platform->listener_seat = listener_seat;

	// pointer listener
	struct wl_pointer_listener listener_pointer =
	{
		.enter = wayland_helpers_pointer_enter,
		.leave = wayland_helpers_pointer_leave,
		.motion = wayland_helpers_pointer_motion,
		.button = wayland_helpers_pointer_button,
		.axis = wayland_helpers_pointer_axis,
		.frame = wayland_helpers_pointer_frame,
		.axis_source = wayland_helpers_pointer_axis_source,
		.axis_stop = wayland_helpers_pointer_axis_stop,
		.axis_discrete = wayland_helpers_pointer_axis_discrete,
	};

	platform->listener_pointer = listener_pointer;

	// frame callback listener
	struct wl_callback_listener listener_surface_frame =
	{
		.done = wayland_helpers_surface_frame_done,
	};

	platform->listener_surface_frame = listener_surface_frame;

	// XDG WM base listener
	struct xdg_wm_base_listener listener_xdg_wm_base =
	{
		.ping = wayland_helpers_xdg_wm_base_ping,
	};

	platform->listener_xdg_wm_base = listener_xdg_wm_base;

	// XDG surface listener
	struct xdg_surface_listener listener_xdg_surface =
	{
		.configure = wayland_helpers_xdg_surface_configure,
	};

	platform->listener_xdg_surface = listener_xdg_surface;

	// XDG toplevel listener
	struct xdg_toplevel_listener listener_xdg_toplevel =
	{
		.configure = wayland_helpers_xdg_toplevel_configure,
		.close = wayland_helpers_xdg_toplevel_close,
	};

	platform->listener_xdg_toplevel = listener_xdg_toplevel;

	// XDG decoration listener
	struct zxdg_toplevel_decoration_v1_listener listener_xdg_decoration =
	{
		.configure = wayland_helpers_xdg_decoration_configure,
	};

	platform->listener_xdg_decoration = listener_xdg_decoration;

	// initialize render thread
	struct wayland_thread_render_loop_data thread_render_loop_data =
	{
		.globuf = NULL,
		.platform = NULL,
		.error = NULL,
	};
	platform->thread_render_loop_data = thread_render_loop_data;
	platform->render_init_callback = NULL;

	// initialize event thread
	struct wayland_thread_event_loop_data thread_event_loop_data =
	{
		.globuf = NULL,
		.platform = NULL,
		.error = NULL,
	};
	platform->thread_event_loop_data = thread_event_loop_data;
	platform->event_init_callback = NULL;

	// get wayland display
	platform->display = wl_display_connect(NULL);

	if (platform->display == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_DISPLAY_GET);
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_common_clean(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	int error_posix;
	int error_cond;

	// disconnect from display
	wl_display_disconnect(platform->display);

	// lock block mutex to be able to destroy the cond
	error_posix = pthread_mutex_lock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy pthread cond
	error_cond = pthread_cond_destroy(&(platform->cond_main));

	// unlock block mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (error_cond != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_DESTROY);
		return;
	}

	// destroy pthread mutex (block)
	error_posix = pthread_mutex_destroy(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	// destroy pthread mutex (main)
	error_posix = pthread_mutex_destroy(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	if (context->feature_title != NULL)
	{
		free_check(context->feature_title->title);
	}

	if (context->feature_icon != NULL)
	{
		free_check(context->feature_icon->pixmap);
	}

	free_check(context->feature_interaction);
	free_check(context->feature_state);
	free_check(context->feature_title);
	free_check(context->feature_icon);
	free_check(context->feature_size);
	free_check(context->feature_pos);
	free_check(context->feature_frame);
	free_check(context->feature_background);
	free_check(context->feature_vsync);

	globuf_error_ok(error);
}

void globuf_wayland_common_window_create(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	// store callback and data
	platform->feature_configs = configs;
	platform->feature_count = count;
	platform->feature_callback = callback;
	platform->feature_callback_data = data;

	globuf_error_ok(error);
}

void globuf_wayland_common_window_destroy(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	int error_posix;

	// lock main mutex
	error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy additional registry handlers
	struct wayland_registry_handler_node* registry_handler = platform->registry_handlers;
	struct wayland_registry_handler_node* registry_handler_next;

	while (registry_handler != NULL)
	{
		registry_handler_next = registry_handler->next;
		free(registry_handler);
		registry_handler = registry_handler_next;
	}

	// destroy additional registry removers
	struct wayland_registry_remover_node* registry_remover = platform->registry_removers;
	struct wayland_registry_remover_node* registry_remover_next;

	while (registry_remover != NULL)
	{
		registry_remover_next = registry_remover->next;
		free(registry_remover);
		registry_remover = registry_remover_next;
	}

	// destroy additional capabilities handlers
	struct wayland_capabilities_handler_node* capabilities_handler = platform->capabilities_handlers;
	struct wayland_capabilities_handler_node* capabilities_handler_next;

	while (capabilities_handler != NULL)
	{
		capabilities_handler_next = capabilities_handler->next;
		free(capabilities_handler);
		capabilities_handler = capabilities_handler_next;
	}

	// destroy optional protocols structures
	if (platform->kde_blur != NULL)
	{
		org_kde_kwin_blur_destroy(platform->kde_blur);
	}

	if (platform->kde_blur_manager != NULL)
	{
		org_kde_kwin_blur_manager_destroy(platform->kde_blur_manager);
	}

	if (platform->xdg_decoration != NULL)
	{
		zxdg_toplevel_decoration_v1_destroy(platform->xdg_decoration);
	}

	if (platform->xdg_decoration_manager != NULL)
	{
		zxdg_decoration_manager_v1_destroy(platform->xdg_decoration_manager);
	}

	if (platform->xdg_wm_base != NULL)
	{
		xdg_wm_base_destroy(platform->xdg_wm_base);
	}

	if (platform->pointer != NULL)
	{
		wl_pointer_destroy(platform->pointer);
	}

	if (platform->seat != NULL)
	{
		wl_seat_destroy(platform->seat);
	}

	if (platform->compositor != NULL)
	{
		wl_compositor_destroy(platform->compositor);
	}

	// destroy the XDG toplevel
	xdg_toplevel_destroy(platform->xdg_toplevel);

	// destroy the XDG surface
	xdg_surface_destroy(platform->xdg_surface);

	// destroy the Wayland surface
	wl_surface_destroy(platform->surface);

	// destroy the registry
	wl_registry_destroy(platform->registry);

	// unlock main mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_common_window_confirm(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	int error_posix;

	// get registry
	platform->registry = wl_display_get_registry(platform->display);

	if (platform->registry == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_REGISTRY_GET);
		return;
	}

	// set registry listener
	error_posix =
		wl_registry_add_listener(
			platform->registry,
			&(platform->listener_registry),
			platform);

	if (error_posix == -1)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	// perform a roundtrip for the registry
	error_posix = wl_display_roundtrip(platform->display);

	if (error_posix == -1)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_ROUNDTRIP);
		return;
	}

	// check we have everything we need
	if (platform->compositor == NULL)
	{
		globuf_error_throw(
			context,
			error,
			GLOBUF_ERROR_WAYLAND_COMPOSITOR_MISSING);
		return;
	}

	if (platform->xdg_wm_base == NULL)
	{
		globuf_error_throw(
			context,
			error,
			GLOBUF_ERROR_WAYLAND_XDG_WM_BASE_MISSSING);
		return;
	}

	// create wayland surface
	platform->surface =
		wl_compositor_create_surface(
			platform->compositor);

	if (platform->surface == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_SURFACE_CREATE);
		return;
	}

	// get xdg surface
	platform->xdg_surface =
		xdg_wm_base_get_xdg_surface(
			platform->xdg_wm_base,
			platform->surface);	

	if (platform->xdg_surface == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_XDG_SURFACE_CREATE);
		return;
	}

	// set xdg surface listener
	error_posix =
		xdg_surface_add_listener(
			platform->xdg_surface,
			&(platform->listener_xdg_surface),
			platform);

	if (error_posix == -1)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	// get xdg toplevel
	platform->xdg_toplevel =
		xdg_surface_get_toplevel(
			platform->xdg_surface);

	if (platform->xdg_toplevel == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_XDG_TOPLEVEL_GET);
		return;
	}

	// set xdg toplevel listener
	error_posix =
		xdg_toplevel_add_listener(
			platform->xdg_toplevel,
			&(platform->listener_xdg_toplevel),
			platform);

	if (error_posix == -1)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	if (platform->xdg_decoration_manager != NULL)
	{
		// get xdg decorations
		platform->xdg_decoration =
			zxdg_decoration_manager_v1_get_toplevel_decoration(
				platform->xdg_decoration_manager,
				platform->xdg_toplevel);

		if (platform->xdg_decoration == NULL)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_XDG_DECORATION_GET);
			return;
		}

		// set xdg decorations listener
		error_posix =
			zxdg_toplevel_decoration_v1_add_listener(
				platform->xdg_decoration,
				&(platform->listener_xdg_decoration),
				platform);

		if (error_posix == -1)
		{
			globuf_error_throw(
				context,
				error,
				GLOBUF_ERROR_WAYLAND_LISTENER_ADD);
		}
	}

	// configure features
	struct globuf_config_reply* reply =
		malloc(platform->feature_count * (sizeof (struct globuf_config_reply)));

	if (reply == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	for (size_t i = 0; i < platform->feature_count; ++i)
	{
		enum globuf_feature feature = platform->feature_configs[i].feature;
		reply[i].feature = feature;

		switch (feature)
		{
			case GLOBUF_FEATURE_STATE:
			{
				globuf_wayland_helpers_set_state(context, platform, &reply[i].error);
				break;
			}
			case GLOBUF_FEATURE_TITLE:
			{
				globuf_wayland_helpers_set_title(context, platform, &reply[i].error);
				break;
			}
			case GLOBUF_FEATURE_ICON:
			{
				globuf_wayland_helpers_set_icon(context, platform, &reply[i].error);
				break;
			}
			case GLOBUF_FEATURE_FRAME:
			{
				globuf_wayland_helpers_set_frame(context, platform, &reply[i].error);
				break;
			}
			case GLOBUF_FEATURE_BACKGROUND:
			{
				globuf_wayland_helpers_set_background(context, platform, &reply[i].error);
				break;
			}
			case GLOBUF_FEATURE_VSYNC:
			{
				globuf_wayland_helpers_set_vsync(context, platform, &reply[i].error);
				break;
			}
			default:
			{
				reply[i].error.code = GLOBUF_ERROR_OK;
				reply[i].error.file = NULL;
				reply[i].error.line = 0;
				break;
			}
		}
	}

	platform->feature_callback(
		reply,
		platform->feature_count,
		platform->feature_callback_data);

	free(reply);

	globuf_error_ok(error);
}

void globuf_wayland_common_window_start(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	// init thread attributes
	int error_posix;
	pthread_attr_t attr;

	error_posix = pthread_attr_init(&attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_ATTR_INIT);
		return;
	}

	error_posix = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (error_posix != 0)
	{
		pthread_attr_destroy(&attr);
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_ATTR_JOINABLE);
		return;
	}

	// show surface
	wl_surface_commit(platform->surface);

	// start the event loop in a new thread
	// init thread function data
	struct wayland_thread_event_loop_data event_data =
	{
		.globuf = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_event_loop_data = event_data;

	// start function in a new thread
	error_posix =
		pthread_create(
			&(platform->thread_event_loop),
			&attr,
			globuf_wayland_helpers_event_loop,
			&(platform->thread_event_loop_data));

	if (error_posix != 0)
	{
		pthread_attr_destroy(&attr);
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_CREATE);
		return;
	}

	if (context->feature_vsync->vsync == false)
	{
		// start the render loop in a new thread
		// init thread function data
		struct wayland_thread_render_loop_data render_data =
		{
			.globuf = context,
			.platform = platform,
			.error = error,
		};

		platform->thread_render_loop_data = render_data;

		// start function in a new thread
		error_posix =
			pthread_create(
				&(platform->thread_render_loop),
				&attr,
				globuf_wayland_helpers_render_loop,
				&(platform->thread_render_loop_data));

		if (error_posix != 0)
		{
			pthread_attr_destroy(&attr);
			globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_CREATE);
			return;
		}
	}
	else
	{
		// get surface frame
		struct wl_callback* surface_frame = wl_surface_frame(platform->surface);

		if (surface_frame == NULL)
		{
			pthread_attr_destroy(&attr);
			globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_SURFACE_FRAME_GET);
			return;
		}

		// set surface frame callback
		error_posix =
			wl_callback_add_listener(
				surface_frame,
				&(platform->listener_surface_frame),
				platform);

		if (error_posix == -1)
		{
			pthread_attr_destroy(&attr);
			wl_callback_destroy(surface_frame);
			globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_LISTENER_ADD);
			return;
		}
	}

	// destroy the attributes
	error_posix = pthread_attr_destroy(&attr);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_ATTR_DESTROY);
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_common_window_block(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	int error_posix;
	int error_cond;

	// lock block mutex
	error_posix = pthread_mutex_lock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	error_cond = pthread_cond_wait(&(platform->cond_main), &(platform->mutex_block));

	// unlock block mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (error_cond != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_COND_WAIT);
		return;
	}

	error_posix = pthread_join(platform->thread_event_loop, NULL);

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_JOIN);
		return;
	}

	if (context->feature_vsync->vsync == false)
	{
		error_posix = pthread_join(platform->thread_render_loop, NULL);

		if (error_posix != 0)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_THREAD_JOIN);
			return;
		}
	}

	globuf_error_ok(error);
}

void globuf_wayland_common_window_stop(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	globuf_error_ok(error);
}


void globuf_wayland_common_init_render(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	// set the event callback
	context->render_callback = *config;
	globuf_error_ok(error);
}

void globuf_wayland_common_init_events(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	// set the event callback
	context->event_callbacks = *config;
	globuf_error_ok(error);
}

enum globuf_event globuf_wayland_common_handle_events(
	struct globuf* context,
	struct wayland_platform* platform,
	void* event,
	struct globuf_error_info* error)
{
	globuf_error_ok(error);
	return GLOBUF_EVENT_UNKNOWN;
}

struct globuf_config_features*
	globuf_wayland_common_init_features(
		struct globuf* context,
		struct wayland_platform* platform,
		struct globuf_error_info* error)
{
	struct globuf_config_features* features =
		malloc(sizeof (struct globuf_config_features));

	if (features == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	features->count = 0;
	features->list =
		malloc(GLOBUF_FEATURE_COUNT * (sizeof (enum globuf_feature)));

	if (features->list == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_INTERACTION;
	context->feature_interaction =
		malloc(sizeof (struct globuf_feature_interaction));
	features->count += 1;

	if (context->feature_interaction == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	context->feature_interaction->action = GLOBUF_INTERACTION_STOP;

	// always available
	features->list[features->count] = GLOBUF_FEATURE_STATE;
	context->feature_state =
		malloc(sizeof (struct globuf_feature_state));
	features->count += 1;

	if (context->feature_state == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_TITLE;
	context->feature_title =
		malloc(sizeof (struct globuf_feature_title));
	features->count += 1;

	if (context->feature_title == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

#if 0
	// always available
	features->list[features->count] = GLOBUF_FEATURE_ICON;
	context->feature_icon =
		malloc(sizeof (struct globuf_feature_icon));
	features->count += 1;

	if (context->feature_icon == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}
#endif

	// always available
	features->list[features->count] = GLOBUF_FEATURE_SIZE;
	context->feature_size =
		malloc(sizeof (struct globuf_feature_size));
	features->count += 1;

	if (context->feature_size == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_POS;
	context->feature_pos =
		malloc(sizeof (struct globuf_feature_pos));
	features->count += 1;

	if (context->feature_pos == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_FRAME;
	context->feature_frame =
		malloc(sizeof (struct globuf_feature_frame));
	features->count += 1;

	if (context->feature_frame == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_BACKGROUND;
	context->feature_background =
		malloc(sizeof (struct globuf_feature_background));
	features->count += 1;

	if (context->feature_background == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_VSYNC;
	context->feature_vsync =
		malloc(sizeof (struct globuf_feature_vsync));
	features->count += 1;

	if (context->feature_vsync == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return NULL;
	}

	globuf_error_ok(error);
	return features;
}

void globuf_wayland_common_feature_set_interaction(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	*(context->feature_interaction) = *config;

	switch (config->action)
	{
		case GLOBUF_INTERACTION_MOVE:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
			break;
		}
		case GLOBUF_INTERACTION_N:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
			break;
		}
		case GLOBUF_INTERACTION_NW:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
			break;
		}
		case GLOBUF_INTERACTION_W:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
			break;
		}
		case GLOBUF_INTERACTION_SW:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
			break;
		}
		case GLOBUF_INTERACTION_S:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
			break;
		}
		case GLOBUF_INTERACTION_SE:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
			break;
		}
		case GLOBUF_INTERACTION_E:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
			break;
		}
		case GLOBUF_INTERACTION_NE:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
			break;
		}
		default:
		{
			break;
		}
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_common_feature_set_state(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	*(context->feature_state) = *config;
	globuf_wayland_helpers_set_state(context, platform, error);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_common_feature_set_title(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	free_check(context->feature_title->title);

	context->feature_title->title = strdup(config->title);
	globuf_wayland_helpers_set_title(context, platform, error);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_common_feature_set_icon(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	free_check(context->feature_icon->pixmap);

	context->feature_icon->pixmap = malloc(config->len * 4);

	if (context->feature_icon->pixmap != NULL)
	{
		memcpy(context->feature_icon->pixmap, config->pixmap, config->len * 4);
		context->feature_icon->len = config->len;
	}
	else
	{
		context->feature_icon->len = 0;
	}

	globuf_wayland_helpers_set_icon(context, platform, error);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

unsigned globuf_wayland_common_get_width(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->width;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return 0;
	}

	// return value
	globuf_error_ok(error);
	return value;
}

unsigned globuf_wayland_common_get_height(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->height;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return 0;
	}

	// return value
	globuf_error_ok(error);
	return value;
}

struct globuf_rect globuf_wayland_common_get_expose(
	struct globuf* context,
	struct wayland_platform* platform,
	struct globuf_error_info* error)
{
	struct globuf_rect dummy =
	{
		.x = 0,
		.y = 0,
		.width = 0,
		.height = 0,
	};

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return dummy;
	}

	// save value
	struct globuf_rect value = context->expose;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return dummy;
	}

	// return value
	globuf_error_ok(error);
	return value;
}


// callback for helper libraries to register capabilities handlers
bool globuf_add_wayland_capabilities_handler(
	void* data,
	void (*capabilities_handler)(
		void* data,
		void* seat,
		uint32_t capabilities),
	void* capabilities_handler_data)
{
	struct globuf* context = data;
	struct wayland_platform* platform = context->backend_callbacks.callback(context);

	struct wayland_capabilities_handler_node* handler =
		malloc(sizeof (struct wayland_capabilities_handler_node));

	if (handler == NULL)
	{
		return false;
	}

	handler->capabilities_handler = capabilities_handler;
	handler->capabilities_handler_data = capabilities_handler_data;
	handler->next = platform->capabilities_handlers;

	platform->capabilities_handlers = handler;
	return true;
}

// callback for helper libraries to register registry handlers
bool globuf_add_wayland_registry_handler(
	void* data,
	void (*registry_handler)(
		void* data,
		void* registry,
		uint32_t name,
		const char* interface,
		uint32_t version),
	void* registry_handler_data)
{
	struct globuf* context = data;
	struct wayland_platform* platform = context->backend_callbacks.callback(context);

	struct wayland_registry_handler_node* handler =
		malloc(sizeof (struct wayland_registry_handler_node));

	if (handler == NULL)
	{
		return false;
	}

	handler->registry_handler = registry_handler;
	handler->registry_handler_data = registry_handler_data;
	handler->next = platform->registry_handlers;

	platform->registry_handlers = handler;
	return true;
}

// callback for helper libraries to register registry remove handlers
bool globuf_add_wayland_registry_remover(
	void* data,
	void (*registry_remover)(
		void* data,
		void* registry,
		uint32_t name),
	void* registry_remover_data)
{
	struct globuf* context = data;
	struct wayland_platform* platform = context->backend_callbacks.callback(context);

	struct wayland_registry_remover_node* remover =
		malloc(sizeof (struct wayland_registry_remover_node));

	if (remover == NULL)
	{
		return false;
	}

	remover->registry_remover = registry_remover;
	remover->registry_remover_data = registry_remover_data;
	remover->next = platform->registry_removers;

	platform->registry_removers = remover;
	return true;
}

void* globuf_get_wayland_surface(
	struct globuf* context)
{
	struct wayland_platform* platform = context->backend_callbacks.callback(context);

	return (void*) platform->surface;
}

