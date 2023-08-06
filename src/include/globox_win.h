#ifndef H_GLOBOX_WIN
#define H_GLOBOX_WIN

#include "globox.h"
#include <windows.h>

HWND globox_get_win_window(struct globox* context);
HDC globox_get_win_device_context(struct globox* context);

#endif
