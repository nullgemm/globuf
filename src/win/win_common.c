#include "include/globuf.h"
#include "include/globuf_win.h"
#include "common/globuf_private.h"
#include "win/win_common.h"
#include "win/win_common_helpers.h"

#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <synchapi.h>
#include <windows.h>
#include <errhandlingapi.h>

static inline void free_check(const void* ptr)
{
	if (ptr != NULL)
	{
		free((void*) ptr);
	}
}

void globuf_win_common_init(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	// create win32 mutexes
	// main mutex
	platform->mutex_main = CreateMutexW(NULL, FALSE, NULL);

	if (platform->mutex_main == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_CREATE);
		return;
	}

	// create win32 cond variables
	// window init cond
	InitializeConditionVariable(&(platform->cond_window));
	InitializeSRWLock(&(platform->lock_window));
	platform->window = false;

	// render init cond
	InitializeConditionVariable(&(platform->cond_render));
	InitializeSRWLock(&(platform->lock_render));
	platform->render = false;

	// block cond
	InitializeConditionVariable(&(platform->cond_block));
	InitializeSRWLock(&(platform->lock_block));
	platform->block = false;

	// initialize the rest
	platform->closed = false;
	platform->event_handle = NULL;

	platform->icon_32 = NULL;
	platform->icon_64 = NULL;
	platform->dpi = globuf_win_helpers_set_dpi_awareness();

	platform->win_module = NULL;

	WNDCLASSEXW win_class = {0};
	platform->win_class = win_class;

	platform->win_name = NULL;
	platform->default_cursor = NULL;

	WINDOWPLACEMENT win_placement = {0};
	platform->win_placement = win_placement;

	platform->sizemove = false;

	// initialize threads
	// initialize render thread
	struct win_thread_render_loop_data thread_render_loop_data =
	{
		.globuf = NULL,
		.platform = NULL,
		.error = NULL,
	};
	platform->thread_render = NULL;
	platform->thread_render_loop_data = thread_render_loop_data;
	platform->render_init_callback = NULL;

	// initialize event thread
	struct win_thread_event_loop_data thread_event_loop_data =
	{
		.globuf = NULL,
		.platform = NULL,
		.error = NULL,
	};
	platform->thread_event = NULL;
	platform->thread_event_loop_data = thread_event_loop_data;
	platform->event_init_callback = NULL;

	globuf_error_ok(error);
}

void globuf_win_common_clean(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	BOOL ok;

	ok = CloseHandle(platform->mutex_main);

	if (ok == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_DESTROY);
		return;
	}

	if (context->feature_title != NULL)
	{
		free_check(context->feature_title->title);
	}

	if (context->feature_icon != NULL)
	{
		free_check(context->feature_icon->pixmap);
	}

	free_check(context->feature_interaction);
	free_check(context->feature_state);
	free_check(context->feature_title);
	free_check(context->feature_icon);
	free_check(context->feature_size);
	free_check(context->feature_pos);
	free_check(context->feature_frame);
	free_check(context->feature_background);
	free_check(context->feature_vsync);

	globuf_error_ok(error);
}

void globuf_win_common_window_create(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error)
{
	// configure features
	struct globuf_config_reply* reply =
		malloc(count * (sizeof (struct globuf_config_reply)));

	if (reply == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		return;
	}

	for (size_t i = 0; i < count; ++i)
	{
		enum globuf_feature feature = configs[i].feature;
		reply[i].feature = feature;

		switch (feature)
		{
			case GLOBUF_FEATURE_TITLE:
			{
				globuf_win_helpers_set_title(context, platform, &reply[i].error);
				break;
			}
			case GLOBUF_FEATURE_ICON:
			{
				globuf_win_helpers_set_icon(context, platform, &reply[i].error);
				break;
			}
			case GLOBUF_FEATURE_BACKGROUND:
			{
				globuf_win_helpers_set_background(context, platform, &reply[i].error);
				break;
			}
			default:
			{
				reply[i].error.code = GLOBUF_ERROR_OK;
				reply[i].error.file = NULL;
				reply[i].error.line = 0;
				break;
			}
		}
	}

	callback(reply, count, data);
	free(reply);

	// get window class module
	platform->win_module = GetModuleHandleW(NULL);

	if (platform->win_module == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MODULE_GET);
		return;
	}

	// load default mouse cursor
	platform->default_cursor = LoadCursorW(NULL, IDC_ARROW);

	if (platform->default_cursor == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_CURSOR_LOAD);
		return;
	}

	// register window class
	WNDCLASSEXW win_class =
	{
		.cbSize = sizeof (WNDCLASSEXW),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = globuf_win_helpers_window_procedure,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = platform->win_module,
		.hIcon = platform->icon_32,
		.hCursor = NULL,
		.hbrBackground = NULL,
		.lpszMenuName = NULL,
		.lpszClassName = platform->win_name,
		.hIconSm = platform->icon_32,
	};

	platform->win_class = win_class;

	ATOM atom = RegisterClassExW(&(platform->win_class));

	if (atom == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_CLASS_CREATE);
	}

	globuf_error_ok(error);
}

void globuf_win_common_window_destroy(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	free_check(platform->win_name);

	globuf_error_ok(error);
}

void globuf_win_common_window_confirm(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	globuf_error_ok(error);
}

void globuf_win_common_window_start(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// start the event loop in a new thread
	// init thread function data
	struct win_thread_event_loop_data event_data =
	{
		.globuf = context,
		.platform = platform,
		.error = error,
	};

	// init render function data
	struct win_thread_render_loop_data render_data =
	{
		.globuf = context,
		.platform = platform,
		.error = error,
	};

	platform->thread_event_loop_data = event_data;
	platform->thread_render_loop_data = render_data;

	platform->thread_event =
		(HANDLE) _beginthreadex(
			NULL,
			0,
			globuf_win_helpers_event_loop,
			&(platform->thread_event_loop_data),
			0,
			NULL);

	if (platform->thread_event == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_THREAD_EVENT_START);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	// wait for the window cond
	AcquireSRWLockExclusive(&(platform->lock_window));

	while (platform->window == false)
	{
		BOOL ok =
			SleepConditionVariableSRW(
				&(platform->cond_window),
				&(platform->lock_window),
				INFINITE,
				0);

		if (ok == 0)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_WIN_COND_WAIT);
			return;
		}
	}

	ReleaseSRWLockExclusive(&(platform->lock_window));

	// success
	globuf_error_ok(error);
}

void globuf_win_common_window_block(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	// wait for the block cond
	AcquireSRWLockExclusive(&(platform->lock_block));

	while (platform->block == false)
	{
		BOOL ok =
			SleepConditionVariableSRW(
				&(platform->cond_block),
				&(platform->lock_block),
				INFINITE,
				0);

		if (ok == 0)
		{
			globuf_error_throw(context, error, GLOBUF_ERROR_WIN_COND_WAIT);
			return;
		}
	}

	ReleaseSRWLockExclusive(&(platform->lock_block));

	// wait for the event thread to finish
	DWORD code = WaitForSingleObject(platform->thread_event, INFINITE);

	if (code == WAIT_FAILED)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_THREAD_WAIT);
		return;
	}

	// success
	globuf_error_ok(error);
}

void globuf_win_common_window_stop(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	BOOL ok;

	ok = CloseHandle(platform->thread_render);

	if (ok == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_THREAD_RENDER_CLOSE);
		return;
	}

	ok = CloseHandle(platform->thread_event);

	if (ok == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_THREAD_EVENT_CLOSE);
		return;
	}

	// TODO more

	globuf_error_ok(error);
}


void globuf_win_common_init_render(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_config_render* config,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// set the event callback
	context->render_callback = *config;

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_win_common_init_events(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_config_events* config,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// set the event callback
	context->event_callbacks = *config;

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

enum globuf_event globuf_win_common_handle_events(
	struct globuf* context,
	struct win_platform* platform,
	void* event,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return GLOBUF_EVENT_INVALID;
	}

	// process system events
	enum globuf_event globuf_event = GLOBUF_EVENT_UNKNOWN;
	MSG* msg = event;

	// only lock the main mutex when making changes to the context
	switch (msg->message)
	{
		case WM_DESTROY:
		{
			// make it known this is the end
			platform->closed = true;
			context->feature_state->state = GLOBUF_STATE_CLOSED;

			PostQuitMessage(0);
			globuf_event = GLOBUF_EVENT_CLOSED;

			break;
		}
		case WM_PAINT:
		{
			RECT region;
			BOOL ok = GetUpdateRect(platform->event_handle, &region, FALSE);

			if (ok != 0)
			{
				context->expose.x = region.left;
				context->expose.y = region.top;
				context->expose.width = (region.right - region.left);
				context->expose.height = (region.bottom - region.top);
				globuf_event = GLOBUF_EVENT_DAMAGED;
			}

			break;
		}
		case WIN_USER_MSG_FULLSCREEN:
		{
			BOOL ok;
			DWORD code;
			DWORD style;

			// set style hints
			SetLastError(0);
			style = WS_POPUP | WS_VISIBLE;
			ok = SetWindowLongPtrW(platform->event_handle, GWL_STYLE, style);
			code = GetLastError();

			if ((ok == 0) && (code != 0))
			{
				globuf_error_throw(context, error, GLOBUF_ERROR_WIN_STYLE_SET);
				break;
			}

			// get monitor
			HMONITOR monitor =
					MonitorFromWindow(
						platform->event_handle,
						MONITOR_DEFAULTTONEAREST);

			// get monitor info
			MONITORINFO info =
			{
				.cbSize = (sizeof (MONITORINFO)),
			};

			ok = GetMonitorInfo(monitor, &info);

			if (ok == 0)
			{
				globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MONITOR_GET);
				break;
			}

			// set window size and position
			ok =
				SetWindowPos(
					platform->event_handle,
					HWND_TOPMOST,
					info.rcMonitor.left,
					info.rcMonitor.top,
					info.rcMonitor.right - info.rcMonitor.left,
					info.rcMonitor.bottom - info.rcMonitor.top,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

			if (ok == 0)
			{
				globuf_error_throw(
					context,
					error,
					GLOBUF_ERROR_WIN_WINDOW_POSITION_SET);
				break;
			}

			globuf_event = GLOBUF_EVENT_FULLSCREEN;
			break;
		}
		case WIN_USER_MSG_MAXIMIZE:
		{
			BOOL ok;
			DWORD style = 0;

			if (context->feature_frame->frame == true)
			{
				style |= WS_OVERLAPPEDWINDOW;
			}
			else
			{
				style |= WS_POPUP;
				style |= WS_BORDER;
			}

			SetLastError(0);
			ok = SetWindowLongW(platform->event_handle, GWL_STYLE, style);
			DWORD code = GetLastError();

			if ((ok == 0) && (code != 0))
			{
				globuf_error_throw(context, error, GLOBUF_ERROR_WIN_STYLE_SET);
				break;
			}

			if (context->feature_frame->frame == true)
			{
				ShowWindow(platform->event_handle, SW_SHOWMAXIMIZED);
			}
			else
			{
				HMONITOR monitor =
					MonitorFromWindow(
						platform->event_handle,
						MONITOR_DEFAULTTONEAREST);

				MONITORINFO info =
				{
					.cbSize = sizeof (MONITORINFO),
				};

				ok = GetMonitorInfoW(monitor, &info);

				if (ok == 0)
				{
					globuf_error_throw(
						context,
						error,
						GLOBUF_ERROR_WIN_MONITOR_INFO_GET);
					break;
				}

				RECT rect = info.rcWork;
				LONG width = rect.right - rect.left;
				LONG height = rect.bottom - rect.top;

				ok =
					SetWindowPos(
						platform->event_handle,
						HWND_TOP,
						0,
						0,
						width,
						height,
						SWP_SHOWWINDOW);

				if (ok == 0)
				{
					globuf_error_throw(
						context,
						error,
						GLOBUF_ERROR_WIN_WINDOW_POSITION_SET);
					break;
				}
			}

			break;
		}
		case WIN_USER_MSG_MINIMIZE:
		{
			DWORD style = 0;

			if (context->feature_frame->frame == true)
			{
				style |= WS_OVERLAPPEDWINDOW;
			}
			else
			{
				style |= WS_POPUP;
				style |= WS_BORDER;
			}

			SetLastError(0);
			BOOL ok = SetWindowLongW(platform->event_handle, GWL_STYLE, style);
			DWORD code = GetLastError();

			if ((ok == 0) && (code != 0))
			{
				globuf_error_throw(context, error, GLOBUF_ERROR_WIN_STYLE_SET);
				break;
			}

			ShowWindow(platform->event_handle, SW_SHOWMINIMIZED);

			break;
		}
		case WIN_USER_MSG_REGULAR:
		{
			BOOL ok;
			DWORD style = 0;

			if (context->feature_frame->frame == true)
			{
				style |= WS_OVERLAPPEDWINDOW;
			}
			else
			{
				style |= WS_POPUP;
				style |= WS_BORDER;
			}

			SetLastError(0);
			ok = SetWindowLongW(platform->event_handle, GWL_STYLE, style);
			DWORD code = GetLastError();

			if ((ok == 0) && (code != 0))
			{
				globuf_error_throw(
					context,
					error,
					GLOBUF_ERROR_WIN_STYLE_SET);
				break;
			}

			ok =
				SetWindowPlacement(
					platform->event_handle,
					&(platform->win_placement));

			if (ok == 0)
			{
				globuf_error_throw(
					context,
					error,
					GLOBUF_ERROR_WIN_PLACEMENT_SET);
				break;
			}

			break;
		}
		case WM_SYSCOMMAND:
		{
			switch (msg->wParam)
			{
				case SC_RESTORE:
				{
					context->feature_state->state = GLOBUF_STATE_REGULAR;
					globuf_event = GLOBUF_EVENT_RESTORED;
					break;
				}
				case SC_MAXIMIZE:
				{
					globuf_win_helpers_save_window_state(context, platform, error);

					if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
					{
						break;
					}

					context->feature_state->state = GLOBUF_STATE_MAXIMIZED;
					globuf_event = GLOBUF_EVENT_MAXIMIZED;
					break;
				}
				case SC_MINIMIZE:
				{
					globuf_win_helpers_save_window_state(context, platform, error);

					if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
					{
						break;
					}

					context->feature_state->state = GLOBUF_STATE_MINIMIZED;
					globuf_event = GLOBUF_EVENT_MINIMIZED;
					break;
				}
				default:
				{
					break;
				}
			}

			break;
		}
		case WM_ENTERSIZEMOVE:
		{
			platform->sizemove = true;
			break;
		}
		case WM_EXITSIZEMOVE:
		{
			if (context->feature_interaction->action != GLOBUF_INTERACTION_STOP)
			{
				context->feature_interaction->action = GLOBUF_INTERACTION_STOP;
			}

			platform->sizemove = false;
			break;
		}
		case WM_MOVING:
		case WM_SIZING:
		{
			if (context->feature_state->state != GLOBUF_STATE_REGULAR)
			{
				context->feature_state->state = GLOBUF_STATE_REGULAR;
				globuf_event = GLOBUF_EVENT_RESTORED;
			}
			else
			{
				globuf_event = GLOBUF_EVENT_MOVED_RESIZED;
			}

			break;
		}
		case WM_MOVE:
		{
			globuf_event = GLOBUF_EVENT_MOVED_RESIZED;
			break;
		}
		case WM_SIZE:
		{
			context->feature_size->width = LOWORD(msg->lParam);
			context->feature_size->height = HIWORD(msg->lParam);
			globuf_event = GLOBUF_EVENT_MOVED_RESIZED;
			break;
		}
		default:
		{
			break;
		}
	}

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return GLOBUF_EVENT_INVALID;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return globuf_event;
	}

	globuf_error_ok(error);
	return globuf_event;
}

struct globuf_config_features*
	globuf_win_common_init_features(
		struct globuf* context,
		struct win_platform* platform,
		struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return NULL;
	}

	struct globuf_config_features* features =
		malloc(sizeof (struct globuf_config_features));

	if (features == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	features->count = 0;
	features->list =
		malloc(GLOBUF_FEATURE_COUNT * (sizeof (enum globuf_feature)));

	if (features->list == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_INTERACTION;
	context->feature_interaction =
		malloc(sizeof (struct globuf_feature_interaction));
	features->count += 1;

	if (context->feature_interaction == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	context->feature_interaction->action = GLOBUF_INTERACTION_STOP;

	// always available
	features->list[features->count] = GLOBUF_FEATURE_STATE;
	context->feature_state =
		malloc(sizeof (struct globuf_feature_state));
	features->count += 1;

	if (context->feature_state == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_TITLE;
	context->feature_title =
		malloc(sizeof (struct globuf_feature_title));
	features->count += 1;

	if (context->feature_title == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_ICON;
	context->feature_icon =
		malloc(sizeof (struct globuf_feature_icon));
	features->count += 1;

	if (context->feature_icon == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_SIZE;
	context->feature_size =
		malloc(sizeof (struct globuf_feature_size));
	features->count += 1;

	if (context->feature_size == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_POS;
	context->feature_pos =
		malloc(sizeof (struct globuf_feature_pos));
	features->count += 1;

	if (context->feature_pos == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_FRAME;
	context->feature_frame =
		malloc(sizeof (struct globuf_feature_frame));
	features->count += 1;

	if (context->feature_frame == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_BACKGROUND;
	context->feature_background =
		malloc(sizeof (struct globuf_feature_background));
	features->count += 1;

	if (context->feature_background == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// always available
	features->list[features->count] = GLOBUF_FEATURE_VSYNC;
	context->feature_vsync =
		malloc(sizeof (struct globuf_feature_vsync));
	features->count += 1;

	if (context->feature_vsync == NULL)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_ALLOC);
		ReleaseMutex(platform->mutex_main);
		return NULL;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return NULL;
	}

	globuf_error_ok(error);
	return features;
}

void globuf_win_common_feature_set_interaction(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// not supported while resizing
	if (platform->sizemove == true)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_INTERACTION_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// configure
	*(context->feature_interaction) = *config;

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_win_common_feature_set_state(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_feature_state* config,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// not supported while resizing
	if (platform->sizemove == true)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_STATE_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	globuf_win_helpers_save_window_state(context, platform, error);

	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// configure
	*(context->feature_state) = *config;
	globuf_win_helpers_set_state(context, platform, error);

	// return on configuration error
	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_win_common_feature_set_title(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_feature_title* config,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// configure
	free_check(context->feature_title->title);

	context->feature_title->title = strdup(config->title);

	free_check(platform->win_name);

	platform->win_name =
		globuf_win_helpers_utf8_to_wchar(context->feature_title->title);

	BOOL ok = SetWindowTextW(platform->event_handle, platform->win_name);

	if (ok == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_TITLE_SET);
		ReleaseMutex(platform->mutex_main);
		return;
	}

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

void globuf_win_common_feature_set_icon(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return;
	}

	// configure
	free_check(context->feature_icon->pixmap);

	context->feature_icon->pixmap = malloc(config->len * 4);

	if (context->feature_icon->pixmap != NULL)
	{
		memcpy(context->feature_icon->pixmap, config->pixmap, config->len * 4);
		context->feature_icon->len = config->len;
	}
	else
	{
		context->feature_icon->len = 0;
	}

	globuf_win_helpers_set_icon(context, platform, error);

	// return on configuration error
	if (globuf_error_get_code(error) != GLOBUF_ERROR_OK)
	{
		ReleaseMutex(platform->mutex_main);
		return;
	}

	SendMessage(
		platform->event_handle,
		WM_SETICON,
		ICON_SMALL,
		(LPARAM) platform->icon_32);

	SendMessage(
		platform->event_handle,
		WM_SETICON,
		ICON_BIG,
		(LPARAM) platform->icon_64);

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return;
	}

	globuf_error_ok(error);
}

unsigned globuf_win_common_get_width(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->width;

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return value;
	}

	// return value
	globuf_error_ok(error);
	return value;
}

unsigned globuf_win_common_get_height(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return 0;
	}

	// save value
	unsigned value = context->feature_size->height;

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return value;
	}

	// return value
	globuf_error_ok(error);
	return value;
}

struct globuf_rect globuf_win_common_get_expose(
	struct globuf* context,
	struct win_platform* platform,
	struct globuf_error_info* error)
{
	struct globuf_rect dummy =
	{
		.x = 0,
		.y = 0,
		.width = 0,
		.height = 0,
	};

	DWORD main_lock;
	BOOL main_unlock;

	// lock mutex
	main_lock = WaitForSingleObject(platform->mutex_main, INFINITE);

	if (main_lock != WAIT_OBJECT_0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_LOCK);
		return dummy;
	}

	// save value
	struct globuf_rect value = context->expose;

	// unlock mutex
	main_unlock = ReleaseMutex(platform->mutex_main);

	if (main_unlock == 0)
	{
		globuf_error_throw(context, error, GLOBUF_ERROR_WIN_MUTEX_UNLOCK);
		return value;
	}

	// return value
	globuf_error_ok(error);
	return value;
}
