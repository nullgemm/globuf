#include "globox.h"
#include "globox_error.h"
#include "x11/globox_x11.h"
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/extensions/Xrender.h>
#include <GL/glx.h>
#include <stdint.h>
#include <stdbool.h>

bool globox_context_glx_init(
	struct globox* globox,
	int version_major,
	int version_minor,
	bool transparent,
	bool blur)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_x11_glx* context = &(platform->globox_x11_glx);

	// get available framebuffer configurations
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

	int fb_config_count;

	GLXFBConfig *fb_config_list =
		glXChooseFBConfig(
			context->globox_glx_display,
			platform->globox_x11_screen_id,
			glx_config_attrib,
			&fb_config_count);

	if ((fb_config_list == NULL) || (fb_config_count == 0))
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_GLX_FAIL);
		return false;
	}

	// find compatible framebuffer configuration
	int i = 0;
	bool fb_valid = false;
	bool fb_alpha = false;

	XVisualInfo* visual_info;
	XRenderPictFormat* pict_format;

	while (i < fb_config_count)
	{
		visual_info =
			glXGetVisualFromFBConfig(
				context->globox_glx_display,
				fb_config_list[i]);

		if (visual_info == NULL)
		{
			continue;
		}

		pict_format =
			XRenderFindVisualFormat(
				context->globox_glx_display,
				visual_info->visual);

		if (pict_format == NULL)
		{
			continue;
		}

		fb_valid = true;
		context->globox_glx_fb_config = fb_config_list[i];

		if (transparent == false)
		{
			break;
		}

		if (pict_format->direct.alphaMask > 0)
		{
			fb_alpha = true;
			break;
		}

		++i;
	}

	XFree(fb_config_list);

	if (fb_valid == false)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_GLX_FAIL);
		return false;
	}

	// query visual ID
	int error_fb;
	int visual_id;

	error_fb =
		glXGetFBConfigAttrib(
			context->globox_glx_display,
			context->globox_glx_fb_config,
			GLX_VISUAL_ID,
			&visual_id);

	platform->globox_x11_visual_id = visual_id;

	if (error_fb != 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_GLX_FAIL);
		return false;
	}

	// query visual depth
	int visual_depth;

	error_fb =
		glXGetFBConfigAttrib(
			context->globox_glx_display,
			context->globox_glx_fb_config,
			GLX_DEPTH_SIZE,
			&visual_depth);

	if (transparent == true)
	{
		if (visual_depth != 24)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_X11_GLX_FAIL);
			return false;
		}

		platform->globox_x11_visual_depth = 32;
	}
	else
	{
		platform->globox_x11_visual_depth = visual_depth;
	}

	if (error_fb != 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_GLX_FAIL);
		return false;
	}

	// create colormap
	xcb_colormap_t colormap =
		xcb_generate_id(
			platform->globox_x11_conn);

	xcb_create_colormap(
		platform->globox_x11_conn,
		XCB_COLORMAP_ALLOC_NONE,
		colormap,
		platform->globox_x11_screen_obj->root,
		visual_id);

	// add colormap to XCB
	platform->globox_x11_attr_val[2] =
		colormap;

	globox->globox_transparent = transparent;
	globox->globox_blur = blur;

	return fb_alpha;
}

void globox_context_glx_create(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_x11_glx* context = &(platform->globox_x11_glx);

	// create GLX context
	// TODO support OpenGL3 using glXCreateContextAttribsARBProc
	context->globox_glx_context =
		glXCreateNewContext(
			context->globox_glx_display,
			context->globox_glx_fb_config,
			GLX_RGBA_TYPE,
			NULL,
			True);

	if (context->globox_glx_context == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_GLX_FAIL);
		return;
	}

	// create GLX window
	context->globox_glx_win =
		glXCreateWindow(
			context->globox_glx_display,
			context->globox_glx_fb_config,
			platform->globox_x11_win,
			NULL);

	if (context->globox_glx_win == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_GLX_FAIL);
		return;
	}

	// bind context to window
	Bool error_glx =
		glXMakeContextCurrent(
			context->globox_glx_display,
			context->globox_glx_win,
			context->globox_glx_win,
			context->globox_glx_context);

	if (error_glx == False)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_X11_GLX_FAIL);
		return;
	}
}

void globox_context_glx_free(struct globox* globox)
{
	// alias for readability
	struct globox_x11_glx* context = &(globox->globox_platform.globox_x11_glx);

	glXDestroyWindow(context->globox_glx_display, context->globox_glx_win);
	glXDestroyContext(context->globox_glx_display, context->globox_glx_context);
}

void globox_context_glx_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height)
{
	// alias for readability
	struct globox_x11_glx* context = &(globox->globox_platform.globox_x11_glx);

	glXSwapBuffers(
		context->globox_glx_display,
		context->globox_glx_win);

	globox->globox_redraw = false;
}

void globox_context_glx_shrink(struct globox* globox)
{

}

void globox_context_glx_reserve(struct globox* globox)
{

}

void globox_context_glx_expose(struct globox* globox, int len)
{

}

// getters
Display* globox_glx_get_display(struct globox* globox)
{
	return globox->globox_platform.globox_x11_glx.globox_glx_display;
}

GLXFBConfig globox_glx_get_fb_config(struct globox* globox)
{
	return globox->globox_platform.globox_x11_glx.globox_glx_fb_config;
}

GLXContext globox_glx_get_context(struct globox* globox)
{
	return globox->globox_platform.globox_x11_glx.globox_glx_context;
}

GLXWindow globox_glx_get_win(struct globox* globox)
{
	return globox->globox_platform.globox_x11_glx.globox_glx_win;
}
