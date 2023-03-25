#ifndef H_EXAMPLE_VULKAN_HELPERS
#define H_EXAMPLE_VULKAN_HELPERS

#include "globox.h"
#include "globox_vulkan.h"

#include <stdint.h>
#include <vulkan/vulkan_core.h>

struct globox_render_data
{
	// globox info
	struct globox* globox;
	struct globox_config_vulkan config;

	int width;
	int height;
	bool shaders;


	// vulkan general info
	VkDebugUtilsMessengerEXT debug;
	VkInstance instance;
	VkDevice device;
	VkQueue queue;

	VkShaderModule module_vert;
	VkShaderModule module_frag;

	VkSemaphore semaphore_present;
	VkSemaphore semaphore_render;
	VkFence fence_frame;

	// vulkan physical device
	VkPhysicalDevice* phys_devs;
	uint32_t phys_devs_index;
	uint32_t phys_devs_len;
	uint32_t selected_queue;

	// vulkan surface
	VkSurfaceKHR* surf;
	VkSurfaceCapabilitiesKHR surf_caps;
	VkExtent2D extent;
	VkFormat format;
	VkColorSpaceKHR color_space;

	VkSurfaceFormatKHR* surf_formats;
	uint32_t surf_formats_index;
	uint32_t surf_formats_len;

	VkPresentModeKHR* surf_modes;
	uint32_t surf_modes_index;
	uint32_t surf_modes_len;

	VkSwapchainKHR swapchain;
	VkImage* swapchain_images;
	VkImageView* swapchain_image_views;
	uint32_t swapchain_images_len;

	// vulkan pipeline
	VkPipelineShaderStageCreateInfo shader_stages[2];
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;

	VkFramebuffer* framebuffers;
	VkRenderPass render_pass;

	VkCommandPool cmd_pool;
	VkCommandBuffer cmd_buf;

	VkBuffer vertex_buf;
	VkDeviceMemory vertex_buf_mem;
};

void free_check(const void* ptr);
void init_vulkan(struct globox_render_data* data);
void config_vulkan(struct globox_render_data* data);
void swapchain_free_vulkan(struct globox_render_data* data);
void swapchain_vulkan(struct globox_render_data* data);
void pipeline_free_vulkan(struct globox_render_data* data);
void pipeline_vulkan(struct globox_render_data* data);
void compile_shaders(struct globox_render_data* data);
void render_vulkan(struct globox_render_data* data);

#endif
