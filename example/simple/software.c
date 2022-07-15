#include "globox.h"
#include "globox_software.h"
#include "globox_x11_software.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern unsigned char iconpix_beg;
extern unsigned char iconpix_end;
extern unsigned char iconpix_len;

void feature_callback_background(void* data, enum globox_background background)
{
	// fair enough
	if (background != GLOBOX_BACKGROUND_BLURRED)
	{
		fprintf(stderr,
			"your desktop environment does not support background blur!\n");
	}
}

void feature_callback_frame(void* data, bool frame)
{
	// fuck you gnome developers
	if (frame == false)
	{
		// love you gnome users
		fprintf(stderr,
			"your desktop environment expects apps to render their own\n"
			"window frames, please report this to its developers so they\n"
			"can fix the issue and improve the user experience for everyone!\n");
	}
}

void event_callback(void* data, void* event)
{
	// print some debug info on internal events
	enum globox_event abstract =
		globox_handle_events(data, event);

	switch (abstract)
	{
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
		case GLOBOX_EVENT_MOVED:
		{
			fprintf(stderr, "received `moved` event\n");
			break;
		}
		case GLOBOX_EVENT_RESIZED_N:
		{
			fprintf(stderr, "received `resized (north)` event\n");
			break;
		}
		case GLOBOX_EVENT_RESIZED_NW:
		{
			fprintf(stderr, "received `resized (north-west)` event\n");
			break;
		}
		case GLOBOX_EVENT_RESIZED_W:
		{
			fprintf(stderr, "received `resized (west)` event\n");
			break;
		}
		case GLOBOX_EVENT_RESIZED_SW:
		{
			fprintf(stderr, "received `resized (south-west)` event\n");
			break;
		}
		case GLOBOX_EVENT_RESIZED_S:
		{
			fprintf(stderr, "received `resized (south)` event\n");
			break;
		}
		case GLOBOX_EVENT_RESIZED_SE:
		{
			fprintf(stderr, "received `resized (south-east)` event\n");
			break;
		}
		case GLOBOX_EVENT_RESIZED_E:
		{
			fprintf(stderr, "received `resized (east)` event\n");
			break;
		}
		case GLOBOX_EVENT_RESIZED_NE:
		{
			fprintf(stderr, "received `resized (north-east)` event\n");
			break;
		}
		case GLOBOX_EVENT_CONTENT_DAMAGED:
		{
			fprintf(stderr, "received `content damaged` event\n");
			break;
		}
		case GLOBOX_EVENT_DISPLAY_CHANGED:
		{
			fprintf(stderr, "received `display changed` event\n");
			break;
		}
	}
}

void vsync_callback(void* data)
{
	// render our trademark square as a simple example, updating the whole
	// buffer each time without taking surface damage events into account
	struct globox* globox = data;
	size_t width = globox_get_width(globox);
	size_t height = globox_get_height(globox);
	uint32_t* argb = malloc(width * height * 4);

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

	globox_update_content(globox, &update);
}

int main(int argc, char** argv)
{
	struct globox* globox = NULL;
	struct globox_config_backend config = {0};
	struct globox_config_features* features = NULL;

	// basic init
	globox_init(globox);

	if (globox_error_catch(globox))
	{
		return 1;
	}

	// platform & backend init
	globox_prepare_init_x11_software(globox, &config);

	if (globox_error_catch(globox))
	{
		globox_clean(globox);
		return 1;
	}

	globox_init_backend(globox, &config);

	if (globox_error_catch(globox))
	{
		globox_clean(globox);
		return 1;
	}

	// get available features
	features = globox_init_features(globox);

	for (size_t i = 0; i < features->count; ++i)
	{
		switch (features->list[i])
		{
			case GLOBOX_FEATURE_STATE:
			{
				struct globox_feature_state state =
				{
					.state = GLOBOX_STATE_REGULAR,
				};

				globox_set_state(
					globox,
					&state);

				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				struct globox_feature_title title =
				{
					.title = "globox",
				};

				globox_set_title(
					globox,
					&title);

				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				struct globox_feature_icon icon =
				{
					// acceptable implementation-defined behavior
					// since it's also the implementation that
					// allows us to bundle resources like so
					.pixmap = (uint32_t*) &iconpix_beg,
					.len = 2 + (16 * 16) + 2 + (32 * 32) + 2 + (64 * 64),
				};

				globox_set_icon(
					globox,
					&icon);

				break;
			}
			case GLOBOX_FEATURE_SIZE:
			{
				struct globox_feature_size size =
				{
					.width = 500,
					.height = 500,
				};

				globox_set_size(
					globox,
					&size);

				break;
			}
			case GLOBOX_FEATURE_POS:
			{
				struct globox_feature_pos pos =
				{
					.x = 250,
					.y = 250,
				};

				globox_set_pos(
					globox,
					&pos);

				break;
			}
			case GLOBOX_FEATURE_FRAMED:
			{
				struct globox_feature_frame frame =
				{
					.frame = true,
					.data = NULL,
					.callback = feature_callback_frame,
				};

				globox_set_frame(
					globox,
					&frame);

				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				struct globox_feature_background background =
				{
					.background = GLOBOX_BACKGROUND_BLURRED,
					.data = NULL,
					.callback = feature_callback_background,
				};

				globox_set_background(
					globox,
					&background);

				break;
			}
			case GLOBOX_FEATURE_VSYNC_CALLBACK:
			{
				struct globox_feature_vsync_callback vsync =
				{
					.data = globox,
					.callback = vsync_callback,
				};

				globox_set_vsync_callback(
					globox,
					&vsync);

				break;
			}
			default:
			{
				break;
			}
		}
	}

	// register an event handler to track the window's state
	struct globox_config_events events =
	{
		.data = NULL,
		.handler = event_callback,
	};

	globox_init_events(globox, &events);

	// create the window
	globox_window_create(globox);

	if (globox_error_catch(globox))
	{
		globox_clean(globox);
		return 1;
	}

	// display the window
	globox_window_start(globox);

	if (globox_error_catch(globox))
	{
		globox_window_destroy(globox);
		globox_clean(globox);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"This is a message from the main thread.\n"
		"The window should now be visible.\n"
		"We can keep computing here.\n");

	// wait for the window to be closed
	globox_window_block(globox);

	// free resources correctly
	globox_window_stop(globox);
	globox_window_destroy(globox);
	globox_clean(globox);

	return 0;
}