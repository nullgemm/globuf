#ifndef H_GLOBOX_INTERNAL_WAYLAND_VULKAN_HELPERS
#define H_GLOBOX_INTERNAL_WAYLAND_VULKAN_HELPERS

#include "include/globox.h"
#include "include/globox_vulkan.h"
#include "wayland/wayland_common.h"
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>

struct wayland_vulkan_backend
{
	struct wayland_platform platform;
	struct globox_config_vulkan* config;

	// vulkan extensions used by globox
	uint32_t ext_len;
	const char** ext_needed;
	bool* ext_found;

	VkSurfaceKHR surface;
	VkWaylandSurfaceCreateInfoKHR vulkan_info;
};

size_t globox_wayland_helpers_vulkan_add_extensions(
	struct globox* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globox_error_info* error);

#endif
