#include "globox.h"
#include "globox_software.h"

#if defined(GLOBOX_EXAMPLE_X11)
#include "globox_x11_software.h"
#elif defined(GLOBOX_EXAMPLE_APPKIT)
#include "globox_appkit_software.h"
#elif defined(GLOBOX_EXAMPLE_WIN)
#include "globox_win_software.h"
#elif defined(GLOBOX_EXAMPLE_WAYLAND)
#include "globox_wayland_software.h"
#endif

#ifdef GLOBOX_EXAMPLE_APPKIT
#define main real_main
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern uint8_t iconpix[];
extern int iconpix_size;

char* feature_names[GLOBOX_FEATURE_COUNT] =
{
	[GLOBOX_FEATURE_INTERACTION] = "interaction",
	[GLOBOX_FEATURE_STATE] = "state",
	[GLOBOX_FEATURE_TITLE] = "title",
	[GLOBOX_FEATURE_ICON] = "icon",
	[GLOBOX_FEATURE_SIZE] = "size",
	[GLOBOX_FEATURE_POS] = "pos",
	[GLOBOX_FEATURE_FRAME] = "frame",
	[GLOBOX_FEATURE_BACKGROUND] = "background",
	[GLOBOX_FEATURE_VSYNC] = "vsync",
};

static void event_callback(void* data, void* event)
{
	struct globox* globox = data;
	struct globox_error_info error = {0};

	// print some debug info on internal events
	enum globox_event abstract =
		globox_handle_events(globox, event, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	switch (abstract)
	{
		case GLOBOX_EVENT_INVALID:
		{
			// shouldn't be possible since we handle the error
			fprintf(stderr, "received invalid event\n");
			break;
		}
		case GLOBOX_EVENT_UNKNOWN:
		{
#ifdef GLOBOX_EXAMPLE_LOG_ALL
			fprintf(stderr, "received unknown event\n");
#endif
			break;
		}
		case GLOBOX_EVENT_RESTORED:
		{
			fprintf(stderr, "received `restored` event\n");
			break;
		}
		case GLOBOX_EVENT_MINIMIZED:
		{
			fprintf(stderr, "received `minimized` event\n");
			break;
		}
		case GLOBOX_EVENT_MAXIMIZED:
		{
			fprintf(stderr, "received `maximized` event\n");
			break;
		}
		case GLOBOX_EVENT_FULLSCREEN:
		{
			fprintf(stderr, "received `fullscreen` event\n");
			break;
		}
		case GLOBOX_EVENT_CLOSED:
		{
			fprintf(stderr, "received `closed` event\n");
			break;
		}
		case GLOBOX_EVENT_MOVED_RESIZED:
		{
			fprintf(stderr, "received `moved` event\n");
			break;
		}
		case GLOBOX_EVENT_DAMAGED:
		{
#ifdef GLOBOX_EXAMPLE_LOG_ALL
			struct globox_rect rect = globox_get_expose(globox, &error);

			if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
			{
				globox_error_log(globox, &error);
				break;
			}

			fprintf(
				stderr,
				"received `content damaged` event:\n"
				"\tx: %d px\n"
				"\ty: %d px\n"
				"\twidth: %d px\n"
				"\theight: %d px\n",
				rect.x,
				rect.y,
				rect.width,
				rect.height);
#endif

			break;
		}
	}
}

static void render_callback(void* data)
{
	// render our trademark square as a simple example, updating the whole
	// buffer each time without taking surface damage events into account
	struct globox* globox = data;
	struct globox_error_info error = {0};

	size_t width = globox_get_width(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	size_t height = globox_get_height(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	if ((width == 0) || (height == 0))
	{
		// skip rendering if the window area is 0
		return;
	}

	uint32_t* argb =
#if defined(GLOBOX_EXAMPLE_X11)
		globox_buffer_alloc_x11_software(
			globox, width, height, &error);
#elif defined(GLOBOX_EXAMPLE_APPKIT)
		globox_buffer_alloc_appkit_software(
			globox, width, height, &error);
#elif defined(GLOBOX_EXAMPLE_WIN)
		globox_buffer_alloc_win_software(
			globox, width, height, &error);
#elif defined(GLOBOX_EXAMPLE_WAYLAND)
		globox_buffer_alloc_wayland_software(
			globox, width, height, &error);
#endif

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	if (argb == NULL)
	{
		return;
	}

	for (size_t i = 0; i < (width * height); ++i)
	{
		argb[i] = 0x22000000;
	}

	size_t pos;
	size_t square_width = (width < 100) ? width : 100;
	size_t square_height = (height < 100) ? height : 100;

	for (size_t i = 0; i < (square_width * square_height); ++i)
	{
		pos =
			(((height - square_height) / 2) + (i / square_width)) * width
			+ (width - square_width) / 2
			+ (i % square_width);

		argb[pos] = 0xFFFFFFFF;
	}

	struct globox_update_software update =
	{
		.buf = argb,
		.x = 0,
		.y = 0,
		.width = width,
		.height = height,
	};

	globox_update_content(globox, &update, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

#if defined(GLOBOX_EXAMPLE_X11)
	globox_buffer_free_x11_software(
		globox, argb, &error);
#elif defined(GLOBOX_EXAMPLE_APPKIT)
	globox_buffer_free_appkit_software(
		globox, argb, &error);
#elif defined(GLOBOX_EXAMPLE_WIN)
	globox_buffer_free_win_software(
		globox, argb, &error);
#elif defined(GLOBOX_EXAMPLE_WAYLAND)
	globox_buffer_free_wayland_software(
		globox, argb, &error);
#endif

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}
}

static void config_callback(struct globox_config_reply* replies, size_t count, void* data)
{
	fprintf(stderr, "window configured succesfully, printing information:\n");

	struct globox* context = data;
	const char* message = NULL;
	size_t feature;

	for (size_t i = 0; i < count; ++i)
	{
		feature = replies[i].feature;

		if (feature < GLOBOX_FEATURE_COUNT)
		{
			if (replies[i].error.code == GLOBOX_ERROR_OK)
			{
				message = "success";
			}
			else
			{
				message = globox_error_get_msg(context, &replies[i].error);
			}

			fprintf(stderr, "\t%s: %s\n", feature_names[feature], message);
		}
	}
}

int main(int argc, char** argv)
{
	struct globox_error_info error = {0};
	struct globox_error_info error_early = {0};
	printf("starting the simple globox example\n");

	// prepare function pointers
	struct globox_config_backend config = {0};

#if defined(GLOBOX_EXAMPLE_X11)
	globox_prepare_init_x11_software(&config, &error_early);
#elif defined(GLOBOX_EXAMPLE_APPKIT)
	globox_prepare_init_appkit_software(&config, &error_early);
#elif defined(GLOBOX_EXAMPLE_WIN)
	globox_prepare_init_win_software(&config, &error_early);
#elif defined(GLOBOX_EXAMPLE_WAYLAND)
	globox_prepare_init_wayland_software(&config, &error_early);
#endif

	// set function pointers and perform basic init
	struct globox* globox = globox_init(&config, &error);

	// Unless the context allocation failed it is always possible to access
	// error messages (even when the context initialization failed) so we can
	// always handle the backend initialization error first.

	// context allocation failed
	if (globox == NULL)
	{
		fprintf(stderr, "could not allocate the main globox context\n");
		return 1;
	}

	// Backend initialization failed. Since it happens before globox
	// initialization and errors are accessible even if it fails, we can handle
	// the errors in the right order regardless.
	if (globox_error_get_code(&error_early) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_early);
		globox_clean(globox, &error);
		return 1;
	}

	// The globox initialization had failed, make it known now if the backend
	// initialization that happened before went fine.
	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// get available features
	struct globox_config_features* feature_list =
		globox_init_features(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// initialize features when creating the window
	struct globox_feature_state state =
	{
		.state = GLOBOX_STATE_REGULAR,
	};

	struct globox_feature_title title =
	{
		.title = "globox",
	};

	struct globox_feature_icon icon =
	{
		// acceptable implementation-defined behavior
		// since it's also the implementation that
		// allows us to bundle resources like so
		.pixmap = (uint32_t*) iconpix,
		.len = 2 + (16 * 16) + 2 + (32 * 32) + 2 + (64 * 64),
	};

	struct globox_feature_size size =
	{
		.width = 500,
		.height = 500,
	};

	struct globox_feature_pos pos =
	{
		.x = 250,
		.y = 250,
	};

	struct globox_feature_frame frame =
	{
		.frame = true,
	};

	struct globox_feature_background background =
	{
		.background = GLOBOX_BACKGROUND_BLURRED,
	};

	struct globox_feature_vsync vsync =
	{
		.vsync = true,
	};

	// configure the feature and print a list
	printf("received a list of available features:\n");

	struct globox_config_request configs[GLOBOX_FEATURE_COUNT] = {0};
	size_t feature_added = 0;
	size_t i = 0;

	while (i < feature_list->count)
	{
		enum globox_feature feature_id = feature_list->list[i];
		printf("\t%s\n", feature_names[feature_id]);
		++i;

		switch (feature_id)
		{
			case GLOBOX_FEATURE_STATE:
			{
				configs[feature_added].config = &state;
				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				configs[feature_added].config = &title;
				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				configs[feature_added].config = &icon;
				break;
			}
			case GLOBOX_FEATURE_SIZE:
			{
				configs[feature_added].config = &size;
				break;
			}
			case GLOBOX_FEATURE_POS:
			{
				configs[feature_added].config = &pos;
				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				configs[feature_added].config = &frame;
				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				configs[feature_added].config = &background;
				break;
			}
			case GLOBOX_FEATURE_VSYNC:
			{
				configs[feature_added].config = &vsync;
				break;
			}
			default:
			{
				continue;
			}
		}

		configs[feature_added].feature = feature_id;
		++feature_added;
	}

	free(feature_list->list);
	free(feature_list);

	// register an event handler to track the window's state
	struct globox_config_events events =
	{
		.data = globox,
		.handler = event_callback,
	};

	struct globox_error_info error_events = {0};
	globox_init_events(globox, &events, &error_events);

	if (globox_error_get_code(&error_events) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_events);
		globox_clean(globox, &error);
		return 1;
	}

	// register a render callback
	struct globox_config_render render =
	{
		.data = globox,
		.callback = render_callback,
	};

	struct globox_error_info error_render = {0};
	globox_init_render(globox, &render, &error_render);

	if (globox_error_get_code(&error_render) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_render);
		globox_clean(globox, &error);
		return 1;
	}

	// create the window
	globox_window_create(globox, configs, feature_added, config_callback, globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// check window
	globox_window_confirm(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// display the window
	globox_window_start(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"this is a message from the main thread\n"
		"the window should now be visible\n"
		"we can keep computing here\n");

	// wait for the window to be closed
	globox_window_block(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// handle event thread errors
	if (globox_error_get_code(&error_events) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_events);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// handle render thread errors
	if (globox_error_get_code(&error_render) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_render);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// free resources correctly
	globox_window_destroy(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	globox_clean(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return 1;
	}

	return 0;
}
