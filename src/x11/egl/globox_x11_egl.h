#ifndef H_GLOBOX_X11_EGL
#define H_GLOBOX_X11_EGL

#include "globox.h"
#include "x11/globox_x11.h"
// context includes
#include <EGL/egl.h>

struct globox_x11_egl
{
	// egl init
	EGLDisplay globox_egl_display;
	EGLContext globox_egl_context;
	EGLSurface globox_egl_surface;

	// framebuffer info
	EGLConfig globox_egl_config;
	EGLint globox_egl_config_size;
};

#if 0
// x11 includes
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/shm.h>

struct globox_x11_software
{
	xcb_shm_segment_info_t globox_x11_software_shm;
	xcb_gcontext_t globox_x11_software_gfx;
	xcb_pixmap_t globox_x11_software_pixmap;
	bool globox_x11_software_pixmap_update;
	bool globox_x11_software_shared_pixmaps;

	uint32_t globox_x11_software_buffer_width;
	uint32_t globox_x11_software_buffer_height;
};
#endif

#endif
