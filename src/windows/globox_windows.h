#ifndef H_GLOBOX_WINDOWS
#define H_GLOBOX_WINDOWS

#include <stdint.h>
#include <stdbool.h>

#if defined(GLOBOX_CONTEXT_SOFTWARE)
	#include "software/globox_windows_software.h"
#elif defined(GLOBOX_CONTEXT_VULKAN)
	#include "vulkan/globox_windows_vulkan.h"
#elif defined(GLOBOX_CONTEXT_EGL)
	#include "egl/globox_windows_egl.h"
#elif defined(GLOBOX_CONTEXT_WGL)
	#include "wgl/globox_windows_wgl.h"
#endif

struct globox;

enum globox_windows_sizemove
{
	GLOBOX_WINDOWS_SIZEMOVE_WAITMODAL = 0, // waiting for the modal loop
	GLOBOX_WINDOWS_SIZEMOVE_WAITEDGES,     // waiting for the edge info
	GLOBOX_WINDOWS_SIZEMOVE_KILLMODAL,     // killing the modal loop
	GLOBOX_WINDOWS_SIZEMOVE_SYNTHDRAG,     // synthetizing a mouse drag
	GLOBOX_WINDOWS_SIZEMOVE_STARTSIZE,     // starting the resize
};

struct globox_platform
{
	HWND globox_platform_event_handle;
	uint32_t* globox_platform_argb;

#if defined(GLOBOX_CONTEXT_SOFTWARE)
	struct globox_windows_software globox_windows_software;
#elif defined(GLOBOX_CONTEXT_VULKAN)
	struct globox_windows_vulkan globox_windows_vulkan;
#elif defined(GLOBOX_CONTEXT_EGL)
	struct globox_windows_egl globox_windows_egl;
#elif defined(GLOBOX_CONTEXT_WGL)
	struct globox_windows_wgl globox_windows_wgl;
#endif

	MSG globox_windows_msg;
	LPWSTR globox_windows_wide_title;
	LPWSTR globox_windows_class_name;
	HMODULE globox_windows_class_module_handle;
	WNDCLASSEX globox_windows_class;

	LONG globox_windows_style_backup;
	LONG globox_windows_exstyle_backup;
	WINDOWPLACEMENT globox_windows_position_backup;

	uint32_t globox_windows_old_outer_x;
	uint32_t globox_windows_old_outer_y;
	uint32_t globox_windows_framediff_x;
	uint32_t globox_windows_framediff_y;

	uint32_t globox_windows_outer_width;
	uint32_t globox_windows_outer_height;
	uint32_t globox_windows_old_outer_width;
	uint32_t globox_windows_old_outer_height;

	int64_t globox_windows_interactive_x;
	int64_t globox_windows_interactive_y;
	enum globox_windows_sizemove globox_windows_sizemove_step;

	void (*globox_windows_resize_callback)(struct globox* globox);
	void (*globox_windows_dcomp_callback)(struct globox* globox);
	void (*globox_windows_dwm_transparency_callback)(struct globox* globox);
};

#endif
