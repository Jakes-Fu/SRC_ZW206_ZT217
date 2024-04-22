/*****************************************************************************

 ****************************************************************************/


#ifndef _DUERAPP_IMAGE_H_
#define _DUERAPP_IMAGE_H_

#include "sci_types.h"
#include "mmi_imagefun.h"
#include "mmi_module.h"
#ifdef __cplusplus
extern   "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Image Resource ID
///////////////////////////////////////////////////////////////////////////////
#define MACRO_MDU_TYPE  MACRO_MDU_IMAGE
#include "macro_mdu_def.h"

typedef enum DUERAPP_MMI_IMAGE_ID_E
{
	DUERAPP_IMAGE_NULL = ( MMI_MODULE_DUER << 16 ) | MMI_SYS_RES_FLAG,
#ifndef CODE_MATCH_RES_CHECK
	#include "duer_mdu_def.h"
#endif
	DUERAPP_IMAGE_MAX_ID
} DUERAPP_MMI_IMAGE_ID_E;

#undef MACRO_MDU_TYPE

#ifdef __cplusplus
}
#endif

#endif //APPSAMPLE_MMI_IMAGE_H_