#include "globuf.h"
#include "globuf_software.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"

#if 1// OR !defined(GLOBUF_SHARED)
#if defined(GLOBUF_EXAMPLE_X11)
#include "globuf_x11_software.h"
#include "cursoryx_x11.h"
#include "dpishit_x11.h"
#include "willis_x11.h"
#elif defined(GLOBUF_EXAMPLE_APPKIT)
#include "globuf_appkit_software.h"
#include "cursoryx_appkit.h"
#include "dpishit_appkit.h"
#include "willis_appkit.h"
#elif defined(GLOBUF_EXAMPLE_WIN)
#include "globuf_win_software.h"
#include "cursoryx_win.h"
#include "dpishit_win.h"
#include "willis_win.h"
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
#include "globuf_wayland_software.h"
#include "cursoryx_wayland.h"
#include "dpishit_wayland.h"
#include "willis_wayland.h"
#endif
#endif

#ifdef GLOBUF_EXAMPLE_APPKIT
#define main real_main
#endif

#if defined(GLOBUF_SHARED)
#if !defined(GLOBUF_EXAMPLE_WIN)
#include <dlfcn.h>
#else
#include <libloaderapi.h>
#endif
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern uint8_t iconpix[];
extern int iconpix_size;

extern uint8_t cursorpix[];
extern int cursorpix_size;

char* feature_names[GLOBUF_FEATURE_COUNT] =
{
	[GLOBUF_FEATURE_INTERACTION] = "interaction",
	[GLOBUF_FEATURE_STATE] = "state",
	[GLOBUF_FEATURE_TITLE] = "title",
	[GLOBUF_FEATURE_ICON] = "icon",
	[GLOBUF_FEATURE_SIZE] = "size",
	[GLOBUF_FEATURE_POS] = "pos",
	[GLOBUF_FEATURE_FRAME] = "frame",
	[GLOBUF_FEATURE_BACKGROUND] = "background",
	[GLOBUF_FEATURE_VSYNC] = "vsync",
};

struct event_callback_data
{
	struct globuf* globuf;

	struct cursoryx* cursoryx;
	struct dpishit* dpishit;
	struct willis* willis;

	struct globuf_feature_interaction action;
	struct cursoryx_custom* mouse_custom[4];
	size_t mouse_custom_active;
	bool mouse_grabbed;
};

static void event_callback(void* data, void* event)
{
	struct event_callback_data* event_callback_data = data;

	struct globuf* globuf = event_callback_data->globuf;
	struct globuf_error_info error = {0};

	// print some debug info on internal events
	enum globuf_event abstract =
		globuf_handle_events(globuf, event, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	switch (abstract)
	{
		case GLOBUF_EVENT_INVALID:
		{
			// shouldn't be possible since we handle the error
			fprintf(stderr, "received invalid event\n");
			break;
		}
		case GLOBUF_EVENT_UNKNOWN:
		{
#ifdef GLOBUF_EXAMPLE_LOG_ALL
			fprintf(stderr, "received unknown event\n");
#endif
			break;
		}
		case GLOBUF_EVENT_RESTORED:
		{
			fprintf(stderr, "received `restored` event\n");
			break;
		}
		case GLOBUF_EVENT_MINIMIZED:
		{
			fprintf(stderr, "received `minimized` event\n");
			break;
		}
		case GLOBUF_EVENT_MAXIMIZED:
		{
			fprintf(stderr, "received `maximized` event\n");
			break;
		}
		case GLOBUF_EVENT_FULLSCREEN:
		{
			fprintf(stderr, "received `fullscreen` event\n");
			break;
		}
		case GLOBUF_EVENT_CLOSED:
		{
			fprintf(stderr, "received `closed` event\n");
			break;
		}
		case GLOBUF_EVENT_MOVED_RESIZED:
		{
			fprintf(stderr, "received `moved` event\n");
			break;
		}
		case GLOBUF_EVENT_DAMAGED:
		{
#ifdef GLOBUF_EXAMPLE_LOG_ALL
			struct globuf_rect rect = globuf_get_expose(globuf, &error);

			if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
			{
				globuf_error_log(globuf, &error);
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

#ifdef GLOBUF_EXAMPLE_LOG_ALL
	// handle dpi changes
	struct dpishit* dpishit = event_callback_data->dpishit;
	struct dpishit_error_info error_dpishit = {0};
	struct dpishit_display_info display_info = {0};
	bool dpishit_valid = false;

	if (dpishit != NULL)
	{
		dpishit_valid =
			dpishit_handle_event(
				dpishit,
				event,
				&display_info,
				&error_dpishit);

		if (dpishit_error_get_code(&error_dpishit) != DPISHIT_ERROR_OK)
		{
			dpishit_error_log(dpishit, &error_dpishit);
			return;
		}

		if (dpishit_valid == true)
		{
			fprintf(
				stderr,
				"dpishit returned display info:\n"
				"\twidth: %u px\n"
				"\theight: %u px\n"
				"\twidth: %u mm\n"
				"\theight: %u mm\n",
				display_info.px_width,
				display_info.px_height,
				display_info.mm_width,
				display_info.mm_height);

			if (display_info.dpi_logic_valid == true)
			{
				fprintf(
					stderr,
					"\tlogic dpi: %lf dpi\n",
					display_info.dpi_logic);
			}

			if (display_info.dpi_scale_valid == true)
			{
				fprintf(
					stderr,
					"\tscale: %lf\n",
					display_info.dpi_scale);
			}
		}
	}
#endif

	// handle cursor changes
	struct cursoryx* cursoryx = event_callback_data->cursoryx;
	struct cursoryx_error_info error_cursoryx = {0};

	// handle dpi changes
	struct willis* willis = event_callback_data->willis;
	struct willis_error_info error_willis = {0};
	struct willis_event_info event_info = {0};

	if (willis != NULL)
	{
		willis_handle_event(
			willis,
			event,
			&event_info,
			&error_willis);

		if (willis_error_get_code(&error_willis) != WILLIS_ERROR_OK)
		{
			willis_error_log(willis, &error_willis);
			return;
		}

		// handle keys
	if (event_info.event_state != WILLIS_STATE_PRESS)
	{
		struct globuf_feature_state state;
		bool sizemove = false;

		switch (event_info.event_code)
		{
			case WILLIS_KEY_G:
			{
				if (event_callback_data->mouse_grabbed == false)
				{
					willis_mouse_grab(willis, &error_willis);
				}
				else
				{
					willis_mouse_ungrab(willis, &error_willis);
				}

				if (willis_error_get_code(&error_willis) != WILLIS_ERROR_OK)
				{
					willis_error_log(willis, &error_willis);
					return;
				}

				event_callback_data->mouse_grabbed =
					!event_callback_data->mouse_grabbed;

				break;
			}
			case WILLIS_KEY_M:
			{
				if (cursoryx == NULL)
				{
					break;
				}

				size_t cur = event_callback_data->mouse_custom_active;

				++cur;

				if (cur > 3)
				{
					cur = 0;
				}

				cursoryx_custom_set(
					cursoryx,
					event_callback_data->mouse_custom[cur],
					&error_cursoryx);

				if (cursoryx_error_get_code(&error_cursoryx) != CURSORYX_ERROR_OK)
				{
					cursoryx_error_log(cursoryx, &error_cursoryx);
					return;
				}

				event_callback_data->mouse_custom_active = cur;

				break;
			}
			case WILLIS_KEY_N:
			{
				// set a default regular cursor for our window (wait/busy cursor)
				cursoryx_set(cursoryx, CURSORYX_BUSY, &error_cursoryx);

				if (cursoryx_error_get_code(&error_cursoryx) != CURSORYX_ERROR_OK)
				{
					cursoryx_error_log(cursoryx, &error_cursoryx);
					return;
				}

				break;
			}
			case WILLIS_KEY_W:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_N;
				sizemove = true;
				break;
			}
			case WILLIS_KEY_Q:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_NW;
				sizemove = true;
				break;
			}
			case WILLIS_KEY_A:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_W;
				sizemove = true;
				break;
			}
			case WILLIS_KEY_Z:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_SW;
				sizemove = true;
				break;
			}
			case WILLIS_KEY_X:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_S;
				sizemove = true;
				break;
			}
			case WILLIS_KEY_C:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_SE;
				sizemove = true;
				break;
			}
			case WILLIS_KEY_D:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_E;
				sizemove = true;
				break;
			}
			case WILLIS_KEY_E:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_NE;
				sizemove = true;
				break;
			}
			case WILLIS_KEY_S:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_MOVE;
				sizemove = true;
				break;
			}
			case WILLIS_MOUSE_CLICK_LEFT:
			{
				event_callback_data->action.action = GLOBUF_INTERACTION_STOP;
				break;
			}
			case WILLIS_KEY_1:
			{
				state.state = GLOBUF_STATE_REGULAR;
				globuf_feature_set_state(globuf, &state, &error);
				break;
			}
			case WILLIS_KEY_2:
			{
				state.state = GLOBUF_STATE_MINIMIZED;
				globuf_feature_set_state(globuf, &state, &error);
				break;
			}
			case WILLIS_KEY_3:
			{
				state.state = GLOBUF_STATE_MAXIMIZED;
				globuf_feature_set_state(globuf, &state, &error);
				break;
			}
			case WILLIS_KEY_4:
			{
				state.state = GLOBUF_STATE_FULLSCREEN;
				globuf_feature_set_state(globuf, &state, &error);
				break;
			}
			default:
			{
				break;
			}
		}

		if (sizemove == true)
		{
			globuf_feature_set_interaction(globuf, &(event_callback_data->action), &error);
		}
	}

		// print debug info
		if (event_info.event_code != WILLIS_NONE)
		{
			fprintf(
				stderr,
				"willis returned event info:\n"
				"\tcode: %s\n"
				"\tstate: %s\n",
				willis_get_event_code_name(willis, event_info.event_code, &error_willis),
				willis_get_event_state_name(willis, event_info.event_state, &error_willis));
		}

		if (event_info.event_code == WILLIS_MOUSE_MOTION)
		{
			if (event_callback_data->mouse_grabbed == false)
			{
				fprintf(
					stderr,
					"\tmouse x: %d\n"
					"\tmouse y: %d\n",
					event_info.mouse_x,
					event_info.mouse_y);
			}
			else
			{
				fprintf(
					stderr,
					"\tdiff x: %d\n"
					"\tdiff y: %d\n",
					(int) (event_info.diff_x >> 32),
					(int) (event_info.diff_y >> 32));
			}
		}
		else if (event_info.utf8_size > 0)
		{
			fprintf(
				stderr,
				"\ttext: %.*s\n",
				(int) event_info.utf8_size,
				event_info.utf8_string);

			free(event_info.utf8_string);
		}
	}
}

static void render_callback(void* data)
{
	// render our trademark square as a simple example, updating the whole
	// buffer each time without taking surface damage events into account
	struct globuf* globuf = data;
	struct globuf_error_info error = {0};

	size_t width = globuf_get_width(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	size_t height = globuf_get_height(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	if ((width == 0) || (height == 0))
	{
		// skip rendering if the window area is 0
		return;
	}

	uint32_t* argb =
		globuf_buffer_alloc_software(
			globuf, width, height, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
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

	struct globuf_update_software update =
	{
		.buf = argb,
		.x = 0,
		.y = 0,
		.width = width,
		.height = height,
	};

	globuf_update_content(globuf, &update, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	globuf_buffer_free_software(
		globuf, argb, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}
}

static void config_callback(struct globuf_config_reply* replies, size_t count, void* data)
{
	fprintf(stderr, "window configured succesfully, printing information:\n");

	struct globuf* context = data;
	const char* message = NULL;
	size_t feature;

	for (size_t i = 0; i < count; ++i)
	{
		feature = replies[i].feature;

		if (feature < count)
		{
			if (replies[i].error.code == GLOBUF_ERROR_OK)
			{
				message = "success";
			}
			else
			{
				message = globuf_error_get_msg(context, &replies[i].error);
			}

			fprintf(stderr, "\t%s: %s\n", feature_names[feature], message);
		}
	}
}

int main(int argc, char** argv)
{
	struct globuf_error_info error = {0};
	struct globuf_error_info error_early = {0};
	printf("starting the complex globuf example\n");

	// prepare function pointers
	struct globuf_config_backend config = {0};

#if !defined(GLOBUF_SHARED)
	// initialize statically
#if defined(GLOBUF_EXAMPLE_X11)
	globuf_prepare_init_x11_software(&config, &error_early);
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	globuf_prepare_init_appkit_software(&config, &error_early);
#elif defined(GLOBUF_EXAMPLE_WIN)
	globuf_prepare_init_win_software(&config, &error_early);

	printf(
		"\nEncoding notice: this example outputs utf-8 encoded text as a"
		" demonstration of the composition capabilities of willis, the"
		" independent lib we use to handle the keyboard and mouse.\n"
		"Since Windows only supports utf-8 console output using wchar_t"
		" (which we do not use) non-ANSI text will not display properly"
		" on this platform, but the text in RAM really is valid.\n\n");
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	globuf_prepare_init_wayland_software(&config, &error_early);
#endif
#else
	// prepare dynamic initializer
	char* path_globuf_lib = NULL;
	char* sym_globuf_init = NULL;

	#if defined(GLOBUF_EXAMPLE_X11)
	path_globuf_lib = "./globuf_x11_software.so";
	sym_globuf_init = "globuf_prepare_init_x11_software";
	#elif defined(GLOBUF_EXAMPLE_APPKIT)
	path_globuf_lib = "./globuf_appkit_software.dylib";
	sym_globuf_init = "globuf_prepare_init_appkit_software";
	#elif defined(GLOBUF_EXAMPLE_WIN)
	path_globuf_lib = "./globuf_win_software.dll";
	sym_globuf_init = "globuf_prepare_init_win_software";
	#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	path_globuf_lib = "./globuf_wayland_software.so";
	sym_globuf_init = "globuf_prepare_init_wayland_software";
	#endif

	// load the backend binder symbol straight from a shared object
#if !defined(GLOBUF_EXAMPLE_WIN)
	void* globuf_lib = dlopen(path_globuf_lib, 0);
	void (*globuf_prepare_init)() = dlsym(globuf_lib, sym_globuf_init);
#else
	HMODULE globuf_lib = LoadLibraryExA(path_globuf_lib, NULL, 0);
	void (*globuf_prepare_init)() = (void(*)()) GetProcAddress(globuf_lib, sym_globuf_init);
#endif

	// run the binder to load the remaining function pointers for the target implementation
	globuf_prepare_init(&config, &error_early);
#endif

	// set function pointers and perform basic init
	struct globuf* globuf = globuf_init(&config, &error);

	// Unless the context allocation failed it is always possible to access
	// error messages (even when the context initialization failed) so we can
	// always handle the backend initialization error first.

	// context allocation failed
	if (globuf == NULL)
	{
		fprintf(stderr, "could not allocate the main globuf context\n");
		return 1;
	}

	// Backend initialization failed. Since it happens before globuf
	// initialization and errors are accessible even if it fails, we can handle
	// the errors in the right order regardless.
	if (globuf_error_get_code(&error_early) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_early);
		globuf_clean(globuf, &error);
		return 1;
	}

	// The globuf initialization had failed, make it known now if the backend
	// initialization that happened before went fine.
	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// get available features
	struct globuf_config_features* feature_list =
		globuf_init_features(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// initialize features when creating the window
	struct globuf_feature_state state =
	{
		.state = GLOBUF_STATE_REGULAR,
	};

	struct globuf_feature_title title =
	{
		.title = "globuf",
	};

	struct globuf_feature_icon icon =
	{
		// acceptable implementation-defined behavior
		// since it's also the implementation that
		// allows us to bundle resources like so
		.pixmap = (uint32_t*) iconpix,
		.len = 2 + (16 * 16) + 2 + (32 * 32) + 2 + (64 * 64),
	};

	struct globuf_feature_size size =
	{
		.width = 500,
		.height = 500,
	};

	struct globuf_feature_pos pos =
	{
		.x = 250,
		.y = 250,
	};

	struct globuf_feature_frame frame =
	{
		.frame = true,
	};

	struct globuf_feature_background background =
	{
		.background = GLOBUF_BACKGROUND_BLURRED,
	};

	struct globuf_feature_vsync vsync =
	{
		.vsync = true,
	};

	// configure the feature and print a list
	printf("received a list of available features:\n");

	struct globuf_config_request configs[GLOBUF_FEATURE_COUNT] = {0};
	size_t feature_added = 0;
	size_t i = 0;

	while (i < feature_list->count)
	{
		enum globuf_feature feature_id = feature_list->list[i];
		printf("\t%s\n", feature_names[feature_id]);
		++i;

		switch (feature_id)
		{
			case GLOBUF_FEATURE_STATE:
			{
				configs[feature_added].config = &state;
				break;
			}
			case GLOBUF_FEATURE_TITLE:
			{
				configs[feature_added].config = &title;
				break;
			}
			case GLOBUF_FEATURE_ICON:
			{
				configs[feature_added].config = &icon;
				break;
			}
			case GLOBUF_FEATURE_SIZE:
			{
				configs[feature_added].config = &size;
				break;
			}
			case GLOBUF_FEATURE_POS:
			{
				configs[feature_added].config = &pos;
				break;
			}
			case GLOBUF_FEATURE_FRAME:
			{
				configs[feature_added].config = &frame;
				break;
			}
			case GLOBUF_FEATURE_BACKGROUND:
			{
				configs[feature_added].config = &background;
				break;
			}
			case GLOBUF_FEATURE_VSYNC:
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

	struct event_callback_data event_callback_data =
	{
		.globuf = globuf,
		.dpishit = NULL,
		.willis = NULL,
		.cursoryx = NULL,
		.action = { .action = GLOBUF_INTERACTION_STOP, },
		.mouse_custom = {0},
		.mouse_custom_active = 4,
		.mouse_grabbed = false,
	};

	// register an event handler to track the window's state
	struct globuf_config_events events =
	{
		.data = &event_callback_data,
		.handler = event_callback,
	};

	struct globuf_error_info error_events = {0};
	globuf_init_events(globuf, &events, &error_events);

	if (globuf_error_get_code(&error_events) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_events);
		globuf_clean(globuf, &error);
		return 1;
	}

	// register a render callback
	struct globuf_config_render render =
	{
		.data = globuf,
		.callback = render_callback,
	};

	struct globuf_error_info error_render = {0};
	globuf_init_render(globuf, &render, &error_render);

	if (globuf_error_get_code(&error_render) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_render);
		globuf_clean(globuf, &error);
		return 1;
	}

	// create the window
	globuf_window_create(globuf, configs, feature_added, config_callback, globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// for instance, we can set the mouse cursor
	struct cursoryx_error_info error_cursor = {0};
	struct cursoryx_config_backend config_cursor = {0};

#if defined(GLOBUF_EXAMPLE_X11)
	cursoryx_prepare_init_x11(&config_cursor);

	struct cursoryx_x11_data cursoryx_data =
	{
		.conn = globuf_get_x11_conn(globuf),
		.window = globuf_get_x11_window(globuf),
		.screen = globuf_get_x11_screen(globuf),
	};
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	cursoryx_prepare_init_appkit(&config_cursor);

	struct cursoryx_appkit_data cursoryx_data =
	{
		.data = NULL,
	};
#elif defined(GLOBUF_EXAMPLE_WIN)
	cursoryx_prepare_init_win(&config_cursor);

	struct cursoryx_win_data cursoryx_data =
	{
		.data = NULL,
	};
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	cursoryx_prepare_init_wayland(&config_cursor);

	struct cursoryx_wayland_data cursoryx_data =
	{
		.add_capabilities_handler = globuf_add_wayland_capabilities_handler,
		.add_capabilities_handler_data = globuf,
		.add_registry_handler = globuf_add_wayland_registry_handler,
		.add_registry_handler_data = globuf,
	};
#endif

	struct cursoryx* cursoryx = cursoryx_init(&config_cursor, &error_cursor);

	if (cursoryx == NULL)
	{
		fprintf(stderr, "could not allocate the main cursoryx context\n");
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	cursoryx_start(cursoryx, &cursoryx_data, &error_cursor);

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// prepare custom cursors
	event_callback_data.cursoryx = cursoryx;

	struct cursoryx_custom_config cursor_config[4] =
	{
		{
			.image = (uint32_t*) (cursorpix + 8 + (16*22*4)*0),
			.width = 16,
			.height = 22,
			.x = 7,
			.y = 13,
		},
		{
			.image = (uint32_t*) (cursorpix + 16 + (16*22*4)*1),
			.width = 16,
			.height = 22,
			.x = 7,
			.y = 13,
		},
		{
			.image = (uint32_t*) (cursorpix + 24 + (16*22*4)*2),
			.width = 16,
			.height = 22,
			.x = 7,
			.y = 13,
		},
		{
			.image = (uint32_t*) (cursorpix + 32 + (16*22*4)*3),
			.width = 16,
			.height = 22,
			.x = 7,
			.y = 13,
		},
	};

	// init willis
	struct willis_error_info error_input = {0};
	struct willis_config_backend config_input = {0};

#if defined(GLOBUF_EXAMPLE_X11)
	willis_prepare_init_x11(&config_input);

	struct willis_x11_data willis_data =
	{
		.conn = globuf_get_x11_conn(globuf),
		.window = globuf_get_x11_window(globuf),
		.root = globuf_get_x11_root(globuf),
	};
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	willis_prepare_init_appkit(&config_input);

	struct willis_appkit_data willis_data =
	{
		.data = NULL,
	};
#elif defined(GLOBUF_EXAMPLE_WIN)
	willis_prepare_init_win(&config_input);

	struct willis_win_data willis_data =
	{
		.data = NULL,
	};
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	willis_prepare_init_wayland(&config_input);

	struct willis_wayland_data willis_data =
	{
		.add_capabilities_handler = globuf_add_wayland_capabilities_handler,
		.add_capabilities_handler_data = globuf,
		.add_registry_handler = globuf_add_wayland_registry_handler,
		.add_registry_handler_data = globuf,
		.event_callback = event_callback,
		.event_callback_data = &event_callback_data,
	};
#endif

	struct willis* willis = willis_init(&config_input, &error_input);

	if (willis == NULL)
	{
		fprintf(stderr, "could not allocate the main willis context\n");
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	event_callback_data.willis = willis;

	// start willis
	if (willis_error_get_code(&error_input) != WILLIS_ERROR_OK)
	{
		willis_error_log(willis, &error_input);
		willis_clean(willis, &error_input);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	willis_start(willis, &willis_data, &error_input);

	if (willis_error_get_code(&error_input) != WILLIS_ERROR_OK)
	{
		willis_error_log(willis, &error_input);
		willis_clean(willis, &error_input);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// init dpishit
	struct dpishit_error_info error_display = {0};
	struct dpishit_config_backend config_display = {0};

#if defined(GLOBUF_EXAMPLE_X11)
	dpishit_prepare_init_x11(&config_display);

	struct dpishit_x11_data dpishit_data =
	{
		.conn = globuf_get_x11_conn(globuf),
		.window = globuf_get_x11_window(globuf),
		.root = globuf_get_x11_root(globuf),
	};
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	dpishit_prepare_init_appkit(&config_display);

	struct dpishit_appkit_data dpishit_data =
	{
		.data = NULL,
	};
#elif defined(GLOBUF_EXAMPLE_WIN)
	dpishit_prepare_init_win(&config_display);

	struct dpishit_win_data dpishit_data =
	{
		.data = NULL,
	};
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	dpishit_prepare_init_wayland(&config_display);

	struct dpishit_wayland_data dpishit_data =
	{
		.add_registry_handler = globuf_add_wayland_registry_handler,
		.add_registry_handler_data = globuf,
		.add_registry_remover = globuf_add_wayland_registry_remover,
		.add_registry_remover_data = globuf,
		.event_callback = event_callback,
		.event_callback_data = &event_callback_data,
	};
#endif

	struct dpishit* dpishit = dpishit_init(&config_display, &error_display);

	if (dpishit == NULL)
	{
		fprintf(stderr, "could not allocate the main dpishit context\n");
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	event_callback_data.dpishit = dpishit;

	// start dpishit
	if (dpishit_error_get_code(&error_display) != DPISHIT_ERROR_OK)
	{
		dpishit_error_log(dpishit, &error_display);
		dpishit_clean(dpishit, &error_display);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	dpishit_start(dpishit, &dpishit_data, &error_display);

	if (dpishit_error_get_code(&error_display) != DPISHIT_ERROR_OK)
	{
		dpishit_error_log(dpishit, &error_display);
		dpishit_clean(dpishit, &error_display);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// check window
	globuf_window_confirm(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	for (size_t i = 0; i < 4; ++i)
	{
		event_callback_data.mouse_custom[i] =
			cursoryx_custom_create(cursoryx, &(cursor_config[i]), &error_cursor);

		if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
		{
			cursoryx_error_log(cursoryx, &error_cursor);
			cursoryx_clean(cursoryx, &error_cursor);
			globuf_window_destroy(globuf, &error);
			globuf_clean(globuf, &error);
			return 1;
		}
	}

#if defined(GLOBUF_EXAMPLE_WAYLAND)
	dpishit_set_wayland_surface(
		dpishit,
		globuf_get_wayland_surface(globuf),
		&error_display);
#endif

	// display the window
	globuf_window_start(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"this is a message from the main thread\n"
		"the window should now be visible\n"
		"we can keep computing here\n");

	// wait for the window to be closed
	globuf_window_block(globuf, &error);

	// stop willis
	willis_stop(willis, &error_input);

	if (willis_error_get_code(&error_input) != WILLIS_ERROR_OK)
	{
		willis_error_log(willis, &error_input);
		willis_clean(willis, &error_input);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	willis_clean(willis, &error_input);

	if (willis_error_get_code(&error_input) != WILLIS_ERROR_OK)
	{
		willis_error_log(willis, &error_input);
		willis_clean(willis, &error_input);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// stop dpishit
	dpishit_stop(dpishit, &error_display);

	if (dpishit_error_get_code(&error_display) != DPISHIT_ERROR_OK)
	{
		dpishit_error_log(dpishit, &error_display);
		dpishit_clean(dpishit, &error_display);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	dpishit_clean(dpishit, &error_display);

	if (dpishit_error_get_code(&error_display) != DPISHIT_ERROR_OK)
	{
		dpishit_error_log(dpishit, &error_display);
		dpishit_clean(dpishit, &error_display);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// stop cursoryx
	cursoryx_stop(cursoryx, &error_cursor);

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globuf_error_log(globuf, &error_render);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	cursoryx_clean(cursoryx, &error_cursor);

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globuf_error_log(globuf, &error_render);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// stop globuf
	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// handle event thread errors
	if (globuf_error_get_code(&error_events) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_events);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// handle render thread errors
	if (globuf_error_get_code(&error_render) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_render);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// free resources correctly
	globuf_window_destroy(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	globuf_clean(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return 1;
	}

	return 0;
}
