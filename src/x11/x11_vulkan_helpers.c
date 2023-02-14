#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_vulkan_helpers.h"
#include "x11/x11_vulkan.h"
#include "x11/x11_common.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>
#include <xcb/render.h>

size_t x11_helpers_vulkan_add_extensions(
	struct globox* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globox_error_info* error)
{
	// list vulkan extensions here to make the code cleaner
	const char* extensions_names[] =
	{
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
	};

	size_t extensions_count =
		(sizeof (extensions_names)) / (sizeof (const char*));

	// allocate required extensions list
	*ext_needed = malloc(extensions_count * sizeof (char*));

	if (*ext_needed == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return 0;
	}

	// allocate found required extensions list
	*ext_found = malloc(extensions_count * sizeof (bool));

	if (*ext_found == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return 0;
	}

	// initialize values
	for (size_t i = 0; i < extensions_count; ++i)
	{
		(*ext_needed)[i] = extensions_names[i];
		(*ext_found)[i] = false;
	}

	globox_error_ok(error);

	return extensions_count;
}

void x11_helpers_vulkan_visual_transparent(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	xcb_render_query_pict_formats_cookie_t cookie_pict;
	xcb_render_query_pict_formats_reply_t* reply_pict;
	xcb_generic_error_t* error_pict;

	cookie_pict =
		xcb_render_query_pict_formats(
			platform->conn);

	reply_pict =
		xcb_render_query_pict_formats_reply(
			platform->conn,
			cookie_pict,
			&error_pict);

	if (error_pict != NULL)
	{
		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE);

		return;
	}

	// loop over all buffer formats to find ARGB8888
	xcb_render_pictforminfo_iterator_t iter_pict;
	xcb_render_pictforminfo_t* pictforminfo;
	xcb_render_pictformat_t pictformat;
	bool found_format;

	iter_pict =
		xcb_render_query_pict_formats_formats_iterator(
			reply_pict);

	found_format = false;

	while (iter_pict.rem != 0)
	{
		pictforminfo = iter_pict.data;

		if ((pictforminfo->direct.alpha_mask == 255)
			&& (pictforminfo->direct.red_mask == 255)
			&& (pictforminfo->direct.green_mask == 255)
			&& (pictforminfo->direct.blue_mask == 255)
			&& (pictforminfo->direct.alpha_shift == 24)
			&& (pictforminfo->direct.red_shift == 16)
			&& (pictforminfo->direct.green_shift == 8)
			&& (pictforminfo->direct.blue_shift == 0)
			&& (pictforminfo->depth == 32))
		{
			platform->visual_depth = 32;
			pictformat = pictforminfo->id;
			found_format = true;
			break;
		}

		xcb_render_pictforminfo_next(
			&iter_pict);
	}

	if (found_format == false)
	{
		free(reply_pict);

		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE);

		return;
	}

	// match the buffer format with a visual id
	xcb_render_pictscreen_iterator_t iter_screens;
	xcb_render_pictdepth_iterator_t iter_depths;
	xcb_render_pictvisual_iterator_t iter_visuals;
	bool found_visual;

	iter_screens =
		xcb_render_query_pict_formats_screens_iterator(
			reply_pict);

	found_visual = false;

	while ((iter_screens.rem != 0) && (found_visual == false))
	{
		iter_depths =
			xcb_render_pictscreen_depths_iterator(
				iter_screens.data);

		while ((iter_depths.rem != 0) && (found_visual == false))
		{
			iter_visuals =
				xcb_render_pictdepth_visuals_iterator(
					iter_depths.data);

			while ((iter_visuals.rem != 0) && (found_visual == false))
			{
				// infer false-positive
				if ((iter_visuals.data != NULL)
					&& (iter_visuals.data->format == pictformat))
				{
					platform->visual_id = iter_visuals.data->visual;
					found_visual = true;
					break;
				}

				xcb_render_pictvisual_next(
					&iter_visuals);
			}

			xcb_render_pictdepth_next(
				&iter_depths);
		}

		xcb_render_pictscreen_next(
			&iter_screens);
	}

	free(reply_pict);

	if (found_visual == false)
	{

		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE);

		return;
	}

	globox_error_ok(error);

	return;
}

void x11_helpers_vulkan_visual_opaque(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	platform->visual_id = platform->screen_obj->root_visual;
	xcb_visualtype_t* visual = NULL;

	xcb_depth_iterator_t depth_iter =
		xcb_screen_allowed_depths_iterator(
			platform->screen_obj);

	xcb_visualtype_iterator_t visual_iter;

	while (depth_iter.rem != 0)
	{
		visual_iter = xcb_depth_visuals_iterator(depth_iter.data);

		while (visual_iter.rem != 0)
		{
			if (platform->visual_id == visual_iter.data->visual_id)
			{
				visual = visual_iter.data;
				break;
			}

			xcb_visualtype_next(&visual_iter);
		}

		if (visual != NULL)
		{
			if ((visual->_class != XCB_VISUAL_CLASS_TRUE_COLOR)
				&& (visual->_class != XCB_VISUAL_CLASS_DIRECT_COLOR))
			{
				globox_error_throw(
					context,
					error,
					GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE);

				return;
			}
			else
			{
				break;
			}
		}

		xcb_depth_next(&depth_iter);
	}

	if (depth_iter.rem == 0)
	{
		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_X11_VISUAL_MISSING);

		return;
	}

	platform->visual_depth = depth_iter.data->depth;

	globox_error_ok(error);
}
