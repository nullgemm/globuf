#ifndef H_GLOBUF_VULKAN
#define H_GLOBUF_VULKAN

#include "globuf.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

// common to all backends supporting Vulkan rendering

struct globuf_config_vulkan
{
	VkInstance instance;
	VkAllocationCallbacks* allocator;
};

struct globuf_calls_vulkan
{
	void (*init)(
		struct globuf* context,
		struct globuf_config_vulkan* config,
		struct globuf_error_info* error);

	void (*get_extensions)(
		struct globuf* context,
		uint32_t* len,
		const char*** list,
		struct globuf_error_info* error);

	VkBool32 (*presentation_support)(
		struct globuf* context,
		VkPhysicalDevice physical_device,
		uint32_t queue_family_index,
		struct globuf_error_info* error);

	VkSurfaceKHR* (*get_surface)(
		struct globuf* context,
		struct globuf_error_info* error);
};

void globuf_init_vulkan(
	struct globuf* context,
	struct globuf_config_vulkan* config,
	struct globuf_error_info* error);

void globuf_get_extensions_vulkan(
	struct globuf* context,
	uint32_t* len,
	const char*** list,
	struct globuf_error_info* error);

VkBool32 globuf_presentation_support_vulkan(
	struct globuf* context,
	VkPhysicalDevice physical_device,
	uint32_t queue_family_index,
	struct globuf_error_info* error);

VkSurfaceKHR* globuf_get_surface_vulkan(
	struct globuf* context,
	struct globuf_error_info* error);

#endif
