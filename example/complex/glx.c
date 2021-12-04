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

extern unsigned char square_frag_beg;
extern unsigned char square_frag_end;
extern unsigned char square_frag_len;

extern unsigned char square_vert_beg;
extern unsigned char square_vert_end;
extern unsigned char square_vert_len;

extern unsigned char decorations_frag_beg;
extern unsigned char decorations_frag_end;
extern unsigned char decorations_frag_len;

extern unsigned char decorations_vert_beg;
extern unsigned char decorations_vert_end;
extern unsigned char decorations_vert_len;

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

	if ((globox_get_redraw(globox) == true)
	|| (frame->redraw == true))
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
		.redraw = false,
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
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char * const square_vert = (char*) &square_vert_beg;
	GLint square_vert_size = &square_vert_end - &square_vert_beg;
	glShaderSource(vertex_shader, 1, &square_vert, &square_vert_size);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char * const square_frag = (char*) &square_frag_beg;
	GLint square_frag_size = &square_frag_end - &square_frag_beg;
	glShaderSource(fragment_shader, 1, &square_frag, &square_frag_size);
	glCompileShader(fragment_shader);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(shader_program);

	GLuint vertex_shader_frame = glCreateShader(GL_VERTEX_SHADER);
	const char * const decorations_vert = (char*) &decorations_vert_beg;
	GLint decorations_vert_size = &decorations_vert_end - &decorations_vert_beg;
	glShaderSource(vertex_shader_frame, 1, &decorations_vert, &decorations_vert_size);
	glCompileShader(vertex_shader_frame);

	GLuint fragment_shader_frame = glCreateShader(GL_FRAGMENT_SHADER);
	const char * const decorations_frag = (char*) &decorations_frag_beg;
	GLint decorations_frag_size = &decorations_frag_end - &decorations_frag_beg;
	glShaderSource(fragment_shader_frame, 1, &decorations_frag, &decorations_frag_size);
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
