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

#define LAYERS_COUNT 2

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

	VkInstance instance;
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
	printf("\navailable vulkan instance extensions:\n");

	for (uint32_t i = 0; i < inst_ext_count; ++i)
	{
		printf(
			" - %s version %u\n",
			inst_ext_props[i].extensionName,
			inst_ext_props[i].specVersion);
	}

	// get layers list
	uint32_t layer_props_len = 0;
	VkLayerProperties* layer_props = NULL;

	error = vkEnumerateInstanceLayerProperties(&layer_props_len, NULL);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list instance layer properties\n");
		return;
	}

	layer_props = malloc(layer_props_len * (sizeof (VkLayerProperties)));

	if (layer_props == NULL)
	{
		fprintf(stderr, "could not allocate instance layer properties list\n");
		return;
	}

	error = vkEnumerateInstanceLayerProperties(&layer_props_len, layer_props);

	if (error != VK_SUCCESS)
	{
		fprintf(stderr, "could not list instance layer properties\n");
		return;
	}

	// print instance layer properties
	printf("\navailable vulkan instance layers:\n");

	for (uint32_t i = 0; i < layer_props_len; ++i)
	{
		printf(
			" - %s version %u\n",
			layer_props[i].layerName,
			layer_props[i].specVersion);
	}

	// check required layers
	uint32_t layers_needed_count = LAYERS_COUNT;
	const char* layers_needed[LAYERS_COUNT] =
	{
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_LUNARG_api_dump",
	};

	bool layers_found[LAYERS_COUNT] =
	{
		0,
	};

	// layers to request
	uint32_t layers_len = 0;
	const char* layers[LAYERS_COUNT] =
	{
		0,
	};

	// check layers
	printf("\nusing vulkan instance layers:\n");

	for (uint32_t i = 0; i < layer_props_len; ++i)
	{
		uint32_t k = 0;

		while (k < layers_needed_count)
		{
			if ((layers_found[k] == false)
				&& (strcmp(layer_props[i].layerName, layers_needed[k]) == 0))
			{
				// save as a layer to request
				layers[layers_len] = layers_needed[k];
				printf(" - %s\n", layers[layers_len]);
				++layers_len;
				// skip saved layers
				layers_found[k] = true;
				++k;

				continue;
			}

			++k;
		}
	}

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
		.enabledLayerCount = layers_len,
		.ppEnabledLayerNames = layers,
		.enabledExtensionCount = ext_globox_len,
		.ppEnabledExtensionNames = ext_globox,
	};

	error = vkCreateInstance(&instance_info, NULL, &(data->instance));

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
	// TODO
}

static void clean_vulkan(struct globox_render_data* data)
{
	// TODO
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
				"received `content damaged` event\n"
				" - x: %d px\n"
				" - y: %d px\n"
				" - width: %d px\n"
				" - height: %d px\n",
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
	fprintf(stderr, "\nwindow configured succesfully, printing information:\n");

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

			fprintf(stderr, " - %s: %s\n", feature_names[feature], message);
		}
	}

	fprintf(stderr, "\n");
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
	printf("\nreceived a list of available features:\n");

	struct globox_config_request configs[GLOBOX_FEATURE_COUNT] = {0};
	size_t feature_added = 0;
	size_t i = 0;

	while (i < feature_list->count)
	{
		enum globox_feature feature_id = feature_list->list[i];
		printf(" - %s\n", feature_names[feature_id]);
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
		"\nThis is a message from the main thread.\n"
		"The window should now be visible.\n"
		"We can keep computing here.\n");

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
	clean_vulkan(&render_data);

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

	return 0;
}
