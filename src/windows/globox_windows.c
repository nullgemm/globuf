/// this file implements the main globox functions for the windows platform
/// it is completely isolated from the graphic context functions

// include globox structures and functions
#include "globox.h"
#include "globox_error.h"
// windows includes
#include <windows.h>

// include platform structures
#include "windows/globox_windows.h"

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_WINDOWPOSCHANGING:
		case WM_WINDOWPOSCHANGED:
		{
			WINDOWPOS* win = (WINDOWPOS*) lParam;
			LPARAM new_param = MAKELPARAM(win->cx, win->cy);

			if ((win->cx != 0) && (win->cy != 0))
			{
				PostMessage(hwnd, WM_SIZE, wParam, new_param);
			}

			break;
		}
		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_MAXIMIZE:
				case SC_MINIMIZE:
				case SC_RESTORE:
				{
					PostMessage(hwnd, msg, wParam, lParam);
					break;
				}
			}

			break;
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LPWSTR utf8_to_wchar(const char* s)
{
	size_t codepoint_count =
		MultiByteToWideChar(
			CP_UTF8,        // codepage
			MB_PRECOMPOSED, // character composition choice
			s,              // input buffer
			-1,             // input buffer size (-1 = auto-compute)
			NULL,           // output buffer
			0);             // output buffer size (0 = do not use)

	if (codepoint_count == 0)
	{
		return NULL;
	}

	wchar_t* buf = malloc(codepoint_count * (sizeof (wchar_t)));

	if (buf == NULL)
	{
		return NULL;
	}

	buf[0] = '\0';

	int ok =
		MultiByteToWideChar(
			CP_UTF8,
			MB_PRECOMPOSED,
			s,
			-1,
			buf,
			codepoint_count);

	if (ok == 0)
	{
		return NULL;
	}

	return buf;
}

HICON bitmap_to_icon(struct globox* globox, BITMAP* bmp)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	// get a device context handle
	HDC hdc = GetDC(platform->globox_platform_event_handle);

	if (hdc == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET);
		return NULL;
	}

	// create a bitmap mask compatible with the device context
	HBITMAP mask =
		CreateCompatibleBitmap(
			hdc,
			bmp->bmWidth,
			bmp->bmHeight);

	if (mask == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_MASK_CREATE);
		return NULL;
	}

	// create a bitmap
	HBITMAP hbm = CreateBitmapIndirect(bmp);

	if (hbm == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_BITMAP_CREATE);
		return NULL;
	}

	// create icon info
	ICONINFO info =
	{
		.fIcon = TRUE,
		.xHotspot = 0, // ignored for icons
		.yHotspot = 0, // ignored for icons
		.hbmMask = mask,
		.hbmColor = hbm,
	};

	HICON icon = CreateIconIndirect(&info);

	if (icon == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_ICON_CREATE);
		return NULL;
	}

	// cleanup
	int ok;

	// delete the bitmap mask
	ok = DeleteObject(mask);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DELETE_MASK);
		return NULL;
	}

	// release the device context handle
	ok = ReleaseDC(platform->globox_platform_event_handle, hdc);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_REMOVE);
		return NULL;
	}

	return icon;
}

void query_pointer(struct globox* globox)
{
	// TODO
}

// initalize the display system
void globox_platform_init(
	struct globox* globox,
	bool transparent,
	bool frameless,
	bool blurred)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	char** log = globox->globox_log;

	globox->globox_redraw = false;
	globox->globox_transparent = transparent;
	globox->globox_frameless = frameless;
	globox->globox_blurred = blurred;

	log[GLOBOX_ERROR_WINDOWS_UTF8_WCHAR] =
		"could not convert UTF-8";
	log[GLOBOX_ERROR_WINDOWS_MODULE_HANDLE] =
		"could not get the application module handle";
	log[GLOBOX_ERROR_WINDOWS_DEFAULT_CURSOR] =
		"could not get a default cursor";
	log[GLOBOX_ERROR_WINDOWS_REGISTER_CLASS] =
		"could not register window class";
	log[GLOBOX_ERROR_WINDOWS_CREATE_WINDOW] =
		"could not create window";
	log[GLOBOX_ERROR_WINDOWS_UPDATE_WINDOW] =
		"could not update window";
	log[GLOBOX_ERROR_WINDOWS_DELETE_BMP_HANDLE] =
		"could not delete bitmap handle";
	log[GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET] =
		"could not get a device context handle";
	log[GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_REMOVE] =
		"could not destroy the device context handle";
	log[GLOBOX_ERROR_WINDOWS_CREATE_DIB_SECTION] =
		"could not create a device-independent bitmap";
	log[GLOBOX_ERROR_WINDOWS_DAMAGE] =
		"could not damage the window area";
	log[GLOBOX_ERROR_WINDOWS_PAINT] =
		"could not start painting the window area";
	log[GLOBOX_ERROR_WINDOWS_BITBLT] =
		"could not blit the surface buffer";
	log[GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_CREATE] =
		"could not create a compatible device context";
	log[GLOBOX_ERROR_WINDOWS_SELECT_BITMAP_HANDLE] =
		"could not select the bitmap handle";
	log[GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_DELETE] =
		"could not delete the compatible device context";
	log[GLOBOX_ERROR_WINDOWS_MASK_CREATE] =
		"could not create the bitmap mask";
	log[GLOBOX_ERROR_WINDOWS_BITMAP_CREATE] =
		"could not create the bitmap";
	log[GLOBOX_ERROR_WINDOWS_ICON_CREATE] =
		"could not create the icon";
	log[GLOBOX_ERROR_WINDOWS_DELETE_MASK] =
		"could not delete the bitmap mask";
	log[GLOBOX_ERROR_WINDOWS_GET_MESSAGE] =
		"could not peek message";
	log[GLOBOX_ERROR_WINDOWS_TITLE] =
		"could not set window title";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET] =
		"could not restore the window style";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET] =
		"could not save the window style";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_PLACEMENT_SET] =
		"could not restore the window position";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_PLACEMENT_GET] =
		"could not save the window position";
	log[GLOBOX_ERROR_WINDOWS_DESTROY] =
		"could not destroy window";
	log[GLOBOX_ERROR_WINDOWS_ADJUST_WINDOW] =
		"could not compute the window size";
	log[GLOBOX_ERROR_WINDOWS_CLIENT_RECT_GET] =
		"could not get the client area rectangle";

	// save class name
	platform->globox_windows_class_name =
		utf8_to_wchar(globox->globox_title);

	if (platform->globox_windows_class_name == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_UTF8_WCHAR);
		return;
	}

	// save title
	platform->globox_windows_wide_title =
		wcsdup(platform->globox_windows_class_name);

	if (platform->globox_windows_wide_title == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	// save class window procedure owner
	platform->globox_windows_class_module_handle =
		GetModuleHandle(NULL);

	if (platform->globox_windows_class_module_handle == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_MODULE_HANDLE);
		return;
	}

	// prepare the default cursor
	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);

	if (cursor == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DEFAULT_CURSOR);
		return;
	}

	// save class
	WNDCLASSEX class =
	{
		.cbSize = sizeof (platform->globox_windows_class),
		.style = CS_HREDRAW | CS_VREDRAW,     // redraw on size changes
		.lpfnWndProc = window_procedure,      // window procedure
		.cbClsExtra = 0, // extra bytes after the window-class structure
		.cbWndExtra = 0, // extra bytes after the window instance
		.hInstance = platform->globox_windows_class_module_handle,
		.hIcon = NULL,                        // default icon
		.hCursor = cursor,                    // arrow default cursor
		.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1),
		.lpszMenuName = NULL,                 // no default menu
		.lpszClassName = platform->globox_windows_class_name,
		.hIconSm = NULL,                      // default small icon
	};

	platform->globox_windows_class = class;

	// register class
	int ok = RegisterClassEx(&(platform->globox_windows_class));

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_REGISTER_CLASS);
		return;
	}
}

// create the window
void globox_platform_create_window(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	BOOL ok;

	RECT rect =
	{
		.left = globox->globox_x,
		.top = globox->globox_y,
		.right = globox->globox_x + globox->globox_width,
		.bottom = globox->globox_y + globox->globox_height,
	};

	ok = AdjustWindowRectEx(
		&rect,
		WS_OVERLAPPEDWINDOW,
		FALSE,
		WS_EX_OVERLAPPEDWINDOW);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_ADJUST_WINDOW);
		return;
	}

	globox->globox_width = rect.right - rect.left;
	globox->globox_height = rect.bottom - rect.top;

	platform->globox_platform_event_handle =
		CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,              // extended style
			platform->globox_windows_class_name, // class name
			platform->globox_windows_class_name, // window title
			WS_OVERLAPPEDWINDOW,
			globox->globox_x,
			globox->globox_y,
			globox->globox_width,
			globox->globox_height,
			NULL,  // parent window handle
			NULL,  // window-specific menu handle
			platform->globox_windows_class_module_handle,
			NULL); // custom data included in CREATESTRUCT

	if (platform->globox_platform_event_handle == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_CREATE_WINDOW);
		return;
	}

	ok = UpdateWindow(platform->globox_platform_event_handle);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_UPDATE_WINDOW);
		return;
	}
}

void globox_platform_hooks(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	// platform update
	globox_platform_set_state(globox, globox->globox_state);
}

void globox_platform_commit(struct globox* globox)
{
	// not needed
}

void globox_platform_prepoll(struct globox* globox)
{
	// not needed
}

void globox_platform_events_poll(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	PeekMessage(
		&(platform->globox_windows_msg),
		platform->globox_platform_event_handle,
		0,          // 0 to return all messages
		0,          // 0 to return all messages
		PM_REMOVE); // remove messages from queue after processing
}

// TODO remove this from the API and externalize it into a library
void globox_platform_events_wait(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	BOOL ok =
		GetMessage(
			&(platform->globox_windows_msg),
			platform->globox_platform_event_handle,
			0,  // 0 to return all messages
			0); // 0 to return all messages

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_GET_MESSAGE);
	}
}

void globox_platform_interactive_mode(
	struct globox* globox,
	enum globox_interactive_mode mode)
{
	if ((mode != GLOBOX_INTERACTIVE_STOP)
		&& (globox->globox_interactive_mode != mode))
	{
		query_pointer(globox);

		if (globox_error_catch(globox))
		{
			return;
		}

		globox->globox_interactive_mode = mode;
	}
	else
	{
		globox->globox_interactive_mode = GLOBOX_INTERACTIVE_STOP;
	}

	// TODO
}

void globox_platform_events_handle(
	struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	bool transmission = true;
	BOOL ok;

	switch (platform->globox_windows_msg.message)
	{
		case WM_SYSCOMMAND:
		{
			switch (platform->globox_windows_msg.wParam)
			{
				case SC_MAXIMIZE:
				{
					globox->globox_state = GLOBOX_STATE_MAXIMIZED;
					transmission = false;
					break;
				}
				case SC_MINIMIZE:
				{
					globox->globox_state = GLOBOX_STATE_MINIMIZED;
					transmission = false;
					break;
				}
				case SC_RESTORE:
				{
					globox->globox_state = GLOBOX_STATE_REGULAR;
					transmission = false;
					break;
				}
				case SC_CLOSE:
				{
					ok = DestroyWindow(
						platform->globox_platform_event_handle);

					if (ok == 0)
					{
						globox_error_throw(
							globox,
							GLOBOX_ERROR_WINDOWS_DESTROY);
						break;
					}

					break;
				}
			}

			break;
		}
		case WM_DESTROY:
		{
			// TODO DeleteObject(hbm);
			PostQuitMessage(0);

			break;
		}
		case WM_PAINT:
		{
			if (globox->globox_state != GLOBOX_STATE_MINIMIZED)
			{
				globox->globox_redraw = true;
			}

			break;
		}
		case WM_SIZE:
		{
			if (globox->globox_state != GLOBOX_STATE_MINIMIZED)
			{
				RECT rect;

				ok = GetClientRect(
					platform->globox_platform_event_handle,
					&rect);

				if (ok == 0)
				{
					globox_error_throw(
						globox,
						GLOBOX_ERROR_WINDOWS_CLIENT_RECT_GET);
					break;
				}

				globox->globox_width = rect.right - rect.left;
				globox->globox_height = rect.bottom - rect.top;
				globox->globox_redraw = true;

				platform->globox_windows_resize_callback(globox);
			}

			break;
		}
		default:
		{
			if (globox->globox_event_callback != NULL)
			{
				globox->globox_event_callback(
					&(platform->globox_windows_msg),
					globox->globox_event_callback_data);
			}

			break;
		}
	}

	if (transmission == true)
	{
		TranslateMessage(&(platform->globox_windows_msg));
		DispatchMessage(&(platform->globox_windows_msg));
	}
}

void globox_platform_free(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	// TODO
}

// automagically converts X11 pixmaps to Windows icons
void globox_platform_set_icon(
	struct globox* globox,
	uint32_t* pixmap,
	uint32_t len)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	// smol
	BITMAP pixmap_32 =
	{
		.bmType = 0,            // always 0
		.bmWidth = 32,          // width in pixels
		.bmHeight = 32,         // height in pixels
		.bmWidthBytes = 32 * 4, // bytes per row
		.bmPlanes = 1,          // bitmap planes
		.bmBitsPixel = 32,      // bits per pixel
		.bmBits = pixmap + 4 + (16 * 16),
	};

	HICON icon_32 = bitmap_to_icon(globox, &pixmap_32);

	if (icon_32 == NULL)
	{
		return;
	}
	else
	{
		SendMessage(
			platform->globox_platform_event_handle,
			WM_SETICON,
			ICON_SMALL,
			(LPARAM) icon_32);
	}

	// thicc
	BITMAP pixmap_64 =
	{
		.bmType = 0,            // always 0
		.bmWidth = 64,          // width in pixels
		.bmHeight = 64,         // height in pixels
		.bmWidthBytes = 64 * 4, // bytes per row
		.bmPlanes = 1,          // bitmap planes
		.bmBitsPixel = 32,      // bits per pixel
		.bmBits = pixmap + 4 + (16 * 16) + (32 * 32),
	};

	HICON icon_64 = bitmap_to_icon(globox, &pixmap_64);

	if (icon_64 == NULL)
	{
		return;
	}
	else
	{
		SendMessage(
			platform->globox_platform_event_handle,
			WM_SETICON,
			ICON_BIG,
			(LPARAM) icon_64);
	}
}

void globox_platform_set_title(
	struct globox* globox,
	const char* title)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	// update internal UTF-8 string
	if (globox->globox_title != NULL)
	{
		free(globox->globox_title);
	}

	globox->globox_title = strdup(title);

	if (globox->globox_title == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	// update internal wide-char string
	if (platform->globox_windows_wide_title != NULL)
	{
		free(globox->globox_title);
	}

	platform->globox_windows_wide_title = utf8_to_wchar(title);

	if (platform->globox_windows_wide_title == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	// update the window title
	BOOL ok =
		SetWindowText(
			platform->globox_platform_event_handle,
			platform->globox_windows_wide_title);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_TITLE);
		return;
	}
}

void globox_platform_set_state(
	struct globox* globox,
	enum globox_state state)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	LONG ok_long;
	BOOL ok_bool;

	if ((globox->globox_state == GLOBOX_STATE_FULLSCREEN)
		&& (globox->globox_state != state))
	{
		// restore style
		ok_long = SetWindowLong(
			platform->globox_platform_event_handle,
			GWL_STYLE,
			platform->globox_windows_style_backup);

		if (ok_long == 0)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET);
			return;
		}

		// restore extended style
		ok_long = SetWindowLong(
			platform->globox_platform_event_handle,
			GWL_EXSTYLE,
			platform->globox_windows_exstyle_backup);

		if (ok_long == 0)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET);
			return;
		}

		// restore window state
		ShowWindow(
			platform->globox_platform_event_handle,
			SW_SHOWMAXIMIZED);

		// save window position
		ok_bool =
			SetWindowPlacement(
				platform->globox_platform_event_handle,
				&(platform->globox_windows_position_backup));

		if (ok_bool == 0)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WINDOWS_WINDOW_PLACEMENT_SET);
			return;
		}
	}

	switch (state)
	{
		case GLOBOX_STATE_REGULAR:
		{
			ShowWindow(
				platform->globox_platform_event_handle,
				SW_SHOWNORMAL);
			break;
		}
		case GLOBOX_STATE_MAXIMIZED:
		{
			ShowWindow(
				platform->globox_platform_event_handle,
				SW_SHOWMAXIMIZED);
			break;
		}
		case GLOBOX_STATE_MINIMIZED:
		{
			ShowWindow(
				platform->globox_platform_event_handle,
				SW_SHOWMINIMIZED);
			break;
		}
		case GLOBOX_STATE_FULLSCREEN:
		{
			// exit if already fullscreen
			if (globox->globox_state == GLOBOX_STATE_FULLSCREEN)
			{
				break;
			}

			// save the window position
			ok_bool =
				GetWindowPlacement(
					platform->globox_platform_event_handle,
					&(platform->globox_windows_position_backup));

			if (ok_bool == 0)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_WINDOWS_WINDOW_PLACEMENT_GET);
				break;
			}

			// save the window style
			platform->globox_windows_style_backup =
				GetWindowLong(
					platform->globox_platform_event_handle,
					GWL_STYLE);

			if (platform->globox_windows_style_backup == 0)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET);
				break;
			}

			// save the window extended style
			platform->globox_windows_exstyle_backup =
				GetWindowLong(
					platform->globox_platform_event_handle,
					GWL_EXSTYLE);

			if (platform->globox_windows_exstyle_backup == 0)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET);
				break;
			}

			// set the new window style
			LONG style =
				platform->globox_windows_style_backup
				& ~WS_BORDER
				& ~WS_DLGFRAME
				& ~WS_THICKFRAME;

			ok_long = SetWindowLong(
				platform->globox_platform_event_handle,
				GWL_STYLE,
				style | WS_POPUP);

			if (ok_long == 0)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET);
				break;
			}

			// set the new window extended style
			LONG exstyle =
				platform->globox_windows_exstyle_backup
				& ~WS_EX_WINDOWEDGE;

			ok_long = SetWindowLong(
				platform->globox_platform_event_handle,
				GWL_EXSTYLE,
				exstyle | WS_EX_TOPMOST);

			if (ok_long == 0)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET);
				break;
			}

			// set the new window state
			ShowWindow(
				platform->globox_platform_event_handle,
				SW_SHOWMAXIMIZED);

			break;
		}
	}

	globox->globox_state = state;
}

// getters
uint32_t* globox_platform_get_argb(struct globox* globox)
{
	return globox->globox_platform.globox_platform_argb;
}

HWND globox_platform_get_event_handle(struct globox* globox)
{
	return globox->globox_platform.globox_platform_event_handle;
}
