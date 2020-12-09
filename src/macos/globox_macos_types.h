#ifndef H_GLOBOX_MACOS_TYPES
#define H_GLOBOX_MACOS_TYPES

// defines Class, id, etc.
#include <objc/objc.h>

// missing native structs
struct macos_point
{
	double x;
	double y;
};

struct macos_size
{
	double width;
	double height;
};

struct macos_rect
{
	struct macos_point origin;
	struct macos_size size;
};

struct macos_appdelegate
{
	Class isa;
	id window;
};

#endif
