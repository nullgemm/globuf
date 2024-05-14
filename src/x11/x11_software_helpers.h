#ifndef H_GLOBUF_INTERNAL_X11_SOFTWARE_HELPERS
#define H_GLOBUF_INTERNAL_X11_SOFTWARE_HELPERS

#include "include/globuf.h"
#include "x11/x11_common.h"
#include <xcb/xcb_image.h>

struct x11_software_backend
{
	struct x11_platform platform;

	bool shared_pixmaps;
	xcb_gcontext_t software_gfx;
	xcb_pixmap_t software_pixmap;
	xcb_shm_segment_info_t software_shm;
};

void x11_helpers_visual_transparent(
	struct globuf* context,
	struct globuf_error_info* error);

void x11_helpers_visual_opaque(
	struct globuf* context,
	struct globuf_error_info* error);

void x11_helpers_shm_create(
	struct globuf* context,
	size_t len,
	struct globuf_error_info* error);

#endif
