#ifndef H_GLOBOX_INTERNAL_X11_SOFTWARE_HELPERS
#define H_GLOBOX_INTERNAL_X11_SOFTWARE_HELPERS

#include "include/globox.h"
#include "x11/x11_common.h"
#include <xcb/xcb_image.h>

struct x11_backend
{
	struct x11_platform platform;

	bool shared_pixmaps;
	xcb_gcontext_t software_gfx;
	xcb_pixmap_t software_pixmap;
	xcb_shm_segment_info_t software_shm;
};

void x11_helpers_visual_transparent(
	struct globox* context,
	struct globox_error_info* error);

void x11_helpers_visual_opaque(
	struct globox* context,
	struct globox_error_info* error);

void x11_helpers_shm_create(
	struct globox* context,
	size_t len,
	struct globox_error_info* error);

#endif
