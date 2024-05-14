#ifndef H_GLOBUF_INTERNAL_WIN_COMMON
#define H_GLOBUF_INTERNAL_WIN_COMMON

#include "include/globuf.h"

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
	struct globuf* globuf;
	struct win_platform* platform;
	struct globuf_error_info* error;
};

struct win_thread_event_loop_data
{
	struct globuf* globuf;
	struct win_platform* platform;
	struct globuf_error_info* error;
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

void globuf_win_common_init(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_common_clean(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_common_window_create(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error);

void globuf_win_common_window_destroy(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_common_window_confirm(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_common_window_start(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_common_window_block(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_common_window_stop(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);


void globuf_win_common_init_render(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_config_render* config,
	struct globuf_error_info* error);

void globuf_win_common_init_events(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_config_events* config,
	struct globuf_error_info* error);

enum globuf_event globuf_win_common_handle_events(
	struct globuf* context,
	struct win_platform* platform,
	void* event,
	struct globuf_error_info* error);


struct globuf_config_features* globuf_win_common_init_features(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_common_feature_set_interaction(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error);

void globuf_win_common_feature_set_state(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_feature_state* config,
	struct globuf_error_info* error);

void globuf_win_common_feature_set_title(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_feature_title* config,
	struct globuf_error_info* error);

void globuf_win_common_feature_set_icon(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error);


unsigned globuf_win_common_get_width(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

unsigned globuf_win_common_get_height(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

struct globuf_rect globuf_win_common_get_expose(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

#endif
