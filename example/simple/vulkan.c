#include "globox.h"

#ifdef GLOBOX_EXAMPLE_X11
#include "globox_x11_vulkan.h"
#endif

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

extern uint8_t iconpix[];
extern int iconpix_size;

extern uint8_t square_frag_vk1[];
extern int square_frag_vk1_size;

extern uint8_t square_vert_vk1[];
extern int square_vert_vk1_size;

char* feature_names[GLOBOX_FEATURE_COUNT] =
{
	[GLOBOX_FEATURE_INTERACTION] = "interaction",
	[GLOBOX_FEATURE_STATE] = "state",
	[GLOBOX_FEATURE_TITLE] = "title",
	[GLOBOX_FEATURE_ICON] = "icon",
	[GLOBOX_FEATURE_SIZE] = "size",
	[GLOBOX_FEATURE_POS] = "pos",
	[GLOBOX_FEATURE_FRAME] = "frame",
	[GLOBOX_FEATURE_BACKGROUND] = "background",
	[GLOBOX_FEATURE_VSYNC] = "vsync",
};

// layers to enable
struct vk_inst_layers
{
	const char* name;
	bool found;
};

struct vk_inst_layers vk_inst_layers[] =
{
	{
		.name = "VK_LAYER_KHRONOS_validation",
		.found = false,
	},
#if 0
	{
		.name = "VK_LAYER_LUNARG_api_dump",
		.found = false,
	},
#endif
};

// device extensions to enable
struct vk_dev_ext
{
	const char* name;
	bool found;
};

struct vk_dev_ext vk_dev_ext[] =
{
	{
		.name = VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		.found = false,
	},
};

// mem type props
struct vk_mem_types
{
	enum VkMemoryPropertyFlagBits flag;
	const char* name;
};

struct vk_mem_types vk_mem_types[] =
{
	{
		.flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.name = "device local",
	},
	{
		.flag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		.name = "host visible",
	},
	{
		.flag = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		.name = "host coherent",
	},
	{
		.flag = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
		.name = "host cached",
	},
	{
		.flag = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
		.name = "lazily allocated",
	},
	{
		.flag = VK_MEMORY_PROPERTY_PROTECTED_BIT,
		.name = "protected",
	},
	{
		.flag = VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD,
		.name = "device coherent",
	},
	{
		.flag = VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD,
		.name = "device uncached",
	},
	{
		.flag = VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV,
		.name = "RDMA capable",
	},
};

// mem heap props
struct vk_mem_heaps
{
	enum VkMemoryHeapFlagBits flag;
	const char* name;
};

struct vk_mem_heaps vk_mem_heaps[] =
{
	{
		.flag = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
		.name = "device local",
	},
	{
		.flag = VK_MEMORY_HEAP_MULTI_INSTANCE_BIT,
		.name = "multi instance",
	},
	{
		.flag = VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR,
		.name = "multi instance",
	},
};

// queue family props
struct vk_queue_fams
{
	enum VkQueueFlagBits flag;
	const char* name;
};

struct vk_queue_fams vk_queue_fams[] =
{
	{
		.flag = VK_QUEUE_GRAPHICS_BIT,
		.name = "graphics",
	},
	{
		.flag = VK_QUEUE_COMPUTE_BIT,
		.name = "compute",
	},
	{
		.flag = VK_QUEUE_TRANSFER_BIT,
		.name = "transfer",
	},
	{
		.flag = VK_QUEUE_SPARSE_BINDING_BIT,
		.name = "sparse binding",
	},
	{
		.flag = VK_QUEUE_PROTECTED_BIT,
		.name = "protected",
	},
#ifdef VK_ENABLE_BETA_EXTENSIONS
	{
		.flag = VK_QUEUE_VIDEO_DECODE_BIT_KHR,
		.name = "video decode",
	},
	{
		.flag = VK_QUEUE_VIDEO_ENCODE_BIT_KHR,
		.name = "video encode",
	},
#endif
	{
		.flag = VK_QUEUE_OPTICAL_FLOW_BIT_NV,
		.name = "optical flow",
	},
};

// presentation mode
struct vk_pres_modes
{
	VkPresentModeKHR mode;
	const char* name;
};

// ordered by priority
struct vk_pres_modes vk_pres_modes[] =
{
	{
		.mode = VK_PRESENT_MODE_FIFO_KHR,
		.name = "fifo",
	},
	{
		.mode = VK_PRESENT_MODE_FIFO_RELAXED_KHR,
		.name = "fifo relaxed",
	},
	{
		.mode = VK_PRESENT_MODE_IMMEDIATE_KHR,
		.name = "immediate",
	},
	{
		.mode = VK_PRESENT_MODE_MAILBOX_KHR,
		.name = "mailbox",
	},
	{
		.mode = VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR,
		.name = "shared demand refresh",
	},
	{
		.mode = VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR,
		.name = "shared continuous refresh",
	},
};

struct globox_render_data
{
	// globox info
	struct globox* globox;
	struct globox_config_vulkan config;

	int width;
	int height;
	bool shaders;


	// vulkan general info
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

static inline void free_check(const void* ptr)
{
	if (ptr != NULL)
	{
		free((void*) ptr);
	}
}

static void init_vulkan(struct globox_render_data* data)
{
	struct globox_error_info globox_error = {0};
	VkResult error = VK_ERROR_UNKNOWN;

	// get vulkan extensions from globox
	uint32_t ext_globox_len;
	const char** ext_globox;

	globox_get_extensions_vulkan(
		data->globox,
		&ext_globox_len,
		&ext_globox,
		&globox_error);

	if (globox_error_get_code(&globox_error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(data->globox, &globox_error);
		globox_clean(data->globox, &globox_error);
		return;
	}

	// get instance extensions
	uint32_t inst_ext_count = 0;

	error =
		vkEnumerateInstanceExtensionProperties(
			NULL,
			&inst_ext_count,
			NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not count instance extensions\n");
		return;
	}

	VkExtensionProperties* inst_ext_props =
		malloc(inst_ext_count * (sizeof (VkExtensionProperties)));

	if (inst_ext_props == NULL)
	{
		fprintf(stderr, "could not allocate instance extensions list\n");
		return;
	}

	error =
		vkEnumerateInstanceExtensionProperties(
			NULL,
			&inst_ext_count,
			inst_ext_props);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list instance extensions\n");
		return;
	}

	// print instance extensions
	printf("available vulkan instance extensions:\n");

	for (uint32_t i = 0; i < inst_ext_count; ++i)
	{
		printf(
			"\t%s version %u\n",
			inst_ext_props[i].extensionName,
			inst_ext_props[i].specVersion);
	}

	free(inst_ext_props);

	printf("using vulkan instance extensions:\n");

	for (uint32_t i = 0; i < ext_globox_len; ++i)
	{
		printf("\t%s\n", ext_globox[i]);
	}

	// get layers list
	uint32_t inst_layer_props_len = 0;
	VkLayerProperties* inst_layer_props = NULL;

	error = vkEnumerateInstanceLayerProperties(&inst_layer_props_len, NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not count instance layer properties\n");
		return;
	}

	inst_layer_props = malloc(inst_layer_props_len * (sizeof (VkLayerProperties)));

	if (inst_layer_props == NULL)
	{
		fprintf(stderr, "could not allocate instance layer properties list\n");
		return;
	}

	error = vkEnumerateInstanceLayerProperties(&inst_layer_props_len, inst_layer_props);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list instance layer properties\n");
		return;
	}

	// print instance layer properties
	printf("available vulkan instance layers:\n");

	for (uint32_t i = 0; i < inst_layer_props_len; ++i)
	{
		printf(
			"\t%s version %u\n",
			inst_layer_props[i].layerName,
			inst_layer_props[i].specVersion);
	}

	// check required layers
	size_t inst_layers_len =
		(sizeof (vk_inst_layers)) / (sizeof (struct vk_inst_layers));
	const char** inst_layers_found =
		malloc(inst_layers_len);
	uint32_t inst_layers_found_count =
		0;

	if (inst_layers_found == NULL)
	{
		fprintf(stderr, "could not allocate found layers list\n");
		return;
	}

	// check layers
	printf("using vulkan instance layers:\n");

	for (uint32_t i = 0; i < inst_layer_props_len; ++i)
	{
		uint32_t k = 0;

		while (k < inst_layers_len)
		{
			if ((vk_inst_layers[k].found == false)
				&& (strcmp(inst_layer_props[i].layerName, vk_inst_layers[k].name) == 0))
			{
				// save as a layer to request
				inst_layers_found[inst_layers_found_count] = vk_inst_layers[k].name;
				printf("\t%s\n", inst_layers_found[inst_layers_found_count]);
				++inst_layers_found_count;

				// skip saved layers
				vk_inst_layers[k].found = true;
				++k;

				continue;
			}

			++k;
		}
	}

	free(inst_layer_props);

	// create vulkan instance
	VkApplicationInfo app_info =
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = "globox example",
		.applicationVersion = 1,
		.pEngineName = "globox vulkan example",
		.engineVersion = 1,
		.apiVersion = VK_MAKE_VERSION(1, 0, 0),
	};

	VkInstanceCreateInfo instance_info =
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.pApplicationInfo = &app_info,
		.enabledLayerCount = inst_layers_found_count,
		.ppEnabledLayerNames = inst_layers_found,
		.enabledExtensionCount = ext_globox_len,
		.ppEnabledExtensionNames = ext_globox,
	};

	error = vkCreateInstance(&instance_info, NULL, &(data->instance));
	free(inst_layers_found);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could create the vulkan instance\n");
		return;
	}

	// set vulkan config
	data->config.instance = data->instance;
	data->config.allocator = NULL;

	globox_init_vulkan(data->globox, &(data->config), &globox_error);

	if (globox_error_get_code(&globox_error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(data->globox, &globox_error);
		globox_clean(data->globox, &globox_error);
		return;
	}
}

static void config_vulkan(struct globox_render_data* data)
{
	struct globox_error_info globox_error = {0};
	VkResult error = VK_ERROR_UNKNOWN;

	// get physical devices list
	error =
		vkEnumeratePhysicalDevices(
			data->instance,
			&(data->phys_devs_len),
			NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not count physical devices\n");
		return;
	}

	data->phys_devs = malloc(data->phys_devs_len * (sizeof (VkPhysicalDevice)));

	if (data->phys_devs == NULL)
	{
		fprintf(stderr, "could not allocate physical devices list\n");
		return;
	}

	error =
		vkEnumeratePhysicalDevices(
			data->instance,
			&(data->phys_devs_len),
			data->phys_devs);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list physical devices\n");
		free(data->phys_devs);
		return;
	}

	// select physical device
	data->phys_devs_index = 0;
	data->selected_queue = 0;
	bool found_device = false;

	size_t mem_types_len =
		(sizeof (vk_mem_types)) / (sizeof (struct vk_mem_types));

	size_t mem_heaps_len =
		(sizeof (vk_mem_heaps)) / (sizeof (struct vk_mem_heaps));

	size_t queue_fams_len =
		(sizeof (vk_queue_fams)) / (sizeof (struct vk_queue_fams));

	for (uint32_t i = 0; i < data->phys_devs_len; ++i)
	{
		// get physical device properties
		VkPhysicalDeviceProperties phys_dev_props =
		{
			0,
		};

		vkGetPhysicalDeviceProperties(
			data->phys_devs[i],
			&phys_dev_props);

		printf(
			"vulkan device #%u:\n"
			"\tmax. vulkan version supported: 0x%0x\n"
			"\tdriver version: 0x%0x\n"
			"\tvendor ID: 0x%0x\n"
			"\tdevice ID: 0x%0x\n"
			"\tdevice name: %s\n",
			i,
			phys_dev_props.apiVersion,
			phys_dev_props.driverVersion,
			phys_dev_props.vendorID,
			phys_dev_props.deviceID,
			phys_dev_props.deviceName);

		// get physical device features
		VkPhysicalDeviceFeatures phys_dev_feat =
		{
			0,
		};

		vkGetPhysicalDeviceFeatures(
			data->phys_devs[i],
			&phys_dev_feat);

		printf(
			"\tsupported features:\n"
			"\t\trobustBufferAccess: %d\n"
			"\t\tfullDrawIndexUint32: %d\n"
			"\t\timageCubeArray: %d\n"
			"\t\tindependentBlend: %d\n"
			"\t\tgeometryShader: %d\n"
			"\t\ttessellationShader: %d\n"
			"\t\tsampleRateShading: %d\n"
			"\t\tdualSrcBlend: %d\n"
			"\t\tlogicOp: %d\n"
			"\t\tmultiDrawIndirect: %d\n"
			"\t\tdrawIndirectFirstInstance: %d\n"
			"\t\tdepthClamp: %d\n"
			"\t\tdepthBiasClamp: %d\n"
			"\t\tfillModeNonSolid: %d\n"
			"\t\tdepthBounds: %d\n"
			"\t\twideLines: %d\n"
			"\t\tlargePoints: %d\n"
			"\t\talphaToOne: %d\n"
			"\t\tmultiViewport: %d\n"
			"\t\tsamplerAnisotropy: %d\n"
			"\t\ttextureCompressionETC2: %d\n"
			"\t\ttextureCompressionASTC_LDR: %d\n"
			"\t\ttextureCompressionBC: %d\n"
			"\t\tocclusionQueryPrecise: %d\n"
			"\t\tpipelineStatisticsQuery: %d\n"
			"\t\tvertexPipelineStoresAndAtomics: %d\n"
			"\t\tfragmentStoresAndAtomics: %d\n"
			"\t\tshaderTessellationAndGeometryPointSize: %d\n"
			"\t\tshaderImageGatherExtended: %d\n"
			"\t\tshaderStorageImageExtendedFormats: %d\n"
			"\t\tshaderStorageImageMultisample: %d\n"
			"\t\tshaderStorageImageReadWithoutFormat: %d\n"
			"\t\tshaderStorageImageWriteWithoutFormat: %d\n"
			"\t\tshaderUniformBufferArrayDynamicIndexing: %d\n"
			"\t\tshaderSampledImageArrayDynamicIndexing: %d\n"
			"\t\tshaderStorageBufferArrayDynamicIndexing: %d\n"
			"\t\tshaderStorageImageArrayDynamicIndexing: %d\n"
			"\t\tshaderClipDistance: %d\n"
			"\t\tshaderCullDistance: %d\n"
			"\t\tshaderFloat64: %d\n"
			"\t\tshaderInt64: %d\n"
			"\t\tshaderInt16: %d\n"
			"\t\tshaderResourceResidency: %d\n"
			"\t\tshaderResourceMinLod: %d\n"
			"\t\tsparseBinding: %d\n"
			"\t\tsparseResidencyBuffer: %d\n"
			"\t\tsparseResidencyImage2D: %d\n"
			"\t\tsparseResidencyImage3D: %d\n"
			"\t\tsparseResidency2Samples: %d\n"
			"\t\tsparseResidency4Samples: %d\n"
			"\t\tsparseResidency8Samples: %d\n"
			"\t\tsparseResidency16Samples: %d\n"
			"\t\tsparseResidencyAliased: %d\n"
			"\t\tvariableMultisampleRate: %d\n"
			"\t\tinheritedQueries: %d\n",
			phys_dev_feat.robustBufferAccess,
			phys_dev_feat.fullDrawIndexUint32,
			phys_dev_feat.imageCubeArray,
			phys_dev_feat.independentBlend,
			phys_dev_feat.geometryShader,
			phys_dev_feat.tessellationShader,
			phys_dev_feat.sampleRateShading,
			phys_dev_feat.dualSrcBlend,
			phys_dev_feat.logicOp,
			phys_dev_feat.multiDrawIndirect,
			phys_dev_feat.drawIndirectFirstInstance,
			phys_dev_feat.depthClamp,
			phys_dev_feat.depthBiasClamp,
			phys_dev_feat.fillModeNonSolid,
			phys_dev_feat.depthBounds,
			phys_dev_feat.wideLines,
			phys_dev_feat.largePoints,
			phys_dev_feat.alphaToOne,
			phys_dev_feat.multiViewport,
			phys_dev_feat.samplerAnisotropy,
			phys_dev_feat.textureCompressionETC2,
			phys_dev_feat.textureCompressionASTC_LDR,
			phys_dev_feat.textureCompressionBC,
			phys_dev_feat.occlusionQueryPrecise,
			phys_dev_feat.pipelineStatisticsQuery,
			phys_dev_feat.vertexPipelineStoresAndAtomics,
			phys_dev_feat.fragmentStoresAndAtomics,
			phys_dev_feat.shaderTessellationAndGeometryPointSize,
			phys_dev_feat.shaderImageGatherExtended,
			phys_dev_feat.shaderStorageImageExtendedFormats,
			phys_dev_feat.shaderStorageImageMultisample,
			phys_dev_feat.shaderStorageImageReadWithoutFormat,
			phys_dev_feat.shaderStorageImageWriteWithoutFormat,
			phys_dev_feat.shaderUniformBufferArrayDynamicIndexing,
			phys_dev_feat.shaderSampledImageArrayDynamicIndexing,
			phys_dev_feat.shaderStorageBufferArrayDynamicIndexing,
			phys_dev_feat.shaderStorageImageArrayDynamicIndexing,
			phys_dev_feat.shaderClipDistance,
			phys_dev_feat.shaderCullDistance,
			phys_dev_feat.shaderFloat64,
			phys_dev_feat.shaderInt64,
			phys_dev_feat.shaderInt16,
			phys_dev_feat.shaderResourceResidency,
			phys_dev_feat.shaderResourceMinLod,
			phys_dev_feat.sparseBinding,
			phys_dev_feat.sparseResidencyBuffer,
			phys_dev_feat.sparseResidencyImage2D,
			phys_dev_feat.sparseResidencyImage3D,
			phys_dev_feat.sparseResidency2Samples,
			phys_dev_feat.sparseResidency4Samples,
			phys_dev_feat.sparseResidency8Samples,
			phys_dev_feat.sparseResidency16Samples,
			phys_dev_feat.sparseResidencyAliased,
			phys_dev_feat.variableMultisampleRate,
			phys_dev_feat.inheritedQueries);

		// print all physical device memory properties
		VkPhysicalDeviceMemoryProperties phys_devs_mem_props =
		{
			0,
		};

		vkGetPhysicalDeviceMemoryProperties(
			data->phys_devs[i],
			&phys_devs_mem_props);

		printf("\tphysical device memory types:\n");

		for (uint32_t k = 0; k < phys_devs_mem_props.memoryTypeCount; ++k)
		{
			VkMemoryPropertyFlags flags =
				phys_devs_mem_props.memoryTypes[k].propertyFlags;

			uint32_t id =
				phys_devs_mem_props.memoryTypes[k].heapIndex;

			printf(
				"\t\ttype #%u:\n\t\t\theap index: %u\n",
				k, id);

			if (flags != 0)
			{
				printf("\t\t\tflags (empty if unknown):\n");

				for (uint32_t m = 0; m < mem_types_len; ++m)
				{
					if ((vk_mem_types[m].flag & flags) != 0)
					{
						printf("\t\t\t\t%s\n", vk_mem_types[m].name);
					}
				}
			}
		}

		printf("\tphysical device memory heaps:\n");

		for (uint32_t k = 0; k < phys_devs_mem_props.memoryHeapCount; ++k)
		{
			VkDeviceSize size = 
				phys_devs_mem_props.memoryHeaps[k].size;

			VkMemoryHeapFlags flags =
				phys_devs_mem_props.memoryHeaps[k].flags;

			printf(
				"\t\theap #%u:\n\t\t\tsize: %lu\n",
				k, (size_t) size);

			if (flags != 0)
			{
				printf("\t\t\tflags (empty if unknown):\n");

				for (uint32_t m = 0; m < mem_heaps_len; ++m)
				{
					if ((vk_mem_heaps[m].flag & flags) != 0)
					{
						printf("\t\t\t\t%s\n", vk_mem_heaps[m].name);
					}
				}
			}
		}

		// get physical device queue family properties
		uint32_t phys_dev_queue_fams_len = 0;
		VkQueueFamilyProperties* phys_dev_queue_fams;

		vkGetPhysicalDeviceQueueFamilyProperties(
			data->phys_devs[i],
			&phys_dev_queue_fams_len,
			NULL);

		phys_dev_queue_fams =
			malloc(phys_dev_queue_fams_len * (sizeof (VkQueueFamilyProperties)));

		if (phys_dev_queue_fams == NULL)
		{
			fprintf(stderr, "could not allocate physical devices list\n");
			free(data->phys_devs);
			return;
		}

		vkGetPhysicalDeviceQueueFamilyProperties(
			data->phys_devs[i],
			&phys_dev_queue_fams_len,
			phys_dev_queue_fams);

		// Search for a suitable queue family.
		// More specifically, we want a graphics queue supporting presentation.
		// We don't support using separate graphics & presentation queues since:
		//  - hardware with graphics queues without presentation support doesn't
		//    currently exist in reality and will probably never exist at all
		//  - purposefully using two separate queues with hardware offering
		//    presentation support on multiple queues is useless here
		printf("\tqueue families:\n");

		for (uint32_t k = 0; k < phys_dev_queue_fams_len; ++k)
		{
			VkQueueFlags flags =
				phys_dev_queue_fams[k].queueFlags;

			uint32_t count =
				phys_dev_queue_fams[k].queueCount;

			printf(
				"\t\tqueue #%u:\n\t\t\tqueue count: %u\n",
				k,
				count);

			if (flags != 0)
			{
				printf("\t\t\tflags (empty if unknown):\n");

				for (uint32_t m = 0; m < queue_fams_len; ++m)
				{
					if ((vk_queue_fams[m].flag & flags) != 0)
					{
						printf("\t\t\t\t%s\n", vk_queue_fams[m].name);
					}
				}
			}

			VkBool32 support =
				globox_presentation_support_vulkan(
					data->globox,
					data->phys_devs[i],
					k,
					&globox_error);

			if (globox_error_get_code(&globox_error) != GLOBOX_ERROR_OK)
			{
				globox_error_log(data->globox, &globox_error);
				globox_clean(data->globox, &globox_error);
				free(data->phys_devs);
				return;
			}

			printf("\t\t\tpresentation support: %u\n", support);

			if ((found_device == false)
				&& (support == VK_TRUE)
				&& ((flags & VK_QUEUE_GRAPHICS_BIT) != 0))
			{
				found_device = true;
				data->phys_devs_index = i;
				data->selected_queue = k;
			}
		}

		free(phys_dev_queue_fams);
	}

	// print selected device and queue family indices
	if (found_device == false)
	{
		fprintf(stderr, "none of the available devices support presentation\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	printf("selected device #%u / queue family #%u\n",
		data->phys_devs_index,
		data->selected_queue);

	// get device extensions list
	VkExtensionProperties* dev_ext_props;
	uint32_t dev_ext_props_len;

	error =
		vkEnumerateDeviceExtensionProperties(
			data->phys_devs[data->phys_devs_index],
			NULL,
			&dev_ext_props_len,
			NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "couldn't count vulkan device extensions\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	dev_ext_props = malloc(dev_ext_props_len * (sizeof (VkExtensionProperties)));

	if (dev_ext_props == NULL)
	{
		fprintf(stderr, "couldn't allocate vulkan device extensions list\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	error =
		vkEnumerateDeviceExtensionProperties(
			data->phys_devs[data->phys_devs_index],
			NULL,
			&dev_ext_props_len,
			dev_ext_props);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "couldn't list vulkan device extensions\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	printf("available vulkan device extensions:\n");

	for (uint32_t i = 0; i < dev_ext_props_len; ++i)
	{
		printf(
			"\t%s (vulkan version: %u)\n",
			dev_ext_props[i].extensionName,
			dev_ext_props[i].specVersion);
	}

	// check needed device extensions
	size_t dev_ext_len = (sizeof (vk_dev_ext)) / (sizeof (struct vk_dev_ext));
	const char** dev_ext_found = malloc(dev_ext_len);
	uint32_t dev_ext_found_count = 0;

	if (dev_ext_found == NULL)
	{
		fprintf(stderr, "could not allocate found device extensions list\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	// check layers
	printf("using vulkan device extensions:\n");

	for (uint32_t i = 0; i < dev_ext_props_len; ++i)
	{
		uint32_t k = 0;

		while (k < dev_ext_len)
		{
			if ((vk_dev_ext[k].found == false)
				&& (strcmp(dev_ext_props[i].extensionName, vk_dev_ext[k].name) == 0))
			{
				// save as a layer to request
				dev_ext_found[dev_ext_found_count] = vk_dev_ext[k].name;
				printf("\t%s\n", dev_ext_found[dev_ext_found_count]);
				++dev_ext_found_count;

				// skip saved layers
				vk_dev_ext[k].found = true;
				++k;

				continue;
			}

			++k;
		}
	}

	free(dev_ext_props);

	if (dev_ext_found_count < dev_ext_len)
	{
		fprintf(stderr, "couldn't get all the required vulkan device extensions\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	// create device
	float queue_priorities[1] = { 1.0f }; // default priority for this example

	VkDeviceQueueCreateInfo queue_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueFamilyIndex = data->selected_queue,
		.queueCount = 1,
		.pQueuePriorities = queue_priorities,
	};

	VkDeviceCreateInfo device_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_create_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = dev_ext_found_count,
		.ppEnabledExtensionNames = dev_ext_found,
		.pEnabledFeatures = NULL, // we don't need any special feature
	};

	vkCreateDevice(
		data->phys_devs[data->phys_devs_index],
		&device_create_info,
		NULL,
		&(data->device));

	vkGetDeviceQueue(
		data->device,
		data->selected_queue,
		0,
		&(data->queue));

	// create semaphores
	VkSemaphoreCreateInfo semaphore_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
	};

	error =
		vkCreateSemaphore(
			data->device,
			&semaphore_create_info,
			NULL,
			&(data->semaphore_present));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "couldn't create present semaphore\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	error =
		vkCreateSemaphore(
			data->device,
			&semaphore_create_info,
			NULL,
			&(data->semaphore_render));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "couldn't create render semaphore\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	// create fence
	VkFenceCreateInfo fence_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};

	error =
		vkCreateFence(
			data->device,
			&fence_create_info,
			NULL,
			&(data->fence_frame));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "couldn't create frame fence\n");
		globox_clean(data->globox, &globox_error);
		free(data->phys_devs);
		return;
	}

	// setup validation layers debug callback
	// TODO

	// free resources
	free(dev_ext_found);
}

static void swapchain_free_vulkan(struct globox_render_data* data)
{
	vkDestroyRenderPass(
		data->device,
		data->render_pass,
		NULL);

	for (size_t i = 0; i < data->swapchain_images_len; ++i)
	{
		vkDestroyFramebuffer(
			data->device,
			data->framebuffers[i],
			NULL);

		vkDestroyImageView(
			data->device,
			data->swapchain_image_views[i],
			NULL);
	}

	free_check(data->swapchain_image_views);
	free_check(data->swapchain_images);
	free_check(data->surf_modes);
	free_check(data->surf_formats);

	vkDestroySwapchainKHR(
		data->device,
		data->swapchain,
		NULL);
}

static void swapchain_vulkan(struct globox_render_data* data)
{
	printf("creating vulkan swapchain\n");
	struct globox_error_info globox_error = {0};
	VkResult error = VK_ERROR_UNKNOWN;

	// get the vulkan surface from globox
	data->surf = globox_get_surface_vulkan(data->globox, &globox_error);

	if (globox_error_get_code(&globox_error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(data->globox, &globox_error);
		return;
	}

	// get surface formats
	error =
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			data->phys_devs[data->phys_devs_index],
			*(data->surf),
			&(data->surf_formats_len),
			NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not count surface formats\n");
		return;
	}

	data->surf_formats =
		malloc(data->surf_formats_len * (sizeof (VkSurfaceFormatKHR)));

	if (data->surf_formats == NULL)
	{
		fprintf(stderr, "could not allocate surface formats list\n");
		return;
	}

	error =
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			data->phys_devs[data->phys_devs_index],
			*(data->surf),
			&(data->surf_formats_len),
			data->surf_formats);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list surface formats\n");
		free(data->surf_formats);
		return;
	}

	// check vulkan surface formats
	uint32_t i = 0;

	while (i < data->surf_formats_len)
	{
		if ((data->surf_formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
			&& (data->surf_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
		{
			printf("found compatible surface format\n");
			data->surf_formats_index = i;
			break;
		}

		++i;
	}

	if (i == data->surf_formats_len)
	{
		fprintf(stderr, "could not find compatible surface format");
		free(data->surf_formats);
		return;
	}

	// get surface capabilities
	error =
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			data->phys_devs[data->phys_devs_index],
			*(data->surf),
			&(data->surf_caps));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not get surface capabilities\n");
		free(data->surf_formats);
		return;
	}

	// get surface present modes
	error =
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			data->phys_devs[data->phys_devs_index],
			*(data->surf),
			&(data->surf_modes_len),
			NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not count surface presentation modes\n");
		free(data->surf_formats);
		return;
	}

	data->surf_modes =
		malloc(data->surf_modes_len * (sizeof (VkPresentModeKHR)));

	if (data->surf_modes == NULL)
	{
		fprintf(stderr, "could not allocate surface presentation modes list\n");
		free(data->surf_formats);
		return;
	}

	error =
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			data->phys_devs[data->phys_devs_index],
			*(data->surf),
			&(data->surf_modes_len),
			data->surf_modes);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list surface presentation modes\n");
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	if (data->surf_modes_len == 0)
	{
		fprintf(stderr, "could not find any surface presentation mode\n");
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	printf("vulkan surface presentation modes:\n");

	size_t pres_modes_len =
		(sizeof (vk_pres_modes)) / (sizeof (struct vk_pres_modes));

	size_t surf_mode_order =
		pres_modes_len;

	for (uint32_t i = 0; i < data->surf_modes_len; ++i)
	{
		size_t k = 0;

		while (k < pres_modes_len)
		{
			if (data->surf_modes[i] == vk_pres_modes[k].mode)
			{
				// print presentation mode name
				printf("\t%s\n", vk_pres_modes[k].name);

				// save as presentation mode if higher priority than current one
				if (surf_mode_order > k)
				{
					data->surf_modes_index = i;
					surf_mode_order = k;
				}

				break;
			}

			++k;
		}
	}

	// print selected presentation mode
	const char* mode_name;

	if (surf_mode_order == pres_modes_len)
	{
		mode_name = "unknown";
		// in case there is absolutely nothing we know of, use whatever we can
		data->surf_modes_index = 0;
	}
	else
	{
		mode_name = vk_pres_modes[surf_mode_order].name;
	}

	printf("using vulkan surface presentation mode:\n\t%s\n", mode_name);

	// create swapchain
	uint32_t image_count = data->surf_caps.minImageCount + 1;

	if ((data->surf_caps.maxImageCount > 0)
		&& (image_count > data->surf_caps.maxImageCount))
	{
		image_count = data->surf_caps.maxImageCount;
	}

	data->extent = data->surf_caps.currentExtent;

	if ((data->extent.width == UINT32_MAX) && (data->extent.height == UINT32_MAX))
	{
		data->extent.width = data->width;
		data->extent.height = data->height;

		if (data->extent.width < data->surf_caps.minImageExtent.width)
		{
			data->extent.width = data->surf_caps.minImageExtent.width;
		}
		else if (data->extent.width > data->surf_caps.maxImageExtent.width)
		{
			data->extent.width = data->surf_caps.maxImageExtent.width;
		}

		if (data->extent.height < data->surf_caps.minImageExtent.height)
		{
			data->extent.height = data->surf_caps.minImageExtent.height;
		}
		else if (data->extent.height > data->surf_caps.maxImageExtent.height)
		{
			data->extent.height = data->surf_caps.maxImageExtent.height;
		}
	}

	data->format = data->surf_formats[data->surf_formats_index].format;
	data->color_space = data->surf_formats[data->surf_formats_index].colorSpace;

	VkSwapchainCreateInfoKHR swapchain_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = *(data->surf),
		.minImageCount = image_count,
		.imageFormat = data->format,
		.imageColorSpace = data->color_space,
		.imageExtent = data->extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

		// again we only support presentation from the graphics queue
		// so we can just always use the exclusive image sharing mode
		// (for which queue family index count & indices are ignored)
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,

		.preTransform = data->surf_caps.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = data->surf_modes[data->surf_modes_index],
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE,
	};

	error =
		vkCreateSwapchainKHR(
			data->device,
			&swapchain_create_info,
			NULL,
			&(data->swapchain));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create a swapchain\n");
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	// get swapchain images
	error =
		vkGetSwapchainImagesKHR(
			data->device,
			data->swapchain,
			&(data->swapchain_images_len),
			NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not count swapchain images\n");
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	data->swapchain_images =
		malloc(data->swapchain_images_len * (sizeof (VkImage)));

	if (data->swapchain_images == NULL)
	{
		fprintf(stderr, "could not allocate swapchain images list\n");
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	error =
		vkGetSwapchainImagesKHR(
			data->device,
			data->swapchain,
			&(data->swapchain_images_len),
			data->swapchain_images);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list swapchain images\n");
		free(data->swapchain_images);
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	// create image views
	data->swapchain_image_views =
		malloc(data->swapchain_images_len * (sizeof (VkImageView)));

	if (data->swapchain_image_views == NULL)
	{
		fprintf(stderr, "could not allocate swapchain image view list\n");
		free(data->swapchain_images);
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	VkImageViewCreateInfo image_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = VK_NULL_HANDLE,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = data->format,
		.components =
		{
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY,
		},
		.subresourceRange =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	for (size_t i = 0; i < data->swapchain_images_len; ++i)
	{
		image_create_info.image = data->swapchain_images[i];

		error =
			vkCreateImageView(
				data->device,
				&image_create_info,
				NULL,
				&(data->swapchain_image_views[i]));

		if (error != VK_SUCCESS)
		{
			fprintf(stderr, "could not list swapchain images\n");
			free(data->swapchain_image_views);
			free(data->swapchain_images);
			free(data->surf_modes);
			free(data->surf_formats);
			return;
		}
	}

	// create render pass
	VkAttachmentDescription color_attach =
	{
		.flags = 0,
		.format = data->format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};

	VkAttachmentReference color_attach_ref =
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	VkSubpassDescription subpass =
	{
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = NULL,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attach_ref,
		.pResolveAttachments = NULL,
		.pDepthStencilAttachment = NULL,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = NULL,
	};

	VkSubpassDependency dependency =
	{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = 0,
	};

	VkRenderPassCreateInfo render_pass_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.attachmentCount = 1,
		.pAttachments = &color_attach,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency,
	};

	error =
		vkCreateRenderPass(
			data->device,
			&render_pass_info,
			NULL,
			&(data->render_pass));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create render pass\n");
		free(data->swapchain_images);
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	// create framebuffers
	data->framebuffers =
		malloc(data->swapchain_images_len * (sizeof (VkFramebuffer)));

	if (data->framebuffers == NULL)
	{
		fprintf(stderr, "could not allocate framebuffers list\n");
		free(data->swapchain_images);
		free(data->surf_modes);
		free(data->surf_formats);
		return;
	}

	VkFramebufferCreateInfo framebuffer_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.renderPass = data->render_pass,
		.attachmentCount = 1,
		.pAttachments = NULL,
		.width = data->extent.width,
		.height = data->extent.height,
		.layers = 1,
	};

	for (size_t i = 0; i < data->swapchain_images_len; ++i)
	{
		framebuffer_create_info.pAttachments =
			&(data->swapchain_image_views[i]);

		error =
			vkCreateFramebuffer(
				data->device,
				&framebuffer_create_info,
				NULL,
				&(data->framebuffers[i]));

		if (error != VK_SUCCESS)
		{
			if (data->framebuffers == NULL)
			{
				fprintf(stderr, "could not allocate framebuffers list\n");
				free(data->framebuffers);
				free(data->swapchain_images);
				free(data->surf_modes);
				free(data->surf_formats);
				return;
			}
		}
	}
}

static void pipeline_free_vulkan(struct globox_render_data* data)
{
	vkFreeMemory(
		data->device,
		data->vertex_buf_mem,
		NULL);

	vkDestroyBuffer(
		data->device,
		data->vertex_buf,
		NULL);

	vkDestroyCommandPool(
		data->device,
		data->cmd_pool,
		NULL);

	vkDestroyPipeline(
		data->device,
		data->pipeline,
		NULL);

	vkDestroyPipelineLayout(
		data->device,
		data->pipeline_layout,
		NULL);
}

static void pipeline_vulkan(struct globox_render_data* data)
{
	VkResult error = VK_ERROR_UNKNOWN;

	// configure dynamic state
	VkDynamicState dynamic_states[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};

	VkPipelineDynamicStateCreateInfo dynamic_states_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.dynamicStateCount =
			(uint32_t) ((sizeof (dynamic_states)) / (sizeof (VkDynamicState))),
		.pDynamicStates = dynamic_states,
	};

	// create pipeline stages
	// vertex input state
	VkVertexInputBindingDescription vertex_binding_desc =
	{
		.binding = 0,
		.stride = 2 * (sizeof (float)),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
	};

	VkVertexInputAttributeDescription vertex_attr_desc =
	{
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = 0,
	};

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertex_binding_desc,
		.vertexAttributeDescriptionCount = 1,
		.pVertexAttributeDescriptions = &vertex_attr_desc,
	};

	// pipeline assembly state
	VkPipelineInputAssemblyStateCreateInfo input_asm_state_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
		.primitiveRestartEnable = VK_FALSE,
	};

	// viewport state
	VkViewport viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = data->extent.width,
		.height = data->extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	VkRect2D scissor =
	{
		.offset = {0, 0},
		.extent = data->extent,
	};

	VkPipelineViewportStateCreateInfo viewport_state_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};

	// rasterizer state
	VkPipelineRasterizationStateCreateInfo rasterizer_state_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f,
	};

	// TODO alpha
	// multisampling state
	VkPipelineMultisampleStateCreateInfo multisampling_state_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
	};

	// TODO alpha
	// color blend attachment state
	VkPipelineColorBlendAttachmentState color_blend_attach_state =
	{
		.blendEnable = VK_FALSE, // VK_TRUE
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // VK_BLEND_FACTOR_SRC_ALPHA
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT
			| VK_COLOR_COMPONENT_B_BIT
			| VK_COLOR_COMPONENT_A_BIT,
	};

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attach_state,
		.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
	};

	// build the pipeline
	VkPipelineLayoutCreateInfo pipeline_layout_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.setLayoutCount = 0,
		.pSetLayouts = NULL,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL,
	};

	error =
		vkCreatePipelineLayout(
			data->device,
			&pipeline_layout_create_info,
			NULL,
			&(data->pipeline_layout));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create vulkan pipeline layout\n");
		return;
	}

	VkGraphicsPipelineCreateInfo pipeline_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stageCount = 2,
		.pStages = data->shader_stages,
		.pVertexInputState = &vertex_input_state_create_info,
		.pInputAssemblyState = &input_asm_state_create_info,
		.pTessellationState = NULL,
		.pViewportState = &viewport_state_create_info,
		.pRasterizationState = &rasterizer_state_create_info,
		.pMultisampleState = &multisampling_state_create_info,
		.pDepthStencilState = NULL,
		.pColorBlendState = &color_blend_state_create_info,
		.pDynamicState = &dynamic_states_create_info,
		.layout = data->pipeline_layout,
		.renderPass = data->render_pass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};

	error =
		vkCreateGraphicsPipelines(
			data->device,
			VK_NULL_HANDLE,
			1,
			&pipeline_create_info,
			NULL,
			&(data->pipeline));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create vulkan graphics pipeline\n");
		return;
	}

	// create command pool
	VkCommandPoolCreateInfo cmd_pool_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = data->selected_queue,
	};

	error =
		vkCreateCommandPool(
			data->device,
			&cmd_pool_create_info,
			NULL,
			&(data->cmd_pool));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create vulkan command pool\n");
		return;
	}

	// create command buffer
	VkCommandBufferAllocateInfo cmd_buf_alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = data->cmd_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	error =
		vkAllocateCommandBuffers(
			data->device,
			&cmd_buf_alloc_info,
			&(data->cmd_buf));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create vulkan command buffer\n");
		return;
	}

	// create vertex buffer
	VkBufferCreateInfo buffer_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.size = 4 * 2 * (sizeof (float)), // four vertices
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
	};

	error =
		vkCreateBuffer(
			data->device,
			&buffer_create_info,
			NULL,
			&(data->vertex_buf));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create vulkan buffer\n");
		return;
	}

	// get buffer memory requirements
	VkMemoryRequirements mem_req =
	{
		0,
	};

	vkGetBufferMemoryRequirements(
		data->device,
		data->vertex_buf,
		&mem_req);

	// find compatible memory type
	VkPhysicalDeviceMemoryProperties mem_props =
	{
		0,
	};

	vkGetPhysicalDeviceMemoryProperties(
		data->phys_devs[data->phys_devs_index],
		&mem_props);

	uint32_t mem_type = 0;
	uint32_t mem_type_bit = 1;
	VkMemoryPropertyFlags buffer_flags =
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	while (mem_type < mem_props.memoryTypeCount)
	{
		VkMemoryPropertyFlags mem_flags =
			mem_props.memoryTypes[mem_type].propertyFlags;

		if (((mem_req.memoryTypeBits & mem_type_bit) == mem_type_bit)
			&& ((mem_flags & buffer_flags) == buffer_flags))
		{
			break;
		}

		mem_type_bit <<= 1;
		mem_type += 1;
	}

	// allocate memory
	VkMemoryAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = mem_req.size,
		.memoryTypeIndex = mem_type,
	};

	error =
		vkAllocateMemory(
			data->device,
			&alloc_info,
			NULL,
			&(data->vertex_buf_mem));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not allocate vulkan memory\n");
		return;
	}

	error =
		vkBindBufferMemory(
			data->device,
			data->vertex_buf,
			data->vertex_buf_mem,
			0);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not bind vulkan memory to vulkan buffer\n");
		return;
	}
}

static void compile_shaders(struct globox_render_data* data)
{
	VkResult error = VK_ERROR_UNKNOWN;

	// create vertex shader module
	VkShaderModuleCreateInfo info_vert =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.codeSize = square_vert_vk1_size,
		.pCode = (uint32_t*) square_vert_vk1,
	};

	error =
		vkCreateShaderModule(
			data->device,
			&info_vert,
			NULL,
			&(data->module_vert));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create vertex shader module\n");
		return;
	}

	// create fragment shader module
	VkShaderModuleCreateInfo info_flag =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.codeSize = square_frag_vk1_size,
		.pCode = (uint32_t*) square_frag_vk1,
	};

	error =
		vkCreateShaderModule(
			data->device,
			&info_flag,
			NULL,
			&(data->module_frag));

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not create fragment shader module\n");
		return;
	}

	// create shader pipeline stages
	VkPipelineShaderStageCreateInfo vert_shader_stage_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = data->module_vert,
		.pName = "main",
		.pSpecializationInfo = NULL,
	};

	data->shader_stages[0] = vert_shader_stage_create_info;

	VkPipelineShaderStageCreateInfo frag_shader_stage_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = data->module_frag,
		.pName = "main",
		.pSpecializationInfo = NULL,
	};

	data->shader_stages[1] = frag_shader_stage_create_info;
}

static void render_vulkan(struct globox_render_data* data)
{
	VkResult error = VK_ERROR_UNKNOWN;
	uint32_t image_index;

	// start
	vkWaitForFences(
		data->device,
		1,
		&(data->fence_frame),
		VK_TRUE,
		UINT64_MAX);

	vkAcquireNextImageKHR(
		data->device,
		data->swapchain,
		UINT64_MAX,
		data->semaphore_render,
		VK_NULL_HANDLE,
		&image_index);

	vkResetCommandBuffer(
		data->cmd_buf,
		0);

	VkCommandBufferBeginInfo cmd_buf_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL,
	};

	error =
		vkBeginCommandBuffer(
			data->cmd_buf,
			&cmd_buf_begin_info);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not begin vulkan command buffer\n");
		return;
	}

	// clear color
	VkClearValue clear_color =
	{
		.color.float32 =
		{
			164.0f / 255.0f,
			30.0f / 255.0f,
			34.0f / 255.0f,
			0.0f,
		},
	};

	VkRenderPassBeginInfo render_pass_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = data->render_pass,
		.framebuffer = data->framebuffers[image_index],
		.renderArea =
		{
			.offset = {0, 0},
			.extent = data->extent,
		},
		.clearValueCount = 1,
		.pClearValues = &clear_color,
	};

	vkCmdBeginRenderPass(
		data->cmd_buf,
		&render_pass_begin_info,
		VK_SUBPASS_CONTENTS_INLINE);

	// pipeline
	vkCmdBindPipeline(
		data->cmd_buf,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		data->pipeline);

	// vertex buffer
	float vertices[] =
	{
		-100.0f / data->width, +100.0f / data->height,
		-100.0f / data->width, -100.0f / data->height,
		+100.0f / data->width, -100.0f / data->height,
		+100.0f / data->width, +100.0f / data->height,
	};

	void* vertex_data;

	vkMapMemory(
		data->device,
		data->vertex_buf_mem,
		0,
		sizeof (vertices),
		0,
		&vertex_data);

	memcpy(vertex_data, vertices, sizeof (vertices));

	vkUnmapMemory(
		data->device,
		data->vertex_buf_mem);

	VkDeviceSize vertex_offsets = 0;

	vkCmdBindVertexBuffers(
		data->cmd_buf,
		0,
		1,
		&(data->vertex_buf),
		&vertex_offsets);

	// viewport
	VkViewport viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = data->extent.width,
		.height = data->extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	vkCmdSetViewport(
		data->cmd_buf,
		0,
		1,
		&viewport);

	// scissor
	VkRect2D scissor =
	{
		.offset = {0, 0},
		.extent = data->extent,
	};

	vkCmdSetScissor(
		data->cmd_buf,
		0,
		1,
		&scissor);

	// draw
	vkCmdDraw(
		data->cmd_buf,
		4,
		1,
		0,
		0);

	// end
	vkCmdEndRenderPass(
		data->cmd_buf);

	error =
		vkEndCommandBuffer(
			data->cmd_buf);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not end vulkan command buffer\n");
		return;
	}

	vkResetFences(
		data->device,
		1,
		&(data->fence_frame));

	// submit the command buffer
	VkPipelineStageFlags dest_stage_mask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(data->semaphore_render),
		.pWaitDstStageMask = &dest_stage_mask,
		.commandBufferCount = 1,
		.pCommandBuffers = &(data->cmd_buf),
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &(data->semaphore_present),
	};

	error =
		vkQueueSubmit(
			data->queue,
			1,
			&submit_info,
			data->fence_frame);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not submit vulkan queue\n");
		return;
	}

	// present
	VkPresentInfoKHR present_info =
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(data->semaphore_present),
		.swapchainCount = 1,
		.pSwapchains = &(data->swapchain),
		.pImageIndices = &image_index,
		.pResults = NULL,
	};

	error =
		vkQueuePresentKHR(
			data->queue,
			&present_info);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not present vulkan queue\n");
		return;
	}
}

static void event_callback(void* data, void* event)
{
	struct globox* globox = data;
	struct globox_error_info error = {0};

	// print some debug info on internal events
	enum globox_event abstract =
		globox_handle_events(globox, event, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	switch (abstract)
	{
		case GLOBOX_EVENT_INVALID:
		{
			// shouldn't be possible since we handle the error
			fprintf(stderr, "received invalid event\n");
			break;
		}
		case GLOBOX_EVENT_UNKNOWN:
		{
			fprintf(stderr, "received unknown event\n");
			break;
		}
		case GLOBOX_EVENT_RESTORED:
		{
			fprintf(stderr, "received `restored` event\n");
			break;
		}
		case GLOBOX_EVENT_MINIMIZED:
		{
			fprintf(stderr, "received `minimized` event\n");
			break;
		}
		case GLOBOX_EVENT_MAXIMIZED:
		{
			fprintf(stderr, "received `maximized` event\n");
			break;
		}
		case GLOBOX_EVENT_FULLSCREEN:
		{
			fprintf(stderr, "received `fullscreen` event\n");
			break;
		}
		case GLOBOX_EVENT_CLOSED:
		{
			fprintf(stderr, "received `closed` event\n");
			break;
		}
		case GLOBOX_EVENT_MOVED_RESIZED:
		{
			fprintf(stderr, "received `moved` event\n");
			break;
		}
		case GLOBOX_EVENT_DAMAGED:
		{
			struct globox_rect rect = globox_get_expose(globox, &error);

			if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
			{
				globox_error_log(globox, &error);
				break;
			}

			fprintf(
				stderr,
				"received `content damaged` event:\n"
				"\tx: %d px\n"
				"\ty: %d px\n"
				"\twidth: %d px\n"
				"\theight: %d px\n",
				rect.x,
				rect.y,
				rect.width,
				rect.height);

			break;
		}
	}
}

static void render_callback(void* data)
{
	// render our trademark square as a simple example, updating the whole
	// buffer each time without taking surface damage events into account
	struct globox_render_data* render_data = data;
	struct globox* globox = render_data->globox;
	struct globox_error_info error = {0};

	if (render_data->shaders == true)
	{
		compile_shaders(render_data);
		render_data->shaders = false;
	}

	int width = globox_get_width(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	int height = globox_get_height(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}

	if ((width == 0) || (height == 0))
	{
		// skip rendering if the window area is 0
		return;
	}

	// re-create the swapchain when the window size changes
	// TODO also handle suboptimal swapchains
	if ((width != render_data->width) || (height != render_data->height))
	{
		render_data->width = width;
		render_data->height = height;
		vkDeviceWaitIdle(render_data->device);
		swapchain_free_vulkan(render_data);
		swapchain_vulkan(render_data);
		pipeline_free_vulkan(render_data);
		pipeline_vulkan(render_data);
	}

	// render with vulkan
	render_vulkan(render_data);

	globox_update_content(globox, NULL, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return;
	}
}

static void config_callback(struct globox_config_reply* replies, size_t count, void* data)
{
	fprintf(stderr, "window configured succesfully, printing information:\n");

	struct globox* context = data;
	const char* message = NULL;
	size_t feature;

	for (size_t i = 0; i < count; ++i)
	{
		feature = replies[i].feature;

		if (feature < count)
		{
			if (replies[i].error.code == GLOBOX_ERROR_OK)
			{
				message = "success";
			}
			else
			{
				message = globox_error_get_msg(context, &replies[i].error);
			}

			fprintf(stderr, "\t%s: %s\n", feature_names[feature], message);
		}
	}
}

int main(int argc, char** argv)
{
	struct globox_error_info error = {0};
	struct globox_error_info error_early = {0};
	printf("starting the simple globox example\n");

	// prepare function pointers
	struct globox_config_backend config = {0};

#ifdef GLOBOX_EXAMPLE_X11
	globox_prepare_init_x11_vulkan(&config, &error_early);
#endif

	// set function pointers and perform basic init
	struct globox* globox = globox_init(&config, &error);

	// Unless the context allocation failed it is always possible to access
	// error messages (even when the context initialization failed) so we can
	// always handle the backend initialization error first.

	// context allocation failed
	if (globox == NULL)
	{
		fprintf(stderr, "could not allocate the main globox context\n");
		return 1;
	}

	// Backend initialization failed. Since it happens before globox
	// initialization and errors are accessible even if it fails, we can handle
	// the errors in the right order regardless.
	if (globox_error_get_code(&error_early) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_early);
		globox_clean(globox, &error);
		return 1;
	}

	// The globox initialization had failed, make it known now if the backend
	// initialization that happened before went fine.
	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// initialize vulkan
	struct globox_render_data render_data =
	{
		.globox = globox,
		.width = 0,
		.height = 0,
		.shaders = true,
		.swapchain_images_len = 0,
	};

	init_vulkan(&render_data);

	// get available features
	struct globox_config_features* feature_list =
		globox_init_features(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// initialize features when creating the window
	struct globox_feature_state state =
	{
		.state = GLOBOX_STATE_REGULAR,
	};

	struct globox_feature_title title =
	{
		.title = "globox",
	};

	struct globox_feature_icon icon =
	{
		// acceptable implementation-defined behavior
		// since it's also the implementation that
		// allows us to bundle resources like so
		.pixmap = (uint32_t*) iconpix,
		.len = 2 + (16 * 16) + 2 + (32 * 32) + 2 + (64 * 64),
	};

	struct globox_feature_size size =
	{
		.width = 500,
		.height = 500,
	};

	struct globox_feature_pos pos =
	{
		.x = 250,
		.y = 250,
	};

	struct globox_feature_frame frame =
	{
		.frame = true,
	};

	struct globox_feature_background background =
	{
		.background = GLOBOX_BACKGROUND_OPAQUE,
	};

	struct globox_feature_vsync vsync =
	{
		.vsync = true,
	};

	// configure the feature and print a list
	printf("received a list of available features:\n");

	struct globox_config_request configs[GLOBOX_FEATURE_COUNT] = {0};
	size_t feature_added = 0;
	size_t i = 0;

	while (i < feature_list->count)
	{
		enum globox_feature feature_id = feature_list->list[i];
		printf("\t%s\n", feature_names[feature_id]);
		++i;

		switch (feature_id)
		{
			case GLOBOX_FEATURE_STATE:
			{
				configs[feature_added].config = &state;
				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				configs[feature_added].config = &title;
				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				configs[feature_added].config = &icon;
				break;
			}
			case GLOBOX_FEATURE_SIZE:
			{
				configs[feature_added].config = &size;
				break;
			}
			case GLOBOX_FEATURE_POS:
			{
				configs[feature_added].config = &pos;
				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				configs[feature_added].config = &frame;
				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				configs[feature_added].config = &background;
				break;
			}
			case GLOBOX_FEATURE_VSYNC:
			{
				configs[feature_added].config = &vsync;
				break;
			}
			default:
			{
				continue;
			}
		}

		configs[feature_added].feature = feature_id;
		++feature_added;
	}

	free(feature_list->list);
	free(feature_list);

	// register an event handler to track the window's state
	struct globox_config_events events =
	{
		.data = globox,
		.handler = event_callback,
	};

	struct globox_error_info error_events = {0};
	globox_init_events(globox, &events, &error_events);

	if (globox_error_get_code(&error_events) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_events);
		globox_clean(globox, &error);
		return 1;
	}

	// register a render callback
	struct globox_config_render render =
	{
		.data = &render_data,
		.callback = render_callback,
	};

	struct globox_error_info error_render = {0};
	globox_init_render(globox, &render, &error_render);

	if (globox_error_get_code(&error_render) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_render);
		globox_clean(globox, &error);
		return 1;
	}

	// create the window
	globox_window_create(globox, configs, feature_added, config_callback, globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// configure vulkan
	config_vulkan(&render_data);

	// display the window
	globox_window_start(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"this is a message from the main thread\n"
		"the window should now be visible\n"
		"we can keep computing here\n");

	// wait for the window to be closed
	globox_window_block(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// handle event thread errors
	if (globox_error_get_code(&error_events) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_events);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// handle render thread errors
	if (globox_error_get_code(&error_render) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error_render);
		globox_window_destroy(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	// free resources correctly
	// vulkan cleanup
	vkDeviceWaitIdle( render_data.device);
	swapchain_free_vulkan(&render_data);

	// globox cleanup
	globox_window_destroy(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		globox_clean(globox, &error);
		return 1;
	}

	globox_clean(globox, &error);

	if (globox_error_get_code(&error) != GLOBOX_ERROR_OK)
	{
		globox_error_log(globox, &error);
		return 1;
	}

	// vulkan cleanup
	vkDeviceWaitIdle(
		render_data.device);

	pipeline_free_vulkan(
		&render_data);

	free_check(render_data.framebuffers);
	free_check(render_data.phys_devs);

	vkDestroyShaderModule(
		render_data.device,
		render_data.module_vert,
		NULL);

	vkDestroyShaderModule(
		render_data.device,
		render_data.module_frag,
		NULL);

	vkDeviceWaitIdle(
		render_data.device);

	vkDestroyFence(
		render_data.device,
		render_data.fence_frame,
		NULL);

	vkDestroySemaphore(
		render_data.device,
		render_data.semaphore_present,
		NULL);

	vkDestroySemaphore(
		render_data.device,
		render_data.semaphore_render,
		NULL);

	vkDestroyDevice(
		render_data.device,
		NULL);

	vkDestroyInstance(
		render_data.instance,
		NULL);

	return 0;
}
