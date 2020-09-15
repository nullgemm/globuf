#include "globox.h"

#if 0
// we try to avoid ifdefs here by relying on helper files
// helper headers for extensions
#include "example_helper_willis.h"
#include "example_helper_dpishit.h"
#include "example_helper_cursoryx.h"
// helper headers for rendering backends
#include "example_helper_software.h"
#include "example_helper_vulkan.h"
#include "example_helper_egl.h"
#endif

extern unsigned char iconpix_beg;
extern unsigned char iconpix_end;
extern unsigned char iconpix_len;

void render(struct globox* globox)
{
	globox_platform_events_handle(
		globox,
		globox_context_software_expose,
		globox_context_software_reserve);

	if (globox_error_catch(globox))
	{
		return;
	}

	uint32_t* argb = globox_platform_get_argb(globox);
	uint32_t height = globox_get_height(globox);
	uint32_t width = globox_get_width(globox);

	if (globox_get_redraw(globox) == true)
	{
		for (uint32_t i = 0; i < height * width; ++i)
		{
			argb[i] = 0x00888888;
		}

		uint32_t pos;

		for (uint32_t i = 0; i < (100*100); ++i)
		{
			pos = ((height / 2) - 50 + (i / 100)) * width + (width / 2) - 50 + (i % 100);

			argb[pos] = 0x00FFFFFF;
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
	struct globox globox;

	globox_open(
		&globox,
		0,
		0,
		500,
		500,
		"globox",
		GLOBOX_STATE_REGULAR,
		NULL,
		NULL);

	if (globox_error_catch(&globox))
	{
		return 1;
	}

	globox_platform_init(&globox);

	if (globox_error_catch(&globox))
	{
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

	globox_context_software_init(&globox);

	if (globox_error_catch(&globox))
	{
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

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

	while (globox_get_closed(&globox) == false)
	{
		render(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_software_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}

		globox_platform_prepoll(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_software_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}

		globox_platform_events_wait(&globox); // TODO compatible with windows bullshit?

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
