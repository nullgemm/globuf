#ifndef H_GLOBUF_INTERNAL_APPKIT_COMMON_HELPERS
#define H_GLOBUF_INTERNAL_APPKIT_COMMON_HELPERS

#include "include/globuf.h"
#include "appkit/appkit_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#import <AppKit/AppKit.h>

@interface GlobufWindow: NSWindow
	@property struct appkit_thread_event_loop_data* globufEventData;
	- (void) sendEvent: (NSEvent*) event;
	- (void) zoom: (id) sender;
@end

@interface GlobufWindowDelegate: NSObject <NSWindowDelegate>
	@property struct appkit_window_delegate_data* globufDelegateData;
@end

@interface GlobufLayerDelegate: NSObject <CALayerDelegate>
	@property struct appkit_layer_delegate_data* globufLayerDelegateData;
	- (void) displayLayer: (CALayer*) layer;
@end

void* globuf_appkit_helpers_render_loop(
	void* data);

void globuf_appkit_helpers_features_init(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	struct globuf_error_info* error);

void globuf_appkit_helpers_send_app_event(
	struct globuf* context,
	struct appkit_platform* platform,
	enum globuf_event event);

void globuf_appkit_helpers_set_state(
	struct globuf* context,
	id window,
	struct globuf_feature_state* config,
	struct globuf_error_info* error);

void globuf_appkit_helpers_handle_interaction(
	struct globuf* context,
	struct appkit_platform* platform,
	struct globuf_error_info* error);

#endif
