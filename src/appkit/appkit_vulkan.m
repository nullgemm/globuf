#include "include/globox.h"
#include "include/globox_vulkan.h"
#include "include/globox_appkit_vulkan.h"

#include "common/globox_private.h"
#include "appkit/appkit_common.h"
#include "appkit/appkit_common_helpers.h"
#include "appkit/appkit_vulkan.h"
#include "appkit/appkit_vulkan_helpers.h"

#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_metal.h>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

void globox_appkit_vulkan_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct appkit_vulkan_backend* backend = malloc(sizeof (struct appkit_vulkan_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct appkit_vulkan_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize values that can be initialized explicitly
	backend->config = NULL;
	backend->ext_needed = NULL;
	backend->ext_found = NULL;
	backend->ext_len =
		appkit_helpers_vulkan_add_extensions(
			context,
			&(backend->ext_needed),
			&(backend->ext_found),
			error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// initialize the platform
	struct appkit_platform* platform = &(backend->platform);
	globox_appkit_common_init(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// get Vulkan Metal extension
	VkResult error_vk;
	uint32_t ext_count;
	VkExtensionProperties* ext_props;

	error_vk =
		vkEnumerateInstanceExtensionProperties(
			NULL,
			&ext_count,
			NULL);

	if (error_vk != VK_SUCCESS)
	{
		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_MACOS_VULKAN_EXTENSIONS_LIST);
		return;
	}

	if (ext_count == 0)
	{
		// error always set
		return;
	}

	// allocate vulkan properties array
	ext_props = malloc(ext_count * (sizeof (VkExtensionProperties)));

	if (ext_props == NULL)
	{
		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	// fill vulkan properties array
	error_vk =
		vkEnumerateInstanceExtensionProperties(
			NULL,
			&ext_count,
			ext_props);

	if (error_vk != VK_SUCCESS)
	{
		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_MACOS_VULKAN_EXTENSIONS_LIST);
		return;
	}

	// check if our extensions are available
	int match;
	uint32_t k;
	uint32_t i = 0;
	uint32_t ext_found = 0;

	// loop through available extensions
	while ((i < ext_count) && (ext_found < backend->ext_len))
	{
		k = 0;

		// loop through required extensions
		while (k < backend->ext_len)
		{
			// skip already found extensions
			if (backend->ext_found[k] == true)
			{
				++k;
				continue;
			}

			// check extension
			match = strcmp(ext_props[i].extensionName, backend->ext_needed[k]);

			// stop comparing available extension
			// and save required extension
			if (match == 0)
			{
				backend->ext_found[k] = true;
				++ext_found;
				break;
			}

			// keep comparing available extension
			// against other required extensions
			++k;
		}

		++i;
	}

	free(ext_props);

	// fail if we couldn't get all the required extensions
	if (ext_found != backend->ext_len)
	{
		globox_error_throw(
			context,
			error,
			GLOBOX_ERROR_MACOS_VULKAN_EXTENSION_UNAVAILABLE);
		return;
	}

	// error always set
}

void globox_appkit_vulkan_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// free backend data
	free(context->backend_callbacks.data);
	free(backend->ext_needed);
	free(backend->ext_found);

	// clean the platform
	globox_appkit_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globox_appkit_vulkan_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	appkit_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// run common AppKit helper
	globox_appkit_common_window_create(
		context,
		platform,
		configs,
		count,
		callback,
		data,
		error);

	dispatch_sync(dispatch_get_main_queue(), ^{
		// create a layer-hosting view
		platform->view = [GloboxMetalView new];
		GloboxMetalView* view = platform->view;

		// create the custom layer delegate data
		struct appkit_layer_delegate_data layer_delegate_data =
		{
			.globox = context,
			.platform = platform,
			.error = error,
		};

		platform->layer_delegate_data = layer_delegate_data;

		// create a custom layer delegate
		platform->layer_delegate = [GloboxLayerDelegate new];
		id layer_delegate = platform->layer_delegate;

		[layer_delegate setGloboxLayerDelegateData: &(platform->layer_delegate_data)];

		// get the view's layer and set its delegate
		platform->layer = [view layer];
		id layer = platform->layer;

		[layer setDelegate: layer_delegate];

		// configure the view to be transparent
		if (context->feature_background->background != GLOBOX_BACKGROUND_OPAQUE)
		{
			[layer setOpaque: NO];
		}

		// create an effects view if we are using background blur
		if (context->feature_background->background == GLOBOX_BACKGROUND_BLURRED)
		{
			// create the blur view
			platform->view_blur = [NSVisualEffectView new];
			NSVisualEffectView* view_blur = platform->view_blur;

			// configure to blur background and resize with master view
			[view_blur setBlendingMode: NSVisualEffectBlendingModeBehindWindow];
			[view_blur setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];

			// create a dedicated master view
			platform->view_master = [NSView new];
			id view_master = platform->view_master;

			// build view hierarchy
			[view_master addSubview: view];
			[view_master addSubview: view_blur positioned: NSWindowBelow relativeTo: view];
		}
		else
		{
			platform->view_master = view;
		}
	});

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globox_appkit_vulkan_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	vkDestroySurfaceKHR(
		backend->config->instance,
		backend->surface,
		backend->config->allocator);

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// run common AppKit helper
	globox_appkit_common_window_destroy(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_appkit_vulkan_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// run common AppKit helper
	globox_appkit_common_window_start(context, platform, error);

	// set window content view
	dispatch_sync(dispatch_get_main_queue(), ^{
		[platform->win setContentView: platform->view_master];

		// create vulkan surface
		VkMetalSurfaceCreateInfoEXT* info = &(backend->vulkan_info);
		info->sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
		info->pNext = NULL;
		info->flags = 0;
		info->pLayer = (const CAMetalLayer*) [platform->view layer];

		VkResult error_vk =
			vkCreateMetalSurfaceEXT(
				backend->config->instance,
				&(backend->vulkan_info),
				backend->config->allocator,
				&(backend->surface));

		if (error_vk != VK_SUCCESS)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_MACOS_VULKAN_SURFACE_CREATE);
			return;
		}
	});

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globox_appkit_vulkan_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_window_block(context, platform, error);

	// error always set
}

void globox_appkit_vulkan_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_window_stop(context, platform, error);

	// error always set
}


void globox_appkit_vulkan_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_init_render(context, platform, config, error);

	// error always set
}

void globox_appkit_vulkan_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_init_events(context, platform, config, error);

	// error always set
}

enum globox_event globox_appkit_vulkan_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	return globox_appkit_common_handle_events(context, platform, event, error);
}


struct globox_config_features* globox_appkit_vulkan_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_init_features(context, platform, error);
}

void globox_appkit_vulkan_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_appkit_vulkan_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_appkit_vulkan_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_appkit_vulkan_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// run common AppKit helper
	globox_appkit_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_appkit_vulkan_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_width(context, platform, error);
}

unsigned globox_appkit_vulkan_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_height(context, platform, error);
}

struct globox_rect globox_appkit_vulkan_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);

	// error always set
	return globox_appkit_common_get_expose(context, platform, error);
}


void globox_appkit_vulkan_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	struct globox_update_vulkan* update = data;

	globox_error_ok(error);
}


// Vulkan configuration setter
void globox_appkit_init_vulkan(
	struct globox* context,
	struct globox_config_vulkan* config,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;

	backend->config = config;

	globox_error_ok(error);
}

// get Vulkan extensions
void globox_appkit_get_extensions_vulkan(
	struct globox* context,
	uint32_t* len,
	const char*** list,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;

	*len = backend->ext_len;
	*list = backend->ext_needed;

	globox_error_ok(error);
}

// create Vulkan surface
VkBool32 globox_appkit_presentation_support_vulkan(
	struct globox* context,
	VkPhysicalDevice physical_device,
	uint32_t queue_family_index,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;
	struct appkit_platform* platform = &(backend->platform);
	struct globox_config_vulkan* config = backend->config;

	error->code = GLOBOX_ERROR_OK;

	// TODO

	return VK_TRUE;
}

// get Vulkan surface
VkSurfaceKHR* globox_appkit_get_surface_vulkan(
	struct globox* context,
	struct globox_error_info* error)
{
	struct appkit_vulkan_backend* backend = context->backend_data;

	globox_error_ok(error);

	return &(backend->surface);
}

// init AppKit Vulkan
void globox_prepare_init_appkit_vulkan(
	struct globox_config_backend* config,
	struct globox_error_info* error)
{
	struct globox_calls_vulkan* vulkan =
		malloc(sizeof (struct globox_calls_vulkan));

	if (vulkan == NULL)
	{
		error->code = GLOBOX_ERROR_ALLOC;
		error->file = __FILE__;
		error->line = __LINE__;
		return;
	}

	vulkan->init = globox_appkit_init_vulkan;
	vulkan->get_extensions = globox_appkit_get_extensions_vulkan;
	vulkan->presentation_support = globox_appkit_presentation_support_vulkan;
	vulkan->get_surface = globox_appkit_get_surface_vulkan;

	config->data = vulkan;
	config->init = globox_appkit_vulkan_init;
	config->clean = globox_appkit_vulkan_clean;
	config->window_create = globox_appkit_vulkan_window_create;
	config->window_destroy = globox_appkit_vulkan_window_destroy;
	config->window_start = globox_appkit_vulkan_window_start;
	config->window_block = globox_appkit_vulkan_window_block;
	config->window_stop = globox_appkit_vulkan_window_stop;
	config->init_render = globox_appkit_vulkan_init_render;
	config->init_events = globox_appkit_vulkan_init_events;
	config->handle_events = globox_appkit_vulkan_handle_events;
	config->init_features = globox_appkit_vulkan_init_features;
	config->feature_set_interaction = globox_appkit_vulkan_feature_set_interaction;
	config->feature_set_state = globox_appkit_vulkan_feature_set_state;
	config->feature_set_title = globox_appkit_vulkan_feature_set_title;
	config->feature_set_icon = globox_appkit_vulkan_feature_set_icon;
	config->get_width = globox_appkit_vulkan_get_width;
	config->get_height = globox_appkit_vulkan_get_height;
	config->get_expose = globox_appkit_vulkan_get_expose;
	config->update_content = globox_appkit_vulkan_update_content;

	globox_error_ok(error);
}
