#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "wayland/wayland_common.h"
#include "wayland/wayland_common_helpers.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>

#include "xdg-decoration-client-protocol.h"
#include "kde-blur-client-protocol.h"

void* globox_wayland_helpers_render_loop(void* data)
{
	struct wayland_thread_render_loop_data* thread_render_loop_data = data;

	struct globox* context = thread_render_loop_data->globox;
	struct wayland_platform* platform = thread_render_loop_data->platform;
	struct globox_error_info* error = thread_render_loop_data->error;

	// lock main mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return NULL;
	}

	bool closed = platform->closed;

	// unlock main mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return NULL;
	}

	// thread init callback
	if (platform->render_init_callback != NULL)
	{
		platform->render_init_callback(context, error);

		if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			return NULL;
		}
	}

	while (closed == false)
	{
		// run developer callback
		context->render_callback.callback(context->render_callback.data);

		// lock main mutex
		error_posix = pthread_mutex_lock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			break;
		}

		closed = platform->closed;

		// unlock main mutex
		error_posix = pthread_mutex_unlock(&(platform->mutex_main));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			break;
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void* globox_wayland_helpers_event_loop(void* data)
{
	struct wayland_thread_event_loop_data* thread_event_loop_data = data;

	struct globox* context = thread_event_loop_data->globox;
	struct wayland_platform* platform = thread_event_loop_data->platform;
	struct globox_error_info* error = thread_event_loop_data->error;

	int error_posix;

	// thread init callback
	if (platform->event_init_callback != NULL)
	{
		platform->event_init_callback(context, error);

		if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			return NULL;
		}
	}

	while (platform->closed == false)
	{
		// block until we get an event
		error_posix = wl_display_dispatch(platform->display);

		// IO error
		if (error_posix == -1)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_EVENT_WAIT);
			break;
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void globox_wayland_helpers_features_init(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error)
{
	for (size_t i = 0; i < count; ++i)
	{
		switch (configs[i].feature)
		{
			case GLOBOX_FEATURE_STATE:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_state) =
						*((struct globox_feature_state*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				if (configs[i].config != NULL)
				{
					struct globox_feature_title* tmp = configs[i].config;
					context->feature_title->title = strdup(tmp->title);
				}

				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				if (configs[i].config != NULL)
				{
					struct globox_feature_icon* tmp = configs[i].config;
					context->feature_icon->pixmap = malloc(tmp->len * 4);

					if (context->feature_icon->pixmap != NULL)
					{
						memcpy(context->feature_icon->pixmap, tmp->pixmap, tmp->len * 4);
						context->feature_icon->len = tmp->len;
					}
					else
					{
						context->feature_icon->len = 0;
					}
				}

				break;
			}
			case GLOBOX_FEATURE_SIZE:
			{
				// handled directly in the wayland window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_size) =
						*((struct globox_feature_size*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_POS:
			{
				// handled directly in the wayland window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_pos) =
						*((struct globox_feature_pos*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_frame) =
						*((struct globox_feature_frame*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				// handled directly in the wayland window creation code for transparency,
				// but some more configuration has to take place afterwards for blur
				if (configs[i].config != NULL)
				{
					*(context->feature_background) =
						*((struct globox_feature_background*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_VSYNC:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_vsync) =
						*((struct globox_feature_vsync*)
							configs[i].config);
				}

				break;
			}
			default:
			{
				globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_INVALID);
				return;
			}
		}
	}
}

void globox_wayland_helpers_set_state(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	switch (context->feature_state->state)
	{
		case GLOBOX_STATE_REGULAR: // TODO special case for software backend
		{
			xdg_toplevel_unset_maximized(platform->xdg_toplevel);
			xdg_toplevel_unset_fullscreen(platform->xdg_toplevel);

			if (context->feature_state->state == GLOBOX_STATE_MINIMIZED)
			{
				// destroy surfaces
				wl_surface_destroy(platform->surface);
				xdg_surface_destroy(platform->xdg_surface);
				xdg_toplevel_destroy(platform->xdg_toplevel);

				// reset title
				globox_feature_set_title(context, context->feature_title, error);

				if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
				{
					return;
				}
			}

			// TODO check it's still needed
			// enlightenment bug workaround
			if (platform->xdg_decoration != NULL)
			{
				zxdg_toplevel_decoration_v1_set_mode(
					platform->xdg_decoration,
					platform->decoration_mode);
			}

			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			xdg_toplevel_unset_maximized(platform->xdg_toplevel);
			xdg_toplevel_unset_fullscreen(platform->xdg_toplevel);
			xdg_toplevel_set_minimized(platform->xdg_toplevel);
			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			xdg_toplevel_unset_fullscreen(platform->xdg_toplevel);
			xdg_toplevel_set_maximized(platform->xdg_toplevel);

			// TODO check it's still needed
			// enlightenment bug workaround
			if (platform->xdg_decoration != NULL)
			{
				zxdg_toplevel_decoration_v1_set_mode(
					platform->xdg_decoration,
					platform->decoration_mode);
			}

			break;
		}
		case GLOBOX_STATE_FULLSCREEN:
		{
			xdg_toplevel_unset_maximized(platform->xdg_toplevel);
			xdg_toplevel_set_fullscreen(platform->xdg_toplevel, NULL); // TODO select current window output?
			break;
		}
		default:
		{
			globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_STATE_INVALID);
			return;
		}
	}

	globox_error_ok(error);
}

void globox_wayland_helpers_set_title(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	xdg_toplevel_set_title(
		platform->xdg_toplevel,
		context->feature_title->title);

	// always ok
	globox_error_ok(error);
}

void globox_wayland_helpers_set_icon(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// never ok
	globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_ICON);
}

void globox_wayland_helpers_set_frame(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// Wayland clients are expected to render their own decorations by default,
	// but we can try to use the decorations negociation protocol to try and
	// have the compositor render them for us if it is able and willing to.

	// by not providing the XDG decoration protocol, compositors enforce CSDs
	if (platform->xdg_decoration == NULL)
	{
		if (context->feature_frame->frame == true)
		{
			context->feature_frame->frame = false;
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_DECORATIONS_UNAVAILABLE);
		}

		return;
	}

	// ask for the decoration mode we want
	if (context->feature_frame->frame == true)
	{
		platform->decoration_mode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE;
	}
	else
	{
		platform->decoration_mode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;
	}

	zxdg_toplevel_decoration_v1_set_mode(
		platform->xdg_decoration,
		platform->decoration_mode);

	// perform a roundtrip to find out if the request was successful
	int error_posix = wl_display_roundtrip(platform->display);

	if (error_posix == -1)
	{
		context->feature_frame->frame = false;
		globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_ROUNDTRIP);
		return;
	}

	// report actual feature status
	if (platform->decoration_mode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE)
	{
		if (context->feature_frame->frame == false)
		{
			context->feature_frame->frame = true;
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_DECORATIONS_FORCED);
			return;
		}
	}
	else
	{
		if (context->feature_frame->frame == true)
		{
			context->feature_frame->frame = false;
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_DECORATIONS_UNAVAILABLE);
			return;
		}
	}

	globox_error_ok(error);
}

void globox_wayland_helpers_set_background(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// The Wayland specification requires alpha support so transparency is
	// always available, however background blur requires the dedicated
	// KDE protocol so we have to check for this.

	// enable background blur if possible, otherwise use regular transparency
	if (context->feature_background->background == GLOBOX_BACKGROUND_BLURRED)
	{
		// check the protocol is supported
		if (platform->kde_blur_manager == NULL)
		{
			context->feature_background->background = GLOBOX_BACKGROUND_TRANSPARENT;
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_BACKGROUND_BLUR);
			return;
		}

		// try to enable blur using the protocol
		platform->kde_blur =
			org_kde_kwin_blur_manager_create(
				platform->kde_blur_manager,
				platform->surface);

		if (platform->kde_blur == NULL)
		{
			context->feature_background->background = GLOBOX_BACKGROUND_TRANSPARENT;
			globox_error_throw(context, error, GLOBOX_ERROR_WAYLAND_REQUEST);
			return;
		}

		org_kde_kwin_blur_commit(platform->kde_blur);
	}

	globox_error_ok(error);
}

void globox_wayland_helpers_set_vsync(
	struct globox* context,
	struct wayland_platform* platform,
	struct globox_error_info* error)
{
	// always ok
	globox_error_ok(error);
}
