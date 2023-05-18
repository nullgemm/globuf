#ifndef H_GLOBOX_INTERNAL_APPKIT_COMMON_HELPERS
#define H_GLOBOX_INTERNAL_APPKIT_COMMON_HELPERS

#include "include/globox.h"
#include "appkit/appkit_common.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#import <AppKit/AppKit.h>

@interface GloboxWindow: NSWindow
	@property struct appkit_thread_event_loop_data* globoxEventData;
	- (void) sendEvent: (NSEvent*) event;
	- (void) zoom: (id) sender;
@end

@interface GloboxWindowDelegate: NSObject <NSWindowDelegate>
	@property struct appkit_window_delegate_data* globoxDelegateData;
@end

@interface GloboxLayerDelegate: NSObject <CALayerDelegate>
	@property struct appkit_layer_delegate_data* globoxLayerDelegateData;
	- (void) displayLayer: (CALayer*) layer;
@end

void* appkit_helpers_render_loop(
	void* data);

void appkit_helpers_features_init(
	struct globox* context,
	struct appkit_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error);

void appkit_helpers_send_app_event(
	struct globox* context,
	struct appkit_platform* platform,
	enum globox_event event);

void appkit_helpers_set_state(
	struct globox* context,
	id window,
	struct globox_feature_state* config,
	struct globox_error_info* error);

#endif
