#include "include/globuf.h"
#include "include/globuf_vulkan.h"
#include "include/globuf_win_vulkan.h"

#include "common/globuf_private.h"
#include "win/win_common.h"
#include "win/win_common_helpers.h"
#include "win/win_vulkan.h"
#include "win/win_vulkan_helpers.h"

#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

void globuf_win_vulkan_init(
	struct globuf* context,
	struct globuf_error_info* error)
{
	// allocate the backend
	struct win_vulkan_backend* backend = malloc(sizeof (struct win_vulkan_backend));

	if (backend == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct win_vulkan_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize the platform
	struct win_platform* platform = &(backend->platform);
	globuf_win_common_init(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// initialize backend
	backend->config = NULL;
	backend->ext_needed = NULL;
	backend->ext_found = NULL;
	backend->ext_len =
		win_helpers_vulkan_add_extensions(
			context,
			&(backend->ext_needed),
			&(backend->ext_found),
			error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		return;
	}

	// get Vulkan Windows extension
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
			GLOBUF_ERROR_WIN_VULKAN_EXTENSIONS_LIST);
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
			GLOBUF_ERROR_WIN_VULKAN_EXTENSIONS_LIST);
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
			GLOBUF_ERROR_WIN_VULKAN_EXTENSION_UNAVAILABLE);
		return;
	}

	// error always set
}

void globuf_win_vulkan_clean(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// free backend data
	free(context->backend_callbacks.data);
	free(backend->ext_needed);
	free(backend->ext_found);

	// clean the platform
	globuf_win_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globuf_win_vulkan_window_create(
	struct globuf* context,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// configure features here
	globuf_win_helpers_features_init(context, platform, configs, count, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// run common win32 helper
	globuf_win_common_window_create(
		context,
		platform,
		configs,
		count,
		callback,
		data,
		error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_win_vulkan_window_destroy(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	vkDestroySurfaceKHR(
		backend->config->instance,
		backend->surface,
		backend->config->allocator);

	// run common win32 helper
	globuf_win_common_window_destroy(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_win_vulkan_window_confirm(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_window_confirm(context, platform, error);

	// error always set
}

void globuf_win_vulkan_window_start(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// run common win32 helper
	globuf_win_common_window_start(context, platform, error);

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// create vulkan surface
	VkWin32SurfaceCreateInfoKHR info =
	{
		.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.hinstance = GetModuleHandleW(NULL),
		.hwnd = platform->event_handle,
	};

	backend->vulkan_info = info;

	VkResult error_vk =
		vkCreateWin32SurfaceKHR(
			backend->config->instance,
			&(backend->vulkan_info),
			backend->config->allocator,
			&(backend->surface));

	if (error_vk != VK_SUCCESS)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_VULKAN_SURFACE_CREATE);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	platform->render = true;
	WakeConditionVariable(&(platform->cond_render));

	// no extra failure check at the moment

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	// error always set
}

void globuf_win_vulkan_window_block(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper (mutex locked when unblocked)
	globuf_win_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_win_vulkan_window_stop(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// run common win32 helper
	globuf_win_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	// error always set
}


void globuf_win_vulkan_init_render(
	struct globuf* context,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globuf_win_vulkan_init_events(
	struct globuf* context,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globuf_event globuf_win_vulkan_handle_events(
	struct globuf* context,
	void* event,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	enum globuf_event out =
		globuf_win_common_handle_events(
			context,
			platform,
			event,
			error);

	return out;
}


struct globuf_config_features* globuf_win_vulkan_init_features(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	struct globuf_config_features* features =
		globuf_win_common_init_features(context, platform, error);

	return features;
}

void globuf_win_vulkan_feature_set_interaction(
	struct globuf* context,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globuf_win_vulkan_feature_set_state(
	struct globuf* context,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globuf_win_vulkan_feature_set_title(
	struct globuf* context,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globuf_win_vulkan_feature_set_icon(
	struct globuf* context,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// run common win32 helper
	globuf_win_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globuf_win_vulkan_get_width(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globuf_win_common_get_width(context, platform, error);
}

unsigned globuf_win_vulkan_get_height(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globuf_win_common_get_height(context, platform, error);
}

struct globuf_rect globuf_win_vulkan_get_expose(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);

	// error always set
	return globuf_win_common_get_expose(context, platform, error);
}


void globuf_win_vulkan_update_content(
	struct globuf* context,
	void* data,
	struct globuf_error_info* error)
{
	// not needed with Vulkan
	globuf_error_ok(error);
}

void* globuf_win_vulkan_callback(
	struct globuf* context)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	return platform;
}


// Vulkan configuration setter
void globuf_win_init_vulkan(
	struct globuf* context,
	struct globuf_config_vulkan* config,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;

	backend->config = config;

	globuf_error_ok(error);
}

// get Vulkan extensions
void globuf_win_get_extensions_vulkan(
	struct globuf* context,
	uint32_t* len,
	const char*** list,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;

	*len = backend->ext_len;
	*list = backend->ext_needed;

	globuf_error_ok(error);
}

// create Vulkan surface
VkBool32 globuf_win_presentation_support_vulkan(
	struct globuf* context,
	VkPhysicalDevice physical_device,
	uint32_t queue_family_index,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;
	struct win_platform* platform = &(backend->platform);
	struct globuf_config_vulkan* config = backend->config;

	error->code = GLOBUF_ERROR_OK;

	return vkGetPhysicalDeviceWin32PresentationSupportKHR(
		physical_device,
		queue_family_index);
}

// get Vulkan surface
VkSurfaceKHR* globuf_win_get_surface_vulkan(
	struct globuf* context,
	struct globuf_error_info* error)
{
	struct win_vulkan_backend* backend = context->backend_data;

	globuf_error_ok(error);

	return &(backend->surface);
}

// init win32 Vulkan
void globuf_prepare_init_win_vulkan(
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

	vulkan->init = globuf_win_init_vulkan;
	vulkan->get_extensions = globuf_win_get_extensions_vulkan;
	vulkan->presentation_support = globuf_win_presentation_support_vulkan;
	vulkan->get_surface = globuf_win_get_surface_vulkan;

	config->data = vulkan;
	config->callback = globuf_win_vulkan_callback;
	config->init = globuf_win_vulkan_init;
	config->clean = globuf_win_vulkan_clean;
	config->window_create = globuf_win_vulkan_window_create;
	config->window_destroy = globuf_win_vulkan_window_destroy;
	config->window_confirm = globuf_win_vulkan_window_confirm;
	config->window_start = globuf_win_vulkan_window_start;
	config->window_block = globuf_win_vulkan_window_block;
	config->window_stop = globuf_win_vulkan_window_stop;
	config->init_render = globuf_win_vulkan_init_render;
	config->init_events = globuf_win_vulkan_init_events;
	config->handle_events = globuf_win_vulkan_handle_events;
	config->init_features = globuf_win_vulkan_init_features;
	config->feature_set_interaction = globuf_win_vulkan_feature_set_interaction;
	config->feature_set_state = globuf_win_vulkan_feature_set_state;
	config->feature_set_title = globuf_win_vulkan_feature_set_title;
	config->feature_set_icon = globuf_win_vulkan_feature_set_icon;
	config->get_width = globuf_win_vulkan_get_width;
	config->get_height = globuf_win_vulkan_get_height;
	config->get_expose = globuf_win_vulkan_get_expose;
	config->update_content = globuf_win_vulkan_update_content;

	globuf_error_ok(error);
}
