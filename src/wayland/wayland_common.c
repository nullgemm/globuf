#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "include/globox_wayland.h"
#include "common/globox_private.h"
#include "wayland/wayland_common.h"
#include "wayland/wayland_common_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static inline void free_check(const void* ptr)
{
	if (ptr != NULL)
	{
		free((void*) ptr);
	}
}

void globox_wayland_common_init(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	int error_posix;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;

	// init pthread mutex attributes
	error_posix = pthread_mutexattr_init(&mutex_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_INIT);
		return;
	}

	// set pthread mutex type (error checking for now)
	error_posix =
		pthread_mutexattr_settype(
			&mutex_attr,
			PTHREAD_MUTEX_ERRORCHECK);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_SETTYPE);
		return;
	}

	// init pthread mutex (main)
	error_posix = pthread_mutex_init(&(platform->mutex_main), &mutex_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// init pthread mutex (block)
	error_posix = pthread_mutex_init(&(platform->mutex_block), &mutex_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_INIT);
		return;
	}

	// destroy pthread mutex attributes
	error_posix = pthread_mutexattr_destroy(&mutex_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_ATTR_DESTROY);
		return;
	}

	// init pthread cond attributes
	error_posix = pthread_condattr_init(&cond_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_ATTR_INIT);
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
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_ATTR_SETCLOCK);
		return;
	}

	// init pthread cond
	error_posix = pthread_cond_init(&(platform->cond_main), &cond_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_INIT);
		return;
	}

	// destroy pthread cond attributes
	error_posix = pthread_condattr_destroy(&cond_attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_ATTR_DESTROY);
		return;
	}

	// initialize the context platform reference
	platform->globox = context;

	// initialize the "closed" boolean
	platform->closed = false;

	// initialize render thread
	struct wayland_thread_render_loop_data thread_render_loop_data =
	{
		.globox = NULL,
		.platform = NULL,
		.error = NULL,
	};
	platform->thread_render_loop_data = thread_render_loop_data;
	platform->render_init_callback = NULL;

	// initialize event thread
	struct wayland_thread_event_loop_data thread_event_loop_data =
	{
		.globox = NULL,
		.platform = NULL,
		.error = NULL,
	};
	platform->thread_event_loop_data = thread_event_loop_data;
	platform->event_init_callback = NULL;

	// get wayland display
	platform->display = wl_display_connect(NULL);

	if (platform->display == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_DISPLAY_GET);
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_common_clean(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	int error_posix;
	int error_cond;

	// disconnect from display
	wl_display_disconnect(platform->display);

	// lock block mutex to be able to destroy the cond
	error_posix = pthread_mutex_lock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy pthread cond
	error_cond = pthread_cond_destroy(&(platform->cond_main));

	// unlock block mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (error_cond != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_DESTROY);
		return;
	}

	// destroy pthread mutex (block)
	error_posix = pthread_mutex_destroy(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
		return;
	}

	// destroy pthread mutex (main)
	error_posix = pthread_mutex_destroy(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_DESTROY);
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

	globox_error_ok(error);
}

void globox_wayland_common_window_create(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	// store callback and data
	platform->feature_configs = configs;
	platform->feature_count = count;
	platform->feature_callback = callback;
	platform->feature_callback_data = data;

	globox_error_ok(error);
}

void globox_wayland_common_window_destroy(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// lock main mutex
	error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// destroy the surface frame
	wl_callback_destroy(platform->surface_frame);

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
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_common_window_confirm(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	int error_posix;

	// get registry
	platform->registry = wl_display_get_registry(platform->display);

	if (platform->registry == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_REGISTRY_GET);
		return;
	}

	// set registry listener
	struct wl_registry_listener listener_registry =
	{
		.global = globox_wayland_helpers_callback_registry,
		.global_remove = globox_wayland_helpers_callback_registry_remove,
	};

	error_posix =
		wl_registry_add_listener(
			platform->registry,
			&listener_registry,
			platform);

	if (error_posix == -1)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	// perform a roundtrip for the registry
	error_posix = wl_display_roundtrip(platform->display);

	if (error_posix == -1)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_ROUNDTRIP);
		return;
	}

	// check we have everything we need
	if (platform->compositor == NULL)
	{
		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_WAYLAND_COMPOSITOR_MISSING);
		return;
	}

	if (platform->xdg_wm_base == NULL)
	{
		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_WAYLAND_XDG_WM_BASE_MISSSING);
		return;
	}

	// create wayland surface
	platform->surface =
		wl_compositor_create_surface(
			platform->compositor);

	if (platform->surface == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_SURFACE_CREATE);
		return;
	}

	// get xdg surface
	platform->xdg_surface =
		xdg_wm_base_get_xdg_surface(
			platform->xdg_wm_base,
			platform->surface);	

	if (platform->xdg_surface == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_XDG_SURFACE_CREATE);
		return;
	}

	// set xdg surface listener
	struct xdg_surface_listener listener_xdg_surface =
	{
		.configure = globox_wayland_helpers_xdg_surface_configure;
	};

	error_posix =
		xdg_surface_add_listener(
			platform->xdg_surface,
			&listener_xdg_surface,
			platform);

	if (error_posix == -1)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	// get xdg toplevel
	platform->xdg_toplevel =
		xdg_surface_get_toplevel(
			platform->xdg_surface);

	if (platform->xdg_toplevel == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_XDG_TOPLEVEL_GET);
		return;
	}

	// set xdg toplevel listener
	struct xdg_toplevel_listener listener_xdg_toplevel =
	{
		.configure = globox_wayland_helpers_xdg_toplevel_configure;
		.close = globox_wayland_helpers_xdg_toplevel_close;
	};

	error_posix =
		xdg_toplevel_add_listener(
			platform->xdg_toplevel,
			&listener_xdg_toplevel,
			platform);

	if (error_posix == -1)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_LISTENER_ADD);
		return;
	}

	// configure features
	struct globox_config_reply* reply =
		malloc(platform->feature_count * (sizeof (struct globox_config_reply)));

	if (reply == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	for (size_t i = 0; i < platform->feature_count; ++i)
	{
		enum globox_feature feature = platform->feature_configs[i].feature;
		reply[i].feature = feature;

		switch (feature)
		{
			case GLOBOX_FEATURE_STATE:
			{
				globox_wayland_helpers_set_state(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				globox_wayland_helpers_set_title(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				globox_wayland_helpers_set_icon(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				globox_wayland_helpers_set_frame(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				globox_wayland_helpers_set_background(context, platform, &reply[i].error);
				break;
			}
			case GLOBOX_FEATURE_VSYNC:
			{
				globox_wayland_helpers_set_vsync(context, platform, &reply[i].error);
				break;
			}
			default:
			{
				reply[i].error.code = GLOBOX_ERROR_OK;
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

	globox_error_ok(error);
}

void globox_wayland_common_window_start(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// init thread attributes
	int error_posix;
	pthread_attr_t attr;

	error_posix = pthread_attr_init(&attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_INIT);
		return;
	}

	error_posix = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (error_posix != 0)
	{
		pthread_attr_destroy(&attr);
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_JOINABLE);
		return;
	}

	// show surface
	wl_surface_commit(platform->surface);

	// start the event loop in a new thread
	// init thread function data
	struct wayland_thread_event_loop_data event_data =
	{
		.globox = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_event_loop_data = event_data;

	// start function in a new thread
	error_posix =
		pthread_create(
			&(platform->thread_event_loop),
			&attr,
			globox_wayland_helpers_event_loop,
			&(platform->thread_event_loop_data));

	if (error_posix != 0)
	{
		pthread_attr_destroy(&attr);
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_CREATE);
		return;
	}

	if (context->feature_vsync->vsync == false)
	{
		// start the render loop in a new thread
		// init thread function data
		struct wayland_thread_render_loop_data render_data =
		{
			.globox = context,
			.platform = platform,
			.error = error,
		};

		platform->thread_render_loop_data = render_data;

		// start function in a new thread
		error_posix =
			pthread_create(
				&(platform->thread_render_loop),
				&attr,
				globox_wayland_helpers_render_loop,
				&(platform->thread_render_loop_data));

		if (error_posix != 0)
		{
			pthread_attr_destroy(&attr);
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_CREATE);
			return;
		}
	}
	else
	{
		// get surface frame
		platform->surface_frame = wl_surface_frame(platform->surface);

		if (platform->surface_frame == NULL)
		{
			pthread_attr_destroy(&attr);
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_SURFACE_FRAME_GET);
			return;
		}

		// set surface frame callback
		struct wl_callback_listener listener_surface_frame =
		{
			.done = globox_wayland_helpers_surface_frame_done;
		};

		error_posix =
			wl_callback_add_listener(
				platform->surface_frame,
				&listener_surface_frame,
				platform);

		if (error_posix == -1)
		{
			pthread_attr_destroy(&attr);
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_LISTENER_ADD);
			return;
		}
	}

	// destroy the attributes
	error_posix = pthread_attr_destroy(&attr);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_ATTR_DESTROY);
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_common_window_block(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	int error_posix;
	int error_cond;

	// lock block mutex
	error_posix = pthread_mutex_lock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	error_cond = pthread_cond_wait(&(platform->cond_main), &(platform->mutex_block));

	// unlock block mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_block));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	if (error_cond != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_COND_WAIT);
		return;
	}

	error_posix = pthread_join(platform->thread_event_loop, NULL);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_JOIN);
		return;
	}

	error_posix = pthread_join(platform->thread_render_loop, NULL);

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_THREAD_JOIN);
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_common_window_stop(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}


void globox_wayland_common_init_render(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	// set the event callback
	context->render_callback = *config;
	globox_error_ok(error);
}

void globox_wayland_common_init_events(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	// set the event callback
	context->event_callbacks = *config;
	globox_error_ok(error);
}

enum globox_event globox_wayland_common_handle_events(
	struct globox* context,
	struct wayland_platform* platform,
	void* event,
	struct globox_error_info* error)
{
	globox_error_ok(error);
	return GLOBOX_EVENT_UNKNOWN;
}

struct globox_config_features*
	globox_wayland_common_init_features(
		struct globox* context,
		struct wayland_platform* platform,
		struct globox_error_info* error)
{
	struct globox_config_features* features =
		malloc(sizeof (struct globox_config_features));

	if (features == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	features->count = 0;
	features->list =
		malloc(GLOBOX_FEATURE_COUNT * (sizeof (enum globox_feature)));

	if (features->list == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_INTERACTION;
	context->feature_interaction =
		malloc(sizeof (struct globox_feature_interaction));
	features->count += 1;

	if (context->feature_interaction == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	context->feature_interaction->action = GLOBOX_INTERACTION_STOP;

	// always available
	features->list[features->count] = GLOBOX_FEATURE_STATE;
	context->feature_state =
		malloc(sizeof (struct globox_feature_state));
	features->count += 1;

	if (context->feature_state == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_TITLE;
	context->feature_title =
		malloc(sizeof (struct globox_feature_title));
	features->count += 1;

	if (context->feature_title == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_ICON;
	context->feature_icon =
		malloc(sizeof (struct globox_feature_icon));
	features->count += 1;

	if (context->feature_icon == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_SIZE;
	context->feature_size =
		malloc(sizeof (struct globox_feature_size));
	features->count += 1;

	if (context->feature_size == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_POS;
	context->feature_pos =
		malloc(sizeof (struct globox_feature_pos));
	features->count += 1;

	if (context->feature_pos == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_FRAME;
	context->feature_frame =
		malloc(sizeof (struct globox_feature_frame));
	features->count += 1;

	if (context->feature_frame == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_BACKGROUND;
	context->feature_background =
		malloc(sizeof (struct globox_feature_background));
	features->count += 1;

	if (context->feature_background == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBOX_FEATURE_VSYNC;
	context->feature_vsync =
		malloc(sizeof (struct globox_feature_vsync));
	features->count += 1;

	if (context->feature_vsync == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	globox_error_ok(error);
	return features;
}

void globox_wayland_common_feature_set_interaction(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	*(context->feature_interaction) = *config;

	switch (config->action)
	{
		case GLOBOX_INTERACTION_MOVE:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
			break;
		}
		case GLOBOX_INTERACTION_N:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
			break;
		}
		case GLOBOX_INTERACTION_NW:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
			break;
		}
		case GLOBOX_INTERACTION_W:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
			break;
		}
		case GLOBOX_INTERACTION_SW:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
			break;
		}
		case GLOBOX_INTERACTION_S:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
			break;
		}
		case GLOBOX_INTERACTION_SE:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
			break;
		}
		case GLOBOX_INTERACTION_E:
		{
			platform->sizing_edge = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
			break;
		}
		case GLOBOX_INTERACTION_NE:
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
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_common_feature_set_state(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	*(context->feature_state) = *config;
	globox_wayland_helpers_set_state(context, platform, error);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_common_feature_set_title(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure
	free_check(context->feature_title->title);

	context->feature_title->title = strdup(config->title);
	globox_wayland_helpers_set_title(context, platform, error);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_wayland_common_feature_set_icon(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
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

	globox_wayland_helpers_set_icon(context, platform, error);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// return on configuration error
	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

unsigned globox_wayland_common_get_width(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->width;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return 0;
	}

	// return value
	globox_error_ok(error);
	return value;
}

unsigned globox_wayland_common_get_height(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->height;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return 0;
	}

	// return value
	globox_error_ok(error);
	return value;
}

struct globox_rect globox_wayland_common_get_expose(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	struct globox_rect dummy =
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
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return dummy;
	}

	// save value
	struct globox_rect value = context->expose;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return dummy;
	}

	// return value
	globox_error_ok(error);
	return value;
}
