#ifndef H_GLOBOX
#define H_GLOBOX

/// this is the main globox header and the only one the developer should include

// common includes
#include <stdint.h>
#include <stdbool.h>

// platform-specific includes
#if defined(GLOBOX_PLATFORM_WAYLAND)
	#include "wayland/globox_wayland.h"
#elif defined(GLOBOX_PLATFORM_X11)
	#include "x11/globox_x11.h"
#elif defined(GLOBOX_PLATFORM_WINDOWS)
	#include "windows/globox_windows.h"
#elif defined(GLOBOX_PLATFORM_OSX)
	#include "osx/globox_osx.h"
#endif

// defines
#define GLOBOX_ERROR_LOG_BASIC
#define GLOBOX_ERROR_LOG_THROW
#define GLOBOX_ERROR_LOG_DEBUG
// #define GLOBOX_ERROR_SKIP
// #define GLOBOX_ERROR_ABORT

// helper structures
enum globox_state
{
	GLOBOX_STATE_REGULAR,
	GLOBOX_STATE_MINIMIZED,
	GLOBOX_STATE_MAXIMIZED,
	GLOBOX_STATE_FULLSCREEN,
};

// globox
struct globox
{
	// window position (signed)
	int32_t globox_x; 
	int32_t globox_y;
	// window size
	uint32_t globox_width;
	uint32_t globox_height;
	// window title (UTF-8)
	char* globox_title;
	bool globox_closed;
	bool globox_redraw;

	// window state (regular, minimized, maximized, fullscreen)
	enum globox_state globox_state;
	// window events callback (resize, hover, click, type)
	void (*globox_event_callback)(
		void* event,
		void* data);
	// window events callback data (developer-provided)
	void* globox_event_callback_data;
	// error handling
	enum globox_error globox_error;
	char* globox_log[GLOBOX_ERROR_SIZE];

	// platform-specific substructure
	struct globox_platform globox_platform;
};

// cleans the error field
void globox_error_reset(struct globox* globox);
// prints the error message
void globox_error_basic_log(struct globox* globox);
// returns a pointer to the error message
char* globox_error_output_log(struct globox* globox);
// returns the error code
enum globox_error globox_error_output_code(struct globox* globox);
// returns true if an error occurred
char globox_error_catch(struct globox* globox);

// generic functions
void globox_open(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height,
	const char* title,
	enum globox_state state,
	void (*callback)(
		void* event,
		void* data),
	void* callback_data);

void globox_close(struct globox* globox);

// platform-dependent functions
void globox_platform_init(struct globox* globox);
void globox_platform_free(struct globox* globox);
void globox_platform_create_window(struct globox* globox);
void globox_platform_hooks(struct globox* globox);
void globox_platform_commit(struct globox* globox);
void globox_platform_prepoll(struct globox* globox);
void globox_platform_events_poll(struct globox* globox);
void globox_platform_events_wait(struct globox* globox);

void globox_platform_events_handle(
	struct globox* globox,
	void (*expose)(struct globox*, int),
	void (*redraw)(struct globox*));

void globox_platform_set_icon(
	struct globox* globox,
	uint32_t* pixmap,
	uint32_t len);

void globox_platform_set_title(
	struct globox* globox,
	const char* title); 

void globox_platform_set_state(
	struct globox* globox,
	enum globox_state state); 

// context-dependent functions
void globox_context_x11_software_init(struct globox* globox);
void globox_context_x11_software_free(struct globox* globox);
void globox_context_x11_software_create(struct globox* globox);
void globox_context_x11_software_shrink(struct globox* globox);
void globox_context_x11_software_reserve(struct globox* globox);
void globox_context_x11_software_expose(struct globox* globox, int len);

void globox_context_x11_software_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);

// generic getters 
// TODO
// platform getters 
// TODO
// context getters
// TODO

#endif
