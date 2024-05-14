#include "include/globuf.h"
#include "include/globuf_vulkan.h"
#include "common/globuf_private.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

void globuf_init_vulkan(
	struct globuf* context,
	struct globuf_config_vulkan* config,
	struct globuf_error_info* error)
{
	struct globuf_calls_vulkan* vulkan = context->backend_callbacks.data;

	vulkan->init(
		context,
		config,
		error);
}

void globuf_get_extensions_vulkan(
	struct globuf* context,
	uint32_t* len,
	const char*** list,
	struct globuf_error_info* error)
{
	struct globuf_calls_vulkan* vulkan = context->backend_callbacks.data;

	vulkan->get_extensions(
		context,
		len,
		list,
		error);
}

VkBool32 globuf_presentation_support_vulkan(
	struct globuf* context,
	VkPhysicalDevice physical_device,
	uint32_t queue_family_index,
	struct globuf_error_info* error)
{
	struct globuf_calls_vulkan* vulkan = context->backend_callbacks.data;

	return vulkan->presentation_support(
		context,
		physical_device,
		queue_family_index,
		error);
}

VkSurfaceKHR* globuf_get_surface_vulkan(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct globuf_calls_vulkan* vulkan = context->backend_callbacks.data;

	return vulkan->get_surface(
		context,
		error);
}
