#include "include/globox.h"
#include "include/globox_vulkan.h"
#include "include/globox_x11_vulkan.h"

#include "common/globox_private.h"
#include "x11/x11_common.h"
#include "x11/x11_common_helpers.h"
#include "x11/x11_vulkan.h"
#include "x11/x11_vulkan_helpers.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>
#include <xcb/xcb.h>

void globox_x11_vulkan_init(
	struct globox* context,
	struct globox_error_info* error)
{
	// allocate the backend
	struct x11_vulkan_backend* backend = malloc(sizeof (struct x11_vulkan_backend));

	if (backend == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return;
	}

	// zero-initialize the backend
	struct x11_vulkan_backend zero = {0};
	*backend = zero;

	// reference the backend in the main context
	context->backend_data = backend;

	// initialize values that can be initialized explicitly
	backend->config = NULL;
	backend->ext_needed = NULL;
	backend->ext_found = NULL;
	backend->ext_len =
		x11_helpers_vulkan_add_extensions(
			context,
			&(backend->ext_needed),
			&(backend->ext_found),
			error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// open a connection to the X server
	struct x11_platform* platform = &(backend->platform);
	platform->conn = xcb_connect(NULL, &(platform->screen_id));
	int error_posix = xcb_connection_has_error(platform->conn);

	if (error_posix > 0)
	{
		xcb_disconnect(platform->conn);
		globox_error_throw(context, error, GLOBOX_ERROR_X11_CONN);
		return;
	}

	// initialize the platform
	globox_x11_common_init(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// get Vulkan XCB extension
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
			GLOBOX_ERROR_X11_VULKAN_EXTENSIONS_LIST);
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
			GLOBOX_ERROR_X11_VULKAN_EXTENSIONS_LIST);
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
			GLOBOX_ERROR_X11_VULKAN_EXTENSION_UNAVAILABLE);
		return;
	}

	// error always set
}

void globox_x11_vulkan_clean(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// free backend data
	free(context->backend_callbacks.data);
	free(backend->ext_needed);
	free(backend->ext_found);

	// close the connection to the X server
	xcb_disconnect(platform->conn);

	// clean the platform
	globox_x11_common_clean(context, platform, error);

	// free the backend
	free(backend);

	// error always set
}

void globox_x11_vulkan_window_create(
	struct globox* context,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// lock mutex
	int error_posix = pthread_mutex_lock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
		return;
	}

	// configure features here
	x11_helpers_features_init(context, platform, configs, count, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// select visual configuration
	if (context->feature_background->background != GLOBOX_BACKGROUND_OPAQUE)
	{
		x11_helpers_vulkan_visual_transparent(context, error);

		if (globox_error_get_code(error) == GLOBOX_ERROR_X11_VISUAL_INCOMPATIBLE)
		{
			x11_helpers_vulkan_visual_opaque(context, error);
		}
	}
	else
	{
		x11_helpers_vulkan_visual_opaque(context, error);
	}

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// run common X11 helper
	globox_x11_common_window_create(context, platform, configs, count, callback, data, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	// create vulkan surface
	VkXcbSurfaceCreateInfoKHR* info = &(backend->vulkan_info);
	info->sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	info->pNext = NULL;
	info->flags = 0;
	info->connection = platform->conn;
	info->window = platform->win;

	VkResult error_vk =
		vkCreateXcbSurfaceKHR(
			backend->config->instance,
			&(backend->vulkan_info),
			backend->config->allocator,
			&(backend->surface));

	if (error_vk != VK_SUCCESS)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_X11_VULKAN_SURFACE_CREATE);
		return;
	}

	// unlock mutex
	error_posix = pthread_mutex_unlock(&(platform->mutex_main));

	if (error_posix != 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
		return;
	}

	globox_error_ok(error);
}

void globox_x11_vulkan_window_destroy(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

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

	// run common X11 helper
	globox_x11_common_window_destroy(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	globox_error_ok(error);
}

void globox_x11_vulkan_window_start(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_window_start(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_vulkan_window_block(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper (mutex locked when unblocked)
	globox_x11_common_window_block(context, platform, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_vulkan_window_stop(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_window_stop(context, platform, error);

	// no extra failure check at the moment

	// error always set
}


void globox_x11_vulkan_init_render(
	struct globox* context,
	struct globox_config_render* config,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_init_render(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

void globox_x11_vulkan_init_events(
	struct globox* context,
	struct globox_config_events* config,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_init_events(context, platform, config, error);

	// no extra failure check at the moment

	// error always set
}

enum globox_event globox_x11_vulkan_handle_events(
	struct globox* context,
	void* event,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	enum globox_event out =
		globox_x11_common_handle_events(
			context,
			platform,
			event,
			error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return out;
	}

	// process configure event specifically
	xcb_generic_event_t* xcb_event = event;

	// only lock the main mutex when making changes to the context
	switch (xcb_event->response_type & ~0x80)
	{
		case XCB_CONFIGURE_NOTIFY:
		{
			xcb_configure_notify_event_t* configure =
				(xcb_configure_notify_event_t*) xcb_event;

			// lock xsync mutex
			int error_posix = pthread_mutex_lock(&(platform->mutex_xsync));

			if (error_posix != 0)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_LOCK);
				break;
			}

			// safe value updates
			if (platform->xsync_status == GLOBOX_XSYNC_CONFIGURED)
			{
				platform->xsync_status = GLOBOX_XSYNC_ACKNOWLEDGED;
			}

			// unlock xsync mutex
			error_posix = pthread_mutex_unlock(&(platform->mutex_xsync));

			if (error_posix != 0)
			{
				globox_error_throw(context, error, GLOBOX_ERROR_POSIX_MUTEX_UNLOCK);
				break;
			}

			out = GLOBOX_EVENT_MOVED_RESIZED;
			break;
		}
	}


	// error always set
	return out;
}


struct globox_config_features* globox_x11_vulkan_init_features(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	struct globox_config_features* features =
		globox_x11_common_init_features(context, platform, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return features;
	}

	// VSync capabilities should be checked outside of globox, report as available
	features->list[features->count] = GLOBOX_FEATURE_VSYNC;
	context->feature_vsync =
		malloc(sizeof (struct globox_feature_vsync));
	features->count += 1;

	if (context->feature_vsync == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	// return the newly created features info structure
	// error always set
	return features;
}

void globox_x11_vulkan_feature_set_interaction(
	struct globox* context,
	struct globox_feature_interaction* config,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_interaction(context, platform, config, error);

	// error always set
}

void globox_x11_vulkan_feature_set_state(
	struct globox* context,
	struct globox_feature_state* config,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_state(context, platform, config, error);

	// error always set
}

void globox_x11_vulkan_feature_set_title(
	struct globox* context,
	struct globox_feature_title* config,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_title(context, platform, config, error);

	// error always set
}

void globox_x11_vulkan_feature_set_icon(
	struct globox* context,
	struct globox_feature_icon* config,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// run common X11 helper
	globox_x11_common_feature_set_icon(context, platform, config, error);

	// error always set
}


unsigned globox_x11_vulkan_get_width(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_width(context, platform, error);
}

unsigned globox_x11_vulkan_get_height(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_height(context, platform, error);
}

struct globox_rect globox_x11_vulkan_get_expose(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	// error always set
	return globox_x11_common_get_expose(context, platform, error);
}


void globox_x11_vulkan_update_content(
	struct globox* context,
	void* data,
	struct globox_error_info* error)
{
	// not needed with Vulkan
	globox_error_ok(error);
}


xcb_connection_t* globox_get_x11_conn(struct globox* context)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	return platform->conn;
}

xcb_window_t globox_get_x11_window(struct globox* context)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	return platform->win;
}

xcb_window_t globox_get_x11_root(struct globox* context)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	return platform->root_win;
}

xcb_screen_t* globox_get_x11_screen(struct globox* context)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);

	return platform->screen_obj;
}

// Vulkan configuration setter
void globox_x11_init_vulkan(
	struct globox* context,
	struct globox_config_vulkan* config,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;

	backend->config = config;

	globox_error_ok(error);
}

// get Vulkan extensions
void globox_x11_get_extensions_vulkan(
	struct globox* context,
	uint32_t* len,
	const char*** list,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;

	*len = backend->ext_len;
	*list = backend->ext_needed;

	globox_error_ok(error);
}

// create Vulkan surface
VkBool32 globox_x11_presentation_support_vulkan(
	struct globox* context,
	VkPhysicalDevice physical_device,
	uint32_t queue_family_index,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;
	struct x11_platform* platform = &(backend->platform);
	struct globox_config_vulkan* config = backend->config;

	error->code = GLOBOX_ERROR_OK;

	return vkGetPhysicalDeviceXcbPresentationSupportKHR(
		physical_device,
		queue_family_index,
		platform->conn,
		platform->visual_id);
}

// get Vulkan surface
VkSurfaceKHR* globox_x11_get_surface_vulkan(
	struct globox* context,
	struct globox_error_info* error)
{
	struct x11_vulkan_backend* backend = context->backend_data;

	globox_error_ok(error);

	return &(backend->surface);
}

// init X11 Vulkan
void globox_prepare_init_x11_vulkan(
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

	vulkan->init = globox_x11_init_vulkan;
	vulkan->get_extensions = globox_x11_get_extensions_vulkan;
	vulkan->presentation_support = globox_x11_presentation_support_vulkan;
	vulkan->get_surface = globox_x11_get_surface_vulkan;

	config->data = vulkan;
	config->init = globox_x11_vulkan_init;
	config->clean = globox_x11_vulkan_clean;
	config->window_create = globox_x11_vulkan_window_create;
	config->window_destroy = globox_x11_vulkan_window_destroy;
	config->window_start = globox_x11_vulkan_window_start;
	config->window_block = globox_x11_vulkan_window_block;
	config->window_stop = globox_x11_vulkan_window_stop;
	config->init_render = globox_x11_vulkan_init_render;
	config->init_events = globox_x11_vulkan_init_events;
	config->handle_events = globox_x11_vulkan_handle_events;
	config->init_features = globox_x11_vulkan_init_features;
	config->feature_set_interaction = globox_x11_vulkan_feature_set_interaction;
	config->feature_set_state = globox_x11_vulkan_feature_set_state;
	config->feature_set_title = globox_x11_vulkan_feature_set_title;
	config->feature_set_icon = globox_x11_vulkan_feature_set_icon;
	config->get_width = globox_x11_vulkan_get_width;
	config->get_height = globox_x11_vulkan_get_height;
	config->get_expose = globox_x11_vulkan_get_expose;
	config->update_content = globox_x11_vulkan_update_content;

	globox_error_ok(error);
}
