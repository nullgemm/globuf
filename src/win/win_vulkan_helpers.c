#include "include/globox.h"
#include "common/globox_private.h"
#include "win/win_vulkan_helpers.h"
#include "win/win_vulkan.h"
#include "win/win_common.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

size_t win_helpers_vulkan_add_extensions(
	struct globox* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globox_error_info* error)
{
	// list vulkan extensions here to make the code cleaner
	const char* extensions_names[] =
	{
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	};

	size_t extensions_count =
		(sizeof (extensions_names)) / (sizeof (const char*));

	// allocate required extensions list
	*ext_needed = malloc(extensions_count * sizeof (char*));

	if (*ext_needed == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return 0;
	}

	// allocate found required extensions list
	*ext_found = malloc(extensions_count * sizeof (bool));

	if (*ext_found == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return 0;
	}

	// initialize values
	for (size_t i = 0; i < extensions_count; ++i)
	{
		(*ext_needed)[i] = extensions_names[i];
		(*ext_found)[i] = false;
	}

	globox_error_ok(error);

	return extensions_count;
}
