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

GLuint shader_program;
GLuint shader_program_frame;

GLfloat title_size_loc;
GLfloat button_size_loc;
GLfloat button_icon_size_loc;
GLfloat border_size_loc;
GLfloat width_loc;
GLfloat height_loc;
GLfloat hover_loc;

void render(struct globox* globox, struct frame_info* frame)
{
	globox_platform_events_handle(
		globox);

	if (globox_error_catch(globox))
	{
		return;
	}

	if (true)
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

		// square
		glUseProgram(shader_program);

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

		// frame
		float hover;

		switch (frame->state)
		{
			case FRAME_STATE_HOVER_CLOSE:
			{
				hover = 1;
				break;
			}
			case FRAME_STATE_HOVER_MAXIMIZE:
			{
				hover = 2;
				break;
			}
			case FRAME_STATE_HOVER_MINIMIZE:
			{
				hover = 3;
				break;
			}
			default:
			{
				hover = -2;
				break;
			}
		}

		frame->window_width = width;
		frame->window_height = height;
		unsigned title_size = frame->title_size;
		unsigned border_size = frame->border_size;

		glUseProgram(shader_program_frame);

		GLfloat vertices_frame[] =
		{
			 1.0f,
			 1.0f,
			 1.0f,

			-1.0f + (2.0f * border_size) / width,
			 1.0f - (2.0f * title_size) / height,
			 1.0f,

			-1.0f,
			 1.0f,
			 1.0f,

			-1.0f + (2.0f * border_size) / width,
			-1.0f + (2.0f * border_size) / height,
			 1.0f,

			-1.0f,
			-1.0f,
			 1.0f,

			 1.0f - (2.0f * border_size) / width,
			-1.0f + (2.0f * border_size) / height,
			 1.0f,

			 1.0f,
			-1.0f,
			 1.0f,

			 1.0f - (2.0f * border_size) / width,
			 1.0f - (2.0f * title_size) / height,
			 1.0f,

			 1.0f,
			 1.0f,
			 1.0f,

			-1.0f + (2.0f * border_size) / width,
			 1.0f - (2.0f * title_size) / height,
			 1.0f,
		};

		title_size_loc =
			glGetUniformLocation(shader_program_frame, "title_size");
		button_size_loc =
			glGetUniformLocation(shader_program_frame, "button_size");
		button_icon_size_loc =
			glGetUniformLocation(shader_program_frame, "button_icon_size");
		border_size_loc =
			glGetUniformLocation(shader_program_frame, "border_size");
		width_loc =
			glGetUniformLocation(shader_program_frame, "width");
		height_loc =
			glGetUniformLocation(shader_program_frame, "height");
		hover_loc =
			glGetUniformLocation(shader_program_frame, "hover");

		glUniform1f(title_size_loc, frame->title_size);
		glUniform1f(button_size_loc, frame->button_size);
		glUniform1f(button_icon_size_loc, frame->button_icon_size);
		glUniform1f(border_size_loc, frame->border_size);
		glUniform1f(width_loc, width);
		glUniform1f(height_loc, height);
		glUniform1f(hover_loc, hover);
		glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			vertices_frame);

		glDrawArrays(
			GL_TRIANGLE_STRIP,
			0,
			10);

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
		free(dpishit_data);
		globox_context_glx_free(&globox);
		globox_platform_free(&globox);
		globox_close(&globox);
		return 1;
	}

	globox_platform_hooks(&globox);

	if (globox_error_catch(&globox))
	{
		free(dpishit_data);
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
		"#version 130\n"
		"attribute vec4 vPosition;"
		"void main()"
		"{"
		"\tgl_Position = vPosition;"
		"}";

	const char* fragment_shader_src =
		"#version 130\n"
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

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(shader_program);

	const char* vertex_shader_src_frame =
		"#version 130\n"
		"attribute vec4 vPosition;"
		"void main()"
		"{"
		"\tgl_Position = vPosition;"
		"}";

	const char* fragment_shader_src_frame =
		"#version 130\n"
		"precision mediump float;\n"
		"uniform float title_size;\n"
		"uniform float button_size;\n"
		"uniform float button_icon_size;\n"
		"uniform float border_size;\n"
		"uniform float width;\n"
		"uniform float height;\n"
		"uniform float hover;\n"
		"void main()\n"
		"{\n"
		"\tvec2 pos = gl_FragCoord.xy;\n"
		"\tfloat button_ya = 1.0f - step(height - (title_size - button_icon_size) / 2, pos.y);\n"
		"\tfloat button_yb = step(height - (title_size + button_icon_size) / 2, pos.y);\n"
		"\tfloat button_close_xa = step(width - border_size - (button_size + button_icon_size) / 2, pos.x);\n"
		"\tfloat button_close_xb = 1.0f - step(width - border_size - (button_size - button_icon_size) / 2, pos.x);\n"
		"\tfloat button_maximize_xa = step(width - border_size - (3 * button_size + button_icon_size) / 2, pos.x);\n"
		"\tfloat button_maximize_xb = 1.0f - step(width - border_size - (3 * button_size - button_icon_size) / 2, pos.x);\n"
		"\tfloat button_minimize_xa = step(width - border_size - (5 * button_size + button_icon_size) / 2, pos.x);\n"
		"\tfloat button_minimize_xb = 1.0f - step(width - border_size - (5 * button_size - button_icon_size) / 2, pos.x);\n"
		"\tfloat button_hover_ya = 1.0f - step(height - border_size, pos.y);\n"
		"\tfloat button_hover_yb = step(height - title_size + border_size, pos.y);\n"
		"\tfloat button_hover_xa = step(width - border_size - (hover * button_size), pos.x);\n"
		"\tfloat button_hover_xb = 1.0f - step(width - border_size - ((hover - 1) * button_size), pos.x);\n"
		"\tfloat close = button_ya * button_yb * button_close_xa * button_close_xb;\n"
		"\tfloat maximize = button_ya * button_yb * button_maximize_xa * button_maximize_xb;\n"
		"\tfloat minimize = button_ya * button_yb * button_minimize_xa * button_minimize_xb;\n"
		"\tfloat hover_state = button_hover_ya * button_hover_yb * button_hover_xa * button_hover_xb;\n"
		"\tgl_FragColor = (1.0f - close) * (1.0f - maximize) * (1.0f - minimize) * (1.0f - hover_state) * vec4(0.117f, 0.117f, 0.117f, 1.0f);\n"
		"\tgl_FragColor += (1.0f - close) * (1.0f - maximize) * (1.0f - minimize) * hover_state * vec4(0.164f, 0.164f, 0.164f, 1.0f);\n"
		"\tgl_FragColor += close * vec4(0.478f, 0.125f, 0.125f, 1.0f);\n"
		"\tgl_FragColor += maximize * vec4(0.188f, 0.482f, 0.184f, 1.0f);\n"
		"\tgl_FragColor += minimize * vec4(0.498f, 0.470f, 0.149f, 1.0f);\n"
		"}";

	GLuint vertex_shader_frame = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_frame, 1, &vertex_shader_src_frame, 0);
	glCompileShader(vertex_shader_frame);

	GLuint fragment_shader_frame = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader_frame, 1, &fragment_shader_src_frame, 0);
	glCompileShader(fragment_shader_frame);

	shader_program_frame = glCreateProgram();
	glAttachShader(shader_program_frame, vertex_shader_frame);
	glAttachShader(shader_program_frame, fragment_shader_frame);
	glDeleteShader(vertex_shader_frame);
	glDeleteShader(fragment_shader_frame);
	glLinkProgram(shader_program_frame);

	// continue initializing globox
	globox_platform_commit(&globox);

	render(&globox, &frame);

	// initialize willis
	void* willis_data = example_willis_data(&globox);

	if (willis_data == NULL)
	{
		free(dpishit_data);
		globox_context_glx_free(&globox);
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
		globox_context_glx_free(&globox);
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
			cursoryx_stop(&cursoryx);
			willis_free(&willis);
			free(cursoryx_data);
			free(dpishit_data);
			free(willis_data);
			globox_context_glx_free(&globox);
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
