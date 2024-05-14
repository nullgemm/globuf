#ifndef H_GLOBUF_INTERNAL_X11_COMMON
#define H_GLOBUF_INTERNAL_X11_COMMON

#include "include/globuf.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcb/sync.h>
#include <xcb/xcb.h>

// # private helpers
enum x11_atoms
{
	X11_ATOM_STATE_MAXIMIZED_HORIZONTAL = 0,
	X11_ATOM_STATE_MAXIMIZED_VERTICAL,
	X11_ATOM_STATE_FULLSCREEN,
	X11_ATOM_STATE_HIDDEN,
	X11_ATOM_STATE,
	X11_ATOM_ICON,
	X11_ATOM_HINTS_MOTIF,
	X11_ATOM_BLUR_KDE,
	X11_ATOM_BLUR_DEEPIN,
	X11_ATOM_PROTOCOLS,
	X11_ATOM_DELETE_WINDOW,
	X11_ATOM_SYNC_REQUEST,
	X11_ATOM_SYNC_REQUEST_COUNTER,
	X11_ATOM_FRAME_DRAWN,
	X11_ATOM_CHANGE_STATE,
	X11_ATOM_NET_SUPPORTED,
	X11_ATOM_NET_FRAME_EXTENTS,
	X11_ATOM_COUNT,
};

enum globuf_xsync_status
{
	GLOBUF_XSYNC_WAITING = 0,
	GLOBUF_XSYNC_CONFIGURED,
	GLOBUF_XSYNC_ACKNOWLEDGED,
	GLOBUF_XSYNC_FINISHED,
};

struct x11_thread_render_loop_data
{
	struct globuf* globuf;
	struct x11_platform* platform;
	struct globuf_error_info* error;
};

struct x11_thread_event_loop_data
{
	struct globuf* globuf;
	struct x11_platform* platform;
	struct globuf_error_info* error;
};

struct x11_platform
{
	pthread_mutex_t mutex_main;
	pthread_mutex_t mutex_block;
	pthread_mutex_t mutex_xsync;
	pthread_cond_t cond_main;

	// connection
	bool closed;
	xcb_connection_t* conn;

	// x11
	int screen_id;
	xcb_screen_t* screen_obj;
	xcb_window_t root_win;
	xcb_atom_t atoms[X11_ATOM_COUNT];

	// window
	uint32_t attr_mask;
	uint32_t attr_val[3];
	xcb_window_t win;
	int visual_depth;
	xcb_visualid_t visual_id;

	// xsync
	enum globuf_xsync_status xsync_status;
	xcb_sync_counter_t xsync_counter;
	xcb_sync_int64_t xsync_value;
	unsigned xsync_width;
	unsigned xsync_height;

	// saved action for globuf mouse action
	int32_t old_mouse_pos_x;
	int32_t old_mouse_pos_y;
	int32_t saved_mouse_pos_x;
	int32_t saved_mouse_pos_y;
	bool saved_window;
	int32_t saved_window_geometry[4];
	bool sizemove;

	// render handling
	pthread_t thread_render_loop;
	struct x11_thread_render_loop_data thread_render_loop_data;
	void (*render_init_callback)(struct globuf*, struct globuf_error_info*);

	// event handling
	pthread_t thread_event_loop;
	struct x11_thread_event_loop_data thread_event_loop_data;
	void (*event_init_callback)(struct globuf*, struct globuf_error_info*);
};

void globuf_x11_common_init(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_common_clean(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_common_window_create(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_config_request* configs,
	size_t count,
	void (*callback)(struct globuf_config_reply* replies, size_t count, void* data),
	void* data,
	struct globuf_error_info* error);

void globuf_x11_common_window_destroy(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_common_window_confirm(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_common_window_start(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_common_window_block(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_common_window_stop(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);


void globuf_x11_common_init_render(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_config_render* config,
	struct globuf_error_info* error);

void globuf_x11_common_init_events(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_config_events* config,
	struct globuf_error_info* error);

enum globuf_event globuf_x11_common_handle_events(
	struct globuf* context,
	struct x11_platform* platform,
	void* event,
	struct globuf_error_info* error);


struct globuf_config_features* globuf_x11_common_init_features(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

void globuf_x11_common_feature_set_interaction(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_feature_interaction* config,
	struct globuf_error_info* error);

void globuf_x11_common_feature_set_state(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_feature_state* config,
	struct globuf_error_info* error);

void globuf_x11_common_feature_set_title(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_feature_title* config,
	struct globuf_error_info* error);

void globuf_x11_common_feature_set_icon(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_feature_icon* config,
	struct globuf_error_info* error);


unsigned globuf_x11_common_get_width(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

unsigned globuf_x11_common_get_height(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

struct globuf_rect globuf_x11_common_get_expose(
	struct globuf* context,
	struct x11_platform* platform,
	struct globuf_error_info* error);

#endif
