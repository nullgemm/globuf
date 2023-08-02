#ifndef H_GLOBOX_INTERNAL_WIN_VULKAN_HELPERS
#define H_GLOBOX_INTERNAL_WIN_VULKAN_HELPERS

#include "include/globox.h"
#include "include/globox_vulkan.h"
#include "win/win_common.h"
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

struct win_vulkan_backend
{
	struct win_platform platform;
	struct globox_config_vulkan* config;

	// vulkan extensions used by globox
	uint32_t ext_len;
	const char** ext_needed;
	bool* ext_found;

	VkSurfaceKHR surface;
	VkWin32SurfaceCreateInfoKHR vulkan_info;
};

size_t win_helpers_vulkan_add_extensions(
	struct globox* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globox_error_info* error);

#endif
