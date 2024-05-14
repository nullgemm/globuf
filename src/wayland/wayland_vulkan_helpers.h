#ifndef H_GLOBUF_INTERNAL_WAYLAND_VULKAN_HELPERS
#define H_GLOBUF_INTERNAL_WAYLAND_VULKAN_HELPERS

#include "include/globuf.h"
#include "include/globuf_vulkan.h"
#include "wayland/wayland_common.h"
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>

struct wayland_vulkan_backend
{
	struct wayland_platform platform;
	struct globuf_config_vulkan* config;

	// vulkan extensions used by globuf
	uint32_t ext_len;
	const char** ext_needed;
	bool* ext_found;

	VkSurfaceKHR surface;
	VkWaylandSurfaceCreateInfoKHR vulkan_info;
};

size_t wayland_helpers_vulkan_add_extensions(
	struct globuf* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globuf_error_info* error);

#endif
