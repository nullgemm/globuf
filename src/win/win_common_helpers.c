#include "include/globox.h"
#include "common/globox_private.h"
#include "win/win_common.h"
#include "win/win_common_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dwmapi.h>
#include <process.h>
#include <shellscalingapi.h>
#include <windows.h>
#include <winuser.h>

#ifdef GLOBOX_ERROR_HELPER_WIN
#include <errhandlingapi.h>
#endif

unsigned __stdcall win_helpers_render_loop(void* data)
{
	struct win_thread_render_loop_data* thread_render_loop_data = data;

	struct globox* context = thread_render_loop_data->globox;
	struct win_platform* platform = thread_render_loop_data->platform;
	struct globox_error_info* error = thread_render_loop_data->error;
	DWORD main_lock;
	BOOL main_unlock;

	// wait for the window cond
	BOOL ok;
	AcquireSRWLockExclusive(&(platform->lock_render));

	while (platform->render == false)
	{
		ok =
			SleepConditionVariableSRW(
				&(platform->cond_render),
				&(platform->lock_render),
				INFINITE,
				0);

		if (ok == 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WIN_COND_WAIT);
			_endthreadex(0);
			return 1;
		}
	}

	ReleaseSRWLockExclusive(&(platform->lock_render));

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		_endthreadex(0);
		return 1;
	}

	// thread init callback
	if (platform->render_init_callback != NULL)
	{
		platform->render_init_callback(thread_render_loop_data);

		if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
		{
			_endthreadex(0);
			return 1;
		}
	}

	bool closed = platform->closed;

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		_endthreadex(0);
		return 1;
	}

	// spin the render loop
	while (closed == false)
	{
		// run developer callback
		context->render_callback.callback(context->render_callback.data);

		// lock mutex
		main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

		if (main_lock != WAIT_OBJECT_0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
			_endthreadex(0);
			return 1;
		}

		// update boolean
		closed = platform->closed;

		// unlock mutex
		main_unlock = ReleaseMutex(platform->mutex_main);

		if (main_unlock == 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
			_endthreadex(0);
			return 1;
		}
	}

	_endthreadex(0);
	return 0;
}

unsigned __stdcall win_helpers_event_loop(void* data)
{
	struct win_thread_event_loop_data* thread_event_loop_data = data;

	struct globox* context = thread_event_loop_data->globox;
	struct win_platform* platform = thread_event_loop_data->platform;
	struct globox_error_info* error = thread_event_loop_data->error;
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		_endthreadex(0);
		return 1;
	}

	DWORD style = 0;
	DWORD exstyle = 0;

	if (context->feature_frame->frame == true)
	{
		style |= WS_OVERLAPPEDWINDOW;
	}
	else
	{
		style |= WS_POPUP;
		style |= WS_BORDER;
	}

	platform->event_handle =
		CreateWindowExW(
			exstyle,
			platform->win_name,
			platform->win_name,
			style,
			context->feature_pos->x,
			context->feature_pos->y,
			context->feature_size->width,
			context->feature_size->height,
			NULL,
			NULL,
			platform->win_module,
			data);

	if (platform->event_handle == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_WINDOW_CREATE);
		_endthreadex(0);
		return 1;
	}

	ShowWindow(platform->event_handle, SW_SHOWNORMAL);

	if (context->feature_background->background != GLOBOX_BACKGROUND_OPAQUE)
	{
		DWM_BLURBEHIND blur_behind =
		{
			.dwFlags = DWM_BB_ENABLE,
			.fEnable = TRUE,
			.hRgnBlur = NULL,
			.fTransitionOnMaximized = FALSE,
		};

		HRESULT ok_blur =
			DwmEnableBlurBehindWindow(
				platform->event_handle,
				&blur_behind);

		if (ok_blur != S_OK)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WIN_DWM_ENABLE);

			#if defined(GLOBOX_COMPAT_WINE)
				globox_error_ok(error);
			#else
				_endthreadex(0);
				return 1;
			#endif
		}
	}

	BOOL ok_placement =
		GetWindowPlacement(platform->event_handle, &(platform->win_placement));

	if (ok_placement == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_PLACEMENT_GET);
		_endthreadex(0);
		return 1;
	}

	switch (context->feature_state->state)
	{
		case GLOBOX_STATE_FULLSCREEN:
		{
			globox_feature_set_state(context, context->feature_state, error);
			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			ShowWindow(platform->event_handle, SW_SHOWMAXIMIZED);
			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			ShowWindow(platform->event_handle, SW_SHOWMINIMIZED);
			break;
		}
		default:
		{
			break;
		}
	}

	// trigger the window creation cond
	platform->window = true;
	WakeConditionVariable(&(platform->cond_window));

	// start the render thread
	platform->thread_render =
		(HANDLE) _beginthreadex(
			NULL,
			0,
			win_helpers_render_loop,
			(void*) &(platform->thread_render_loop_data),
			0,
			NULL);

	if (platform->thread_render == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_THREAD_RENDER_START);
		_endthreadex(0);
		return 1;
	}

	// start the event loop
	MSG msg;
	BOOL ok_msg = GetMessageW(&msg, NULL, 0, 0);

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		_endthreadex(0);
		return 1;
	}

	while (ok_msg != 0)
	{
		// According to the documentation, the BOOL returned by GetMessage has
		// three states, because it's obviously what bools are made for...
		// -1 means failure
		// 0 means we just received a WM_QUIT message
		// any other value means we received another message
		if (ok_msg == -1)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WIN_MSG_GET);
			_endthreadex(0);
			return 1;
		}

		// dispatch messsage
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		// lock mutex
		main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

		if (main_lock != WAIT_OBJECT_0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
			_endthreadex(0);
			return 1;
		}

		// get next message
		ok_msg = GetMessageW(&msg, NULL, 0, 0);

		// unlock mutex
		main_unlock = ReleaseMutex(platform->mutex_main);

		if (main_unlock == 0)
		{
			globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
			_endthreadex(0);
			return 1;
		}
	}

	globox_error_ok(error);

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
	}

	// make it known this is the end
	if (platform->closed == false)
	{
		context->feature_state->state = GLOBOX_STATE_CLOSED;
		platform->closed = true;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
	}

	// wait for the render thread to finish
	DWORD code = WaitForSingleObject(platform->thread_render, INFINITE);

	if (code == WAIT_FAILED)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_THREAD_WAIT);
	}

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
	}

	// stop the window
	platform->block = true;
	WakeConditionVariable(&(platform->cond_block));

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
	}

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		_endthreadex(0);
		return 1;
	}

	_endthreadex(0);
	return 0;
}

LRESULT CALLBACK win_helpers_window_procedure(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	struct win_thread_event_loop_data* thread_event_loop_data = NULL;
	struct globox* context;
	struct win_platform* platform;
	struct globox_error_info* error;

	// deal with user data, saving it in the window as soon as possible when the
	// first message (WM_CREATE) arrives and getting it from there afterwards
	if (msg == WM_CREATE)
	{
		// save a context pointer in the window
		SetLastError(0);

		BOOL ok =
			SetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA,
				(LONG_PTR) ((CREATESTRUCT*) lParam)->lpCreateParams);

		DWORD code = GetLastError();

		// if we can't save the user data in the window,
		// we can still try to use it to report the error
		if ((ok == 0) && (code != 0))
		{
			thread_event_loop_data =
				(struct win_thread_event_loop_data*)
					((CREATESTRUCT*) lParam)->lpCreateParams;

			if (thread_event_loop_data != NULL)
			{
				context = thread_event_loop_data->globox;
				platform = thread_event_loop_data->platform;
				error = thread_event_loop_data->error;

				globox_error_throw(
					context,
					error,
					GLOBOX_ERROR_WIN_USERDATA_SET);
			}
		}
	}
	else
	{
		// if we can't get the user data from the window then
		// we don't have any way to report the error so we just
		// fail silently after running the default message processor
		thread_event_loop_data =
			(struct win_thread_event_loop_data*)
				GetWindowLongPtrW(hwnd, GWLP_USERDATA);

		if (thread_event_loop_data != NULL)
		{
			context = thread_event_loop_data->globox;
			platform = thread_event_loop_data->platform;
			error = thread_event_loop_data->error;
		}
	}

	// directly send events to DefWindowProc if possible
	// otherwise return success and rely on our own code
	LRESULT result = S_OK;

	switch (msg)
	{
		case WM_ERASEBKGND:
		case WM_PAINT:
		{
			result = S_OK;
			break;
		}
		default:
		{
			result = DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}

	// stop here if we weren't able to get user data
	if (thread_event_loop_data == NULL)
	{
		return result;
	}

	// run developer callback
	MSG event =
	{
		.message = msg,
		.wParam = wParam,
		.lParam = lParam,
	};

	context->event_callbacks.handler(context->event_callbacks.data, &event);

	// handle interactive move and resize
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_LOCK);
		return result;
	}

	if (msg == WM_NCHITTEST)
	{
		switch (context->feature_interaction->action)
		{
			case GLOBOX_INTERACTION_MOVE:
			{
				result = HTCAPTION;
				break;
			}
			case GLOBOX_INTERACTION_N:
			{
				result = HTTOP;
				break;
			}
			case GLOBOX_INTERACTION_NW:
			{
				result = HTTOPLEFT;
				break;
			}
			case GLOBOX_INTERACTION_W:
			{
				result = HTLEFT;
				break;
			}
			case GLOBOX_INTERACTION_SW:
			{
				result = HTBOTTOMLEFT;
				break;
			}
			case GLOBOX_INTERACTION_S:
			{
				result = HTBOTTOM;
				break;
			}
			case GLOBOX_INTERACTION_SE:
			{
				result = HTBOTTOMRIGHT;
				break;
			}
			case GLOBOX_INTERACTION_E:
			{
				result = HTRIGHT;
				break;
			}
			case GLOBOX_INTERACTION_NE:
			{
				result = HTTOPRIGHT;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_MUTEX_UNLOCK);
		return result;
	}

	// pass over the message processor return value
	return result;
}

void win_helpers_features_init(
	struct globox* context,
	struct win_platform* platform,
	struct globox_config_request* configs,
	size_t count,
	struct globox_error_info* error)
{
	for (size_t i = 0; i < count; ++i)
	{
		switch (configs[i].feature)
		{
			case GLOBOX_FEATURE_STATE:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_state) =
						*((struct globox_feature_state*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_TITLE:
			{
				if (configs[i].config != NULL)
				{
					struct globox_feature_title* tmp = configs[i].config;
					context->feature_title->title = strdup(tmp->title);
				}

				break;
			}
			case GLOBOX_FEATURE_ICON:
			{
				if (configs[i].config != NULL)
				{
					struct globox_feature_icon* tmp = configs[i].config;
					context->feature_icon->pixmap = malloc(tmp->len * 4);

					if (context->feature_icon->pixmap != NULL)
					{
						memcpy(
							context->feature_icon->pixmap,
							tmp->pixmap,
							tmp->len * 4);

						context->feature_icon->len = tmp->len;
					}
					else
					{
						context->feature_icon->len = 0;
					}
				}

				break;
			}
			case GLOBOX_FEATURE_SIZE:
			{
				// handled directly in xcb's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_size) =
						*((struct globox_feature_size*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_POS:
			{
				// handled directly in xcb's window creation code
				if (configs[i].config != NULL)
				{
					*(context->feature_pos) =
						*((struct globox_feature_pos*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_FRAME:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_frame) =
						*((struct globox_feature_frame*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_BACKGROUND:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_background) =
						*((struct globox_feature_background*)
							configs[i].config);
				}

				break;
			}
			case GLOBOX_FEATURE_VSYNC:
			{
				if (configs[i].config != NULL)
				{
					*(context->feature_vsync) =
						*((struct globox_feature_vsync*)
							configs[i].config);
					context->feature_vsync->vsync = true;
				}

				break;
			}
			default:
			{
				globox_error_throw(context, error, GLOBOX_ERROR_FEATURE_INVALID);
				return;
			}
		}
	}
}

void win_helpers_set_state(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error)
{
	switch (context->feature_state->state)
	{
		case GLOBOX_STATE_MINIMIZED:
		{
			SendMessage(
				platform->event_handle,
				WIN_USER_MSG_MINIMIZE,
				0,
				0);
			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			SendMessage(
				platform->event_handle,
				WIN_USER_MSG_MAXIMIZE,
				0,
				0);
			break;
		}
		case GLOBOX_STATE_FULLSCREEN:
		{
			SendMessage(
				platform->event_handle,
				WIN_USER_MSG_FULLSCREEN,
				0,
				0);
			break;
		}
		default:
		{
			SendMessage(
				platform->event_handle,
				WIN_USER_MSG_REGULAR,
				0,
				0);
			break;
		}
	}

	globox_error_ok(error);
}

LPWSTR win_helpers_utf8_to_wchar(const char* string)
{
	int codepoints =
		MultiByteToWideChar(
			CP_UTF8,
			MB_PRECOMPOSED,
			string,
			-1,
			NULL,
			0);

	if (codepoints == 0)
	{
		return NULL;
	}

	wchar_t* buf = malloc(codepoints * (sizeof (wchar_t)));

	if (buf == NULL)
	{
		return NULL;
	}

	buf[0] = '\0';

	int ok =
		MultiByteToWideChar(
			CP_UTF8,
			MB_PRECOMPOSED,
			string,
			-1,
			buf,
			codepoints);

	if (ok == 0)
	{
		free(buf);
		return NULL;
	}

	return buf;
}

HICON win_helpers_bitmap_to_icon(
	struct globox* context,
	struct win_platform* platform,
	BITMAP* bmp,
	struct globox_error_info* error)
{
	size_t buf_len = (sizeof (uint32_t)) * bmp->bmWidth * bmp->bmHeight;
	uint32_t* buf = malloc(buf_len);

	if (buf == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_ALLOC);
		return NULL;
	}

	memset(buf, 0xffffffff, buf_len);
	HBITMAP mask = CreateBitmap(bmp->bmWidth, bmp->bmHeight, 1, 32, buf);

	if (mask == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_BMP_MASK_CREATE);
		free(buf);
		return NULL;
	}

	HBITMAP color = CreateBitmapIndirect(bmp);

	if (color == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_BMP_COLOR_CREATE);
		free(buf);
		return NULL;
	}

	ICONINFO info =
	{
		.fIcon = TRUE,
		.xHotspot = 0,
		.yHotspot = 0,
		.hbmMask = mask,
		.hbmColor = color,
	};

	HICON icon = CreateIconIndirect(&info);
	free(buf);

	if (icon == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_ICON_CREATE);
		return NULL;
	}

	int ok;

	ok = DeleteObject(mask);

	if (ok == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_OBJECT_DELETE);
		return NULL;
	}

	ok = DeleteObject(color);

	if (ok == 0)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_OBJECT_DELETE);
		return NULL;
	}

	return icon;
}

void win_helpers_save_window_state(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error)
{
	// save window state if relevant
	if (context->feature_state->state == GLOBOX_STATE_REGULAR)
	{
		BOOL ok =
			GetWindowPlacement(
				platform->event_handle,
				&(platform->win_placement));

		if (ok == 0)
		{
			globox_error_throw(
				context,
				error,
				GLOBOX_ERROR_WIN_PLACEMENT_GET);
		}
	}
}

enum win_dpi_api win_helpers_set_dpi_awareness()
{
	// try the Windows 10 API, v2 (available since the "creators update")
	BOOL ok;

	ok =
		SetProcessDpiAwarenessContext(
			DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	if (ok == TRUE)
	{
		return WIN_DPI_API_10_V2;
	}

	// try the Windows 10 API, v1
	ok =
		SetProcessDpiAwarenessContext(
			DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

	if (ok == TRUE)
	{
		return WIN_DPI_API_10_V1;
	}

	// try the Windows 8 API
	HRESULT result =
		SetProcessDpiAwareness(
			PROCESS_PER_MONITOR_DPI_AWARE);

	if (result == S_OK)
	{
		return WIN_DPI_API_8;
	}

	// try the Windows Vista API
	ok = SetProcessDPIAware();

	if (ok != 0)
	{
		return WIN_DPI_API_VISTA;
	}

	return WIN_DPI_API_NONE;
}

void win_helpers_set_title(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error)
{
	platform->win_name =
		win_helpers_utf8_to_wchar(context->feature_title->title);

	if (platform->win_name == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_NAME_SET);
		return;
	}

	globox_error_ok(error);
}

void win_helpers_set_icon(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error)
{
	BITMAP pixmap_32 =
	{
		.bmType = 0,
		.bmWidth = 32,
		.bmHeight = 32,
		.bmWidthBytes = 32 * 4,
		.bmPlanes = 1,
		.bmBitsPixel = 32,
		.bmBits = context->feature_icon->pixmap + 4 + (16 * 16),
	};

	platform->icon_32 =
		win_helpers_bitmap_to_icon(context, platform, &pixmap_32, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	if (platform->icon_32 == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_ICON_SMALL);
		return;
	}

	BITMAP pixmap_64 =
	{
		.bmType = 0,
		.bmWidth = 64,
		.bmHeight = 64,
		.bmWidthBytes = 64 * 4,
		.bmPlanes = 1,
		.bmBitsPixel = 32,
		.bmBits = context->feature_icon->pixmap + 6 + (16 * 16) + (32 * 32),
	};

	platform->icon_64 =
		win_helpers_bitmap_to_icon(context, platform, &pixmap_64, error);

	if (globox_error_get_code(error) != GLOBOX_ERROR_OK)
	{
		return;
	}

	if (platform->icon_64 == NULL)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_ICON_BIG);
		return;
	}

	globox_error_ok(error);
}

void win_helpers_set_frame(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

void win_helpers_set_background(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error)
{
	if (context->feature_background->background == GLOBOX_BACKGROUND_BLURRED)
	{
		globox_error_throw(context, error, GLOBOX_ERROR_WIN_BACKGROUND_BLUR);
		return;
	}

	globox_error_ok(error);
}

void win_helpers_set_vsync(
	struct globox* context,
	struct win_platform* platform,
	struct globox_error_info* error)
{
	globox_error_ok(error);
}

#ifdef GLOBOX_ERROR_HELPER_WIN
void win_helpers_win32_error_log()
{
	DWORD error;
	LPVOID message;

	error = GetLastError(); 

	DWORD error_format =
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &message,
			0,
			NULL);

	if (error_format == 0)
	{
		fprintf(stderr, "could not get win32 error message\n");
		return;
	}

	fprintf(
		stderr,
		"# Win32 Error Report\n"
		"Error ID: %d\n"
		"Error Message: %s\n",
		error,
		(char*) message);
}
#endif
