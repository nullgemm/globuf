#include "globox.h"
#include "globox_error.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include <windowsx.h> // GET_X_LPARAM(), GET_Y_LPARAM()
#include <dwmapi.h>

#if defined(GLOBOX_COMPATIBILITY_WINE) && defined(GLOBOX_ERROR_LOG_BASIC)
#include <stdio.h>
#endif

#include "windows/globox_windows.h"

// Microsoft's low-level APIs do not enforce any window size limit,
// but Windows does not handle small windows correctly so glitches
// can appear, like action buttons rendered outside of the titlebar.
// To avoid this we came up with an arbitrary limit for globox.
#define WINDOW_MIN_X 170
#define WINDOW_MIN_Y 50

enum ACCENT_STATE
{
	ACCENT_ENABLE_BLURBEHIND = 3,
};

enum WINDOWCOMPOSITIONATTRIB
{
	WCA_ACCENT_POLICY = 19,
};

struct ACCENT_POLICY
{
	enum ACCENT_STATE AccentState;
	DWORD AccentFlags;
	DWORD GradientColor;
	DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA
{
	enum WINDOWCOMPOSITIONATTRIB dwAttrib;
	PVOID pvData;
	SIZE_T cbData;
};

union NAMED_INPUT_UNION
{
	MOUSEINPUT mi;
	KEYBDINPUT ki;
	HARDWAREINPUT hi;
};

struct NAMED_INPUT
{
	DWORD type;
	union NAMED_INPUT_UNION data;
};

LPWSTR utf8_to_wchar(const char* s)
{
	size_t codepoint_count =
		MultiByteToWideChar(
			CP_UTF8,
			MB_PRECOMPOSED,
			s,
			-1,
			NULL,
			0);

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
	struct globox_platform* platform = globox->globox_platform;

	HDC hdc = GetDC(platform->globox_platform_event_handle);

	if (hdc == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET);
		return NULL;
	}

	HBITMAP mask =
		CreateCompatibleBitmap(
			hdc,
			bmp->bmWidth,
			bmp->bmHeight);

	if (mask == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_BMP_MASK_CREATE);
		return NULL;
	}

	HBITMAP color = CreateBitmapIndirect(bmp);

	if (color == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_BMP_COLOR_CREATE);
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

	if (icon == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_ICON_CREATE);
		return NULL;
	}

	int ok;

	ok = DeleteObject(mask);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DELETE);
		return NULL;
	}

	ok = DeleteObject(color);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DELETE);
		return NULL;
	}

	ReleaseDC(platform->globox_platform_event_handle, hdc);

	return icon;
}

void query_pointer(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;

	POINT point;
	RECT rect;
	BOOL ok;

	ok = GetCursorPos(&point);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_CURSOR_POS_GET);
		return;
	}

	ok = GetWindowRect(platform->globox_platform_event_handle, &rect);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WINDOW_RECT_GET);
		return;
	}

	// save the mouse position
	platform->globox_windows_interactive_x = point.x;
	platform->globox_windows_interactive_y = point.y;

	// save the window position
	platform->globox_windows_old_outer_x = rect.left;
	platform->globox_windows_old_outer_y = rect.top;

	// update the window size
	platform->globox_windows_outer_width = rect.right - rect.left;
	platform->globox_windows_outer_height = rect.bottom - rect.top;

	// save the window size
	platform->globox_windows_old_outer_width = platform->globox_windows_outer_width;
	platform->globox_windows_old_outer_height = platform->globox_windows_outer_height;
}

void dwm_transparency(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;

	// TODO communicate only the client region without the frame and update when resizing
	HRGN region = CreateRectRgn(0, 0, globox->globox_width, globox->globox_height);

	if (region == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_TRANSPARENCY_REGION);
		return;
	}

	DWM_BLURBEHIND blur_behind =
	{
		.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION,
		.fEnable = TRUE,
		.hRgnBlur = region,
		.fTransitionOnMaximized = FALSE,
	};

	int ok =
		DwmEnableBlurBehindWindow(
			platform->globox_platform_event_handle,
			&blur_behind);

	if (ok != S_OK)
	{
#if !defined(GLOBOX_COMPATIBILITY_WINE)
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_TRANSPARENCY_DWM);
		// do not return yet to perform some extra clean-up
#elif defined(GLOBOX_ERROR_LOG_BASIC)
		fprintf(
			stderr,
			"Wine compatibility mode; skipping the following error:\n%s\n",
			globox->globox_log[GLOBOX_ERROR_WINDOWS_TRANSPARENCY_DWM]);
#endif
	}

	BOOL ok_bool = DeleteObject(region);

	if (ok_bool == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DELETE);
		return;
	}
}

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			// saves the globox pointer in the current event structure
			// for use in this callback when processing other events
			struct globox* globox =
				((CREATESTRUCT*) lParam)->lpCreateParams;

			// we have no way of handling any error
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) globox);

			break;
		}
		case WM_ENTERSIZEMOVE:
		{
			struct globox* globox =
				(struct globox*)
					GetWindowLongPtr(
						hwnd,
						GWLP_USERDATA);

			// we have no way of handling any error
			if (globox == NULL)
			{
				break;
			}

			struct globox_platform* platform = globox->globox_platform;

			// confirm we entered the modal loop
			platform->globox_windows_sizemove_step =
				GLOBOX_WINDOWS_SIZEMOVE_WAITEDGES;

			break;
		}
		case WM_MOVING:
		{
			// fallthrough, set wParam to 0 for simplicity
			wParam = 0;
		}
		case WM_SIZING:
		{
			struct globox* globox =
				(struct globox*)
					GetWindowLongPtr(
						hwnd,
						GWLP_USERDATA);

			// we have no way of handling any error
			if (globox == NULL)
			{
				break;
			}

			struct globox_platform* platform = globox->globox_platform;

			// only proceed after entering the
			// interactive move and resize modal loop
			if (platform->globox_windows_sizemove_step !=
				GLOBOX_WINDOWS_SIZEMOVE_WAITEDGES)
			{
				break;
			}

			// confirm we started killing the modal loop
			platform->globox_windows_sizemove_step =
				GLOBOX_WINDOWS_SIZEMOVE_KILLMODAL;

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
				default:
				{
					mode = GLOBOX_INTERACTIVE_STOP;
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

			struct NAMED_INPUT input =
			{
				.type = INPUT_MOUSE,
				.data.mi = mouse,
			};

			UINT ok = SendInput(1, (PINPUT) &input, sizeof (INPUT));

			// we have no way of handling any error
			// because if the previous function fails
			// the app will be stuck in the modal loop
			if ((ok == 0) || (mode == GLOBOX_INTERACTIVE_STOP))
			{
				platform->globox_windows_sizemove_step =
					GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL;
				break;
			}

			globox_platform_interactive_mode(
				globox,
				mode);

			break;
		}
		case WM_WINDOWPOSCHANGING:
		{
			struct globox* globox =
				(struct globox*)
					GetWindowLongPtr(
						hwnd,
						GWLP_USERDATA);

			if (globox == NULL)
			{
				break;
			}

			struct globox_platform* platform = globox->globox_platform;

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
				case SC_RESTORE:
				{
					struct globox* globox =
						(struct globox*)
							GetWindowLongPtr(
								hwnd,
								GWLP_USERDATA);

					// we have no way of handling any error
					if (globox == NULL)
					{
						break;
					}

					globox->globox_state = GLOBOX_STATE_REGULAR;

					break;
				}
				case SC_MAXIMIZE:
				case SC_MINIMIZE:
				{
					// we have no way of handling any error
					PostMessage(hwnd, msg, wParam, lParam);
					break;
				}
			}

			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);

			struct globox* globox =
				(struct globox*)
					GetWindowLongPtr(
						hwnd,
						GWLP_USERDATA);

			// we have no way of handling any error
			if (globox == NULL)
			{
				break;
			}

			globox->globox_closed = true;

			break;
		}
		case WM_ERASEBKGND:
		{
			// necessary to avoid background flickering
			// when resizing gdi, egl and wgl contexts
			return 1;
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void globox_platform_init(
	struct globox* globox,
	bool transparent,
	bool frameless,
	bool blurred)
{
	struct globox_platform* platform = malloc(sizeof (struct globox_platform));

	globox->globox_platform = platform;
	globox->globox_redraw = false;
	globox->globox_transparent = transparent;
	globox->globox_frameless = frameless;
	globox->globox_blurred = blurred;

	char** log = globox->globox_log;
	log[GLOBOX_ERROR_WINDOWS_SYM] =
		"could not find symbol";
	log[GLOBOX_ERROR_WINDOWS_MODULE_APP] =
		"could not get module handle";
	log[GLOBOX_ERROR_WINDOWS_MODULE_USER32] =
		"could not find user32.dll";
	log[GLOBOX_ERROR_WINDOWS_DELETE] =
		"could not delete win32 object";
	log[GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET] =
		"could not get device context";
	log[GLOBOX_ERROR_WINDOWS_MESSAGE_GET] =
		"could not get event message";
	log[GLOBOX_ERROR_WINDOWS_CLASS_REGISTER] =
		"could not register extended window class";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_CREATE] =
		"could not create window";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_ADJUST] =
		"could not adjust window rectangle";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_UPDATE] =
		"could not update window";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_RECT_GET] =
		"could not get window rectangle";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_POS_SET] =
		"could not set window position";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET] =
		"could not get window style";
	log[GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET] =
		"could not set window style";
	log[GLOBOX_ERROR_WINDOWS_PLACEMENT_GET] =
		"could not save window placement";
	log[GLOBOX_ERROR_WINDOWS_PLACEMENT_SET] =
		"could not set window placement";
	log[GLOBOX_ERROR_WINDOWS_CLIENT_RECT] =
		"could not get window client rectangle";
	log[GLOBOX_ERROR_WINDOWS_CLIENT_POS] =
		"could not get window client position";
	log[GLOBOX_ERROR_WINDOWS_TRANSPARENCY_REGION] =
		"could not create transparency region";
	log[GLOBOX_ERROR_WINDOWS_TRANSPARENCY_DWM] =
		"could not initialize transparency";
	log[GLOBOX_ERROR_WINDOWS_COMP_ATTR] =
		"could not set window composition attribute";
	log[GLOBOX_ERROR_WINDOWS_INPUT_SEND] =
		"could not synthetize input event";
	log[GLOBOX_ERROR_WINDOWS_CAPTURE_RELEASE] =
		"could not release mouse capture";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_LOAD] =
		"could not load mouse cursor";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_POS_GET] =
		"could not get mouse cursor position";
	log[GLOBOX_ERROR_WINDOWS_CURSOR_POS_SET] =
		"could not set mouse cursor position";
	log[GLOBOX_ERROR_WINDOWS_BMP_MASK_CREATE] =
		"could not create mask bitmap";
	log[GLOBOX_ERROR_WINDOWS_BMP_COLOR_CREATE] =
		"could not create color bitmap";
	log[GLOBOX_ERROR_WINDOWS_ICON_CREATE] =
		"could not create icon information";
	log[GLOBOX_ERROR_WINDOWS_ICON_SMALL] =
		"could not create small icon";
	log[GLOBOX_ERROR_WINDOWS_ICON_BIG] =
		"could not create big icon";
	log[GLOBOX_ERROR_WINDOWS_UTF8] =
		"could not translate UTF-8";
	log[GLOBOX_ERROR_WINDOWS_TITLE] =
		"could not update window title";
	log[GLOBOX_ERROR_WINDOWS_GDI_DAMAGE] =
		"could not damage GDI region";
	log[GLOBOX_ERROR_WINDOWS_GDI_PAINT] =
		"could not start GDI rendering";
	log[GLOBOX_ERROR_WINDOWS_GDI_BITBLT] =
		"could not copy GDI buffer to window";
	log[GLOBOX_ERROR_WINDOWS_GDI_DIB_CREATE] =
		"could not create GDI device-independent bitmap";
	log[GLOBOX_ERROR_WINDOWS_GDI_BITMAP_SELECT] =
		"could not select GDI bitmap";
	log[GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_CREATE] =
		"could not create GDI device context";
	log[GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_DELETE] =
		"could not delete GDI device context";
	log[GLOBOX_ERROR_WINDOWS_EGL_INIT] =
		"could not init EGL";
	log[GLOBOX_ERROR_WINDOWS_EGL_API_BIND] =
		"could not set EGL target graphics API";
	log[GLOBOX_ERROR_WINDOWS_EGL_DISPLAY_GET] =
		"could not get EGL display";
	log[GLOBOX_ERROR_WINDOWS_EGL_CONFIG_CHOOSE] =
		"could not choose EGL config";
	log[GLOBOX_ERROR_WINDOWS_EGL_CONFIG_ATTRIB_GET] =
		"could not get EGL visual ID";
	log[GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_CREATE] =
		"could not create EGL context";
	log[GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_DESTROY] =
		"could not destroy EGL context";
	log[GLOBOX_ERROR_WINDOWS_EGL_SURFACE_CREATE] =
		"could not create EGL surface";
	log[GLOBOX_ERROR_WINDOWS_EGL_SURFACE_DESTROY] =
		"could not destroy EGL surface";
	log[GLOBOX_ERROR_WINDOWS_EGL_MAKE_CURRENT] =
		"could not make EGL surface current";
	log[GLOBOX_ERROR_WINDOWS_EGL_TERMINATE] =
		"could not terminate EGL";
	log[GLOBOX_ERROR_WINDOWS_EGL_SWAP] =
		"could not swap EGL buffers";
	log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE] =
		"could not choose WGL pixel format";
	log[GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET] =
		"could not set WGL pixel format";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE] =
		"could not create WGL context";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_DELETE] =
		"could not delete WGL context";
	log[GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET] =
		"could not set WGL context";
	log[GLOBOX_ERROR_WINDOWS_WGL_SWAP] =
		"could not swap WGL buffers";

	platform->globox_windows_sizemove_step =
		GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL;

	platform->globox_windows_class_name =
		utf8_to_wchar(globox->globox_title);

	if (platform->globox_windows_class_name == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_UTF8);
		return;
	}

	platform->globox_windows_wide_title =
		wcsdup(platform->globox_windows_class_name);

	if (platform->globox_windows_wide_title == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	platform->globox_windows_class_module_handle =
		GetModuleHandle(NULL);

	if (platform->globox_windows_class_module_handle == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_MODULE_APP);
		return;
	}

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);

	if (cursor == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_CURSOR_LOAD);
		return;
	}

	WNDCLASSEX class =
	{
		.cbSize = sizeof (platform->globox_windows_class),
		.style = 0,
		.lpfnWndProc = window_procedure,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = platform->globox_windows_class_module_handle,
		.hIcon = NULL,
		.hCursor = cursor,
		.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1),
		.lpszMenuName = NULL,
		.lpszClassName = platform->globox_windows_class_name,
		.hIconSm = NULL,
	};

	platform->globox_windows_class = class;

	int ok = RegisterClassEx(&(platform->globox_windows_class));

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_CLASS_REGISTER);
		return;
	}
}

void globox_platform_create_window(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;

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
		exstyle = 0;
	}
	else
	{
		style = WS_POPUP;
		exstyle = 0;
	}

	// transparency does not work with EGLproxy due to a bug in the library
#if defined(GLOBOX_CONTEXT_EGL)
	globox->globox_transparent = false;
	globox->globox_blurred = false;
#endif

#if !defined(GLOBOX_CONTEXT_SOFTWARE) && !defined(GLOBOX_CONTEXT_WGL)
	if (globox->globox_transparent == true)
	{
		exstyle |= WS_EX_NOREDIRECTIONBITMAP;
	}
#endif

	ok = AdjustWindowRectEx(&rect, style, FALSE, exstyle);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WINDOW_ADJUST);
		return;
	}

	platform->globox_windows_outer_width =
		rect.right - rect.left;
	platform->globox_windows_outer_height =
		rect.bottom - rect.top;
	platform->globox_windows_framediff_x =
		platform->globox_windows_outer_width - globox->globox_width;
	platform->globox_windows_framediff_y =
		platform->globox_windows_outer_height - globox->globox_height;

	platform->globox_platform_event_handle =
		CreateWindowEx(
			exstyle,
			platform->globox_windows_class_name,
			platform->globox_windows_class_name,
			style,
			globox->globox_x,
			globox->globox_y,
			platform->globox_windows_outer_width,
			platform->globox_windows_outer_height,
			NULL,
			NULL,
			platform->globox_windows_class_module_handle,
			globox);

	if (platform->globox_platform_event_handle == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WINDOW_CREATE);
		return;
	}

	ok = UpdateWindow(platform->globox_platform_event_handle);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_WINDOW_UPDATE);
		return;
	}

	globox_platform_set_state(globox, globox->globox_state);

	platform->globox_windows_dwm_transparency_callback =
		dwm_transparency;

	if (globox->globox_transparent == false)
	{
		return;
	}

	// from here we handle transparency

#if defined(GLOBOX_CONTEXT_SOFTWARE) || defined(GLOBOX_CONTEXT_WGL)
	dwm_transparency(globox);
#endif

	if (globox->globox_blurred == false)
	{
		return;
	}

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

	// this is windows, forget standards...
	HRESULT (*SetWindowCompositionAttribute)(HWND, void*) =
		(HRESULT (*)(HWND, void*)) func;

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
#if !defined(GLOBOX_COMPATIBILITY_WINE)
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_COMP_ATTR);
		return;
#elif defined(GLOBOX_ERROR_LOG_BASIC)
		fprintf(
			stderr,
			"Wine compatibility mode; skipping the following error:\n%s\n",
			globox->globox_log[GLOBOX_ERROR_WINDOWS_COMP_ATTR]);
#endif
	}
}

void globox_platform_hooks(struct globox* globox)
{
	// not needed
}

void globox_platform_commit(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;

	platform->globox_windows_dcomp_callback(globox);
}

void globox_platform_prepoll(struct globox* globox)
{
	// not needed
}

void globox_platform_events_poll(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;

	PeekMessage(
		&(platform->globox_windows_msg),
		platform->globox_platform_event_handle,
		0,
		0,
		PM_REMOVE);
}

void globox_platform_events_wait(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;

	BOOL ok =
		GetMessage(
			&(platform->globox_windows_msg),
			platform->globox_platform_event_handle,
			0,
			0);

	if (ok == 0)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_MESSAGE_GET);
		return;
	}
}

void globox_platform_interactive_mode(
	struct globox* globox,
	enum globox_interactive_mode mode)
{
	struct globox_platform* platform = globox->globox_platform;

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

		// get the screen coordinates of the client area origin
		UINT ok;

		POINT origin =
		{
			.x = 0,
			.y = 0,
		};

		ok = ClientToScreen(platform->globox_platform_event_handle, &origin);

		if (ok == 0)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WINDOWS_CLIENT_POS);
			return;
		}

		// move the cursor to the middle of the client area
		ok = SetCursorPos(
			origin.x + (globox->globox_width / 2),
			origin.y + (globox->globox_height / 2));

		if (ok == 0)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WINDOWS_CURSOR_POS_SET);
			return;
		}

		// synthetize a left mouse click to be able to track
		// the mouse outside of the client area while moving/resizing
		MOUSEINPUT mouse2 =
		{
			.dx = 0,
			.dy = 0,
			.mouseData = 0,
			.dwFlags = MOUSEEVENTF_LEFTDOWN,
			.time = 0,
			.dwExtraInfo = 0,
		};

		struct NAMED_INPUT input2 =
		{
			.type = INPUT_MOUSE,
			.data.mi = mouse2,
		};

		ok = SendInput(1, (PINPUT) &input2, sizeof (INPUT));

		if (ok == 0)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WINDOWS_INPUT_SEND);
			return;
		}

		// compute the screen coordinates of the
		// interactive resize reference point
		POINT old_mouse_pos =
		{
			.x = platform->globox_windows_interactive_x,
			.y = platform->globox_windows_interactive_y,
		};

		// move the cursor back to the interactive resize reference point
		ok = SetCursorPos(
			old_mouse_pos.x,
			old_mouse_pos.y);

		if (ok == 0)
		{
			globox_error_throw(
				globox,
				GLOBOX_ERROR_WINDOWS_INPUT_SEND);
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

void globox_platform_events_handle(struct globox* globox)
{
	struct globox_platform* platform = globox->globox_platform;

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
			}

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
						GLOBOX_ERROR_WINDOWS_CLIENT_RECT);
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
				ok = ReleaseCapture();

				if (ok == 0)
				{
					globox_error_throw(
						globox,
						GLOBOX_ERROR_WINDOWS_CAPTURE_RELEASE);
					break;
				}

				globox_platform_interactive_mode(
					globox,
					GLOBOX_INTERACTIVE_STOP);
			}
			else if ((platform->globox_windows_msg.message ==
				WM_MOUSEMOVE)
			&& (platform->globox_windows_sizemove_step ==
				GLOBOX_WINDOWS_SIZEMOVE_STARTSIZE))
			{
				int16_t x =
					GET_X_LPARAM(
						platform->globox_windows_msg.lParam);

				int16_t y =
					GET_Y_LPARAM(
						platform->globox_windows_msg.lParam);

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
					globox_error_throw(
						globox,
						GLOBOX_ERROR_WINDOWS_CLIENT_POS);
					break;
				}

				RECT origin_rect;

				ok = GetWindowRect(
					platform->globox_platform_event_handle,
					&origin_rect);

				if (ok == 0)
				{
					globox_error_throw(
						globox,
						GLOBOX_ERROR_WINDOWS_CLIENT_RECT);
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
						platform->globox_windows_outer_height =
							platform->globox_windows_old_outer_height
							+ (y - start.y);
						origin_x =
							platform->globox_windows_old_outer_x
							+ (x - start.x);
						platform->globox_windows_outer_width =
							platform->globox_windows_old_outer_width
							- (x - start.x);
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
						platform->globox_windows_outer_height =
							platform->globox_windows_old_outer_height
							+ (y - start.y);
						platform->globox_windows_outer_width =
							platform->globox_windows_old_outer_width
							+ (x - start.x);
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
					default:
					{
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
					platform->globox_windows_outer_width +=
						(WINDOW_MIN_X - diff_x);
					origin_x = origin_rect.left;
				}

				if (diff_y < WINDOW_MIN_Y)
				{
					platform->globox_windows_outer_height +=
						(WINDOW_MIN_Y - diff_y);
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
					globox_error_throw(
						globox,
						GLOBOX_ERROR_WINDOWS_WINDOW_POS_SET);
					break;
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
	struct globox_platform* platform = globox->globox_platform;

	free(platform->globox_windows_class_name);
	free(platform->globox_windows_wide_title);
	free(platform);
}

void globox_platform_set_icon(
	struct globox* globox,
	uint32_t* pixmap,
	uint32_t len)
{
	struct globox_platform* platform = globox->globox_platform;

	// small icon
	BITMAP pixmap_32 =
	{
		.bmType = 0,
		.bmWidth = 32,
		.bmHeight = 32,
		.bmWidthBytes = 32 * 4,
		.bmPlanes = 1,
		.bmBitsPixel = 32,
		.bmBits = pixmap + 4 + (16 * 16),
	};

	HICON icon_32 = bitmap_to_icon(globox, &pixmap_32);

	if (icon_32 == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_ICON_SMALL);
		return;
	}

	SendMessage(
		platform->globox_platform_event_handle,
		WM_SETICON,
		ICON_SMALL,
		(LPARAM) icon_32);

	// big icon
	BITMAP pixmap_64 =
	{
		.bmType = 0,
		.bmWidth = 64,
		.bmHeight = 64,
		.bmWidthBytes = 64 * 4,
		.bmPlanes = 1,
		.bmBitsPixel = 32,
		.bmBits = pixmap + 4 + (16 * 16) + (32 * 32),
	};

	HICON icon_64 = bitmap_to_icon(globox, &pixmap_64);

	if (icon_64 == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_ICON_BIG);
		return;
	}

	SendMessage(
		platform->globox_platform_event_handle,
		WM_SETICON,
		ICON_BIG,
		(LPARAM) icon_64);
}

void globox_platform_set_title(
	struct globox* globox,
	 const char* title)
{
	struct globox_platform* platform = globox->globox_platform;

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
		free(platform->globox_windows_wide_title);
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
	struct globox_platform* platform = globox->globox_platform;

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
				GLOBOX_ERROR_WINDOWS_PLACEMENT_SET);
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
					GLOBOX_ERROR_WINDOWS_PLACEMENT_GET);
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
	return globox->globox_platform->globox_platform_argb;
}

HWND globox_platform_get_event_handle(struct globox* globox)
{
	return globox->globox_platform->globox_platform_event_handle;
}
