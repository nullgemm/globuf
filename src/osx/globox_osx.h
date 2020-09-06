#ifndef H_GLOBOX_OSX
#define H_GLOBOX_OSX

#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "software/globox_osx_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "vulkan/globox_osx_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "egl/globox_osx_egl.h"
#endif

struct globox_platform
{
};

#endif
