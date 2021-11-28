#include "globox.h"
#include "globox_private_getters.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"
#include "desktop.h"
#include <stddef.h>
#include <stdio.h>

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
	struct event_data* event_data = data;

#if defined(WILLIS_WAYLAND)
	// wayland dumbness workaround
	if ((event_code == WILLIS_MOUSE_MOTION)
	&& (globox_get_interactive_mode(event_data->globox) != GLOBOX_INTERACTIVE_STOP))
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
		cursoryx_set(event_data->cursoryx, CURSORYX_HAND);
	}

	if ((event_code == WILLIS_KEY_E)
	&& (event_state == WILLIS_STATE_PRESS))
	{
		cursoryx_set(event_data->cursoryx, CURSORYX_HAND);
	}

	if ((event_code == WILLIS_KEY_R)
	&& (event_state == WILLIS_STATE_PRESS))
	{
		struct dpishit_display_info* display_info =
			dpishit_get_display_info(event_data->dpishit);
		bool dpishit_real =
			dpishit_refresh_real_density(event_data->dpishit);
		bool dpishit_logic =
			dpishit_refresh_scale(event_data->dpishit);
		bool dpishit_scale =
			dpishit_refresh_logic_density(event_data->dpishit);

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
			printf(
				"mouse absolute position: %5i %5i\n",
				willis_get_mouse_x(willis),
				willis_get_mouse_y(willis));
		}
	}
}
