#include "include/globox.h"
#include "common/globox_private.h"
#include "x11/x11_common.h"

#include <stdlib.h>

void globox_x11_common_init(void** platform_data)
{
	struct x11_platform* data = malloc(sizeof (struct x11_platform));

	if (data == NULL)
	{
		globox_error_throw(context, GLOBOX_ERROR_ALLOC);
		return;
	}

	*platform = data;
}
