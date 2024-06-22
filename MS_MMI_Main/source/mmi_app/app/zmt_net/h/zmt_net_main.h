#ifndef _ZMT_EXPORT_H_
#define _ZMT_EXPORT_H_

#include "sci_types.h"
#include "os_api.h"
#include "mmk_type.h"
#include "mmi_module.h"

typedef enum
{
	ZMT_WIN_ID_START = (MMI_MODULE_ZMT_NET << 16),
	ZMT_MAIN_WIN_ID,
		
	ZMT_MAX_WIN_ID
}ZMT_WINDOW_ID_E;



#endif
