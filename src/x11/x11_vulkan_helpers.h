#ifndef H_GLOBUF_INTERNAL_X11_VULKAN_HELPERS
#define H_GLOBUF_INTERNAL_X11_VULKAN_HELPERS

#include "include/globuf.h"
#include "include/globuf_vulkan.h"
#include "x11/x11_common.h"
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>

struct x11_vulkan_backend
{
	struct x11_platform platform;
	struct globuf_config_vulkan* config;

	// vulkan extensions used by globuf
	uint32_t ext_len;
	const char** ext_needed;
	bool* ext_found;

	VkSurfaceKHR surface;
	VkXcbSurfaceCreateInfoKHR vulkan_info;
};

size_t x11_helpers_vulkan_add_extensions(
	struct globuf* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globuf_error_info* error);

void x11_helpers_vulkan_visual_transparent(
	struct globuf* context,
	struct globuf_error_info* error);

void x11_helpers_vulkan_visual_opaque(
	struct globuf* context,
	struct globuf_error_info* error);

#endif
