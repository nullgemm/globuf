#include "globox.h"
#include "globox_software.h"
#include "globox_x11_software.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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
	enum globox_event abstract =
		globox_handle_events(data, event);

	switch (abstract)
	{
		case GLOBOX_EVENT_RESTORED:
		case GLOBOX_EVENT_MINIMIZED:
		case GLOBOX_EVENT_MAXIMIZED:
		case GLOBOX_EVENT_FULLSCREEN:
		case GLOBOX_EVENT_RESIZED_N:
		case GLOBOX_EVENT_RESIZED_NW:
		case GLOBOX_EVENT_RESIZED_W:
		case GLOBOX_EVENT_RESIZED_SW:
		case GLOBOX_EVENT_RESIZED_S:
		case GLOBOX_EVENT_RESIZED_SE:
		case GLOBOX_EVENT_RESIZED_E:
		case GLOBOX_EVENT_RESIZED_NE:
		{
			// TODO update app buffer size
			break;
		}
	}
}

void vsync_callback(void* data)
{
	struct globox* globox = data;

	// TODO render something
	// TODO fill the update structure
	struct globox_update_software update =
	{
		.buf = NULL,
		.x = 0,
		.y = 0,
		.width = 0,
		.height = 0,
	};

	globox_update_content(globox, &update);
}

int main(int argc, char** argv)
{
	struct globox globox = {0};
	struct globox_config_backend config = {0};
	struct globox_config_features* features = NULL;

	// basic init
	globox_init(&globox);

	if (globox_error_catch(&globox))
	{
		return 1;
	}

	// platform & backend init
	globox_init_prepare_x11_software(&globox, &config);

	if (globox_error_catch(&globox))
	{
		globox_clean(&globox);
		return 1;
	}

	globox_init_backend(&globox, &config);

	if (globox_error_catch(&globox))
	{
		globox_clean(&globox);
		return 1;
	}

	// get available features
	features = globox_init_features(&globox);

	for (size_t i = 0; i < features->count; ++i)
	{
		switch (features->list[i])
		{
			case GLOBOX_FEATURE_TITLE:
			{
				struct globox_feature_title title =
				{
					.title = "globox",
				};

				globox_set_title(
					&globox,
					&title);

				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				// TODO
				struct globox_feature_icon icon =
				{
					.pixmap = NULL,
					.len = 0,
				};

				globox_set_icon(
					&globox,
					&icon);

				break;
			}
			case GLOBOX_FEATURE_INIT_SIZE:
			{
				struct globox_feature_init_size size =
				{
					.width_init = 500,
					.height_init = 500,
				};

				globox_set_init_size(
					&globox,
					&size);

				break;
			}
			case GLOBOX_FEATURE_INIT_POS:
			{
				struct globox_feature_init_pos pos =
				{
					.x_init = 250,
					.y_init = 250,
				};

				globox_set_init_pos(
					&globox,
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
					&globox,
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
					&globox,
					&background);

				break;
			}
			case GLOBOX_FEATURE_VSYNC_CALLBACK:
			{
				struct globox_feature_vsync_callback vsync =
				{
					.data = &globox,
					.callback = vsync_callback,
				};

				globox_set_vsync_callback(
					&globox,
					&vsync);

				break;
			}
		}
	}

	// register an event handler to track the window's state
	globox_init_events(&globox, event_callback);

	// create the window
	globox_window_create(&globox);

	if (globox_error_catch(&globox))
	{
		globox_clean(&globox);
		return 1;
	}

	// display the window
	globox_window_start(&globox);

	if (globox_error_catch(&globox))
	{
		globox_window_destroy(&globox);
		globox_clean(&globox);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"This is a message from the main thread.\n"
		"The window should now be visible.\n"
		"We can keep computing here.\n");

	// wait for the window to be closed
	globox_window_block(&globox);

	// free resources correctly
	globox_window_stop(&globox);
	globox_window_destroy(&globox);
	globox_clean(&globox);

	return 0;
}
