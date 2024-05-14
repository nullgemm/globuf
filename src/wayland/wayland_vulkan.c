#include "include/globuf.h"
#include "include/globuf_vulkan.h"
#include "include/globuf_wayland_vulkan.h"

#include "common/globuf_private.h"
#include "wayland/wayland_common.h"
#include "wayland/wayland_common_helpers.h"
#include "wayland/wayland_vulkan.h"
#include "wayland/wayland_vulkan_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>
#include <wayland-client.h>

void globuf_wayland_vulkan_init(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// allocate the backend
	struct wayland_vulkan_backend* backend = malloc(sizeof (struct wayland_vulkan_backend));

	if (backend == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct wayland_vulkan_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize values that can be initialized explicitly
	backend->config = NULL;
	backend->ext_needed = NULL;
	backend->ext_found = NULL;
	backend->ext_len =
		wayland_helpers_vulkan_add_extensions(
			context,
			&(backend->ext_needed),
			&(backend->ext_found),
			error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// initialize the platform
	globuf_wayland_common_init(context, &(backend->platform), error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// get Vulkan Wayland extension
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
		globuf_error_throw(
			context,
			error,
			GLOBUF_ERROR_WAYLAND_VULKAN_EXTENSIONS_LIST);
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
		globuf_error_throw(
			context,
			error,
			GLOBUF_ERROR_ALLOC);
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
		globuf_error_throw(
			context,
			error,
			GLOBUF_ERROR_WAYLAND_VULKAN_EXTENSIONS_LIST);
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
		globuf_error_throw(
			context,
			error,
			GLOBUF_ERROR_WAYLAND_VULKAN_EXTENSION_UNAVAILABLE);
		return;
	}

	// error always set
}

void globuf_wayland_vulkan_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// free backend data
	free(context->backend_callbacks.data);
	free(backend->ext_needed);
	free(backend->ext_found);

	// clean the platform
	globuf_wayland_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globuf_wayland_vulkan_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	globuf_wayland_helpers_features_init(context, platform, configs, count, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// run common Wayland helper
	globuf_wayland_common_window_create(context, platform, configs, count, callback, data, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_vulkan_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_LOCK);
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
		globuf_error_throw(context, error, GLOBUF_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	// run common Wayland helper
	globuf_wayland_common_window_destroy(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	globuf_error_ok(error);
}

void globuf_wayland_vulkan_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_window_confirm(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// create vulkan surface
	VkWaylandSurfaceCreateInfoKHR* info = &(backend->vulkan_info);
	info->sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	info->pNext = NULL;
	info->flags = 0;
	info->display = platform->display;
	info->surface = platform->surface;

	VkResult error_vk =
		vkCreateWaylandSurfaceKHR(
			backend->config->instance,
			&(backend->vulkan_info),
			backend->config->allocator,
			&(backend->surface));

	if (error_vk != VK_SUCCESS)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WAYLAND_VULKAN_SURFACE_CREATE);
		return;
	}

	// error always set
}

void globuf_wayland_vulkan_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_wayland_vulkan_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper (mutex locked when unblocked)
	globuf_wayland_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_wayland_vulkan_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globuf_wayland_vulkan_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_wayland_vulkan_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globuf_event globuf_wayland_vulkan_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	enum globuf_event out =
		globuf_wayland_common_handle_events(
			context,
			platform,
			event,
			error);

	return out;
}


struct globuf_config_features* globuf_wayland_vulkan_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	struct globuf_config_features* features =
		globuf_wayland_common_init_features(context, platform, error);

	// return the newly created features info structure
	// error always set
	return features;
}

void globuf_wayland_vulkan_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globuf_wayland_vulkan_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globuf_wayland_vulkan_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globuf_wayland_vulkan_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// run common Wayland helper
	globuf_wayland_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globuf_wayland_vulkan_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globuf_wayland_common_get_width(context, platform, error);
}

unsigned globuf_wayland_vulkan_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globuf_wayland_common_get_height(context, platform, error);
}

struct globuf_rect globuf_wayland_vulkan_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	// error always set
	return globuf_wayland_common_get_expose(context, platform, error);
}


void globuf_wayland_vulkan_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);

	wl_surface_commit(platform->surface);
	globuf_error_ok(error);
}

void* globuf_wayland_vulkan_callback(
	struct globuf* context)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);
	return platform;
}


// Vulkan configuration setter
void globuf_wayland_init_vulkan(
	struct globuf* context,
	struct globuf_config_vulkan* config,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;

	backend->config = config;

	globuf_error_ok(error);
}

// get Vulkan extensions
void globuf_wayland_get_extensions_vulkan(
	struct globuf* context,
	uint32_t* len,
	const char*** list,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;

	*len = backend->ext_len;
	*list = backend->ext_needed;

	globuf_error_ok(error);
}

// create Vulkan surface
VkBool32 globuf_wayland_presentation_support_vulkan(
	struct globuf* context,
	VkPhysicalDevice physical_device,
	uint32_t queue_family_index,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;
	struct wayland_platform* platform = &(backend->platform);
	struct globuf_config_vulkan* config = backend->config;

	error->code = GLOBUF_ERROR_OK;

	return vkGetPhysicalDeviceWaylandPresentationSupportKHR(
		physical_device,
		queue_family_index,
		platform->display);
}

// get Vulkan surface
VkSurfaceKHR* globuf_wayland_get_surface_vulkan(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct wayland_vulkan_backend* backend = context->backend_data;

	globuf_error_ok(error);

	return &(backend->surface);
}


// init Wayland Vulkan
void globuf_prepare_init_wayland_vulkan(
	struct globuf_config_backend* config,
	struct globuf_error_info* error)
{
	struct globuf_calls_vulkan* vulkan =
		malloc(sizeof (struct globuf_calls_vulkan));

	if (vulkan == NULL)
	{
		error->code = GLOBUF_ERROR_ALLOC;
		error->file = __FILE__;
		error->line = __LINE__;
		return;
	}

	vulkan->init = globuf_wayland_init_vulkan;
	vulkan->get_extensions = globuf_wayland_get_extensions_vulkan;
	vulkan->presentation_support = globuf_wayland_presentation_support_vulkan;
	vulkan->get_surface = globuf_wayland_get_surface_vulkan;

	config->data = vulkan;
	config->callback = globuf_wayland_vulkan_callback;
	config->init = globuf_wayland_vulkan_init;
	config->clean = globuf_wayland_vulkan_clean;
	config->window_create = globuf_wayland_vulkan_window_create;
	config->window_destroy = globuf_wayland_vulkan_window_destroy;
	config->window_confirm = globuf_wayland_vulkan_window_confirm;
	config->window_start = globuf_wayland_vulkan_window_start;
	config->window_block = globuf_wayland_vulkan_window_block;
	config->window_stop = globuf_wayland_vulkan_window_stop;
	config->init_render = globuf_wayland_vulkan_init_render;
	config->init_events = globuf_wayland_vulkan_init_events;
	config->handle_events = globuf_wayland_vulkan_handle_events;
	config->init_features = globuf_wayland_vulkan_init_features;
	config->feature_set_interaction = globuf_wayland_vulkan_feature_set_interaction;
	config->feature_set_state = globuf_wayland_vulkan_feature_set_state;
	config->feature_set_title = globuf_wayland_vulkan_feature_set_title;
	config->feature_set_icon = globuf_wayland_vulkan_feature_set_icon;
	config->get_width = globuf_wayland_vulkan_get_width;
	config->get_height = globuf_wayland_vulkan_get_height;
	config->get_expose = globuf_wayland_vulkan_get_expose;
	config->update_content = globuf_wayland_vulkan_update_content;

	globuf_error_ok(error);
}
