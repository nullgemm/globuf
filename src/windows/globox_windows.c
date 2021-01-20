/// this file implements the main globox functions for the windows platform
/// it is completely isolated from the graphic context functions

// include globox structures and functions
#include "globox.h"
#include "globox_error.h"
// windows includes
#include <windows.h>
#include <windowsx.h> // GET_X_LPARAM(), GET_Y_LPARAM()

// include platform structures
#include <dwmapi.h>
#include "windows/globox_windows.h"
#include "windows/globox_windows_symbols.h"

#define WINDOW_MIN_X 170
#define WINDOW_MIN_Y 50

void dwm_transparency(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	DWM_BLURBEHIND blur_behind = {0};

	HRGN region = CreateRectRgn(0, 0, -1, -1);

	blur_behind.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	blur_behind.hRgnBlur = region;
	blur_behind.fEnable = TRUE;

	DwmEnableBlurBehindWindow(
		platform->globox_platform_event_handle,
		&blur_behind);
}

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			// saves the globox pointer in the window structure for
			// use in this callback when processing other events
			struct globox* globox =
				((CREATESTRUCT*) lParam)->lpCreateParams;

			// could fail...
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) globox);

			break;
		}
		case WM_ENTERSIZEMOVE:
		{
			// get the globox pointer
			struct globox* globox =
				(struct globox*)
					GetWindowLongPtr(
						hwnd,
						GWLP_USERDATA);

			if (globox == NULL)
			{
				break;
			}

			struct globox_platform* platform =
				&(globox->globox_platform);

			// confirm we entered the modal loop and
			// started waiting for the edge info
			platform->globox_windows_sizemove_step =
				GLOBOX_WINDOWS_SIZEMOVE_WAITEDGES;

			break;
		}
		case WM_MOVING:
		{
			wParam = 0;

			// fallthrough
		}
		case WM_SIZING:
		{
			// get the globox pointer
			struct globox* globox =
				(struct globox*)
					GetWindowLongPtr(
						hwnd,
						GWLP_USERDATA);

			if (globox == NULL)
			{
				break;
			}

			struct globox_platform* platform =
				&(globox->globox_platform);

			// only proceed when in the modal loop
			if (platform->globox_windows_sizemove_step !=
				GLOBOX_WINDOWS_SIZEMOVE_WAITEDGES)
			{
				break;
			}

			// confirm we got the info and
			// started killing the loop
			platform->globox_windows_sizemove_step =
				GLOBOX_WINDOWS_SIZEMOVE_KILLMODAL;

			// save the sizing edge for use during the
			// globox-initiated interactive resize
			enum globox_interactive_mode mode;

			switch (wParam)
			{
				case 0:
				{
					mode = GLOBOX_INTERACTIVE_MOVE;
					break;
				}
				case WMSZ_LEFT:
				{
					mode = GLOBOX_INTERACTIVE_W;
					break;
				}
				case WMSZ_RIGHT:
				{
					mode = GLOBOX_INTERACTIVE_E;
					break;
				}
				case WMSZ_TOP:
				{
					mode = GLOBOX_INTERACTIVE_N;
					break;
				}
				case WMSZ_TOPLEFT:
				{
					mode = GLOBOX_INTERACTIVE_NW;
					break;
				}
				case WMSZ_TOPRIGHT:
				{
					mode = GLOBOX_INTERACTIVE_NE;
					break;
				}
				case WMSZ_BOTTOM:
				{
					mode = GLOBOX_INTERACTIVE_S;
					break;
				}
				case WMSZ_BOTTOMLEFT:
				{
					mode = GLOBOX_INTERACTIVE_SW;
					break;
				}
				case WMSZ_BOTTOMRIGHT:
				{
					mode = GLOBOX_INTERACTIVE_SE;
					break;
				}
			}

			// kills the SIZEMOVE modal loop by
			// synthetizing a left mouse release
			MOUSEINPUT mouse =
			{
				.dx = 0,
				.dy = 0,
				.mouseData = 0,
				.dwFlags = MOUSEEVENTF_LEFTUP,
				.time = 0,
				.dwExtraInfo = 0,
			};

			INPUT input =
			{
				INPUT_MOUSE,
				mouse,
			};

			UINT ok = SendInput(1, &input, sizeof (INPUT));

			if (ok == 0)
			{
				// we can't fail properly inside
				// this fucking callback
				break;
			}

			// initiate an interactive move and resize operation
			// the next sizemove step will be set automatically
			globox_platform_interactive_mode(
				globox,
				mode);

			break;
		}
		case WM_WINDOWPOSCHANGING:
		{
			// get the globox pointer
			struct globox* globox =
				(struct globox*)
					GetWindowLongPtr(
						hwnd,
						GWLP_USERDATA);

			if (globox == NULL)
			{
				break;
			}

			struct globox_platform* platform =
				&(globox->globox_platform);

			// block window resizing and moving until
			// globox has finished fooling windows
			if ((platform->globox_windows_sizemove_step <
				GLOBOX_WINDOWS_SIZEMOVE_STARTSIZE)
			&& (platform->globox_windows_sizemove_step >
				GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL))
			{
				((WINDOWPOS*) lParam)->flags |=
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOSENDCHANGING;
			}
			else
			{
				WINDOWPOS* win = (WINDOWPOS*) lParam;
				LPARAM new_param = MAKELPARAM(win->cx, win->cy);

				if ((win->cx != 0) && (win->cy != 0))
				{
					PostMessage(hwnd, WM_SIZE, wParam, new_param);
				}
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
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	POINT point;
	RECT rect;
	BOOL ok;

	// get the cursor position
	ok = GetCursorPos(&point);

	if (ok == 0)
	{
		return;
	}

	// get the window bounds
	ok = GetWindowRect(platform->globox_platform_event_handle, &rect);

	if (ok == 0)
	{
		return;
	}

	// save the last mouse position
	platform->globox_windows_interactive_x =
		point.x;
	platform->globox_windows_interactive_y =
		point.y;

	// save the last window position
	platform->globox_windows_old_outer_x =
		rect.left;
	platform->globox_windows_old_outer_y =
		rect.top;

	// update the current window size
	platform->globox_windows_outer_width =
		rect.right - rect.left;
	platform->globox_windows_outer_height =
		rect.bottom - rect.top;

	// save the last window size
	platform->globox_windows_old_outer_width =
		platform->globox_windows_outer_width;
	platform->globox_windows_old_outer_height =
		platform->globox_windows_outer_height;
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

	platform->globox_windows_sizemove_step =
		GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL;

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
	log[GLOBOX_ERROR_WINDOWS_GLOBOX_PTR] =
		"could not associate the globox pointer to the window";

	log[GLOBOX_ERROR_WINDOWS_MODULE_USER32] =
		"could not load user32.dll";
	log[GLOBOX_ERROR_WINDOWS_SYM] =
		"could not find the composition function symbol";
	log[GLOBOX_ERROR_WINDOWS_TRANSPARENT] =
		"could not make the window transparent";
	log[GLOBOX_ERROR_WINDOWS_SWAPCHAIN_CREATE] =
		"could not create the swapchain";
	log[GLOBOX_ERROR_WINDOWS_FACTORY_CREATE] =
		"could not create a DirectX factory";
	log[GLOBOX_ERROR_WINDOWS_ADAPTERS_END] =
		"could not find a suitable DirectX adapter";
	log[GLOBOX_ERROR_WINDOWS_ADAPTERS_LIST] =
		"could not list the available DirectX adapters";
	log[GLOBOX_ERROR_WINDOWS_DXGI_DEVICE] =
		"could not get the DirectX device";
	log[GLOBOX_ERROR_WINDOWS_DCOMP_DEVICE] =
		"could not create a DirectComposition device";
	log[GLOBOX_ERROR_WINDOWS_DCOMP_TARGET] =
		"could not create a DirectComposition target";
	log[GLOBOX_ERROR_WINDOWS_DCOMP_VISUAL] =
		"could not create a DirectComposition visual";
	log[GLOBOX_ERROR_WINDOWS_D2D_FACTORY] =
		"could not create a Direct2D factory";
	log[GLOBOX_ERROR_WINDOWS_D2D_DEVICE] =
		"could not create a Direct2D device";
	log[GLOBOX_ERROR_WINDOWS_D2D_DEVICE_CONTEXT] =
		"could not get a Direct2D device context";
	log[GLOBOX_ERROR_WINDOWS_D2D_SWAPCHAIN_SURFACE] =
		"could not get a swap chain surface";
	log[GLOBOX_ERROR_WINDOWS_D2D_SURFACE_BITMAP] =
		"could not get the surface bitmap";
	log[GLOBOX_ERROR_WINDOWS_D2D_COPY] =
		"could not copy the buffer to the surface bitmap";
	log[GLOBOX_ERROR_WINDOWS_D2D_PRESENT] =
		"could not present the swap chain surface";
	log[GLOBOX_ERROR_WINDOWS_DCOMP_BIND] =
		"could not bind the swap chain surface to the visual";
	log[GLOBOX_ERROR_WINDOWS_DCOMP_SET_ROOT] =
		"could not set the visual as the composition tree\'s root";
	log[GLOBOX_ERROR_WINDOWS_DCOMP_COMMIT] =
		"could not commit the composition operation";

	log[GLOBOX_ERROR_WINDOWS_WGL_DEVICE_CONTEXT] =
		"could not get the window device context";
	log[GLOBOX_ERROR_WINDOWS_WGL_SWAP] =
		"could not swap OpenGL buffers";
	log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE] =
		"could not choose pixel format";
	log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET] =
		"could not set pixel format";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE] =
		"could not create a WGL context";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET] =
		"could not set the WGL context";

	log[GLOBOX_ERROR_WINDOWS_EGL_FAIL] =
		"EGL error";

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
		.style = 0,      // we don't need extra PAINT events
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

	DWORD style;
	DWORD exstyle;

	if (globox->globox_frameless == false)
	{
		style = WS_OVERLAPPEDWINDOW;
		exstyle = WS_EX_OVERLAPPEDWINDOW;
	}
	else
	{
		style = WS_POPUP;
		exstyle = 0;
	}

	// because of a bug in EGLproxy transparency does not work
#if defined(GLOBOX_CONTEXT_EGL)
	globox->globox_transparent = false;
	globox->globox_blurred = false;
#endif

#if !defined(GLOBOX_CONTEXT_GDI) && !defined(GLOBOX_CONTEXT_WGL)
	if (globox->globox_transparent == true)
	{
		exstyle |= WS_EX_NOREDIRECTIONBITMAP;
	}
#endif

	ok = AdjustWindowRectEx(
		&rect,
		style,
		FALSE,
		exstyle);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_ADJUST_WINDOW);
		return;
	}

	platform->globox_windows_outer_width = rect.right - rect.left;
	platform->globox_windows_outer_height = rect.bottom - rect.top;

	platform->globox_windows_framediff_x =
		platform->globox_windows_outer_width
		- globox->globox_width;

	platform->globox_windows_framediff_y =
		platform->globox_windows_outer_height
		- globox->globox_height;

	platform->globox_platform_event_handle =
		CreateWindowEx(
			exstyle,
			platform->globox_windows_class_name, // class name
			platform->globox_windows_class_name, // window title
			style,
			globox->globox_x,
			globox->globox_y,
			platform->globox_windows_outer_width,
			platform->globox_windows_outer_height,
			NULL,  // parent window handle
			NULL,  // window-specific menu handle
			platform->globox_windows_class_module_handle,
			globox); // custom data included in CREATESTRUCT

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

	platform->globox_windows_dwm_transparency_callback =
		dwm_transparency;

	if (globox->globox_transparent == false)
	{
		return;
	}

#if defined(GLOBOX_CONTEXT_GDI) || defined(GLOBOX_CONTEXT_WGL)
	dwm_transparency(globox);
#endif

	if (globox->globox_blurred == false)
	{
		return;
	}

	// enable blur
	HMODULE user32 = GetModuleHandle(TEXT("user32.dll"));

	if (user32 == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_MODULE_USER32);
		return;
	}

	FARPROC func = GetProcAddress(user32, "SetWindowCompositionAttribute");

	if (func == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_SYM);
		return;
	}

	SetWindowCompositionAttribute = (HRESULT (*)(HWND, void*)) func;

	struct ACCENT_POLICY accent =
	{
		.AccentState = ACCENT_ENABLE_BLURBEHIND,
		.AccentFlags = 0,
		.GradientColor = 0,
		.AnimationId = 0,
	};

	struct WINDOWCOMPOSITIONATTRIBDATA data =
	{
		.dwAttrib = WCA_ACCENT_POLICY,
		.pvData = &accent,
		.cbData = sizeof (accent),
	};

	ok = SetWindowCompositionAttribute(
		platform->globox_platform_event_handle,
		&data);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_TRANSPARENT);
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
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	platform->globox_windows_dcomp_callback(globox);
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
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);

	if ((mode != GLOBOX_INTERACTIVE_STOP)
		&& (globox->globox_interactive_mode != mode))
	{
		query_pointer(globox);

		if (globox_error_catch(globox))
		{
			platform->globox_windows_sizemove_step =
				GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL;

			return;
		}

		globox->globox_interactive_mode = mode;

		platform->globox_windows_sizemove_step =
			GLOBOX_WINDOWS_SIZEMOVE_SYNTHDRAG;

		// gets mouse capture authorization by
		// synthetizing a left mouse press
		UINT ok;

		// compute the screen coordinates for the client area origin
		POINT origin =
		{
			.x = 0,
			.y = 0,
		};

		ok = ClientToScreen(platform->globox_platform_event_handle, &origin);

		if (ok == 0)
		{
			// TODO error ?
			return;
		}

		// move the cursor to the middle of the client area
		ok = SetCursorPos(
			origin.x + (globox->globox_width / 2),
			origin.y + (globox->globox_height / 2));

		if (ok == 0)
		{
			// TODO error ?
			return;
		}

		// press the left mouse button
		MOUSEINPUT mouse2 =
		{
			.dx = 0,
			.dy = 0,
			.mouseData = 0,
			.dwFlags = MOUSEEVENTF_LEFTDOWN,
			.time = 0,
			.dwExtraInfo = 0,
		};

		INPUT input2 =
		{
			INPUT_MOUSE,
			mouse2,
		};

		ok = SendInput(1, &input2, sizeof (INPUT));

		if (ok == 0)
		{
			// TODO error ?
			return;
		}

		// compute the screen coordinates for the
		// interactive resize reference point
		POINT old_mouse_pos =
		{
			.x = platform->globox_windows_interactive_x,
			.y = platform->globox_windows_interactive_y,
		};

		// move the cursor back to the interactive
		// resize reference point
		ok = SetCursorPos(
			old_mouse_pos.x,
			old_mouse_pos.y);

		if (ok == 0)
		{
			// TODO error ?
			return;
		}
	}
	else
	{
		globox->globox_interactive_mode = GLOBOX_INTERACTIVE_STOP;

		platform->globox_windows_sizemove_step =
			GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL;
	}
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
			// get mouse movements outside of the window
			// when we are in interactive resize mode
			if ((platform->globox_windows_msg.message ==
				WM_LBUTTONDOWN)
			&& (platform->globox_windows_sizemove_step ==
				GLOBOX_WINDOWS_SIZEMOVE_SYNTHDRAG))
			{
				SetCapture(
					platform->globox_platform_event_handle);

				platform->globox_windows_sizemove_step =
					GLOBOX_WINDOWS_SIZEMOVE_STARTSIZE;
			}
			else if ((platform->globox_windows_msg.message ==
				WM_LBUTTONUP)
			&& (platform->globox_windows_sizemove_step ==
				GLOBOX_WINDOWS_SIZEMOVE_STARTSIZE))
			{
				ReleaseCapture();

				globox_platform_interactive_mode(
					globox,
					GLOBOX_INTERACTIVE_STOP);
			}
			else if ((platform->globox_windows_msg.message ==
				WM_MOUSEMOVE)
			&& (platform->globox_windows_sizemove_step ==
				GLOBOX_WINDOWS_SIZEMOVE_STARTSIZE))
			{
				int16_t x = GET_X_LPARAM(platform->globox_windows_msg.lParam);
				int16_t y = GET_Y_LPARAM(platform->globox_windows_msg.lParam);

				POINT start =
				{
					.x = platform->globox_windows_interactive_x,
					.y = platform->globox_windows_interactive_y,
				};

				ok = ScreenToClient(
					platform->globox_platform_event_handle,
					&start);

				if (ok == 0)
				{
					// TODO error ?
					return;
				}

				RECT origin_rect;

				ok = GetWindowRect(
					platform->globox_platform_event_handle,
					&origin_rect);

				if (ok == 0)
				{
					// TODO error ?
					break;
				}

				int32_t origin_x = origin_rect.left;
				int32_t origin_y = origin_rect.top;

				switch (globox->globox_interactive_mode)
				{
					case GLOBOX_INTERACTIVE_MOVE:
					{
						origin_y =
							platform->globox_windows_old_outer_y
							+ (y - start.y);

						origin_x =
							platform->globox_windows_old_outer_x
							+ (x - start.x);
						break;
					}
					case GLOBOX_INTERACTIVE_N:
					{
						origin_y =
							platform->globox_windows_old_outer_y
							+ (y - start.y);

						platform->globox_windows_outer_height =
							platform->globox_windows_old_outer_height
							- (y - start.y);

						break;
					}
					case GLOBOX_INTERACTIVE_NW:
					{
						origin_y =
							platform->globox_windows_old_outer_y
							+ (y - start.y);

						platform->globox_windows_outer_height =
							platform->globox_windows_old_outer_height
							- (y - start.y);

						origin_x =
							platform->globox_windows_old_outer_x
							+ (x - start.x);

						platform->globox_windows_outer_width =
							platform->globox_windows_old_outer_width
							- (x - start.x);

						break;
					}
					case GLOBOX_INTERACTIVE_W:
					{
						origin_x =
							platform->globox_windows_old_outer_x
							+ (x - start.x);

						platform->globox_windows_outer_width =
							platform->globox_windows_old_outer_width
							- (x - start.x);

						break;
					}
					case GLOBOX_INTERACTIVE_SW:
					{
						origin_x =
							platform->globox_windows_old_outer_x
							+ (x - start.x);

						platform->globox_windows_outer_width =
							platform->globox_windows_old_outer_width
							- (x - start.x);

						platform->globox_windows_outer_height =
							platform->globox_windows_old_outer_height
							+ (y - start.y);

						break;
					}
					case GLOBOX_INTERACTIVE_S:
					{
						platform->globox_windows_outer_height =
							platform->globox_windows_old_outer_height
							+ (y - start.y);

						break;
					}
					case GLOBOX_INTERACTIVE_SE:
					{
						platform->globox_windows_outer_width =
							platform->globox_windows_old_outer_width
							+ (x - start.x);

						platform->globox_windows_outer_height =
							platform->globox_windows_old_outer_height
							+ (y - start.y);

						break;
					}
					case GLOBOX_INTERACTIVE_E:
					{
						platform->globox_windows_outer_width =
							platform->globox_windows_old_outer_width
							+ (x - start.x);

						break;
					}
					case GLOBOX_INTERACTIVE_NE:
					{
						origin_y =
							platform->globox_windows_old_outer_y
							+ (y - start.y);

						platform->globox_windows_outer_height =
							platform->globox_windows_old_outer_height
							- (y - start.y);

						platform->globox_windows_outer_width =
							platform->globox_windows_old_outer_width
							+ (x - start.x);

						break;
					}
				}

				int32_t diff_x =
					platform->globox_windows_outer_width
					- platform->globox_windows_framediff_x;

				int32_t diff_y =
					platform->globox_windows_outer_height
					- platform->globox_windows_framediff_y;

				if (diff_x < WINDOW_MIN_X)
				{
					platform->globox_windows_outer_width += (WINDOW_MIN_X - diff_x);
					origin_x = origin_rect.left;
				}

				if (diff_y < WINDOW_MIN_Y)
				{
					platform->globox_windows_outer_height += (WINDOW_MIN_Y - diff_y);
					origin_y = origin_rect.top;
				}

				ok = SetWindowPos(
					platform->globox_platform_event_handle,
					HWND_TOPMOST,
					origin_x,
					origin_y,
					platform->globox_windows_outer_width,
					platform->globox_windows_outer_height,
					0);

				if (ok == 0)
				{
					// TODO error ?
				}
			}

			if ((platform->globox_windows_sizemove_step ==
				GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL)
			&& ((platform->globox_windows_msg.message ==
				WM_LBUTTONDOWN)
			|| (platform->globox_windows_msg.message ==
				WM_LBUTTONUP)
			|| (platform->globox_windows_msg.message ==
				WM_MOUSEMOVE)))
			{
				break;
			}

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
