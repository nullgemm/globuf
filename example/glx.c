#include "globox.h"
#include <GL/gl.h>

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
		globox);

	if (globox_error_catch(globox))
	{
		return;
	}

	if (globox_get_redraw(globox) == true)
	{
		int32_t width = globox_get_width(globox);
		int32_t height = globox_get_height(globox);
		GLint viewport_rect[4];

		// we can make OpenGL 1 calls without any loader
		glGetIntegerv(GL_VIEWPORT, viewport_rect);

		if ((viewport_rect[2] != width) || (viewport_rect[3] != height))
		{
			glViewport(0, 0, width, height);
		}

		glClearColor(0.2f, 0.4f, 0.9f, (0xEE / 255.0f));
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLE_FAN);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-100.0f / width, +100.0f / height, 0.0f);
		glVertex3f(-100.0f / width, -100.0f / height, 0.0f);
		glVertex3f(+100.0f / width, -100.0f / height, 0.0f);
		glVertex3f(+100.0f / width, +100.0f / height, 0.0f);
		glEnd();

		globox_context_glx_copy(
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

	globox_platform_init(&globox, true, true, true);

	if (globox_error_catch(&globox))
	{
		globox_close(&globox);
		return 1;
	}

	// use OpenGL 1
	globox_context_glx_init(&globox, 1, 0);

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

	globox_context_glx_create(&globox);

	if (globox_error_catch(&globox))
	{
		globox_context_glx_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	globox_platform_hooks(&globox);

	if (globox_error_catch(&globox))
	{
		globox_context_glx_free(&globox);
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

#if 0
	globox_platform_interactive_mode(&globox, GLOBOX_INTERACTIVE_N);
#endif

	while (globox_get_closed(&globox) == false)
	{
		globox_platform_prepoll(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_glx_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}

#if 1
		globox_platform_events_wait(&globox); // TODO compatible with windows bullshit?

		if (globox_error_catch(&globox))
		{
			globox_context_glx_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}
#endif

		render(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_glx_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}
	}

#if 0
	globox_platform_interactive_mode(&globox, GLOBOX_INTERACTIVE_STOP);
#endif

	globox_context_glx_free(&globox);
	globox_platform_free(&globox);
	globox_close(&globox);

	return 0;
}
