#ifndef H_GLOBOX_INTERNAL_WIN_COMMON
#define H_GLOBOX_INTERNAL_WIN_COMMON

#include "include/globox.h"

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

// # private helpers
enum win_user_msg
{
	WIN_USER_MSG_FULLSCREEN = WM_USER,
	WIN_USER_MSG_MAXIMIZE,
	WIN_USER_MSG_MINIMIZE,
	WIN_USER_MSG_REGULAR,
};

enum win_dpi_api
{
	WIN_DPI_API_NONE = 0,
	WIN_DPI_API_VISTA,
	WIN_DPI_API_8,
	WIN_DPI_API_10_V1,
	WIN_DPI_API_10_V2,
};

struct win_thread_render_loop_data
{
	struct globox* globox;
	struct win_platform* platform;
	struct globox_error_info* error;
};

struct win_thread_event_loop_data
{
	struct globox* globox;
	struct win_platform* platform;
	struct globox_error_info* error;
};

struct win_platform
{
	HANDLE mutex_main;
	CONDITION_VARIABLE cond_block;
	CONDITION_VARIABLE cond_window;
	CONDITION_VARIABLE cond_render;
	SRWLOCK lock_block;
	SRWLOCK lock_window;
	SRWLOCK lock_render;
	bool block;
	bool window;
	bool render;

	// connection
	bool closed;
	HWND event_handle;

	// win
	HICON icon_32;
	HICON icon_64;
	enum win_dpi_api dpi;

	// window
	HMODULE win_module;
	WNDCLASSEX win_class;
	LPWSTR win_name;
	HCURSOR default_cursor;
	WINDOWPLACEMENT win_placement;
	bool sizemove;

	// render handling
	HANDLE thread_render;
	struct win_thread_render_loop_data thread_render_loop_data;
	void (*render_init_callback)(struct win_thread_render_loop_data* data);

	// event handling
	HANDLE thread_event;
	struct win_thread_event_loop_data thread_event_loop_data;
	void (*event_init_callback)(struct win_thread_render_loop_data* data);
};

void globox_win_common_init(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void globox_win_common_clean(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void globox_win_common_window_create(
	struct globox* context,
	struct win_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	void (*callback)(struct globox_config_reply* replies, size_t count, void* data),
	void* data,
	struct globox_error_info* error);

void globox_win_common_window_destroy(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void globox_win_common_window_confirm(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void globox_win_common_window_start(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void globox_win_common_window_block(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void globox_win_common_window_stop(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);


void globox_win_common_init_render(
	struct globox* context,
	struct win_platform* platform,
	struct globox_config_render* config,
	struct globox_error_info* error);

void globox_win_common_init_events(
	struct globox* context,
	struct win_platform* platform,
	struct globox_config_events* config,
	struct globox_error_info* error);

enum globox_event globox_win_common_handle_events(
	struct globox* context,
	struct win_platform* platform,
	void* event,
	struct globox_error_info* error);


struct globox_config_features* globox_win_common_init_features(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void globox_win_common_feature_set_interaction(
	struct globox* context,
	struct win_platform* platform,
	struct globox_feature_interaction* config,
	struct globox_error_info* error);

void globox_win_common_feature_set_state(
	struct globox* context,
	struct win_platform* platform,
	struct globox_feature_state* config,
	struct globox_error_info* error);

void globox_win_common_feature_set_title(
	struct globox* context,
	struct win_platform* platform,
	struct globox_feature_title* config,
	struct globox_error_info* error);

void globox_win_common_feature_set_icon(
	struct globox* context,
	struct win_platform* platform,
	struct globox_feature_icon* config,
	struct globox_error_info* error);


unsigned globox_win_common_get_width(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

unsigned globox_win_common_get_height(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

struct globox_rect globox_win_common_get_expose(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

#endif
