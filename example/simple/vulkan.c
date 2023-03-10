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

struct globox_render_data
{
	// globox info
	struct globox* globox;
	struct globox_config_vulkan config;

	int width;
	int height;
	bool shaders;

	// vulkan info
	VkDevice device;
	VkShaderModule module_vert;
	VkShaderModule module_frag;
	VkSemaphore semaphore_present;
	VkSemaphore semaphore_render;

	VkInstance instance;
	VkQueue queue;
};

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
	uint32_t phys_devs_len;
	VkPhysicalDevice* phys_devs;

	error =
		vkEnumeratePhysicalDevices(
			data->instance,
			&phys_devs_len,
			NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not count physical devices\n");
		return;
	}

	phys_devs = malloc(phys_devs_len * (sizeof (VkPhysicalDevice)));

	if (phys_devs == NULL)
	{
		fprintf(stderr, "could not allocate physical devices list\n");
		return;
	}

	error =
		vkEnumeratePhysicalDevices(
			data->instance,
			&phys_devs_len,
			phys_devs);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list physical devices\n");
		free(phys_devs);
		return;
	}

	// select physical device
	uint32_t selected_device = 0;
	uint32_t selected_queue = 0;
	bool found_device = false;

	size_t mem_types_len =
		(sizeof (vk_mem_types)) / (sizeof (struct vk_mem_types));

	size_t mem_heaps_len =
		(sizeof (vk_mem_heaps)) / (sizeof (struct vk_mem_heaps));

	size_t queue_fams_len =
		(sizeof (vk_queue_fams)) / (sizeof (struct vk_queue_fams));

	for (uint32_t i = 0; i < phys_devs_len; ++i)
	{
		// get physical device properties
		VkPhysicalDeviceProperties phys_dev_props =
		{
			0,
		};

		vkGetPhysicalDeviceProperties(
			phys_devs[i],
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
			phys_devs[i],
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
			phys_devs[i],
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
			phys_devs[i],
			&phys_dev_queue_fams_len,
			NULL);

		phys_dev_queue_fams =
			malloc(phys_dev_queue_fams_len * (sizeof (VkQueueFamilyProperties)));

		if (phys_dev_queue_fams == NULL)
		{
			fprintf(stderr, "could not allocate physical devices list\n");
			free(phys_devs);
			return;
		}

		vkGetPhysicalDeviceQueueFamilyProperties(
			phys_devs[i],
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
					phys_devs[i],
					k,
					&globox_error);

			if (globox_error_get_code(&globox_error) != GLOBOX_ERROR_OK)
			{
				globox_error_log(data->globox, &globox_error);
				globox_clean(data->globox, &globox_error);
				free(phys_devs);
				return;
			}

			printf("\t\t\tpresentation support: %u\n", support);

			if ((found_device == false)
				&& (support == VK_TRUE)
				&& ((flags & VK_QUEUE_GRAPHICS_BIT) != 0))
			{
				found_device = true;
				selected_device = i;
				selected_queue = k;
			}
		}

		free(phys_dev_queue_fams);
	}

	// print selected device and queue family indices
	if (found_device == false)
	{
		fprintf(stderr, "none of the available devices support presentation\n");
		globox_clean(data->globox, &globox_error);
		free(phys_devs);
		return;
	}

	printf("selected device #%u / queue family #%u\n",
		selected_device,
		selected_queue);

	// get device extensions list
	VkExtensionProperties* dev_ext_props;
	uint32_t dev_ext_props_len;

	error =
		vkEnumerateDeviceExtensionProperties(
			phys_devs[selected_device],
			NULL,
			&dev_ext_props_len,
			NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "couldn't count vulkan device extensions\n");
		globox_clean(data->globox, &globox_error);
		free(phys_devs);
		return;
	}

	dev_ext_props = malloc(dev_ext_props_len * (sizeof (VkExtensionProperties)));

	if (dev_ext_props == NULL)
	{
		fprintf(stderr, "couldn't allocate vulkan device extensions list\n");
		globox_clean(data->globox, &globox_error);
		free(phys_devs);
		return;
	}

	error =
		vkEnumerateDeviceExtensionProperties(
			phys_devs[selected_device],
			NULL,
			&dev_ext_props_len,
			dev_ext_props);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "couldn't list vulkan device extensions\n");
		globox_clean(data->globox, &globox_error);
		free(phys_devs);
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
		free(phys_devs);
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
		free(phys_devs);
		return;
	}

	// create device
	float queue_priorities[1] = { 1.0f }; // default priority for this example

	VkDeviceQueueCreateInfo queue_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueFamilyIndex = selected_queue,
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
		phys_devs[selected_device],
		&device_create_info,
		NULL,
		&(data->device));

	vkGetDeviceQueue(
		data->device,
		selected_queue,
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
		free(phys_devs);
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
		free(phys_devs);
		return;
	}

	// TODO setup validation layers debug callback

	// free resources
	free(dev_ext_found);
	free(phys_devs);
}

static void clean_vulkan(struct globox_render_data* data)
{
	vkDestroyShaderModule(
		data->device,
		data->module_vert,
		NULL);

	vkDestroyShaderModule(
		data->device,
		data->module_frag,
		NULL);

	vkDeviceWaitIdle(data->device);

	vkDestroySemaphore(
		data->device,
		data->semaphore_present,
		NULL);

	vkDestroySemaphore(
		data->device,
		data->semaphore_render,
		NULL);

	vkDestroyDevice(
		data->device,
		NULL);

	vkDestroyInstance(
		data->instance,
		NULL);
}

static void compile_shaders(
	VkDevice* device,
	VkShaderModule* module_vert,
	VkShaderModule* module_frag)
{
	VkShaderModuleCreateInfo info_vert =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.codeSize = square_vert_vk1_size,
		.pCode = (uint32_t*) &square_vert_vk1,
	};

	VkResult error_vk =
		vkCreateShaderModule(
			*device,
			&info_vert,
			NULL,
			module_vert);

	if (error_vk != VK_SUCCESS)
	{
		return;
	}

	VkShaderModuleCreateInfo info_flag =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.codeSize = square_frag_vk1_size,
		.pCode = (uint32_t*) &square_frag_vk1,
	};

	error_vk =
		vkCreateShaderModule(
			*device,
			&info_flag,
			NULL,
			module_frag);

	if (error_vk != VK_SUCCESS)
	{
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

	if (render_data->shaders == true)
	{
		compile_shaders(
			&(render_data->device),
			&(render_data->module_vert),
			&(render_data->module_frag));

		render_data->shaders = false;
	}

	// render with vulkan
	// TODO

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

	clean_vulkan(&render_data);

	return 0;
}