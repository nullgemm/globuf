#ifndef H_GLOBOX_X11_SOFTWARE
#define H_GLOBOX_X11_SOFTWARE

#include "globox.h"
#include "globox_software.h"

// for this backend, `data` is of type `struct globox_update_software*`
void globox_software_update_content(
	struct globox* context,
	void* data);

#endif
