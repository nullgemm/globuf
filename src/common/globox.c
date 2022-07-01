#include "include/globox.h"
#include "common/globox_private.h"

#include <stddef.h>

struct globox
{
	// common
	struct globox_config config;
	void* platform_config;

	// not common but neutral in signature
	struct globox_event_handlers event_handlers;
	struct globox_backend_callbacks backend_callbacks;

	// various common details
	enum globox_state state;
	enum globox_interaction interaction;

	enum globox_error error;
	char* error_messages[GLOBOX_ERROR_SIZE];
};

// use callback setters to make configuration easier for the user of the library
struct globox_config
{
	int x_init;
	int y_init;
	unsigned width_init;
	unsigned height_init;

	char* title;
	bool framed;
	bool blurred;
	bool transparent;
	enum globox_backend backend;

	size_t backend_callbacks_setter_count;
	void** backend_callbacks_setter_data;
	void (**backend_callbacks_setter)(
		struct globox_backend_callbacks_entry* entry,
		enum globox_platform,
		enum globox_backend,
		void* data);

	size_t event_handler_setter_count;
	void** event_handler_setter_data;
	void (**event_handler_setter)(
		struct globox_event_handlers_entry* entry,
		enum globox_platform,
		enum globox_backend,
		void* data);
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
