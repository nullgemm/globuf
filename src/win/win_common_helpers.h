#ifndef H_GLOBUF_INTERNAL_WIN_COMMON_HELPERS
#define H_GLOBUF_INTERNAL_WIN_COMMON_HELPERS

#include "include/globuf.h"
#include "win/win_common.h"

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

unsigned __stdcall globuf_win_helpers_render_loop(void* data);

unsigned __stdcall globuf_win_helpers_event_loop(void* data);

LRESULT CALLBACK globuf_win_helpers_window_procedure(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

void globuf_win_helpers_features_init(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	struct globuf_error_info* error);

void globuf_win_helpers_set_state(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

LPWSTR globuf_win_helpers_utf8_to_wchar(const char* string);

HICON globuf_win_helpers_bitmap_to_icon(
	struct globuf* context,
	struct win_platform* platform,
	BITMAP* bmp,
	struct globuf_error_info* error);

void globuf_win_helpers_save_window_state(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

enum win_dpi_api globuf_win_helpers_set_dpi_awareness();

void globuf_win_helpers_set_title(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_helpers_set_icon(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_helpers_set_frame(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_helpers_set_background(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

void globuf_win_helpers_set_vsync(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error);

#ifdef GLOBUF_ERROR_HELPER_WIN
void globuf_win_helpers_win32_error_log();
#endif

#endif
