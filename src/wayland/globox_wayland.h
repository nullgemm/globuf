#ifndef H_GLOBOX_WAYLAND
#define H_GLOBOX_WAYLAND

#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "software/globox_wayland_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "vulkan/globox_wayland_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "egl/globox_wayland_egl.h"
#endif

struct globox_platform
{
};

#endif
