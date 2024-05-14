#ifndef H_GLOBUF_INTERNAL_WIN_VULKAN_HELPERS
#define H_GLOBUF_INTERNAL_WIN_VULKAN_HELPERS

#include "include/globuf.h"
#include "include/globuf_vulkan.h"
#include "win/win_common.h"
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

struct win_vulkan_backend
{
	struct win_platform platform;
	struct globuf_config_vulkan* config;

	// vulkan extensions used by globuf
	uint32_t ext_len;
	const char** ext_needed;
	bool* ext_found;

	VkSurfaceKHR surface;
	VkWin32SurfaceCreateInfoKHR vulkan_info;
};

size_t win_helpers_vulkan_add_extensions(
	struct globuf* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globuf_error_info* error);

#endif
