#ifndef H_GLOBOX_INTERNAL_X11_VULKAN_HELPERS
#define H_GLOBOX_INTERNAL_X11_VULKAN_HELPERS

#include "include/globox.h"
#include "include/globox_vulkan.h"
#include "x11/x11_common.h"
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>

struct x11_vulkan_backend
{
	struct x11_platform platform;
	struct globox_config_vulkan* config;

	// vulkan extensions used by globox
	uint32_t ext_len;
	const char** ext_needed;
	bool* ext_found;

	VkSurfaceKHR surface;
	VkXcbSurfaceCreateInfoKHR vulkan_info;
};

size_t x11_helpers_vulkan_add_extensions(
	struct globox* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globox_error_info* error);

void x11_helpers_vulkan_visual_transparent(
	struct globox* context,
	struct globox_error_info* error);

void x11_helpers_vulkan_visual_opaque(
	struct globox* context,
	struct globox_error_info* error);

#endif
