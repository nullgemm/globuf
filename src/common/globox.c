#include "include/globox.h"
#include "common/globox_private.h"

#include <stdbool.h>
#include <stddef.h>

struct globox
{
	// common
	struct globox_config config;
	void* platform_config;

	// not common but neutral in signature
	struct globox_event_handlers event_handlers;
	struct globox_backend_callbacks backend_callbacks;

	// common details set using the window feature transaction system
	int x_init;
	int y_init;
	unsigned width_init;
	unsigned height_init;
	const char* title;
	bool framed;
	enum globox_background background;

	// other common details not directly available prior to window creation
	enum globox_state state;
	enum globox_interaction interaction;

	// error handling
	enum globox_error error;
	char* error_messages[GLOBOX_ERROR_SIZE];
};

// use composition for optimized accesses
struct globox_event_handlers
{
	size_t event_handlers_count;
	void (**event_handlers)(void* event, void* data);
};

struct globox_backend_callbacks
{
	size_t backend_callbacks_count;
	void** backend_callbacks_data;
	void (**backend_callback_create)(struct globox* context, void* data);
	void (**backend_callback_destroy)(struct globox* context, void* data);
};
