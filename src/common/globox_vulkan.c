#include "include/globox.h"
#include "include/globox_vulkan.h"
#include "common/globox_private.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

void globox_init_vulkan(
	struct globox* context,
	struct globox_config_vulkan* config,
	struct globox_error_info* error)
{
	struct globox_calls_vulkan* vulkan = context->backend_callbacks.data;

	vulkan->init(
		context,
		config,
		error);
}

void globox_get_extensions_vulkan(
	struct globox* context,
	uint32_t* len,
	const char*** list,
	struct globox_error_info* error)
{
	struct globox_calls_vulkan* vulkan = context->backend_callbacks.data;

	vulkan->get_extensions(
		context,
		len,
		list,
		error);
}

VkBool32 globox_presentation_support_vulkan(
	struct globox* context,
	VkPhysicalDevice physical_device,
	uint32_t queue_family_index,
	struct globox_error_info* error)
{
	struct globox_calls_vulkan* vulkan = context->backend_callbacks.data;

	return vulkan->presentation_support(
		context,
		physical_device,
		queue_family_index,
		error);
}

VkSurfaceKHR* globox_get_surface_vulkan(
	struct globox* context,
	struct globox_error_info* error)
{
	struct globox_calls_vulkan* vulkan = context->backend_callbacks.data;

	return vulkan->get_surface(
		context,
		error);
}
