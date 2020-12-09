#include "globox.h"
#include "globox_error.h"
// system includes
#include <stdlib.h>
// macos includes
#include "macos/globox_macos_types.h"
#include "macos/globox_macos_symbols.h"

// include platform structures
#include "macos/globox_macos.h"
#include "macos/software/globox_macos_software.h"

void appdelegate_software_callback(
	struct macos_appdelegate* appdelegate,
	SEL cmd,
	id msg)
{
	void* out = NULL;

	object_getInstanceVariable(
		(id) appdelegate,
		"globox",
		&out);

	if (out == NULL)
	{
		return;
	}

	struct globox* globox = (struct globox*) out;
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_macos_software* context = &(platform->globox_macos_software);

	// initialize the view object with the view frame
	id tmp;

	struct macos_rect frame =
	{
		.origin.x = 0,
		.origin.y = 0,
		.size.width = globox->globox_width,
		.size.height = globox->globox_height,
	};

	tmp =
		macos_msg_id_rect(
			platform->globox_macos_obj_masterview,
			sel_getUid("initWithFrame:"),
			frame);

	if (tmp == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	platform->globox_macos_obj_masterview = tmp;

	tmp =
		macos_msg_id_rect(
			platform->globox_macos_obj_view,
			sel_getUid("initWithFrame:"),
			frame);

	if (tmp == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	platform->globox_macos_obj_view = tmp;

	macos_msg_void_bool(
		platform->globox_macos_obj_view,
		sel_getUid("setWantsLayer:"),
		YES);

	tmp =
		macos_msg_id_none(
			platform->globox_macos_obj_view,
			sel_getUid("layer"));

	if (tmp == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	context->globox_software_layer = tmp;

	tmp =
		macos_msg_id_rect(
			platform->globox_macos_obj_blur,
			sel_getUid("initWithFrame:"),
			frame);

	if (tmp == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	platform->globox_macos_obj_blur = tmp;
}

void globox_context_software_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_macos_software* context = &(platform->globox_macos_software);

	context->globox_software_buffer_width = globox->globox_width;
	context->globox_software_buffer_height = globox->globox_height;

	platform->globox_macos_appdelegate_context_callback =
		appdelegate_software_callback;

	platform->globox_platform_argb =
		(uint32_t*) malloc(4 * globox->globox_width * globox->globox_height);

	if (platform->globox_platform_argb == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_NULL);
		return;
	}

	return;
}

void globox_context_software_create(struct globox* globox)
{
	// not needed
}

void globox_context_software_shrink(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_macos_software* context = &(platform->globox_macos_software);

	context->globox_software_buffer_width = globox->globox_width;
	context->globox_software_buffer_height = globox->globox_height;

	platform->globox_platform_argb =
		realloc(
			platform->globox_platform_argb,
			4 * globox->globox_width * globox->globox_height);

	if (platform->globox_platform_argb == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
	}
}

void globox_context_software_free(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	free(platform->globox_platform_argb);
}

void globox_context_software_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_macos_software* context = &(platform->globox_macos_software);

	globox->globox_redraw = false;

	// build layer-compatible image
	id colorspace =
		CGColorSpaceCreateDeviceRGB();

	if (colorspace == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);
		return;
	}

	id bitmap =
		CGBitmapContextCreate(
			platform->globox_platform_argb,
			context->globox_software_buffer_width,
			context->globox_software_buffer_height,
			8,
			context->globox_software_buffer_width * 4,
			colorspace,
			(4 << 12) | (1)); // TODO names

	if (bitmap == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);

		macos_msg_void_none(
			colorspace,
			sel_getUid("dealloc"));

		return;
	}

	id image =
		CGBitmapContextCreateImage(bitmap);

	if (image == Nil)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_MACOS_OBJ_NIL);

		macos_msg_void_none(
			bitmap,
			sel_getUid("dealloc"));

		macos_msg_void_none(
			colorspace,
			sel_getUid("dealloc"));

		return;
	}

	// copy to layer
	macos_msg_void_id(
		context->globox_software_layer,
		sel_getUid("setContents:"),
		image);

	// re-allocate memory if needed
	struct macos_rect frame =
		macos_msg_rect_none(
			platform->globox_macos_obj_view,
			sel_getUid("frame"));

	size_t frame_virtual_len =
		frame.size.width * frame.size.height;

	size_t backend_buffer_len =
		context->globox_software_buffer_width * context->globox_software_buffer_height;

	if (frame_virtual_len > backend_buffer_len)
	{
		free(platform->globox_platform_argb);

		platform->globox_platform_argb =
			(uint32_t*) malloc(4 * frame.size.width * frame.size.height);

		globox->globox_redraw = true;
	}

	context->globox_software_buffer_width = frame.size.width;
	context->globox_software_buffer_height = frame.size.height;
	globox->globox_width = frame.size.width;
	globox->globox_height = frame.size.height;

	// free memory
	macos_msg_void_none(
		image,
		sel_getUid("dealloc"));

	macos_msg_void_none(
		bitmap,
		sel_getUid("dealloc"));

	macos_msg_void_none(
		colorspace,
		sel_getUid("dealloc"));

	// commit
	globox_platform_commit(globox);
}

// getters

uint32_t globox_software_get_buffer_width(struct globox* globox)
{
	return globox->globox_platform.globox_macos_software.globox_software_buffer_width;
}

uint32_t globox_software_get_buffer_height(struct globox* globox)
{
	return globox->globox_platform.globox_macos_software.globox_software_buffer_height;
}
