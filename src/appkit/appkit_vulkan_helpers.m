#define _XOPEN_SOURCE 700

#include "include/globox.h"
#include "common/globox_private.h"
#include "appkit/appkit_vulkan_helpers.h"
#include "appkit/appkit_vulkan.h"
#include "appkit/appkit_common.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_macos.h>

#import <AppKit/AppKit.h>
#import <MetalKit/MTKView.h>

@implementation GloboxMetalView

+ (Class) layerClass
{
	return NSClassFromString(@"CAMetalLayer");
}

- (BOOL) wantsUpdateLayer
{
	return YES;
}

- (CALayer*) makeBackingLayer
{
	return [self.class.layerClass layer];
}

- (instancetype) initWithFrame: (NSRect) frame
{
	if ((self = [super initWithFrame:frame]))
	{
		self.wantsLayer = YES;
		self.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

		[self updateDrawableSize];
	}

	return self;
}

- (void) updateDrawableSize
{
	CAMetalLayer *metalLayer = (CAMetalLayer *)self.layer;
	CGSize size = self.bounds.size;
	CGSize backingSize = [self convertSizeToBacking:size];

	metalLayer.contentsScale = backingSize.height / size.height;
	metalLayer.drawableSize = backingSize;
}

- (void) resizeWithOldSuperviewSize: (NSSize) oldSize
{
	[super resizeWithOldSuperviewSize:oldSize];
	[self updateDrawableSize];
}

@end

size_t appkit_helpers_vulkan_add_extensions(
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
		VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
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
