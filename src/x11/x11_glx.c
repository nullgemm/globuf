#include "include/globox.h"
#include "include/globox_glx.h"
#include "include/globox_x11_glx.h"

#include "common/globox_private.h"
#include "x11/x11_common.h"
#include "x11/x11_common_helpers.h"
#include "x11/x11_glx.h"
#include "x11/x11_glx_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <xcb/glx.h>
#include <xcb/xcb.h>
#include <GL/glx.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>

void globox_x11_glx_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct x11_glx_backend* backend = malloc(sizeof (struct x11_glx_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct x11_glx_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize values that can be initialized explicitly
	backend->config = NULL;

	// open a connection to the X server
	struct x11_platform* platform = &(backend->platform);
	backend->display = XOpenDisplay(NULL);

	if (backend->display == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX_DISPLAY_OPEN);
		return;
	}

	platform->conn = XGetXCBConnection(backend->display);

	if (platform->conn == NULL)
	{
		XCloseDisplay(backend->display);
		globox_error_throw(context, error, GLOBOX_ERROR_X11_CONN);
		return;
	}

	XSetEventQueueOwner(backend->display, XCBOwnsEventQueue);
	platform->screen_id = XDefaultScreen(backend->display);

	Bool error_glx =
		glXQueryExtension(
			backend->display,
			&(backend->error_base),
			&(backend->event_base));

	if (error_glx == False)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX);
		return;
	}

	// initialize the platform
	globox_x11_common_init(context, platform, error);

	// error always set
}

void globox_x11_glx_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// close the connection to the X server
	XCloseDisplay(backend->display);

	// clean the platform
	globox_x11_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globox_x11_glx_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	x11_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// run common X11 helper
	globox_x11_common_window_create(context, platform, configs, count, callback, data, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// get framebuffer configurations list
	int fb_config_count;

	GLXFBConfig* fb_config_list =
		glXChooseFBConfig(
			backend->display,
			platform->screen_id,
			backend->config->attributes,
			&fb_config_count);

	if ((fb_config_list == NULL) || (fb_config_count == 0))
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX_FB_CONF_LIST);
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
				backend->display,
				fb_config_list[i]);

		if (visual_info == NULL)
		{
			continue;
		}

		pict_format =
			XRenderFindVisualFormat(
				backend->display,
				visual_info->visual);

		XFree(visual_info);

		if (pict_format == NULL)
		{
			continue;
		}

		fb_valid = true;
		backend->fb_config = fb_config_list[i];

		if (context->feature_background->background == GLOBOX_BACKGROUND_OPAQUE)
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
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX_FB_INVALID);
		return;
	}

	// query visual ID
	int error_fb;
	int visual_id;

	error_fb =
		glXGetFBConfigAttrib(
			backend->display,
			backend->fb_config,
			GLX_VISUAL_ID,
			&visual_id);

	platform->visual_id = visual_id;

	if (error_fb != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX_FB_CONF_ATTR);
		return;
	}

	// query visual depth
	int visual_depth;

	error_fb =
		glXGetFBConfigAttrib(
			backend->display,
			backend->fb_config,
			GLX_DEPTH_SIZE,
			&visual_depth);

	if (context->feature_background->background != GLOBOX_BACKGROUND_OPAQUE)
	{
		// TODO clarify that
		if (visual_depth != 24)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE);
			return;
		}

		platform->visual_depth = 32;
	}
	else
	{
		platform->visual_depth = visual_depth;
	}

	if (error_fb != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX_FB_CONF_ATTR);
		return;
	}

	// create colormap
	xcb_colormap_t colormap = xcb_generate_id(platform->conn);

	xcb_create_colormap(
		platform->conn,
		XCB_COLORMAP_ALLOC_NONE,
		colormap,
		platform->root_win,
		visual_id);

	// add colormap to XCB
	platform->attr_val[2] = colormap;

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_x11_glx_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	glXDestroyWindow(backend->display, backend->win);
	glXDestroyContext(backend->display, backend->glx);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// run common X11 helper
	globox_x11_common_window_destroy(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_x11_glx_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// create GLX context
	const char* list =
		glXQueryExtensionsString(
			backend->display,
			platform->screen_id);

	bool arb_ext =
		x11_helpers_glx_ext_support(
			list,
			"GLX_ARB_create_context");

	if (arb_ext == true)
	{
		// get function pointer
		GLXContext (*glXCreateContextAttribsARB)() =
			(GLXContext (*)())
				glXGetProcAddressARB(
					(const GLubyte*) "glXCreateContextAttribsARB");

		if (glXCreateContextAttribsARB == NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX_ATTR_ARB);
			return;
		}

		int attr[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, backend->config->major_version,
			GLX_CONTEXT_MINOR_VERSION_ARB, backend->config->minor_version,
			None,
		};

		backend->glx =
			glXCreateContextAttribsARB(
				backend->display,
				backend->fb_config,
				0,
				True,
				attr);
	}
	else
	{
		backend->glx =
			glXCreateNewContext(
				backend->display,
				backend->fb_config,
				GLX_RGBA_TYPE,
				NULL,
				True);
	}

	if (backend->glx == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX_CONTEXT);
		return;
	}

	// create GLX window
	backend->win =
		glXCreateWindow(
			backend->display,
			backend->fb_config,
			platform->win,
			NULL);

	if (backend->win == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_GLX_WINDOW);
		return;
	}

	// configure VSync if available
	if (context->feature_vsync != NULL)
	{
		// check for Adaptive VSync extension
		bool tear_ext =
			x11_helpers_glx_ext_support(
				list,
				"EXT_swap_control_tear");

		// enable VSync
		int interval;

		if (context->feature_vsync->vsync == true)
		{
			if (tear_ext == true)
			{
				interval = -1;
			}
			else
			{
				interval = 1;
			}
		}
		else
		{
			interval = 0;
		}

		// get extension function pointer
		PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = NULL;

		glXSwapIntervalEXT =
			(PFNGLXSWAPINTERVALEXTPROC)
				glXGetProcAddressARB(
					(const GLubyte*) "glXSwapIntervalEXT");

		if (glXSwapIntervalEXT != NULL)
		{
			glXSwapIntervalEXT(
				backend->display,
				backend->win,
				interval);
		}
	}

	glXSelectEvent(
		backend->display,
		backend->win,
		GLX_BUFFER_SWAP_COMPLETE_INTEL_MASK);

	// run common X11 helper
	globox_x11_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_glx_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper (mutex locked when unblocked)
	globox_x11_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_glx_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globox_x11_glx_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_init_render(context, platform, config, error);

	platform->render_init_callback = x11_helpers_glx_bind;

	// error always set
}

void globox_x11_glx_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globox_event globox_x11_glx_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// process GLX events
	enum globox_event out;
	xcb_generic_event_t* xcb_event = event;

	int event_swap = backend->event_base + XCB_GLX_BUFFER_SWAP_COMPLETE;
	int event_type = xcb_event->response_type & ~0x80;

	if (event_type == event_swap)
	{
		// synchronize with XSync
		out = GLOBOX_EVENT_UNKNOWN;
		// lock xsync mutex
		int error_posix = pthread_mutex_lock(&(platform->mutex_xsync));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
			return out;
		}

		// safe value updates
		if (platform->xsync_status == GLOBOX_XSYNC_CONFIGURED)
		{
			platform->xsync_status = GLOBOX_XSYNC_ACKNOWLEDGED;
		}

		// unlock xsync mutex
		error_posix = pthread_mutex_unlock(&(platform->mutex_xsync));

		if (error_posix != 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
			return out;
		}
	}
	else
	{
		// run common X11 helper
		out =
			globox_x11_common_handle_events(
				context,
				platform,
				event,
				error);
	}

	// no extra failure check at the moment

	// error always set
	return out;
}


struct globox_config_features* globox_x11_glx_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	struct globox_config_features* features =
		globox_x11_common_init_features(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return features;
	}

	// available if the VSync extension is available
	const char* list =
		glXQueryExtensionsString(
			backend->display,
			platform->screen_id);

	// check for VSync extension
	bool vsync_ext =
		x11_helpers_glx_ext_support(
			list,
			"GLX_EXT_swap_control");

	if (vsync_ext == true)
	{
		features->list[features->count] = GLOBOX_FEATURE_VSYNC;
		context->feature_vsync =
			malloc(sizeof (struct globox_feature_vsync));
		features->count += 1;

		if (context->feature_vsync == NULL)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
			return NULL;
		}
	}

	// return the newly created features info structure
	// error always set
	return features;
}

void globox_x11_glx_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_x11_glx_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_x11_glx_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_x11_glx_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_x11_glx_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_width(context, platform, error);
}

unsigned globox_x11_glx_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_height(context, platform, error);
}

struct globox_rect globox_x11_glx_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_expose(context, platform, error);
}


void globox_x11_glx_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	glXSwapBuffers(
		backend->display,
		backend->win);

	globox_error_ok(error);
}


xcb_connection_t* globox_get_x11_conn(struct globox* context)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	return platform->conn;
}

xcb_window_t globox_get_x11_window(struct globox* context)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	return platform->win;
}

xcb_window_t globox_get_x11_root(struct globox* context)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	return platform->root_win;
}

xcb_screen_t* globox_get_x11_screen(struct globox* context)
{
	struct x11_glx_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	return platform->screen_obj;
}


void globox_prepare_init_x11_glx(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	config->data = NULL;
	config->init = globox_x11_glx_init;
	config->clean = globox_x11_glx_clean;
	config->window_create = globox_x11_glx_window_create;
	config->window_destroy = globox_x11_glx_window_destroy;
	config->window_start = globox_x11_glx_window_start;
	config->window_block = globox_x11_glx_window_block;
	config->window_stop = globox_x11_glx_window_stop;
	config->init_render = globox_x11_glx_init_render;
	config->init_events = globox_x11_glx_init_events;
	config->handle_events = globox_x11_glx_handle_events;
	config->init_features = globox_x11_glx_init_features;
	config->feature_set_interaction = globox_x11_glx_feature_set_interaction;
	config->feature_set_state = globox_x11_glx_feature_set_state;
	config->feature_set_title = globox_x11_glx_feature_set_title;
	config->feature_set_icon = globox_x11_glx_feature_set_icon;
	config->get_width = globox_x11_glx_get_width;
	config->get_height = globox_x11_glx_get_height;
	config->get_expose = globox_x11_glx_get_expose;
	config->update_content = globox_x11_glx_update_content;

	globox_error_ok(error);
}


// OpenGL configuration setter
void globox_init_x11_glx(
	struct globox* context,
	struct globox_config_glx* config,
	struct globox_error_info* error)
{
	struct x11_glx_backend* backend = context->backend_data;

	backend->config = config;

	globox_error_ok(error);
}
