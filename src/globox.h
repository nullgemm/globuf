// common includes
#include <>
#include ""

// platform-specific includes
#if defined(GLOBOX_PLATFORM_WAYLAND)
	#include "wayland/globox_wayland.h"
#elif defined(GLOBOX_PLATFORM_X11)
	#include "x11/globox_x11.h"
#elif defined(GLOBOX_PLATFORM_WINDOWS)
	#include "windows/globox_windows.h"
#elif defined(GLOBOX_PLATFORM_OSX)
	#include "osx/globox_osx.h"
#endif

// globox
struct globox
{
	// common fields
	uint16_t width;
	uint16_t height;

	// platform-specific substructure
	struct globox_platform platform;
};

// errors for all platforms
enum globox_errors
{
	GLOBOX_ERROR_OK = 0,

	GLOBOX_ERROR_NULL,
	GLOBOX_ERROR_ALLOC,
	GLOBOX_ERROR_BOUNDS,
	GLOBOX_ERROR_DOMAIN,

#if defined(GLOBOX_PLATFORM_WAYLAND)
#elif defined(GLOBOX_PLATFORM_X11)
#elif defined(GLOBOX_PLATFORM_WINDOWS)
#elif defined(GLOBOX_PLATFORM_OSX)
#endif

	GLOBOX_ERROR_SIZE,
};
