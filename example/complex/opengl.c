#include "globox.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"

#ifdef GLOBOX_EXAMPLE_X11
#if defined(GLOBOX_EXAMPLE_GLX)
	#include "globox_x11_glx.h"
#elif defined(GLOBOX_EXAMPLE_EGL)
	#include "globox_x11_egl.h"
#endif
	#include "cursoryx_x11.h"
	#include "dpishit_x11.h"
	#include "willis_x11.h"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLES2/gl2.h>

#if defined(GLOBOX_EXAMPLE_GLX)
	#include <GL/glx.h>
#elif defined(GLOBOX_EXAMPLE_EGL)
	#include <EGL/egl.h>
#endif

extern uint8_t iconpix[];
extern int iconpix_size;

extern uint8_t cursorpix[];
extern int cursorpix_size;

extern uint8_t square_frag_gl1[];
extern int square_frag_gl1_size;

extern uint8_t square_vert_gl1[];
extern int square_vert_gl1_size;

#define VERTEX_ATTR_POSITION 0

char* feature_names[GLOBOX_FEATURE_COUNT] =
{
	[GLOBOX_FEATURE_INTERACTION] = "interaction",
	[GLOBOX_FEATURE_STATE] = "state",
	[GLOBOX_FEATURE_TITLE] = "title",
	[GLOBOX_FEATURE_ICON] = "icon",
	[GLOBOX_FEATURE_SIZE] = "size",
	[GLOBOX_FEATURE_POS] = "pos",
	[GLOBOX_FEATURE_FRAME] = "frame",
	[GLOBOX_FEATURE_BACKGROUND] = "background",
	[GLOBOX_FEATURE_VSYNC] = "vsync",
};

#if defined(GLOBOX_EXAMPLE_GLX)
int glx_config_attrib[] =
{
	GLX_DOUBLEBUFFER, True,
	GLX_RENDER_TYPE, GLX_RGBA_BIT,
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,

	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DEPTH_SIZE, 24,
	None,
};
#elif defined(GLOBOX_EXAMPLE_EGL)
EGLint egl_config_attrib[] =
{
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
	EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
	EGL_NONE,
};
#endif

struct event_callback_data
{
	struct globox* globox;

	struct cursoryx* cursoryx;
	struct dpishit* dpishit;
	struct willis* willis;

	struct globox_feature_interaction action;
	struct cursoryx_custom* mouse_custom[4];
	size_t mouse_custom_active;
	bool mouse_grabbed;
	bool shaders;
};

struct globox_render_data
{
	struct globox* globox;
	bool shaders;
};

static void compile_shaders()
{
	GLint out;

	// compile OpenGL or glES shaders
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char * const square_vert_gl = (char*) &square_vert_gl1;
	GLint square_vert_size_gl = square_vert_gl1_size;
	glShaderSource(vertex_shader, 1, &square_vert_gl, &square_vert_size_gl);
	fprintf(stderr, "compiling vertex shader:\n%.*s\n", square_vert_size_gl, square_vert_gl);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char * const square_frag_gl = (char*) &square_frag_gl1;
	GLint square_frag_size_gl = square_frag_gl1_size;
	glShaderSource(fragment_shader, 1, &square_frag_gl, &square_frag_size_gl);
	fprintf(stderr, "compiling fragment shader:\n%.*s\n", square_frag_size_gl, square_frag_gl);
	glCompileShader(fragment_shader);

	// check for compilation errors
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &out);

	if (out != GL_TRUE)
	{
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &out);
		GLint len;
		GLchar* log = malloc(out);
		glGetShaderInfoLog(vertex_shader, out, &len, log);
		fprintf(stderr, "vertex shader compilation error:\n%.*s\n", len, log);
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &out);

	if (out != GL_TRUE)
	{
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &out);
		GLint len;
		GLchar* log = malloc(out);
		glGetShaderInfoLog(fragment_shader, out, &len, log);
		fprintf(stderr, "fragment shader compilation error:\n%.*s\n", len, log);
	}

	// link shader program
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(shader_program);

	// check for compilation errors
	glGetProgramiv(shader_program, GL_LINK_STATUS, &out);

	if (out != GL_TRUE)
	{
		GLint len;
		GLchar* log = malloc(out);
		glGetProgramInfoLog(shader_program, out, &len, log);
		fprintf(stderr, "shader program link error:\n%.*s\n", len, log);
	}

	glUseProgram(shader_program);
}

static void event_callback(void* data, void* event)
{
	struct event_callback_data* event_callback_data = data;

	struct globox* globox = event_callback_data->globox;
	struct globox_error_info error = {0};

	// print some debug info on internal events
	enum globox_event abstract =
		globox_handle_events(globox, event, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	switch (abstract)
	{
		case GLOBOX_EVENT_INVALID:
		{
			// shouldn't be possible since we handle the error
			fprintf(stderr, "received invalid event\n");
			break;
		}
		case GLOBOX_EVENT_UNKNOWN:
		{
			fprintf(stderr, "received unknown event\n");
			break;
		}
		case GLOBOX_EVENT_RESTORED:
		{
			fprintf(stderr, "received `restored` event\n");
			break;
		}
		case GLOBOX_EVENT_MINIMIZED:
		{
			fprintf(stderr, "received `minimized` event\n");
			break;
		}
		case GLOBOX_EVENT_MAXIMIZED:
		{
			fprintf(stderr, "received `maximized` event\n");
			break;
		}
		case GLOBOX_EVENT_FULLSCREEN:
		{
			fprintf(stderr, "received `fullscreen` event\n");
			break;
		}
		case GLOBOX_EVENT_CLOSED:
		{
			fprintf(stderr, "received `closed` event\n");
			break;
		}
		case GLOBOX_EVENT_MOVED_RESIZED:
		{
			fprintf(stderr, "received `moved` event\n");
			break;
		}
		case GLOBOX_EVENT_DAMAGED:
		{
			struct globox_rect rect = globox_get_expose(globox, &error);

			if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
			{
				globox_error_log(globox, &error);
				break;
			}

			fprintf(
				stderr,
				"received `content damaged` event\n"
				" - x: %d px\n"
				" - y: %d px\n"
				" - width: %d px\n"
				" - height: %d px\n",
				rect.x,
				rect.y,
				rect.width,
				rect.height);

			break;
		}
	}

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
				" - width: %u px\n"
				" - height: %u px\n"
				" - width: %u mm\n"
				" - height: %u mm\n",
				display_info.px_width,
				display_info.px_height,
				display_info.mm_width,
				display_info.mm_height);

			if (display_info.dpi_logic_valid == true)
			{
				fprintf(
					stderr,
					" - logic dpi: %lf dpi\n",
					display_info.dpi_logic);
			}

			if (display_info.dpi_scale_valid == true)
			{
				fprintf(
					stderr,
					" - scale: %lf\n",
					display_info.dpi_scale);
			}
		}
	}

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
		struct globox_feature_state state;

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
			case WILLIS_KEY_W:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_N;
				break;
			}
			case WILLIS_KEY_Q:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_NW;
				break;
			}
			case WILLIS_KEY_A:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_W;
				break;
			}
			case WILLIS_KEY_Z:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_SW;
				break;
			}
			case WILLIS_KEY_X:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_S;
				break;
			}
			case WILLIS_KEY_C:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_SE;
				break;
			}
			case WILLIS_KEY_D:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_E;
				break;
			}
			case WILLIS_KEY_E:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_NE;
				break;
			}
			case WILLIS_KEY_S:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_MOVE;
				break;
			}
			case WILLIS_MOUSE_CLICK_LEFT:
			{
				event_callback_data->action.action = GLOBOX_INTERACTION_STOP;
				break;
			}
			case WILLIS_KEY_1:
			{
				state.state = GLOBOX_STATE_REGULAR;
				globox_feature_set_state(globox, &state, &error);
				break;
			}
			case WILLIS_KEY_2:
			{
				state.state = GLOBOX_STATE_MINIMIZED;
				globox_feature_set_state(globox, &state, &error);
				break;
			}
			case WILLIS_KEY_3:
			{
				state.state = GLOBOX_STATE_MAXIMIZED;
				globox_feature_set_state(globox, &state, &error);
				break;
			}
			case WILLIS_KEY_4:
			{
				state.state = GLOBOX_STATE_FULLSCREEN;
				globox_feature_set_state(globox, &state, &error);
				break;
			}
			default:
			{
				break;
			}
		}
	}
	else
	{
		switch (event_info.event_code)
		{
			case WILLIS_MOUSE_CLICK_LEFT:
			{
				if (event_callback_data->action.action == GLOBOX_INTERACTION_STOP)
				{
					break;
				}

				globox_feature_set_interaction(globox, &(event_callback_data->action), &error);

				if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
				{
					globox_error_log(globox, &error);
					return;
				}
			}
			default:
			{
				break;
			}
		}
	}


		// print debug info
		if (event_info.event_code != WILLIS_NONE)
		{
			fprintf(
				stderr,
				"willis returned event info:\n"
				" - code: %s\n"
				" - state: %s\n",
				willis_get_event_code_name(willis, event_info.event_code, &error_willis),
				willis_get_event_state_name(willis, event_info.event_state, &error_willis));

			if (event_info.event_code == WILLIS_MOUSE_MOTION)
			{
				if (event_callback_data->mouse_grabbed == false)
				{
					fprintf(
						stderr,
						" - mouse x: %d\n"
						" - mouse y: %d\n",
						event_info.mouse_x,
						event_info.mouse_y);
				}
				else
				{
					fprintf(
						stderr,
						" - diff x: %d\n"
						" - diff y: %d\n",
						(int) (event_info.diff_x >> 32),
						(int) (event_info.diff_y >> 32));
				}
			}
			else if (event_info.utf8_size > 0)
			{
				fprintf(
					stderr,
					" - text: %.*s\n",
					(int) event_info.utf8_size,
					event_info.utf8_string);

				free(event_info.utf8_string);
			}
		}
	}
}

static void render_callback(void* data)
{
	// render our trademark square as a simple example, updating the whole
	// buffer each time without taking surface damage events into account
	struct globox_render_data* render_data = data;
	struct globox* globox = render_data->globox;
	struct globox_error_info error = {0};

	int width = globox_get_width(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	int height = globox_get_height(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	if ((width == 0) || (height == 0))
	{
		// skip rendering if the window area is 0
		return;
	}

	// we can make OpenGL 1 calls without any loader
	if (render_data->shaders == true)
	{
		compile_shaders();
		render_data->shaders = false;
	}

	GLint viewport_rect[4];

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

	globox_update_content(globox, NULL, &error);

	// reducing latency when resizing
	glFinish();

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}
}

static void config_callback(struct globox_config_reply* replies, size_t count, void* data)
{
	fprintf(stderr, "window configured succesfully, printing information:\n");

	struct globox* context = data;
	const char* message = NULL;
	size_t feature;

	for (size_t i = 0; i < count; ++i)
	{
		feature = replies[i].feature;

		if (feature < count)
		{
			if (replies[i].error.code == GLOBOX_ERROR_OK)
			{
				message = "success";
			}
			else
			{
				message = globox_error_get_msg(context, &replies[i].error);
			}

			fprintf(stderr, " - %s: %s\n", feature_names[feature], message);
		}
	}
}

int main(int argc, char** argv)
{
	struct globox_error_info error = {0};
	struct globox_error_info error_early = {0};
	printf("starting the complex globox example\n");

	// prepare function pointers
	struct globox_config_backend config = {0};

#ifdef GLOBOX_EXAMPLE_X11
#if defined(GLOBOX_EXAMPLE_GLX)
	globox_prepare_init_x11_glx(&config, &error_early);
#elif defined(GLOBOX_EXAMPLE_EGL)
	globox_prepare_init_x11_egl(&config, &error_early);
#endif
#endif

	// set function pointers and perform basic init
	struct globox* globox = globox_init(&config, &error);

	// Unless the context allocation failed it is always possible to access
	// error messages (even when the context initialization failed) so we can
	// always handle the backend initialization error first.

	// context allocation failed
	if (globox == NULL)
	{
		fprintf(stderr, "could not allocate the main globox context\n");
		return 1;
	}

	// Backend initialization failed. Since it happens before globox
	// initialization and errors are accessible even if it fails, we can handle
	// the errors in the right order regardless.
	if (globox_error_get_code(&error_early) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_early);
		globox_clean(globox, &error);
		return 1;
	}

	// The globox initialization had failed, make it known now if the backend
	// initialization that happened before went fine.
	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// set OpenGL configuration attributes
#if defined(GLOBOX_EXAMPLE_GLX)
	struct globox_config_glx config_opengl =
	{
		.major_version = 2,
		.minor_version = 0,
		.attributes = glx_config_attrib,
	};

#elif defined(GLOBOX_EXAMPLE_EGL)
	struct globox_config_egl config_opengl =
	{
		.major_version = 2,
		.minor_version = 0,
		.attributes = egl_config_attrib,
	};
#endif

#ifdef GLOBOX_EXAMPLE_X11
#if defined(GLOBOX_EXAMPLE_GLX)
	globox_init_x11_glx(globox, &config_opengl, &error);
#elif defined(GLOBOX_EXAMPLE_EGL)
	globox_init_x11_egl(globox, &config_opengl, &error);
#endif
#endif

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// get available features
	struct globox_config_features* feature_list =
		globox_init_features(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// initialize features when creating the window
	struct globox_feature_state state =
	{
		.state = GLOBOX_STATE_REGULAR,
	};

	struct globox_feature_title title =
	{
		.title = "globox",
	};

	struct globox_feature_icon icon =
	{
		// acceptable implementation-defined behavior
		// since it's also the implementation that
		// allows us to bundle resources like so
		.pixmap = (uint32_t*) iconpix,
		.len = 2 + (16 * 16) + 2 + (32 * 32) + 2 + (64 * 64),
	};

	struct globox_feature_size size =
	{
		.width = 500,
		.height = 500,
	};

	struct globox_feature_pos pos =
	{
		.x = 250,
		.y = 250,
	};

	struct globox_feature_frame frame =
	{
		.frame = true,
	};

	struct globox_feature_background background =
	{
		.background = GLOBOX_BACKGROUND_BLURRED,
	};

	struct globox_feature_vsync vsync =
	{
		.vsync = true,
	};

	// configure the feature and print a list
	printf("received a list of available features:\n");

	struct globox_config_request configs[GLOBOX_FEATURE_COUNT] = {0};
	size_t feature_added = 0;
	size_t i = 0;

	while (i < feature_list->count)
	{
		enum globox_feature feature_id = feature_list->list[i];
		printf(" - %s\n", feature_names[feature_id]);
		++i;

		switch (feature_id)
		{
			case GLOBOX_FEATURE_STATE:
			{
				configs[feature_added].config = &state;
				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				configs[feature_added].config = &title;
				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				configs[feature_added].config = &icon;
				break;
			}
			case GLOBOX_FEATURE_SIZE:
			{
				configs[feature_added].config = &size;
				break;
			}
			case GLOBOX_FEATURE_POS:
			{
				configs[feature_added].config = &pos;
				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				configs[feature_added].config = &frame;
				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				configs[feature_added].config = &background;
				break;
			}
			case GLOBOX_FEATURE_VSYNC:
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
		.globox = globox,
		.dpishit = NULL,
		.willis = NULL,
		.cursoryx = NULL,
		.action = { .action = GLOBOX_INTERACTION_STOP, },
		.mouse_custom = {0},
		.mouse_custom_active = 4,
		.mouse_grabbed = false,
	};

	// register an event handler to track the window's state
	struct globox_config_events events =
	{
		.data = &event_callback_data,
		.handler = event_callback,
	};

	struct globox_error_info error_events = {0};
	globox_init_events(globox, &events, &error_events);

	if (globox_error_get_code(&error_events) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_events);
		globox_clean(globox, &error);
		return 1;
	}

	// register a render callback
	struct globox_render_data render_data =
	{
		.globox = globox,
		.shaders = true,
	};

	struct globox_config_render render =
	{
		.data = &render_data,
		.callback = render_callback,
	};

	struct globox_error_info error_render = {0};
	globox_init_render(globox, &render, &error_render);

	if (globox_error_get_code(&error_render) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_render);
		globox_clean(globox, &error);
		return 1;
	}

	// create the window
	globox_window_create(globox, configs, feature_added, config_callback, globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// for instance, we can set the mouse cursor
	struct cursoryx_error_info error_cursor = {0};
	struct cursoryx_config_backend config_cursor = {0};

#ifdef GLOBOX_EXAMPLE_X11
	cursoryx_prepare_init_x11(&config_cursor);

	struct cursoryx_x11_data cursoryx_data =
	{
		.conn = globox_get_x11_conn(globox),
		.window = globox_get_x11_window(globox),
		.screen = globox_get_x11_screen(globox),
	};
#endif

	struct cursoryx* cursoryx = cursoryx_init(&config_cursor, &error_cursor);

	if (cursoryx == NULL)
	{
		fprintf(stderr, "could not allocate the main cursoryx context\n");
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	cursoryx_start(cursoryx, &cursoryx_data, &error_cursor);

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
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

	for (size_t i = 0; i < 4; ++i)
	{
		event_callback_data.mouse_custom[i] =
			cursoryx_custom_create(cursoryx, &(cursor_config[i]), &error_cursor);

		if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
		{
			cursoryx_error_log(cursoryx, &error_cursor);
			cursoryx_clean(cursoryx, &error_cursor);
			globox_window_destroy(globox, &error);
			globox_clean(globox, &error);
			return 1;
		}
	}

	// set a default regular cursor for our window (wait/busy cursor)
	cursoryx_set(cursoryx, CURSORYX_BUSY, &error_cursor);

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// init willis
	struct willis_error_info error_input = {0};
	struct willis_config_backend config_input = {0};

#ifdef GLOBOX_EXAMPLE_X11
	willis_prepare_init_x11(&config_input);

	struct willis_x11_data willis_data =
	{
		.conn = globox_get_x11_conn(globox),
		.window = globox_get_x11_window(globox),
		.root = globox_get_x11_root(globox),
	};
#endif

	struct willis* willis = willis_init(&config_input, &error_input);

	if (willis == NULL)
	{
		fprintf(stderr, "could not allocate the main willis context\n");
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	event_callback_data.willis = willis;

	// start willis
	if (willis_error_get_code(&error_input) != WILLIS_ERROR_OK)
	{
		willis_error_log(willis, &error_input);
		willis_clean(willis, &error_input);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	willis_start(willis, &willis_data, &error_input);

	if (willis_error_get_code(&error_input) != WILLIS_ERROR_OK)
	{
		willis_error_log(willis, &error_input);
		willis_clean(willis, &error_input);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// init dpishit
	struct dpishit_error_info error_display = {0};
	struct dpishit_config_backend config_display = {0};

#ifdef GLOBOX_EXAMPLE_X11
	dpishit_prepare_init_x11(&config_display);

	struct dpishit_x11_data dpishit_data =
	{
		.conn = globox_get_x11_conn(globox),
		.window = globox_get_x11_window(globox),
		.root = globox_get_x11_root(globox),
	};
#endif

	struct dpishit* dpishit = dpishit_init(&config_display, &error_display);

	if (dpishit == NULL)
	{
		fprintf(stderr, "could not allocate the main dpishit context\n");
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	event_callback_data.dpishit = dpishit;

	// start dpishit
	if (dpishit_error_get_code(&error_display) != DPISHIT_ERROR_OK)
	{
		dpishit_error_log(dpishit, &error_display);
		dpishit_clean(dpishit, &error_display);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	dpishit_start(dpishit, &dpishit_data, &error_display);

	if (dpishit_error_get_code(&error_display) != DPISHIT_ERROR_OK)
	{
		dpishit_error_log(dpishit, &error_display);
		dpishit_clean(dpishit, &error_display);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// display the window
	globox_window_start(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"this is a message from the main thread\n"
		"the window should now be visible\n"
		"we can keep computing here\n");

	// wait for the window to be closed
	globox_window_block(globox, &error);

	// stop willis
	willis_stop(willis, &error_input);

	if (willis_error_get_code(&error_input) != WILLIS_ERROR_OK)
	{
		willis_error_log(willis, &error_input);
		willis_clean(willis, &error_input);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	willis_clean(willis, &error_input);

	if (willis_error_get_code(&error_input) != WILLIS_ERROR_OK)
	{
		willis_error_log(willis, &error_input);
		willis_clean(willis, &error_input);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// stop dpishit
	dpishit_stop(dpishit, &error_display);

	if (dpishit_error_get_code(&error_display) != DPISHIT_ERROR_OK)
	{
		dpishit_error_log(dpishit, &error_display);
		dpishit_clean(dpishit, &error_display);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	dpishit_clean(dpishit, &error_display);

	if (dpishit_error_get_code(&error_display) != DPISHIT_ERROR_OK)
	{
		dpishit_error_log(dpishit, &error_display);
		dpishit_clean(dpishit, &error_display);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// stop cursoryx
	cursoryx_stop(cursoryx, &error_cursor);

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globox_error_log(globox, &error_render);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	cursoryx_clean(cursoryx, &error_cursor);

	if (cursoryx_error_get_code(&error_cursor) != CURSORYX_ERROR_OK)
	{
		cursoryx_error_log(cursoryx, &error_cursor);
		cursoryx_clean(cursoryx, &error_cursor);
		globox_error_log(globox, &error_render);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// stop globox
	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// handle event thread errors
	if (globox_error_get_code(&error_events) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_events);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// handle render thread errors
	if (globox_error_get_code(&error_render) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_render);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// free resources correctly
	globox_window_destroy(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	globox_clean(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return 1;
	}

	return 0;
}
