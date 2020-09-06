#ifndef H_GLOBOX_X11
#define H_GLOBOX_X11

#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "software/globox_x11_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "vulkan/globox_x11_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "egl/globox_x11_egl.h"
#endif

struct globox_platform
{
};

#endif
