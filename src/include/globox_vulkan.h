#ifndef H_GLOBOX_VULKAN
#define H_GLOBOX_VULKAN

#include "globox.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

// common to all backends supporting Vulkan rendering

struct globox_config_vulkan
{
	VkInstance instance;
	VkAllocationCallbacks* allocator;
};

struct globox_calls_vulkan
{
	void (*init)(
		struct globox* context,
		struct globox_config_vulkan* config,
		struct globox_error_info* error);

	void (*get_extensions)(
		struct globox* context,
		uint32_t* len,
		const char*** list,
		struct globox_error_info* error);

	VkBool32 (*presentation_support)(
		struct globox* context,
		VkPhysicalDevice physical_device,
		uint32_t queue_family_index,
		struct globox_error_info* error);

	VkSurfaceKHR* (*get_surface)(
		struct globox* context,
		struct globox_error_info* error);
};

void globox_init_vulkan(
	struct globox* context,
	struct globox_config_vulkan* config,
	struct globox_error_info* error);

void globox_get_extensions_vulkan(
	struct globox* context,
	uint32_t* len,
	const char*** list,
	struct globox_error_info* error);

VkBool32 globox_presentation_support_vulkan(
	struct globox* context,
	VkPhysicalDevice physical_device,
	uint32_t queue_family_index,
	struct globox_error_info* error);

VkSurfaceKHR* globox_get_surface_vulkan(
	struct globox* context,
	struct globox_error_info* error);

#endif
