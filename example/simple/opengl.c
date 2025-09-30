#include "globuf.h"

#if defined(GLOBUF_EXAMPLE_X11)
#if defined(GLOBUF_EXAMPLE_GLX)
	#include "globuf_x11_glx.h"
#elif defined(GLOBUF_EXAMPLE_EGL)
	#include "globuf_x11_egl.h"
#endif
#elif defined(GLOBUF_EXAMPLE_APPKIT)
#include "globuf_appkit_egl.h"
#elif defined(GLOBUF_EXAMPLE_WIN)
#include "globuf_win_wgl.h"
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
#include "globuf_wayland_egl.h"
#endif

#ifdef GLOBUF_EXAMPLE_APPKIT
#define main real_main
#endif

#if defined(GLOBUF_SHARED)
#include <dlfcn.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(GLOBUF_EXAMPLE_GLX)
	#include <GL/glx.h>
	#include <GLES2/gl2.h>
#elif defined(GLOBUF_EXAMPLE_EGL)
	#include <EGL/egl.h>
	#include <GLES2/gl2.h>
#elif defined(GLOBUF_EXAMPLE_WGL)
	#include <GL/gl.h>
	#undef WGL_WGLEXT_PROTOTYPES
	#include <GL/wglext.h>
	#define GL_GLES_PROTOTYPES 0
	#include <GLES2/gl2.h>
#endif

extern uint8_t iconpix[];
extern int iconpix_size;

#if defined(GLOBUF_EXAMPLE_APPKIT)
extern uint8_t square_frag_gles2[];
extern int square_frag_gles2_size;

extern uint8_t square_vert_gles2[];
extern int square_vert_gles2_size;
#else
extern uint8_t square_frag_gl1[];
extern int square_frag_gl1_size;

extern uint8_t square_vert_gl1[];
extern int square_vert_gl1_size;
#endif

#define VERTEX_ATTR_POSITION 0

char* feature_names[GLOBUF_FEATURE_COUNT] =
{
	[GLOBUF_FEATURE_INTERACTION] = "interaction",
	[GLOBUF_FEATURE_STATE] = "state",
	[GLOBUF_FEATURE_TITLE] = "title",
	[GLOBUF_FEATURE_ICON] = "icon",
	[GLOBUF_FEATURE_SIZE] = "size",
	[GLOBUF_FEATURE_POS] = "pos",
	[GLOBUF_FEATURE_FRAME] = "frame",
	[GLOBUF_FEATURE_BACKGROUND] = "background",
	[GLOBUF_FEATURE_VSYNC] = "vsync",
};

#if defined(GLOBUF_EXAMPLE_GLX)
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
#elif defined(GLOBUF_EXAMPLE_EGL)
EGLint egl_config_attrib[] =
{
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
#if defined (GLOBUF_EXAMPLE_APPKIT)
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#else
	EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
#endif
	EGL_NONE,
};
#elif defined(GLOBUF_EXAMPLE_WGL)
int wgl_config_attrib[] =
{
	WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
	WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
	WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
	WGL_COLOR_BITS_ARB, 32,
	WGL_DEPTH_BITS_ARB, 16,
	WGL_STENCIL_BITS_ARB, 0,
	0,
};

// opengl32.dll only supports OpenGL 1
// so we have to load these functions
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;

PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

PFNGLATTACHSHADERPROC glAttachShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLDELETESHADERPROC glDeleteShader;

PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

static void load_wgl_functions()
{
	glCreateShader =
		(PFNGLCREATESHADERPROC)
			wglGetProcAddress("glCreateShader");
	glShaderSource =
		(PFNGLSHADERSOURCEPROC)
			wglGetProcAddress("glShaderSource");
	glCompileShader =
		(PFNGLCOMPILESHADERPROC)
			wglGetProcAddress("glCompileShader");

	glGetShaderiv =
		(PFNGLGETSHADERIVPROC)
			wglGetProcAddress("glGetShaderiv");
	glGetShaderInfoLog =
		(PFNGLGETSHADERINFOLOGPROC)
			wglGetProcAddress("glGetShaderInfoLog");

	glAttachShader =
		(PFNGLATTACHSHADERPROC)
			wglGetProcAddress("glAttachShader");
	glCreateProgram =
		(PFNGLCREATEPROGRAMPROC)
			wglGetProcAddress("glCreateProgram");
	glLinkProgram =
		(PFNGLLINKPROGRAMPROC)
			wglGetProcAddress("glLinkProgram");
	glDeleteShader =
		(PFNGLDELETESHADERPROC)
			wglGetProcAddress("glDeleteShader");

	glGetProgramiv =
		(PFNGLGETPROGRAMIVPROC)
			wglGetProcAddress("glGetProgramiv");
	glGetProgramInfoLog =
		(PFNGLGETPROGRAMINFOLOGPROC)
			wglGetProcAddress("glGetProgramInfoLog");

	glUseProgram =
		(PFNGLUSEPROGRAMPROC)
			wglGetProcAddress("glUseProgram");
	glEnableVertexAttribArray =
		(PFNGLENABLEVERTEXATTRIBARRAYPROC)
			wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer =
		(PFNGLVERTEXATTRIBPOINTERPROC)
			wglGetProcAddress("glVertexAttribPointer");
}
#endif

struct globuf_render_data
{
	struct globuf* globuf;
	bool shaders;
};

static void compile_shaders()
{
	GLint out;

	// compile OpenGL or glES shaders
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
#if defined(GLOBUF_EXAMPLE_APPKIT)
	const char * const square_vert_gl = (char*) &square_vert_gles2;
	GLint square_vert_size_gl = square_vert_gles2_size;
#else
	const char * const square_vert_gl = (char*) &square_vert_gl1;
	GLint square_vert_size_gl = square_vert_gl1_size;
#endif
	glShaderSource(vertex_shader, 1, &square_vert_gl, &square_vert_size_gl);
	fprintf(stderr, "compiling vertex shader:\n%.*s\n", square_vert_size_gl, square_vert_gl);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
#if defined(GLOBUF_EXAMPLE_APPKIT)
	const char * const square_frag_gl = (char*) &square_frag_gles2;
	GLint square_frag_size_gl = square_frag_gles2_size;
#else
	const char * const square_frag_gl = (char*) &square_frag_gl1;
	GLint square_frag_size_gl = square_frag_gl1_size;
#endif
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
	struct globuf* globuf = data;
	struct globuf_error_info error = {0};

	// print some debug info on internal events
	enum globuf_event abstract =
		globuf_handle_events(globuf, event, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	switch (abstract)
	{
		case GLOBUF_EVENT_INVALID:
		{
			// shouldn't be possible since we handle the error
			fprintf(stderr, "received invalid event\n");
			break;
		}
		case GLOBUF_EVENT_UNKNOWN:
		{
#ifdef GLOBUF_EXAMPLE_LOG_ALL
			fprintf(stderr, "received unknown event\n");
#endif
			break;
		}
		case GLOBUF_EVENT_RESTORED:
		{
			fprintf(stderr, "received `restored` event\n");
			break;
		}
		case GLOBUF_EVENT_MINIMIZED:
		{
			fprintf(stderr, "received `minimized` event\n");
			break;
		}
		case GLOBUF_EVENT_MAXIMIZED:
		{
			fprintf(stderr, "received `maximized` event\n");
			break;
		}
		case GLOBUF_EVENT_FULLSCREEN:
		{
			fprintf(stderr, "received `fullscreen` event\n");
			break;
		}
		case GLOBUF_EVENT_CLOSED:
		{
			fprintf(stderr, "received `closed` event\n");
			break;
		}
		case GLOBUF_EVENT_MOVED_RESIZED:
		{
			fprintf(stderr, "received `moved` event\n");
			break;
		}
		case GLOBUF_EVENT_DAMAGED:
		{
#ifdef GLOBUF_EXAMPLE_LOG_ALL
			struct globuf_rect rect = globuf_get_expose(globuf, &error);

			if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
			{
				globuf_error_log(globuf, &error);
				break;
			}

			fprintf(
				stderr,
				"received `content damaged` event:\n"
				"\tx: %d px\n"
				"\ty: %d px\n"
				"\twidth: %d px\n"
				"\theight: %d px\n",
				rect.x,
				rect.y,
				rect.width,
				rect.height);
#endif

			break;
		}
	}
}

static void render_callback(void* data)
{
	// render our trademark square as a simple example, updating the whole
	// buffer each time without taking surface damage events into account
	struct globuf_render_data* render_data = data;
	struct globuf* globuf = render_data->globuf;
	struct globuf_error_info error = {0};

	int width = globuf_get_width(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	int height = globuf_get_height(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
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
#ifdef GLOBUF_EXAMPLE_WGL
		load_wgl_functions();
#endif
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

	globuf_update_content(globuf, NULL, &error);

	// reducing latency when resizing
	glFinish();

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}
}

static void config_callback(struct globuf_config_reply* replies, size_t count, void* data)
{
	fprintf(stderr, "window configured succesfully, printing information:\n");

	struct globuf* context = data;
	const char* message = NULL;
	size_t feature;

	for (size_t i = 0; i < count; ++i)
	{
		feature = replies[i].feature;

		if (feature < GLOBUF_FEATURE_COUNT)
		{
			if (replies[i].error.code == GLOBUF_ERROR_OK)
			{
				message = "success";
			}
			else
			{
				message = globuf_error_get_msg(context, &replies[i].error);
			}

			fprintf(stderr, "\t%s: %s\n", feature_names[feature], message);
		}
	}
}

int main(int argc, char** argv)
{
	struct globuf_error_info error = {0};
	struct globuf_error_info error_early = {0};
	printf("starting the simple globuf example\n");

	// prepare function pointers
	struct globuf_config_backend config = {0};

#if !defined(GLOBUF_SHARED)
	// initialize statically
#if defined(GLOBUF_EXAMPLE_X11)
#if defined(GLOBUF_EXAMPLE_GLX)
	globuf_prepare_init_x11_glx(&config, &error_early);
#elif defined(GLOBUF_EXAMPLE_EGL)
	globuf_prepare_init_x11_egl(&config, &error_early);
#endif
#elif defined(GLOBUF_EXAMPLE_APPKIT)
	globuf_prepare_init_appkit_egl(&config, &error_early);
#elif defined(GLOBUF_EXAMPLE_WIN)
	globuf_prepare_init_win_wgl(&config, &error_early);
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	globuf_prepare_init_wayland_egl(&config, &error_early);
#endif
#else
	// prepare dynamic initializer
	void* globuf_lib = NULL;
	void (*globuf_prepare_init)() = NULL;

	char* path_globuf_lib = NULL;
	char* sym_globuf_init = NULL;

	#if defined(GLOBUF_EXAMPLE_X11)
	path_globuf_lib = "./globuf_x11_vulkan.so";
	#if defined(GLOBUF_EXAMPLE_GLX)
	sym_globuf_init = "globuf_prepare_init_x11_glx";
	#elif defined(GLOBUF_EXAMPLE_EGL)
	sym_globuf_init = "globuf_prepare_init_x11_egl";
	#endif
	#elif defined(GLOBUF_EXAMPLE_APPKIT)
	path_globuf_lib = "./globuf_appkit_vulkan.dylib";
	sym_globuf_init = "globuf_prepare_init_appkit_vulkan";
	#elif defined(GLOBUF_EXAMPLE_WIN)
	path_globuf_lib = "./globuf_win_vulkan.dll";
	sym_globuf_init = "globuf_prepare_init_win_vulkan";
	#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	path_globuf_lib = "./globuf_wayland_vulkan.so";
	sym_globuf_init = "globuf_prepare_init_wayland_vulkan";
	#endif

	// load the backend binder symbol straight from a shared object
	globuf_lib = dlopen(path_globuf_lib, 0);
	globuf_prepare_init = dlsym(globuf_lib, sym_globuf_init);

	// run the binder to load the remaining function pointers for the target implementation
	globuf_prepare_init(&config, &error_early);
#endif

	// set function pointers and perform basic init
	struct globuf* globuf = globuf_init(&config, &error);

	// Unless the context allocation failed it is always possible to access
	// error messages (even when the context initialization failed) so we can
	// always handle the backend initialization error first.

	// context allocation failed
	if (globuf == NULL)
	{
		fprintf(stderr, "could not allocate the main globuf context\n");
		return 1;
	}

	// Backend initialization failed. Since it happens before globuf
	// initialization and errors are accessible even if it fails, we can handle
	// the errors in the right order regardless.
	if (globuf_error_get_code(&error_early) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_early);
		globuf_clean(globuf, &error);
		return 1;
	}

	// The globuf initialization had failed, make it known now if the backend
	// initialization that happened before went fine.
	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// set OpenGL configuration attributes
	struct globuf_config_opengl config_opengl =
	{
		.major_version = 2,
		.minor_version = 0,
#if defined(GLOBUF_EXAMPLE_GLX)
		.attributes = glx_config_attrib,
#elif defined(GLOBUF_EXAMPLE_EGL)
		.attributes = egl_config_attrib,
#elif defined(GLOBUF_EXAMPLE_WGL)
		.attributes = wgl_config_attrib,
#endif
	};

	globuf_init_opengl(globuf, &config_opengl, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// get available features
	struct globuf_config_features* feature_list =
		globuf_init_features(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// initialize features when creating the window
	struct globuf_feature_state state =
	{
		.state = GLOBUF_STATE_REGULAR,
	};

	struct globuf_feature_title title =
	{
		.title = "globuf",
	};

	struct globuf_feature_icon icon =
	{
		// acceptable implementation-defined behavior
		// since it's also the implementation that
		// allows us to bundle resources like so
		.pixmap = (uint32_t*) iconpix,
		.len = 2 + (16 * 16) + 2 + (32 * 32) + 2 + (64 * 64),
	};

	struct globuf_feature_size size =
	{
		.width = 500,
		.height = 500,
	};

	struct globuf_feature_pos pos =
	{
		.x = 250,
		.y = 250,
	};

	struct globuf_feature_frame frame =
	{
		.frame = true,
	};

	struct globuf_feature_background background =
	{
		.background = GLOBUF_BACKGROUND_BLURRED,
	};

	struct globuf_feature_vsync vsync =
	{
		.vsync = true,
	};

	// configure the feature and print a list
	printf("received a list of available features:\n");

	struct globuf_config_request configs[GLOBUF_FEATURE_COUNT] = {0};
	size_t feature_added = 0;
	size_t i = 0;

	while (i < feature_list->count)
	{
		enum globuf_feature feature_id = feature_list->list[i];
		printf("\t%s\n", feature_names[feature_id]);
		++i;

		switch (feature_id)
		{
			case GLOBUF_FEATURE_STATE:
			{
				configs[feature_added].config = &state;
				break;
			}
			case GLOBUF_FEATURE_TITLE:
			{
				configs[feature_added].config = &title;
				break;
			}
			case GLOBUF_FEATURE_ICON:
			{
				configs[feature_added].config = &icon;
				break;
			}
			case GLOBUF_FEATURE_SIZE:
			{
				configs[feature_added].config = &size;
				break;
			}
			case GLOBUF_FEATURE_POS:
			{
				configs[feature_added].config = &pos;
				break;
			}
			case GLOBUF_FEATURE_FRAME:
			{
				configs[feature_added].config = &frame;
				break;
			}
			case GLOBUF_FEATURE_BACKGROUND:
			{
				configs[feature_added].config = &background;
				break;
			}
			case GLOBUF_FEATURE_VSYNC:
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

	// register an event handler to track the window's state
	struct globuf_config_events events =
	{
		.data = globuf,
		.handler = event_callback,
	};

	struct globuf_error_info error_events = {0};
	globuf_init_events(globuf, &events, &error_events);

	if (globuf_error_get_code(&error_events) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_events);
		globuf_clean(globuf, &error);
		return 1;
	}

	// register a render callback
	struct globuf_render_data render_data =
	{
		.globuf = globuf,
		.shaders = true,
	};

	struct globuf_config_render render =
	{
		.data = &render_data,
		.callback = render_callback,
	};

	struct globuf_error_info error_render = {0};
	globuf_init_render(globuf, &render, &error_render);

	if (globuf_error_get_code(&error_render) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_render);
		globuf_clean(globuf, &error);
		return 1;
	}

	// create the window
	globuf_window_create(globuf, configs, feature_added, config_callback, globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// check window
	globuf_window_confirm(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// display the window
	globuf_window_start(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"this is a message from the main thread\n"
		"the window should now be visible\n"
		"we can keep computing here\n");

	// wait for the window to be closed
	globuf_window_block(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// handle event thread errors
	if (globuf_error_get_code(&error_events) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_events);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// handle render thread errors
	if (globuf_error_get_code(&error_render) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_render);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// free resources correctly
	globuf_window_destroy(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	globuf_clean(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return 1;
	}

	return 0;
}
