#ifndef H_GLOBOX_WINDOWS
#define H_GLOBOX_WINDOWS

#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "software/globox_windows_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "vulkan/globox_windows_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "egl/globox_windows_egl.h"
#endif

struct globox_platform
{
};

#endif
