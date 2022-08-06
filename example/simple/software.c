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

void event_callback(void* data, void* event)
{
	struct globox_error_info error = {0};

	// print some debug info on internal events
	enum globox_event abstract =
		globox_handle_events(data, event, &error);

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
		case GLOBOX_EVENT_INVALID:
		{
			fprintf(stderr, "received invalid event\n");
			break;
		}
	}
}

void vsync_callback(void* data)
{
	// render our trademark square as a simple example, updating the whole
	// buffer each time without taking surface damage events into account
	struct globox* globox = data;
	struct globox_error_info error = {0};

	size_t width = globox_get_width(globox, &error);
	size_t height = globox_get_height(globox, &error);
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

	globox_update_content(globox, &update, &error);
}

int main(int argc, char** argv)
{
	struct globox_error_info error = {0};
	printf("starting the simple globox example\n");

	// prepare function pointers
	struct globox_config_backend config = {0};

	globox_prepare_init_x11_software(&config, &error);

	// set function pointers and perform basic init
	struct globox* globox = globox_init(&config, &error);

	if (globox == NULL)
	{
		fprintf(stderr, "\ncould not allocate the main globox context\n");
		return 1;
	}

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_clean(globox, &error);
		return 1;
	}

	// get available features
	struct globox_config_features* feature_list =
		globox_init_features(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_clean(globox, &error);
		return 1;
	}

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
		[GLOBOX_FEATURE_VSYNC_CALLBACK] = "vsync callback",
	};

	printf("\nreceived a list of available features:\n");

	for (size_t i = 0; i < feature_list->count; ++i)
	{
		printf(" - %s\n", feature_names[feature_list->list[i]]);
	}

	free(feature_list);

	// register an event handler to track the window's state
	struct globox_config_events events =
	{
		.data = globox,
		.handler = event_callback,
	};

	globox_init_events(globox, &events, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_clean(globox, &error);
		return 1;
	}

	// create the window
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
		.pixmap = (uint32_t*) &iconpix_beg,
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

	struct globox_feature_vsync_callback vsync =
	{
		.data = globox,
		.callback = vsync_callback,
	};

	void* feature_configs[GLOBOX_FEATURE_COUNT] =
	{
		[GLOBOX_FEATURE_INTERACTION] = NULL,
		[GLOBOX_FEATURE_STATE] = &state,
		[GLOBOX_FEATURE_TITLE] = &title,
		[GLOBOX_FEATURE_ICON] = &icon,
		[GLOBOX_FEATURE_SIZE] = &size,
		[GLOBOX_FEATURE_POS] = &pos,
		[GLOBOX_FEATURE_FRAME] = &frame,
		[GLOBOX_FEATURE_BACKGROUND] = &background,
		[GLOBOX_FEATURE_VSYNC_CALLBACK] = &vsync,
	};

	globox_window_create(globox, feature_configs, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		if (globox_error_get_code(&error) == GLOBOX_ERROR_FEATURE_SET)
		{
// TODO
#if 0
			// check for frame presence
			struct globox_feature_frame frame;
			globox_feature_get_frame(globox, &frame);

			if (frame.frame == false)
			{
				fprintf(stderr,
					"your desktop environment expects apps to render their own\n"
					"window frames, please report this to its developers so they\n"
					"can fix the issue and improve the user experience for everyone!\n");
			}

			// check for background blur presence
			struct globox_feature_background background;
			globox_feature_get_background(globox, &background);

			if (background.background != GLOBOX_BACKGROUND_BLURRED)
			{
				fprintf(stderr,
					"your desktop environment does not support background blur!\n");
			}
#endif
		}
		else
		{
			globox_clean(globox, &error);
			return 1;
		}
	}

	// display the window
	globox_window_start(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"\nThis is a message from the main thread.\n"
		"The window should now be visible.\n"
		"We can keep computing here.\n");

	// TODO temporarily disable this and test all setters (with sleeps)
	// wait for the window to be closed
	globox_window_block(globox, &error);

	// free resources correctly
	globox_window_destroy(globox, &error);
	globox_clean(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		return 1;
	}

	return 0;
}
