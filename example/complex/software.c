#include "globox.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"
#include "desktop.h"
#include <stddef.h>

extern unsigned char iconpix_beg;
extern unsigned char iconpix_end;
extern unsigned char iconpix_len;

void render(struct globox* globox)
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
		globox_context_software_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	globox_platform_hooks(&globox);

	if (globox_error_catch(&globox))
	{
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

	render(&globox);

	// initialize willis
	void* willis_data = example_willis_data(&globox);

	struct event_data event_data =
	{
		.globox = &globox,
		.cursoryx = &cursoryx,
		.dpishit = &dpishit,
	};

	willis_init(
		&willis,
		willis_data,
		true,
		example_willis_event_callback,
		&event_data);

	// cursoryx
	void* cursoryx_data = example_cursoryx_data(&globox, &willis);

	cursoryx_start(
		&cursoryx,
		cursoryx_data);

	// main loop
	globox_platform_prepoll(&globox);

	if (globox_error_catch(&globox))
	{
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
			globox_context_software_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}

		render(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_software_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}
	}

	globox_context_software_free(&globox);
	globox_platform_free(&globox);
	globox_close(&globox);

	return 0;
}
