#include "globox.h"
#include "globox_error.h"
#include "x11/globox_x11.h"
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/extensions/Xrender.h>
#include <GL/glx.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

int glx_extension_support(const char *list, const char *extension)
{
	const char* beg = list;
	const char* end;
	const char* cur;

	cur = strstr(beg, extension);

	while (cur != NULL)
	{
		end = cur + strlen(extension);

		// the extension name might be a subset of another one so
		// we must check the surrouding characters to make sure
		if (((cur == beg) || (cur[-1] == ' '))
			&& ((end[0] == '\0') || (end[0] == ' ')))
		{
			return 1;
		}

		beg = end;
		cur = strstr(beg, extension);
	}

	return 0;
}

void expose(struct globox* globox, int len)
{

}

void reserve(struct globox* globox)
{

}

void globox_context_glx_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;
	struct globox_x11_glx* context = &(platform->globox_x11_glx);

	context->globox_glx_version_major = version_major;
	context->globox_glx_version_minor = version_minor;

	platform->globox_x11_reserve = reserve;
	platform->globox_x11_expose = expose;

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
		return;
	}

	// find compatible framebuffer configuration
	int i = 0;
	bool fb_valid = false;

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

		XFree(visual_info);

		if (pict_format == NULL)
		{
			continue;
		}

		fb_valid = true;
		context->globox_glx_fb_config = fb_config_list[i];

		if (globox->globox_transparent == false)
		{
			break;
		}

		if (pict_format->direct.alphaMask > 0)
		{
			// found an alpha-compatible configuration
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
		return;
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
		return;
	}

	// query visual depth
	int visual_depth;

	error_fb =
		glXGetFBConfigAttrib(
			context->globox_glx_display,
			context->globox_glx_fb_config,
			GLX_DEPTH_SIZE,
			&visual_depth);

	if (globox->globox_transparent == true)
	{
		if (visual_depth != 24)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_X11_GLX_FAIL);
			return;
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
		return;
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

	return;
}

void globox_context_glx_create(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = globox->globox_platform;
	struct globox_x11_glx* context = &(platform->globox_x11_glx);

	// create GLX context
	const char* list =
		glXQueryExtensionsString(
			context->globox_glx_display,
			platform->globox_x11_screen_id);

	int attribs_support =
		glx_extension_support(
			list,
			"GLX_ARB_create_context");

	if (attribs_support != 0)
	{
		// get function pointer
		GLXContext (*glXCreateContextAttribsARB)() =
			(GLXContext (*)())
				glXGetProcAddressARB(
					(const GLubyte*) "glXCreateContextAttribsARB");

		if (glXCreateContextAttribsARB == NULL)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_X11_GLX_FAIL);
			return;
		}

		int attr[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, context->globox_glx_version_major,
			GLX_CONTEXT_MINOR_VERSION_ARB, context->globox_glx_version_minor,
			None
		};

		context->globox_glx_context =
			glXCreateContextAttribsARB(
				context->globox_glx_display,
				context->globox_glx_fb_config,
				0,
				True,
				attr);
	}
	else
	{
		context->globox_glx_context =
			glXCreateNewContext(
				context->globox_glx_display,
				context->globox_glx_fb_config,
				GLX_RGBA_TYPE,
				NULL,
				True);
	}

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
	struct globox_x11_glx* context = &(globox->globox_platform->globox_x11_glx);

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
	struct globox_x11_glx* context = &(globox->globox_platform->globox_x11_glx);

	glXSwapBuffers(
		context->globox_glx_display,
		context->globox_glx_win);

	globox->globox_redraw = false;
}

void globox_context_glx_shrink(struct globox* globox)
{
	// not needed
}
