#ifndef H_EXAMPLE_DESKTOP
#define H_EXAMPLE_DESKTOP

#include "globox.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"

#include <stdbool.h>

enum frame_section
{
	FRAME_SECTION_NONE = 0,
	FRAME_SECTION_BORDER_A,
	FRAME_SECTION_CORNER_A,
	FRAME_SECTION_BORDER_SIDE,
	FRAME_SECTION_CORNER_B,
	FRAME_SECTION_BORDER_B,
};

enum frame_state
{
	FRAME_STATE_IDLE = 0,
	FRAME_STATE_HOVER_TITLEBAR,
	FRAME_STATE_HOVER_MINIMIZE,
	FRAME_STATE_HOVER_MAXIMIZE,
	FRAME_STATE_HOVER_CLOSE,
	FRAME_STATE_HOVER_N,
	FRAME_STATE_HOVER_S,
	FRAME_STATE_HOVER_W,
	FRAME_STATE_HOVER_E,
	FRAME_STATE_HOVER_NE,
	FRAME_STATE_HOVER_SE,
	FRAME_STATE_HOVER_NW,
	FRAME_STATE_HOVER_SW,
};

struct frame_info
{
	unsigned title_size;
	unsigned button_size;
	unsigned button_icon_size;
	unsigned border_size;
	unsigned corner_reach;
	unsigned window_width;
	unsigned window_height;
	enum frame_state state;
	enum frame_state state_press;
	bool interactive;
	bool redraw;
};

struct event_data
{
	struct globox* globox;
	struct cursoryx* cursoryx;
	struct dpishit* dpishit;
	struct frame_info* frame_info;
};

void* example_cursoryx_data(
	struct globox* globox,
	struct willis* willis);

void* example_dpishit_data(
	struct globox* globox);

void* example_willis_data(
	struct globox* globox);

void example_willis_event_callback(
	struct willis* willis,
	enum willis_event_code event_code,
	enum willis_event_state event_state,
	void* data);

#endif
