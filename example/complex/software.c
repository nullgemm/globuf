#include "globox.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"
#include "desktop.h"
#include <stddef.h>

extern unsigned char iconpix_beg;
extern unsigned char iconpix_end;
extern unsigned char iconpix_len;

void render(struct globox* globox, struct frame_info* frame)
{
	globox_platform_events_handle(
		globox);

	if (globox_error_catch(globox))
	{
		return;
	}

	if (globox_get_redraw(globox) == true)
	{
		globox_platform_prepare_buffer(globox);

		uint32_t width = globox_get_width(globox);
		uint32_t height = globox_get_height(globox);
		uint32_t* argb = globox_platform_get_argb(globox);

		// render the example square
		for (uint32_t i = 0; i < height * width; ++i)
		{
			argb[i] = 0x22000000;
		}

		uint32_t pos;

		unsigned int square_width = 100;
		unsigned int square_height = 100;

		if (width < square_width)
		{
			square_width = width;
		}

		if (height < square_height)
		{
			square_height = height;
		}

		for (uint32_t i = 0; i < (square_width * square_height); ++i)
		{
			pos =
				((height - square_height) / 2
				+ (i / square_width)) * width
				+ (width - square_width) / 2
				+ (i % square_width);

			argb[pos] = 0xFFFFFFFF;
		}

		// render the window frame
		frame->window_width = width;
		frame->window_height = height;
		unsigned title_size = frame->title_size;
		unsigned border_size = frame->border_size;
		unsigned button_size = frame->button_size;
		unsigned button_icon_size = frame->button_icon_size;

		unsigned border_height;

		if ((title_size + border_size) > height)
		{
			border_height = 0;
		}
		else
		{
			border_height = height - title_size - border_size;
		}

		unsigned border_x_right;

		if (border_size > width)
		{
			border_x_right = 0;
		}
		else
		{
			border_x_right = width - border_size;
		}

		unsigned border_y_bottom;

		if (border_size > height)
		{
			border_y_bottom = 0;
		}
		else
		{
			border_y_bottom = height - border_size;
		}

		// frame title
		for (unsigned i = 0; i < title_size; ++i)
		{
			for (unsigned k = 0; k < width; ++k)
			{
				argb[(i * width) + k] = 0xFF1E1E1E;
			}
		}

		// frame left & right borders
		for (unsigned i = 0; i < border_size; ++i)
		{
			for (unsigned k = 0; k < border_height; ++k)
			{
				argb[((k + title_size) * width) + i] =
					0xFF1E1E1E;

				argb[((k + title_size) * width) + i + border_x_right] =
					0xFF1E1E1E;
			}
		}

		// frame bottom border
		for (unsigned i = border_y_bottom; i < height; ++i)
		{
			for (unsigned k = 0; k < width; ++k)
			{
				argb[(i * width) + k] = 0xFF1E1E1E;
			}
		}

		// frame hover rectangle
		unsigned rect_x;

		switch (frame->state)
		{
			case FRAME_STATE_HOVER_MINIMIZE:
			{
				for (unsigned i = (2 * border_size); i < title_size; ++i)
				{
					for (unsigned k = 0; k < button_size; ++k)
					{
						rect_x = k + width - border_size - (3 * button_size);
						argb[((i - border_size) * width) + rect_x] = 0xFF292929;
					}
				}

				break;
			}
			case FRAME_STATE_HOVER_MAXIMIZE:
			{
				for (unsigned i = (2 * border_size); i < title_size; ++i)
				{
					for (unsigned k = 0; k < button_size; ++k)
					{
						rect_x = k + width - border_size - (2 * button_size);
						argb[((i - border_size) * width) + rect_x] = 0xFF292929;
					}
				}

				break;
			}
			case FRAME_STATE_HOVER_CLOSE:
			{
				for (unsigned i = (2 * border_size); i < title_size; ++i)
				{
					for (unsigned k = 0; k < button_size; ++k)
					{
						rect_x = k + width - border_size - (1 * button_size);
						argb[((i - border_size) * width) + rect_x] = 0xFF292929;
					}
				}

				break;
			}
			default:
			{
				break;
			}
		}

		// frame buttons
		unsigned rect_y;

		for (unsigned i = 0; i < button_icon_size; ++i)
		{
			for (unsigned k = 0; k < button_icon_size; ++k)
			{
				rect_x =
					k
					+ width
					- border_size
					- 1 * button_size / 2
					- button_icon_size / 2;

				rect_y =
					i + (title_size - button_icon_size) / 2;

				argb[(rect_y * width) + rect_x] =
					0xFF7A2020;
			}
		}
		for (unsigned i = 0; i < button_icon_size; ++i)
		{
			for (unsigned k = 0; k < button_icon_size; ++k)
			{
				rect_x =
					k
					+ width
					- border_size
					- 3 * button_size / 2
					- button_icon_size / 2;

				rect_y =
					i + (title_size - button_icon_size) / 2;

				argb[(rect_y * width) + rect_x] =
					0xFF307B2F;
			}
		}
		for (unsigned i = 0; i < button_icon_size; ++i)
		{
			for (unsigned k = 0; k < button_icon_size; ++k)
			{
				rect_x =
					k
					+ width
					- border_size
					- 5 * button_size / 2
					- button_icon_size / 2;

				rect_y =
					i + (title_size - button_icon_size) / 2;

				argb[(rect_y * width) + rect_x] =
					0xFF7F7826;
			}
		}

		// finish rendering
		globox_context_software_copy(
			globox,
			0,
			0,
			width,
			height);
	}
}

int main(void)
{
	struct globox globox = {0};
	struct cursoryx cursoryx = {0};
	struct dpishit dpishit = {0};
	struct willis willis = {0};
	struct frame_info frame =
	{
		.title_size = 28,
		.button_size = 32,
		.button_icon_size = 12,
		.border_size = 3,
		.corner_reach = 6,
		.window_width = 0,
		.window_height = 0,
		.state = FRAME_STATE_IDLE,
		.state_press = FRAME_STATE_IDLE,
		.interactive = false,
	};

	globox_open(
		&globox,
		0,
		0,
		500,
		500,
		"globox",
		GLOBOX_STATE_REGULAR,
		willis_handle_events,
		&willis);

	if (globox_error_catch(&globox))
	{
		return 1;
	}

	globox_platform_init(&globox, true, true);

	if (globox_error_catch(&globox))
	{
		globox_close(&globox);
		return 1;
	}

	globox_context_software_init(&globox, 0, 0);

	if (globox_error_catch(&globox))
	{
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	globox_platform_create_window(&globox);

	if (globox_error_catch(&globox))
	{
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	// initialized dpishit
	void* dpishit_data = example_dpishit_data(&globox);

	if (dpishit_data == NULL)
	{
		globox_context_software_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	dpishit_init(&dpishit, dpishit_data);

	// finish initializing globox
	globox_context_software_create(&globox);

	if (globox_error_catch(&globox))
	{
		free(dpishit_data);
		globox_context_software_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	globox_platform_hooks(&globox);

	if (globox_error_catch(&globox))
	{
		free(dpishit_data);
		globox_context_software_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	globox_platform_set_icon(
		&globox,
		(uint32_t*) &iconpix_beg,
		2 + (16 * 16) + 2 + (32 * 32) + 2 + (64 * 64));

	globox_platform_commit(&globox);

	render(&globox, &frame);

	// initialize willis
	void* willis_data = example_willis_data(&globox);

	if (willis_data == NULL)
	{
		free(dpishit_data);
		globox_context_software_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	struct event_data event_data =
	{
		.globox = &globox,
		.cursoryx = &cursoryx,
		.dpishit = &dpishit,
		.frame_info = &frame,
	};

	willis_init(
		&willis,
		willis_data,
		true,
		example_willis_event_callback,
		&event_data);

	// cursoryx
	void* cursoryx_data = example_cursoryx_data(&globox, &willis);

	if (cursoryx_data == NULL)
	{
		willis_free(&willis);
		free(dpishit_data);
		free(willis_data);
		globox_context_software_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	cursoryx_start(
		&cursoryx,
		cursoryx_data);

	// main loop
	globox_platform_prepoll(&globox);

	if (globox_error_catch(&globox))
	{
		cursoryx_stop(&cursoryx);
		willis_free(&willis);
		free(cursoryx_data);
		free(dpishit_data);
		free(willis_data);
		globox_context_software_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	while (globox_get_closed(&globox) == false)
	{
		globox_platform_events_wait(&globox);

		if (globox_error_catch(&globox))
		{
			cursoryx_stop(&cursoryx);
			willis_free(&willis);
			free(cursoryx_data);
			free(dpishit_data);
			free(willis_data);
			globox_context_software_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}

		render(&globox, &frame);

		if (globox_error_catch(&globox))
		{
			cursoryx_stop(&cursoryx);
			willis_free(&willis);
			free(cursoryx_data);
			free(dpishit_data);
			free(willis_data);
			globox_context_software_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}
	}

	cursoryx_stop(&cursoryx);
	willis_free(&willis);
	free(cursoryx_data);
	free(dpishit_data);
	free(willis_data);
	globox_context_software_free(&globox);
	globox_platform_free(&globox);
	globox_close(&globox);

	return 0;
}
