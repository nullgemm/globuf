#include "globox.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"
#include "desktop.h"
#include <stddef.h>
#include <GLES2/gl2.h>

extern unsigned char iconpix_beg;
extern unsigned char iconpix_end;
extern unsigned char iconpix_len;

#define VERTEX_ATTR_POSITION 0

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

		int32_t width = globox_get_width(globox);
		int32_t height = globox_get_height(globox);
		GLint viewport_rect[4];

		// we can make OpenGL 1 calls without any loader
		glGetIntegerv(GL_VIEWPORT, viewport_rect);

		if ((viewport_rect[2] != width) || (viewport_rect[3] != height))
		{
			glViewport(0, 0, width, height);
		}

		glClearColor(0.2f, 0.4f, 0.9f, (0x22 / 255.0f));
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat vertices[] =
		{
			-100.0f / width, +100.0f / height, 1.0f,
			-100.0f / width, -100.0f / height, 1.0f,
			+100.0f / width, -100.0f / height, 1.0f,
			+100.0f / width, +100.0f / height, 1.0f,
		};

		glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			vertices);

		glDrawArrays(
			GL_TRIANGLE_FAN,
			0,
			4);

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

	// use OpenGL 2 or glES 2
	globox_context_glx_init(&globox, 2, 0);

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
		globox_context_glx_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	dpishit_init(&dpishit, dpishit_data);

	// finish initializing globox
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

	// prepare OpenGL or glES
	const char* vertex_shader_src =
		"attribute vec4 vPosition;"
		"void main()"
		"{"
		"\tgl_Position = vPosition;"
		"}";

	const char* fragment_shader_src =
		"precision mediump float;"
		"void main()"
		"{"
		"\tgl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
		"}";

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_src, 0);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_src, 0);
	glCompileShader(fragment_shader);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	// continue initializing globox
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
		globox_context_glx_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	while (globox_get_closed(&globox) == false)
	{
		globox_platform_events_wait(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_glx_free(&globox);
			globox_platform_free(&globox);
			globox_close(&globox);
			return 1;
		}

		render(&globox);

		if (globox_error_catch(&globox))
		{
			globox_context_glx_free(&globox);
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
	globox_context_glx_free(&globox);
	globox_platform_free(&globox);
	globox_close(&globox);

	return 0;
}
