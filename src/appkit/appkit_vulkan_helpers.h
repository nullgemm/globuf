#ifndef H_GLOBOX_INTERNAL_APPKIT_VULKAN_HELPERS
#define H_GLOBOX_INTERNAL_APPKIT_VULKAN_HELPERS

#include "include/globox.h"
#include "include/globox_vulkan.h"
#include "appkit/appkit_common.h"
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_metal.h>

#import <AppKit/AppKit.h>
#import <MetalKit/MTKView.h>

@interface GloboxMetalView : MTKView
	- (instancetype) initWithFrame: (NSRect) frame;
@end

struct appkit_vulkan_backend
{
	struct appkit_platform platform;
	struct globox_config_vulkan* config;

	// vulkan extensions used by globox
	uint32_t ext_len;
	const char** ext_needed;
	bool* ext_found;

	VkSurfaceKHR surface;
	VkMetalSurfaceCreateInfoEXT vulkan_info;
};

size_t appkit_helpers_vulkan_add_extensions(
	struct globox* context,
	const char*** ext_needed,
	bool** ext_found,
	struct globox_error_info* error);

#endif
