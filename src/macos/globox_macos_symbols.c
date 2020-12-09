#include <objc/message.h>
#include <objc/runtime.h>

#include "macos/globox_macos_types.h"
#include "macos/globox_macos_symbols.h"

void (*macos_msg_void_none)(id, SEL) =
	(void (*)(id, SEL)) objc_msgSend;
id (*macos_msg_id_none)(id, SEL) =
	(id (*)(id, SEL)) objc_msgSend;
void (*macos_msg_void_bool)(id, SEL, BOOL) =
	(void (*)(id, SEL, BOOL)) objc_msgSend;
void (*macos_msg_void_id)(id, SEL, id) =
	(void (*)(id, SEL, id)) objc_msgSend;
void (*macos_msg_void_voidptr)(id, SEL, void*) =
	(void (*)(id, SEL, void*)) objc_msgSend;
BOOL (*macos_msg_bool_none)(id, SEL) =
	(BOOL (*)(id, SEL)) objc_msgSend;
void (*macos_msg_void_int)(id, SEL, int) =
	(void (*)(id, SEL, int)) objc_msgSend;
struct macos_point (*macos_msg_point_none)(id, SEL) =
	(struct macos_point (*)(id, SEL)) objc_msgSend;
struct macos_rect (*macos_msg_rect_none)(id, SEL) =
	(struct macos_rect (*)(id, SEL)) objc_msgSend_stret;
void (*macos_msg_void_size)(id, SEL, struct macos_size) =
	(void (*)(id, SEL, struct macos_size)) objc_msgSend;
id* (*macos_msg_idptr_none)(id, SEL) =
	(id* (*)(id, SEL)) objc_msgSend;
id* (*macos_msg_poll)(id, SEL, unsigned long long, id*, id*, BOOL) =
	(id* (*)(id, SEL, unsigned long long, id*, id*, BOOL)) objc_msgSend;
void (*macos_msg_resize)(id, SEL, struct macos_rect, BOOL) =
	(void (*)(id, SEL, struct macos_rect, BOOL)) objc_msgSend;
id (*macos_msg_id_size)(id, SEL, struct macos_size) =
	(id (*)(id, SEL, struct macos_size)) objc_msgSend;
unsigned long (*macos_msg_event_type)(id*, SEL) =
	(unsigned long (*)(id*, SEL)) objc_msgSend;
short (*macos_msg_event_subtype)(id*, SEL) =
	(short (*)(id*, SEL)) objc_msgSend;
id (*macos_msg_string)(id, SEL, char*, int) =
	(id (*)(id, SEL, char*, int)) objc_msgSend;
void (*macos_msg_post)(id, SEL, id*, BOOL) =
	(void (*)(id, SEL, id*, BOOL)) objc_msgSend;
id (*macos_msg_win)(id, SEL, struct macos_rect, int, int, BOOL) =
	(id (*)(id, SEL, struct macos_rect, int, int, BOOL)) objc_msgSend;
void (*macos_msg_void_ulong)(id, SEL, unsigned long) =
	(void (*)(id, SEL, unsigned long)) objc_msgSend;
id* (*macos_msg_idptr_int)(id, SEL, int) =
	(id* (*)(id, SEL, int)) objc_msgSend;
void (*macos_msgptr_void_bool)(id*, SEL, BOOL) =
	(void (*)(id*, SEL, BOOL)) objc_msgSend;
id (*macos_msg_id_rect)(id, SEL, struct macos_rect) =
	(id (*)(id, SEL, struct macos_rect)) objc_msgSend;
void (*macos_msg_subview)(id, SEL, id, int, id) =
	(void (*)(id, SEL, id, int, id)) objc_msgSend;
id* (*macos_msg_event)(
	id, SEL, unsigned long, struct macos_point, unsigned long,
	double, long, id, short, long, long) =
		(id* (*)(
			 id, SEL, unsigned long, struct macos_point, unsigned long,
			 double, long, id, short, long, long))
				objc_msgSend;
