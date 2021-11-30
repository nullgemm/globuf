#include "globox.h"
#include "globox_private_getters.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"
#include "desktop.h"
#include <stddef.h>
#include <stdio.h>

// client-side decorations
enum frame_section example_willis_frame_section(
	struct frame_info* frame,
	unsigned pos,
	unsigned size)
{
	if (pos < frame->border_size)
	{
		return FRAME_SECTION_BORDER_A;
	}
	else if (pos < frame->corner_reach)
	{
		return FRAME_SECTION_CORNER_A;
	}
	else if (pos + frame->corner_reach < size)
	{
		return FRAME_SECTION_BORDER_SIDE;
	}
	else if (pos + frame->border_size < size)
	{
		return FRAME_SECTION_CORNER_B;
	}
	else if (pos < size)
	{
		return FRAME_SECTION_BORDER_B;
	}

	return FRAME_SECTION_NONE;
}

void example_willis_frame_callback(
	void* data,
	unsigned x,
	unsigned y)
{
	struct event_data* event = data;
	struct frame_info* frame = event->frame_info;

	unsigned title_size = frame->title_size;
	unsigned border_size = frame->border_size;
	unsigned button_size = frame->button_size;
	unsigned window_width = frame->window_width;
	unsigned window_height = frame->window_height;

	// buttons & buttons-level title bar
	if ((y + border_size < title_size)
	&& (y > border_size)
	&& (x + border_size < window_width)
	&& (x > border_size))
	{
		if (x + border_size + (3 * button_size) < window_width)
		{
			frame->state = FRAME_STATE_HOVER_TITLEBAR;
			return;
		}
		else if (x + border_size + (2 * button_size) < window_width)
		{
			frame->state = FRAME_STATE_HOVER_MINIMIZE;
			return;
		}
		else if (x + border_size + (1 * button_size) < window_width)
		{
			frame->state = FRAME_STATE_HOVER_MAXIMIZE;
			return;
		}
		else
		{
			frame->state = FRAME_STATE_HOVER_CLOSE;
			return;
		}
	}

	// bottom-border-level title bar
	if ((y + border_size > title_size)
	&& (y < title_size)
	&& (x + border_size < window_width)
	&& (x > border_size))
	{
		frame->state = FRAME_STATE_HOVER_TITLEBAR;
		return;
	}

	// borders
	enum frame_section section_x =
		example_willis_frame_section(
			frame,
			x,
			window_width);

	enum frame_section section_y =
		example_willis_frame_section(
			frame,
			y,
			window_height);

	switch (section_x)
	{
		case FRAME_SECTION_BORDER_A:
		{
			switch (section_y)
			{
				case FRAME_SECTION_BORDER_A:
				case FRAME_SECTION_CORNER_A:
				{
					frame->state = FRAME_STATE_HOVER_NW;
					break;
				}
				case FRAME_SECTION_BORDER_SIDE:
				{
					frame->state = FRAME_STATE_HOVER_W;
					break;
				}
				case FRAME_SECTION_CORNER_B:
				case FRAME_SECTION_BORDER_B:
				{
					frame->state = FRAME_STATE_HOVER_SW;
					break;
				}
				default:
				{
					frame->state = FRAME_STATE_IDLE;
					break;
				}
			}

			break;
		}
		case FRAME_SECTION_CORNER_A:
		{
			switch (section_y)
			{
				case FRAME_SECTION_BORDER_A:
				{
					frame->state = FRAME_STATE_HOVER_NW;
					break;
				}
				case FRAME_SECTION_BORDER_B:
				{
					frame->state = FRAME_STATE_HOVER_SW;
					break;
				}
				default:
				{
					frame->state = FRAME_STATE_IDLE;
					break;
				}
			}

			break;
		}
		case FRAME_SECTION_BORDER_SIDE:
		{
			switch (section_y)
			{
				case FRAME_SECTION_BORDER_A:
				{
					frame->state = FRAME_STATE_HOVER_N;
					break;
				}
				case FRAME_SECTION_BORDER_B:
				{
					frame->state = FRAME_STATE_HOVER_S;
					break;
				}
				default:
				{
					frame->state = FRAME_STATE_IDLE;
					break;
				}
			}

			break;
		}
		case FRAME_SECTION_CORNER_B:
		{
			switch (section_y)
			{
				case FRAME_SECTION_BORDER_A:
				{
					frame->state = FRAME_STATE_HOVER_NE;
					break;
				}
				case FRAME_SECTION_BORDER_B:
				{
					frame->state = FRAME_STATE_HOVER_SE;
					break;
				}
				default:
				{
					frame->state = FRAME_STATE_IDLE;
					break;
				}
			}

			break;
		}
		case FRAME_SECTION_BORDER_B:
		{
			switch (section_y)
			{
				case FRAME_SECTION_BORDER_A:
				case FRAME_SECTION_CORNER_A:
				{
					frame->state = FRAME_STATE_HOVER_NE;
					break;
				}
				case FRAME_SECTION_BORDER_SIDE:
				{
					frame->state = FRAME_STATE_HOVER_E;
					break;
				}
				case FRAME_SECTION_CORNER_B:
				case FRAME_SECTION_BORDER_B:
				{
					frame->state = FRAME_STATE_HOVER_SE;
					break;
				}
				default:
				{
					frame->state = FRAME_STATE_IDLE;
					break;
				}
			}

			break;
		}
		default:
		{
			frame->state = FRAME_STATE_IDLE;
			break;
		}
	}
}

void example_cursoryx_frame_set_cursor(struct event_data* event)
{
	struct frame_info* frame = event->frame_info;

	switch (frame->state)
	{
		case FRAME_STATE_HOVER_E:
		case FRAME_STATE_HOVER_W:
		{
			cursoryx_set(event->cursoryx, CURSORYX_SIZE_W_E);
			break;
		}
		case FRAME_STATE_HOVER_N:
		case FRAME_STATE_HOVER_S:
		{
			cursoryx_set(event->cursoryx, CURSORYX_SIZE_N_S);
			break;
		}
		case FRAME_STATE_HOVER_NW:
		case FRAME_STATE_HOVER_SE:
		{
			cursoryx_set(event->cursoryx, CURSORYX_SIZE_NW_SE);
			break;
		}
		case FRAME_STATE_HOVER_NE:
		case FRAME_STATE_HOVER_SW:
		{
			cursoryx_set(event->cursoryx, CURSORYX_SIZE_NE_SW);
			break;
		}
		case FRAME_STATE_HOVER_MINIMIZE:
		case FRAME_STATE_HOVER_MAXIMIZE:
		case FRAME_STATE_HOVER_CLOSE:
		{
			cursoryx_set(event->cursoryx, CURSORYX_HAND);
			break;
		}
		case FRAME_STATE_HOVER_TITLEBAR:
		default:
		{
			cursoryx_set(event->cursoryx, CURSORYX_ARROW);
			break;
		}
	}
}

void example_globox_frame_interactive(struct event_data* event)
{
	struct frame_info* frame = event->frame_info;

	switch (frame->state)
	{
		case FRAME_STATE_HOVER_E:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_E);
			break;
		}
		case FRAME_STATE_HOVER_W:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_W);
			break;
		}
		case FRAME_STATE_HOVER_N:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_N);
			break;
		}
		case FRAME_STATE_HOVER_S:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_S);
			break;
		}
		case FRAME_STATE_HOVER_NW:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_NW);
			break;
		}
		case FRAME_STATE_HOVER_SE:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_SE);
			break;
		}
		case FRAME_STATE_HOVER_NE:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_NE);
			break;
		}
		case FRAME_STATE_HOVER_SW:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_SW);
			break;
		}
		case FRAME_STATE_HOVER_TITLEBAR:
		{
			frame->interactive = true;
			globox_platform_interactive_mode(
				event->globox,
				GLOBOX_INTERACTIVE_MOVE);
			break;
		}
		default:
		{
			break;
		}
	}
}

void example_globox_frame_button(struct event_data* event)
{
	struct frame_info* frame = event->frame_info;

	if (frame->state != frame->state_press)
	{
		return;
	}

	switch (frame->state)
	{
		case FRAME_STATE_HOVER_MINIMIZE:
		{
			globox_platform_set_state(
				event->globox,
				GLOBOX_STATE_MINIMIZED);

			break;
		}
		case FRAME_STATE_HOVER_MAXIMIZE:
		{
			enum globox_state state = globox_get_state(event->globox);

			if (state == GLOBOX_STATE_MAXIMIZED)
			{
				globox_platform_set_state(
					event->globox,
					GLOBOX_STATE_REGULAR);
			}
			else
			{
				globox_platform_set_state(
					event->globox,
					GLOBOX_STATE_MAXIMIZED);
			}

			break;
		}
		case FRAME_STATE_HOVER_CLOSE:
		{
			globox_set_closed(
				event->globox,
				true);

			break;
		}
		default:
		{
			break;
		}
	}
}

// cursoryx
void* example_cursoryx_data(struct globox* globox, struct willis* willis)
{
#if defined(CURSORYX_X11)
	struct cursoryx_data_x11* cursoryx_data = malloc(sizeof (struct cursoryx_data_x11));

	if (cursoryx_data == NULL)
	{
		return NULL;
	}

	cursoryx_data->conn = globox_get_x11_conn(globox);
	cursoryx_data->window = globox_get_x11_win(globox);
	cursoryx_data->screen = globox_get_x11_screen_obj(globox);
#elif defined(CURSORYX_WAYLAND)
	struct cursoryx_data_wayland* cursoryx_data =
		malloc(sizeof (struct cursoryx_data_wayland));

	if (cursoryx_data == NULL)
	{
		return NULL;
	}

	cursoryx_data->compositor = globox_get_wayland_compositor(globox);
	cursoryx_data->pointer = willis_get_wl_pointer(willis);
	cursoryx_data->shm = globox_get_wayland_shm(globox);
#elif defined(CURSORYX_WINDOWS)
	struct cursoryx_data_windows* cursoryx_data =
		malloc(sizeof (struct cursoryx_data_windows));

	if (cursoryx_data == NULL)
	{
		return NULL;
	}

	cursoryx_data->handle = globox_platform_get_event_handle(globox);
#elif defined(CURSORYX_MACOS)
	void* cursoryx_data = NULL;
#endif

	return cursoryx_data;
}

// dpishit
void* example_dpishit_data(struct globox* globox)
{
#if defined(DPISHIT_X11)
	struct dpishit_data_x11* dpishit_data =
		malloc(sizeof (struct dpishit_data_x11));

	if (dpishit_data == NULL)
	{
		return NULL;
	}

	dpishit_data->conn = globox_get_x11_conn(globox);
	dpishit_data->win = globox_get_x11_win(globox);
#elif defined(DPISHIT_WAYLAND)
	struct dpishit_data_wayland* dpishit_data =
		malloc(sizeof (struct dpishit_data_wayland));

	if (dpishit_data == NULL)
	{
		return NULL;
	}

	dpishit_data->listener = globox_get_wayland_output_listener(globox);
	dpishit_data->output_data = globox_get_wayland_output_data(globox);
#elif defined(DPISHIT_WINDOWS)
	struct dpishit_data_windows* dpishit_data =
		malloc(sizeof (struct dpishit_data_windows));

	if (dpishit_data == NULL)
	{
		return NULL;
	}

	dpishit_data->hdc = GetDC(globox_platform_get_event_handle(globox));
	dpishit_data->hwnd = globox_platform_get_event_handle(globox);
#elif defined(DPISHIT_MACOS)
	struct dpishit_data_macos* dpishit_data =
		malloc(sizeof (struct dpishit_data_macos));

	if (dpishit_data == NULL)
	{
		return NULL;
	}

	dpishit_data->window_obj = globox_get_macos_obj_window(globox);
#endif

	return dpishit_data;
}

// willis
#if defined(WILLIS_WAYLAND)
void callback_serial(
	void* data,
	uint32_t serial)
{
	struct globox* globox = data;
	globox_wayland_save_serial(globox, serial);
}
#endif

void* example_willis_data(struct globox* globox)
{
#if defined(WILLIS_X11)
	struct willis_data_x11* willis_data =
		malloc(sizeof (struct willis_data_x11));

	if (willis_data == NULL)
	{
		return NULL;
	}

	willis_data->x11_conn =
		globox_get_x11_conn(globox);
	willis_data->x11_root =
		globox_get_x11_root_win(globox);
	willis_data->x11_window =
		globox_get_x11_win(globox);
#elif defined(WILLIS_WAYLAND)
	struct willis_data_wayland* willis_data =
		malloc(sizeof (struct willis_data_wayland));

	if (willis_data == NULL)
	{
		return NULL;
	}

	willis_data->wl_surface =
		globox_get_wayland_surface(globox);
	willis_data->wl_relative_pointer =
		globox_get_wayland_pointer_manager(globox);
	willis_data->wl_pointer_constraints =
		globox_get_wayland_pointer_constraints(globox);
	willis_data->callback_serial =
		callback_serial;
	willis_data->callback_serial_data =
		globox;
#elif defined(WILLIS_WINDOWS)
	void* willis_data = NULL;
#elif defined(WILLIS_MACOS)
	void* willis_data = NULL;
#endif

	return willis_data;
}

void example_willis_event_callback(
	struct willis* willis,
	enum willis_event_code event_code,
	enum willis_event_state event_state,
	void* data)
{
	struct event_data* event = data;
	struct frame_info* frame = event->frame_info;

#if defined(WILLIS_WAYLAND)
	// wayland dumbness workaround
	if ((event_code == WILLIS_MOUSE_MOTION)
	&& (globox_get_interactive_mode(event->globox) != GLOBOX_INTERACTIVE_STOP))
	{
		event_code = WILLIS_MOUSE_CLICK_LEFT;
		event_state = WILLIS_STATE_RELEASE;
	}
#endif

	// print the event's details
	printf(
		"%-27s: %s\n",
		willis_event_code_names[event_code],
		willis_event_state_names[event_state]);

	// if Q is pressed, grab the mouse
	if ((event_code == WILLIS_KEY_Q)
	&& (event_state == WILLIS_STATE_PRESS))
	{
		willis_mouse_grab(willis);
	}

	// if W is pressed, release the mouse
	if ((event_code == WILLIS_KEY_W)
	&& (event_state == WILLIS_STATE_PRESS))
	{
		willis_mouse_ungrab(willis);
		cursoryx_set(event->cursoryx, CURSORYX_HAND);
	}

	if ((event_code == WILLIS_KEY_E)
	&& (event_state == WILLIS_STATE_PRESS))
	{
		cursoryx_set(event->cursoryx, CURSORYX_HAND);
	}

	if ((event_code == WILLIS_KEY_R)
	&& (event_state == WILLIS_STATE_PRESS))
	{
		struct dpishit_display_info* display_info =
			dpishit_get_display_info(event->dpishit);
		bool dpishit_real =
			dpishit_refresh_real_density(event->dpishit);
		bool dpishit_logic =
			dpishit_refresh_logic_density(event->dpishit);
		bool dpishit_scale =
			dpishit_refresh_scale(event->dpishit);

		printf(
			"DPIshit info:\n"
			"width: %5u px | height: %5u px\n"
			"width: %5u mm | height: %5u mm | valid: %d\n"
			"logic: %.17g dpi | valid: %d\n"
			"scale: %.17g     | valid: %d\n",
			display_info->px_width,
			display_info->px_height,
			display_info->mm_width,
			display_info->mm_height,
			dpishit_real,
			display_info->dpi_logic,
			dpishit_logic,
			display_info->scale,
			dpishit_scale);

		double scale;

		if ((dpishit_scale == true) && (display_info->scale != 0.0))
		{
			scale = display_info->scale;
		}
		else
		{
			scale = 1.0;
		}

		if ((dpishit_logic == true) && (display_info->dpi_logic != 0.0))
		{
			frame->title_size =
				scale * (9 / 25.4)
				* display_info->dpi_logic;

			frame->button_size =
				scale * (10 / 25.4)
				* display_info->dpi_logic;

			frame->button_icon_size =
				scale * (4 / 25.4)
				* display_info->dpi_logic;

			frame->border_size =
				scale * (1 / 25.4)
				* display_info->dpi_logic;

			frame->corner_reach =
				2 * frame->border_size;
		}
		else if (dpishit_real == true)
		{
			frame->title_size =
				scale * 9
				* display_info->px_width
				/ display_info->mm_width;

			frame->button_size =
				scale * 10
				* display_info->px_width
				/ display_info->mm_width;

			frame->button_icon_size =
				scale * 4
				* display_info->px_width
				/ display_info->mm_width;

			frame->border_size =
				scale * 1
				* display_info->px_width
				/ display_info->mm_width;

			frame->corner_reach =
				2 * frame->border_size;
		}
	}

	// if this event synthesized some text, print it
	char* utf8_string = willis_get_utf8_string(willis);

	if (utf8_string != NULL)
	{
		printf("textual output: %s\n", utf8_string);
	}

	// if the mouse moved, print some info about the event
	if (event_code == WILLIS_MOUSE_MOTION)
	{
		if (willis_get_mouse_grab(willis) == true)
		{
			printf(
				"mouse relative position: %08x %08x\n",
				(uint32_t) (willis_get_diff_x(willis) >> 32),
				(uint32_t) (willis_get_diff_y(willis) >> 32));
		}
		else
		{
			int x = willis_get_mouse_x(willis);
			int y = willis_get_mouse_y(willis);

			printf(
				"mouse absolute position: %5i %5i\n",
				x,
				y);

			example_willis_frame_callback(event, x, y);
			example_cursoryx_frame_set_cursor(event);
		}
	}

	if (event_code == WILLIS_MOUSE_CLICK_LEFT)
	{
		if ((event_state == WILLIS_STATE_PRESS)
		&& (frame->interactive == false))
		{
			frame->state_press = frame->state;
			example_globox_frame_interactive(event);
		}

		if (event_state == WILLIS_STATE_RELEASE)
		{
			if (frame->interactive == true)
			{
				frame->interactive = false;
				globox_platform_interactive_mode(
					event->globox,
					GLOBOX_INTERACTIVE_STOP);
			}
			else
			{
				example_globox_frame_button(event);
			}
		}
	}
}
