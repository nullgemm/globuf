#ifndef H_EXAMPLE_DESKTOP
#define H_EXAMPLE_DESKTOP

#include "globox.h"
#include "cursoryx.h"
#include "dpishit.h"
#include "willis.h"

struct event_data
{
	struct globox* globox;
	struct cursoryx* cursoryx;
	struct dpishit* dpishit;
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
