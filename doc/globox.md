# Globox documentation
## Structures
```
enum globox_error
```
Holds a total of `GLOBOX_ERROR_SIZE` values used to report an error.
The special zero-value `GLOBOX_ERROR_OK` is used to report a success.

Generic error values:
 - `GLOBOX_ERROR_NULL`
 - `GLOBOX_ERROR_ALLOC`
 - `GLOBOX_ERROR_BOUNDS`
 - `GLOBOX_ERROR_DOMAIN`
 - `GLOBOX_ERROR_FD`

Wayland-specific error values:
 - `GLOBOX_ERROR_WAYLAND_EPOLL_CREATE`
 - `GLOBOX_ERROR_WAYLAND_EPOLL_CTL`
 - `GLOBOX_ERROR_WAYLAND_EPOLL_WAIT`
 - `GLOBOX_ERROR_WAYLAND_DISPLAY`
 - `GLOBOX_ERROR_WAYLAND_ROUNDTRIP`
 - `GLOBOX_ERROR_WAYLAND_FLUSH`
 - `GLOBOX_ERROR_WAYLAND_DISPATCH`
 - `GLOBOX_ERROR_WAYLAND_MMAP`
 - `GLOBOX_ERROR_WAYLAND_MUNMAP`
 - `GLOBOX_ERROR_WAYLAND_REQUEST`
 - `GLOBOX_ERROR_WAYLAND_LISTENER`
 - `GLOBOX_ERROR_WAYLAND_EGL_FAIL` (EGL context)

X11-specific error values:
 - `GLOBOX_ERROR_X11_CONN`
 - `GLOBOX_ERROR_X11_FLUSH`
 - `GLOBOX_ERROR_X11_WIN`
 - `GLOBOX_ERROR_X11_MAP`
 - `GLOBOX_ERROR_X11_GC`
 - `GLOBOX_ERROR_X11_VISUAL_NOT_COMPATIBLE`
 - `GLOBOX_ERROR_X11_VISUAL_NOT_FOUND`
 - `GLOBOX_ERROR_X11_PIXMAP`
 - `GLOBOX_ERROR_X11_SHM_VERSION_REPLY`
 - `GLOBOX_ERROR_X11_SHMID`
 - `GLOBOX_ERROR_X11_SHMADDR`
 - `GLOBOX_ERROR_X11_SHM_ATTACH`
 - `GLOBOX_ERROR_X11_SHMCTL`
 - `GLOBOX_ERROR_X11_SHM_PIXMAP`
 - `GLOBOX_ERROR_X11_SHM_DETACH`
 - `GLOBOX_ERROR_X11_SHMDT`
 - `GLOBOX_ERROR_X11_EPOLL_CREATE`
 - `GLOBOX_ERROR_X11_EPOLL_CTL`
 - `GLOBOX_ERROR_X11_EPOLL_WAIT`
 - `GLOBOX_ERROR_X11_WIN_ATTR`
 - `GLOBOX_ERROR_X11_ATOMS`
 - `GLOBOX_ERROR_X11_ICON`
 - `GLOBOX_ERROR_X11_TITLE`
 - `GLOBOX_ERROR_X11_IMAGE`
 - `GLOBOX_ERROR_X11_COPY`
 - `GLOBOX_ERROR_X11_SCREEN_INFO`
 - `GLOBOX_ERROR_X11_WIN_INFO`
 - `GLOBOX_ERROR_X11_STATE`
 - `GLOBOX_ERROR_X11_INTERACTIVE`
 - `GLOBOX_ERROR_X11_EGL_FAIL` (EGL context)
 - `GLOBOX_ERROR_X11_GLX_FAIL` (GLX context)

Windows-specific error values:
 - `GLOBOX_ERROR_WINDOWS_SYM`
 - `GLOBOX_ERROR_WINDOWS_MODULE_APP`
 - `GLOBOX_ERROR_WINDOWS_MODULE_USER32`
 - `GLOBOX_ERROR_WINDOWS_DELETE`
 - `GLOBOX_ERROR_WINDOWS_DEVICE_CONTEXT_GET`
 - `GLOBOX_ERROR_WINDOWS_MESSAGE_GET`
 - `GLOBOX_ERROR_WINDOWS_CLASS_REGISTER`
 - `GLOBOX_ERROR_WINDOWS_WINDOW_CREATE`
 - `GLOBOX_ERROR_WINDOWS_WINDOW_ADJUST`
 - `GLOBOX_ERROR_WINDOWS_WINDOW_UPDATE`
 - `GLOBOX_ERROR_WINDOWS_WINDOW_RECT_GET`
 - `GLOBOX_ERROR_WINDOWS_WINDOW_POS_SET`
 - `GLOBOX_ERROR_WINDOWS_WINDOW_LONG_GET`
 - `GLOBOX_ERROR_WINDOWS_WINDOW_LONG_SET`
 - `GLOBOX_ERROR_WINDOWS_PLACEMENT_GET`
 - `GLOBOX_ERROR_WINDOWS_PLACEMENT_SET`
 - `GLOBOX_ERROR_WINDOWS_CLIENT_RECT`
 - `GLOBOX_ERROR_WINDOWS_CLIENT_POS`
 - `GLOBOX_ERROR_WINDOWS_TRANSPARENCY_REGION`
 - `GLOBOX_ERROR_WINDOWS_TRANSPARENCY_DWM`
 - `GLOBOX_ERROR_WINDOWS_COMP_ATTR`
 - `GLOBOX_ERROR_WINDOWS_INPUT_SEND`
 - `GLOBOX_ERROR_WINDOWS_CAPTURE_RELEASE`
 - `GLOBOX_ERROR_WINDOWS_CURSOR_LOAD`
 - `GLOBOX_ERROR_WINDOWS_CURSOR_POS_GET`
 - `GLOBOX_ERROR_WINDOWS_CURSOR_POS_SET`
 - `GLOBOX_ERROR_WINDOWS_BMP_MASK_CREATE`
 - `GLOBOX_ERROR_WINDOWS_BMP_COLOR_CREATE`
 - `GLOBOX_ERROR_WINDOWS_ICON_CREATE`
 - `GLOBOX_ERROR_WINDOWS_ICON_SMALL`
 - `GLOBOX_ERROR_WINDOWS_ICON_BIG`
 - `GLOBOX_ERROR_WINDOWS_UTF8`
 - `GLOBOX_ERROR_WINDOWS_TITLE`

Windows-specific error values for the software context:
 - `GLOBOX_ERROR_WINDOWS_GDI_DAMAGE`
 - `GLOBOX_ERROR_WINDOWS_GDI_PAINT`
 - `GLOBOX_ERROR_WINDOWS_GDI_BITBLT`
 - `GLOBOX_ERROR_WINDOWS_GDI_DIB_CREATE`
 - `GLOBOX_ERROR_WINDOWS_GDI_BITMAP_SELECT`
 - `GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_CREATE`
 - `GLOBOX_ERROR_WINDOWS_GDI_DEVICE_CONTEXT_DELETE`

Windows-specific error values for the EGL context:
 - `GLOBOX_ERROR_WINDOWS_EGL_INIT`
 - `GLOBOX_ERROR_WINDOWS_EGL_API_BIND`
 - `GLOBOX_ERROR_WINDOWS_EGL_DISPLAY_GET`
 - `GLOBOX_ERROR_WINDOWS_EGL_CONFIG_CHOOSE`
 - `GLOBOX_ERROR_WINDOWS_EGL_CONFIG_ATTRIB_GET`
 - `GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_CREATE`
 - `GLOBOX_ERROR_WINDOWS_EGL_CONTEXT_DESTROY`
 - `GLOBOX_ERROR_WINDOWS_EGL_SURFACE_CREATE`
 - `GLOBOX_ERROR_WINDOWS_EGL_SURFACE_DESTROY`
 - `GLOBOX_ERROR_WINDOWS_EGL_MAKE_CURRENT`
 - `GLOBOX_ERROR_WINDOWS_EGL_TERMINATE`
 - `GLOBOX_ERROR_WINDOWS_EGL_SWAP`

Windows-specific error values for the WGL context:
 - `GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_CHOOSE`
 - `GLOBOX_ERROR_WINDOWS_WGL_PIXEL_FORMAT_SET`
 - `GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_CREATE`
 - `GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_DELETE`
 - `GLOBOX_ERROR_WINDOWS_WGL_CONTEXT_SET`
 - `GLOBOX_ERROR_WINDOWS_WGL_SWAP`

macOS-specific error values:
 - `GLOBOX_ERROR_MACOS_CLASS_GET`
 - `GLOBOX_ERROR_MACOS_CLASS_ALLOC`
 - `GLOBOX_ERROR_MACOS_CLASS_ADDVAR`
 - `GLOBOX_ERROR_MACOS_CLASS_ADDMETHOD`
 - `GLOBOX_ERROR_MACOS_OBJ_INIT`
 - `GLOBOX_ERROR_MACOS_OBJ_NIL`
 - `GLOBOX_ERROR_MACOS_EGL_FAIL` (EGL context)

```
enum globox_state
```
Describes the state of the window:
 - `GLOBOX_STATE_REGULAR`
 - `GLOBOX_STATE_MINIMIZED`
 - `GLOBOX_STATE_MAXIMIZED`
 - `GLOBOX_STATE_FULLSCREEN`

```
enum globox_interactive_mode
```
Describes a window interaction.
The special zero-value `GLOBOX_INTERACTIVE_STOP` means no interaction.
Other valid values include:
 - `GLOBOX_INTERACTIVE_MOVE`
 - `GLOBOX_INTERACTIVE_N` (North)
 - `GLOBOX_INTERACTIVE_NW` (North-West)
 - `GLOBOX_INTERACTIVE_W` (West)
 - `GLOBOX_INTERACTIVE_SW` (South-West)
 - `GLOBOX_INTERACTIVE_S` (South)
 - `GLOBOX_INTERACTIVE_SE` (South-East)
 - `GLOBOX_INTERACTIVE_E` (East)
 - `GLOBOX_INTERACTIVE_NE` (North-East)

```
struct globox
```
Holds all the information about your window.
This is the context you can use with all the library's functions.

## Generic functions (implemented in the same way for all platforms)
```
void globox_error_reset(struct globox* globox)
```
Clears the internal error report.
This must be done after any error before calling a Globox function again.

```
void globox_error_basic_log(struct globox* globox)
```
Prints a simple description of the internal error on `stderr`.

```
char* globox_error_output_log(struct globox* globox)
```
Returns a pointer to the description message matching the internal error.

```
enum globox_error globox_error_output_code(struct globox* globox)
```
Returns the internal error value.

```
char globox_error_catch(struct globox* globox)
```
Returns true if any error occured.

```
void globox_open(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height,
	const char* title,
	enum globox_state state,
	void (*callback)(
		void* event,
		void* data),
	void* callback_data)
```
Initializes a Globox context with the given properties.

```
void globox_close(struct globox* globox)
```
Cleans the Globox context.

```
void globox_set_closed(struct globox* globox, bool closed)
```
Closes the window programmatically.

## Platform-specific functions (implemented specifically for each platform)
```
void globox_platform_free(struct globox* globox);
```
Cleans the platform-specific bits of the Globox context.

```
void globox_platform_create_window(struct globox* globox);
```
Creates the actual window object for the target windowing system.

```
void globox_platform_hooks(struct globox* globox);
```
Finishes configuring the window after the context was properly created.
This must only be called after successfully creating a context.

```
void globox_platform_commit(struct globox* globox);
```
Flushes all window configuration requests.

```
void globox_platform_prepoll(struct globox* globox);
```
Performs the window configuration tasks with a dependency on the previous ones.
This must only be called after successfully flushing configuration requests.

```
void globox_platform_events_poll(struct globox* globox);
```
Prepares any queued window event.
Returns immediately.

```
void globox_platform_events_wait(struct globox* globox);
```
Waits for the queue to contain at least one window and prepares it.
Returns after an event was succesfully received.

```
void globox_platform_interactive_mode(
	struct globox* globox,
	enum globox_interactive_mode mode);
```
Starts or ends a window interaction.
This must be called when a mouse button is being pressed down.
Otherwise, some windowing systems might not accept the interaction request.

```
void globox_platform_init(
	struct globox* globox,
	bool transparent,
	bool blurred);
```
Initializes the platform-specific bits of the Globox context.

```
void globox_platform_events_handle(
	struct globox* globox);
```
Handles any window event previously prepared.

```
void globox_platform_prepare_buffer(
	struct globox* globox);
```
Prepares the buffer for drawing.
Must be called before copying.

```
void globox_platform_set_icon(
	struct globox* globox,
	uint32_t* pixmap,
	uint32_t len);
```
Sets the window's icon on the target windowing system.
The pixmap must be a valid X11 pixmap.

```
void globox_platform_set_title(
	struct globox* globox,
	const char* title); 
```
Sets the window's title on the target windowing system.
The encoding must be UTF-8.

```
void globox_platform_set_state(
	struct globox* globox,
	enum globox_state state); 
```
Sets the window's state on the target windowing system.

# Context-specific functions (implemented specifically for each platform)
## Software context
```
void globox_context_software_free(struct globox* globox);
```
Cleans the context-specific bits of the Globox context.

```
void globox_context_software_create(struct globox* globox);
```
Creates the software buffer and prepares the window.

```
void globox_context_software_shrink(struct globox* globox);
```
Shrinks the software buffer so it strictly covers the window area,
freeing the extra space used to reduce system strain while resizing.

```
void globox_context_software_init(
	struct globox* globox,
	int version_major,
	int version_minor);
```
Initializes the context-specific bits of the Globox context for the target OS.

```
void globox_context_software_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);
```
Copies the software buffer on screen.

## Vulkan context
Vulkan contexts are not implemented yet.
Contributions welcome!

## EGL context
```
void globox_context_egl_free(struct globox* globox);
```
Cleans the context-specific bits of the Globox context.

```
void globox_context_egl_create(struct globox* globox);
```
Creates the EGL context and prepares the window.

```
void globox_context_egl_shrink(struct globox* globox);
```
Dummy function.

```
void globox_context_egl_init(
	struct globox* globox,
	int version_major,
	int version_minor);
```
Initializes the context-specific bits of the Globox context for the target OS.

```
void globox_context_egl_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);
```
Swaps the OpenGL or GL ES buffer

## WGL context
```
void globox_context_wgl_free(struct globox* globox);
```
Cleans the context-specific bits of the Globox context.

```
void globox_context_wgl_create(struct globox* globox);
```
Creates the WGL context and prepares the window.

```
void globox_context_wgl_shrink(struct globox* globox);
```
Dummy function.

```
void globox_context_wgl_init(
	struct globox* globox,
	int version_major,
	int version_minor);
```
Initializes the context-specific bits of the Globox context for the target OS.

```
void globox_context_wgl_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);
```
Swaps the OpenGL buffer.

## GLX context
```
void globox_context_glx_free(struct globox* globox);
```
Cleans the context-specific bits of the Globox context.

```
void globox_context_glx_create(struct globox* globox);
```
Creates the GLX context and prepares the window.

```
void globox_context_glx_shrink(struct globox* globox);
```
Dummy function.

```
void globox_context_glx_init(
	struct globox* globox,
	int version_major,
	int version_minor);
```
Initializes the context-specific bits of the Globox context for the target OS.

```
void globox_context_glx_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height);
```
Swaps the OpenGL or GL ES buffer.

## Generic getters & setters (implemented in the same way for all platforms)
```
int32_t globox_get_x(struct globox* globox);
```
Returns the initial x position used when creating the window.

```
int32_t globox_get_y(struct globox* globox);
```
Returns the initial y position used when creating the window.

```
uint32_t globox_get_width(struct globox* globox);
```
Returns the current window width.

```
uint32_t globox_get_height(struct globox* globox);
```
Returns the current window height.

```
char* globox_get_title(struct globox* globox);
```
Returns the last saved window title.

```
bool globox_get_closed(struct globox* globox);
```
Returns true if the window has been closed.

```
bool globox_get_redraw(struct globox* globox);
```
Returns true if the windowing system requires the window to be redrawn.

```
enum globox_state globox_get_state(struct globox* globox);
```
Returns the current window state.

```
enum globox_interactive_mode
	globox_get_interactive_mode(struct globox* globox);
```
Returns the current window interaction.

```
bool globox_get_transparent(struct globox* globox);
```
Returns the initial transparency setting used when creating the window.

```
bool globox_get_blurred(struct globox* globox);
```
Returns the initial background blur setting used when creating the window.

```
void (*globox_get_event_callback(struct globox* globox))(void* event, void* data);
```
Returns the initial event callback function pointer used when creating the window.

```
void* globox_get_event_callback_data(struct globox* globox);
```
Returns the initial event callback data pointer used when creating the window.

## Platform-specific getters & setters (implemented specifically for each platform)
```
uint32_t* globox_platform_get_argb(struct globox* globox);
```
Returns a pointer to the internal argb software buffer.

```
void globox_wayland_save_serial(struct globox* globox, uint32_t serial);
```
Saves the given event serial so Globox can properly fool Wayland
when asked to programmatically initiate a window interaction.
Only available for the Wayland platform.

```
double globox_macos_get_egl_scale(struct globox* globox);
```
Returns the window's backing scale to allow manually scaling EGL geometry,
since ANGLE's EGL contexts require this on retina© displays.
Only available for the macOS platform.
