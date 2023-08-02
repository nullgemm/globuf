#ifndef H_GLOBOX_INTERNAL_WIN_COMMON_HELPERS
#define H_GLOBOX_INTERNAL_WIN_COMMON_HELPERS

#include "include/globox.h"
#include "win/win_common.h"

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

unsigned __stdcall win_helpers_render_loop(void* data);

unsigned __stdcall win_helpers_event_loop(void* data);

LRESULT CALLBACK win_helpers_window_procedure(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

void win_helpers_features_init(
	struct globox* context,
	struct win_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error);

void win_helpers_set_state(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

LPWSTR win_helpers_utf8_to_wchar(const char* string);

HICON win_helpers_bitmap_to_icon(
	struct globox* context,
	struct win_platform* platform,
	BITMAP* bmp,
	struct globox_error_info* error);

void win_helpers_save_window_state(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

enum win_dpi_api win_helpers_set_dpi_awareness();

void win_helpers_set_title(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void win_helpers_set_icon(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void win_helpers_set_frame(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void win_helpers_set_background(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

void win_helpers_set_vsync(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error);

#ifdef GLOBOX_ERROR_HELPER_WIN
void win_helpers_win32_error_log(
	struct globox* context,
	struct win_platform* platform);
#endif

#endif
