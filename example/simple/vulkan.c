#include "globuf.h"
#include "vulkan_helpers.h"

#if !defined(GLOBUF_SHARED)
#if defined(GLOBUF_EXAMPLE_X11)
#include "globuf_x11_vulkan.h"
#elif defined(GLOBUF_EXAMPLE_APPKIT)
#include "globuf_appkit_vulkan.h"
#elif defined(GLOBUF_EXAMPLE_WIN)
#include "globuf_win_vulkan.h"
#elif defined(GLOBUF_EXAMPLE_WAYLAND)
#include "globuf_wayland_vulkan.h"
#endif
#endif

#ifdef GLOBUF_EXAMPLE_APPKIT
#define main real_main
#endif

#if defined(GLOBUF_SHARED)
#if !defined(GLOBUF_EXAMPLE_WIN)
#include <dlfcn.h>
#else
#include <libloaderapi.h>
#endif
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

extern uint8_t iconpix[];
extern int iconpix_size;

char* feature_names[GLOBUF_FEATURE_COUNT] =
{
	[GLOBUF_FEATURE_INTERACTION] = "interaction",
	[GLOBUF_FEATURE_STATE] = "state",
	[GLOBUF_FEATURE_TITLE] = "title",
	[GLOBUF_FEATURE_ICON] = "icon",
	[GLOBUF_FEATURE_SIZE] = "size",
	[GLOBUF_FEATURE_POS] = "pos",
	[GLOBUF_FEATURE_FRAME] = "frame",
	[GLOBUF_FEATURE_BACKGROUND] = "background",
	[GLOBUF_FEATURE_VSYNC] = "vsync",
};

static void event_callback(void* data, void* event)
{
	struct globuf* globuf = data;
	struct globuf_error_info error = {0};

	// print some debug info on internal events
	enum globuf_event abstract =
		globuf_handle_events(globuf, event, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	switch (abstract)
	{
		case GLOBUF_EVENT_INVALID:
		{
			// shouldn't be possible since we handle the error
			fprintf(stderr, "received invalid event\n");
			break;
		}
		case GLOBUF_EVENT_UNKNOWN:
		{
#ifdef GLOBUF_EXAMPLE_LOG_ALL
			fprintf(stderr, "received unknown event\n");
#endif
			break;
		}
		case GLOBUF_EVENT_RESTORED:
		{
			fprintf(stderr, "received `restored` event\n");
			break;
		}
		case GLOBUF_EVENT_MINIMIZED:
		{
			fprintf(stderr, "received `minimized` event\n");
			break;
		}
		case GLOBUF_EVENT_MAXIMIZED:
		{
			fprintf(stderr, "received `maximized` event\n");
			break;
		}
		case GLOBUF_EVENT_FULLSCREEN:
		{
			fprintf(stderr, "received `fullscreen` event\n");
			break;
		}
		case GLOBUF_EVENT_CLOSED:
		{
			fprintf(stderr, "received `closed` event\n");
			break;
		}
		case GLOBUF_EVENT_MOVED_RESIZED:
		{
			fprintf(stderr, "received `moved` event\n");
			break;
		}
		case GLOBUF_EVENT_DAMAGED:
		{
#ifdef GLOBUF_EXAMPLE_LOG_ALL
			struct globuf_rect rect = globuf_get_expose(globuf, &error);

			if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
			{
				globuf_error_log(globuf, &error);
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
#endif

			break;
		}
	}
}

static void render_callback(void* data)
{
	// render our trademark square as a simple example, updating the whole
	// buffer each time without taking surface damage events into account
	struct globuf_render_data* render_data = data;
	struct globuf* globuf = render_data->globuf;
	struct globuf_error_info error = {0};

	if (render_data->shaders == true)
	{
		compile_shaders(render_data);
		render_data->shaders = false;
	}

	int width = globuf_get_width(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	int height = globuf_get_height(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}

	if ((width == 0) || (height == 0))
	{
		// skip rendering if the window area is 0
		return;
	}

	// re-create the swapchain when the window size changes
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

	globuf_update_content(globuf, NULL, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return;
	}
}

static void config_callback(struct globuf_config_reply* replies, size_t count, void* data)
{
	fprintf(stderr, "window configured succesfully, printing information:\n");

	struct globuf* context = data;
	const char* message = NULL;
	size_t feature;

	for (size_t i = 0; i < count; ++i)
	{
		feature = replies[i].feature;

		if (feature < GLOBUF_FEATURE_COUNT)
		{
			if (replies[i].error.code == GLOBUF_ERROR_OK)
			{
				message = "success";
			}
			else
			{
				message = globuf_error_get_msg(context, &replies[i].error);
			}

			fprintf(stderr, "\t%s: %s\n", feature_names[feature], message);
		}
	}
}

int main(int argc, char** argv)
{
	struct globuf_error_info error = {0};
	struct globuf_error_info error_early = {0};
	printf("starting the simple globuf example\n");

	// prepare function pointers
	struct globuf_config_backend config = {0};

#if !defined(GLOBUF_SHARED)
	// initialize statically
	#if defined(GLOBUF_EXAMPLE_X11)
	globuf_prepare_init_x11_vulkan(&config, &error_early);
	#elif defined(GLOBUF_EXAMPLE_APPKIT)
	globuf_prepare_init_appkit_vulkan(&config, &error_early);
	#elif defined(GLOBUF_EXAMPLE_WIN)
	globuf_prepare_init_win_vulkan(&config, &error_early);
	#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	globuf_prepare_init_wayland_vulkan(&config, &error_early);
	#endif
#else
	// prepare dynamic initializer
	char* path_globuf_lib = NULL;
	char* sym_globuf_init = NULL;

	#if defined(GLOBUF_EXAMPLE_X11)
	path_globuf_lib = "./globuf_x11_vulkan.so";
	sym_globuf_init = "globuf_prepare_init_x11_vulkan";
	#elif defined(GLOBUF_EXAMPLE_APPKIT)
	path_globuf_lib = "./globuf_appkit_vulkan.dylib";
	sym_globuf_init = "globuf_prepare_init_appkit_vulkan";
	#elif defined(GLOBUF_EXAMPLE_WIN)
	path_globuf_lib = "./globuf_win_vulkan.dll";
	sym_globuf_init = "globuf_prepare_init_win_vulkan";
	#elif defined(GLOBUF_EXAMPLE_WAYLAND)
	path_globuf_lib = "./globuf_wayland_vulkan.so";
	sym_globuf_init = "globuf_prepare_init_wayland_vulkan";
	#endif

	// load the backend binder symbol straight from a shared object
#if !defined(GLOBUF_EXAMPLE_WIN)
	void* globuf_lib = dlopen(path_globuf_lib, 0);
	void (*globuf_prepare_init)() = dlsym(globuf_lib, sym_globuf_init);
#else
	HMODULE globuf_lib = LoadLibraryExA(path_globuf_lib, NULL, 0);
	void (*globuf_prepare_init)() = (void(*)()) GetProcAddress(globuf_lib, sym_globuf_init);
#endif

	// run the binder to load the remaining function pointers for the target implementation
	globuf_prepare_init(&config, &error_early);
#endif

	// set function pointers and perform basic init
	struct globuf* globuf = globuf_init(&config, &error);

	// Unless the context allocation failed it is always possible to access
	// error messages (even when the context initialization failed) so we can
	// always handle the backend initialization error first.

	// context allocation failed
	if (globuf == NULL)
	{
		fprintf(stderr, "could not allocate the main globuf context\n");
		return 1;
	}

	// Backend initialization failed. Since it happens before globuf
	// initialization and errors are accessible even if it fails, we can handle
	// the errors in the right order regardless.
	if (globuf_error_get_code(&error_early) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_early);
		globuf_clean(globuf, &error);
		return 1;
	}

	// The globuf initialization had failed, make it known now if the backend
	// initialization that happened before went fine.
	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// initialize vulkan
	struct globuf_render_data render_data =
	{
		.globuf = globuf,
		.width = 0,
		.height = 0,
		.shaders = true,
		.swapchain_images_len = 0,
	};

	init_vulkan(&render_data);

	// get available features
	struct globuf_config_features* feature_list =
		globuf_init_features(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// initialize features when creating the window
	struct globuf_feature_state state =
	{
		.state = GLOBUF_STATE_REGULAR,
	};

	struct globuf_feature_title title =
	{
		.title = "globuf",
	};

	struct globuf_feature_icon icon =
	{
		// acceptable implementation-defined behavior
		// since it's also the implementation that
		// allows us to bundle resources like so
		.pixmap = (uint32_t*) iconpix,
		.len = 2 + (16 * 16) + 2 + (32 * 32) + 2 + (64 * 64),
	};

	struct globuf_feature_size size =
	{
		.width = 500,
		.height = 500,
	};

	struct globuf_feature_pos pos =
	{
		.x = 250,
		.y = 250,
	};

	struct globuf_feature_frame frame =
	{
		.frame = true,
	};

	struct globuf_feature_background background =
	{
		.background = GLOBUF_BACKGROUND_BLURRED,
	};

	struct globuf_feature_vsync vsync =
	{
		.vsync = true,
	};

	// configure the feature and print a list
	printf("received a list of available features:\n");

	struct globuf_config_request configs[GLOBUF_FEATURE_COUNT] = {0};
	size_t feature_added = 0;
	size_t i = 0;

	while (i < feature_list->count)
	{
		enum globuf_feature feature_id = feature_list->list[i];
		printf("\t%s\n", feature_names[feature_id]);
		++i;

		switch (feature_id)
		{
			case GLOBUF_FEATURE_STATE:
			{
				configs[feature_added].config = &state;
				break;
			}
			case GLOBUF_FEATURE_TITLE:
			{
				configs[feature_added].config = &title;
				break;
			}
			case GLOBUF_FEATURE_ICON:
			{
				configs[feature_added].config = &icon;
				break;
			}
			case GLOBUF_FEATURE_SIZE:
			{
				configs[feature_added].config = &size;
				break;
			}
			case GLOBUF_FEATURE_POS:
			{
				configs[feature_added].config = &pos;
				break;
			}
			case GLOBUF_FEATURE_FRAME:
			{
				configs[feature_added].config = &frame;
				break;
			}
			case GLOBUF_FEATURE_BACKGROUND:
			{
				configs[feature_added].config = &background;
				break;
			}
			case GLOBUF_FEATURE_VSYNC:
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
	struct globuf_config_events events =
	{
		.data = globuf,
		.handler = event_callback,
	};

	struct globuf_error_info error_events = {0};
	globuf_init_events(globuf, &events, &error_events);

	if (globuf_error_get_code(&error_events) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_events);
		globuf_clean(globuf, &error);
		return 1;
	}

	// register a render callback
	struct globuf_config_render render =
	{
		.data = &render_data,
		.callback = render_callback,
	};

	struct globuf_error_info error_render = {0};
	globuf_init_render(globuf, &render, &error_render);

	if (globuf_error_get_code(&error_render) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_render);
		globuf_clean(globuf, &error);
		return 1;
	}

	// create the window
	globuf_window_create(globuf, configs, feature_added, config_callback, globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// configure vulkan
	config_vulkan(&render_data);

	// check window
	globuf_window_confirm(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// display the window
	globuf_window_start(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// do some more stuff while the window runs in another thread
	printf(
		"this is a message from the main thread\n"
		"the window should now be visible\n"
		"we can keep computing here\n");

	// wait for the window to be closed
	globuf_window_block(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// handle event thread errors
	if (globuf_error_get_code(&error_events) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_events);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// handle render thread errors
	if (globuf_error_get_code(&error_render) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error_render);
		globuf_window_destroy(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	// free resources correctly
	// vulkan cleanup
	vkDeviceWaitIdle( render_data.device);
	swapchain_free_vulkan(&render_data);

	// globuf cleanup
	globuf_window_destroy(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		globuf_clean(globuf, &error);
		return 1;
	}

	globuf_clean(globuf, &error);

	if (globuf_error_get_code(&error) != GLOBUF_ERROR_OK)
	{
		globuf_error_log(globuf, &error);
		return 1;
	}

	// vulkan cleanup
	vkDeviceWaitIdle(
		render_data.device);

	pipeline_free_vulkan(
		&render_data);

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

	vkDestroyDevice(
		render_data.device,
		NULL);

	PFN_vkDestroyDebugUtilsMessengerEXT debug_destroy =
		(PFN_vkDestroyDebugUtilsMessengerEXT)
		vkGetInstanceProcAddr(
			render_data.instance,
			"vkDestroyDebugUtilsMessengerEXT");

	if (debug_destroy != NULL)
	{
		debug_destroy(
			render_data.instance,
			render_data.debug,
			NULL);
	}

	vkDestroyInstance(
		render_data.instance,
		NULL);

	return 0;
}
