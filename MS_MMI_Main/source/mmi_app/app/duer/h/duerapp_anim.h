/*****************************************************************************

******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** Create by Spreadtrum Resource Editor tool                                 *
******************************************************************************/

#ifndef _DUERAPP_MMI_ANIM_H_
#define _DUERAPP_MMI_ANIM_H_

#include "sci_types.h"
#include "mmi_module.h"

#ifdef __cplusplus
extern   "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Animation Resource ID
///////////////////////////////////////////////////////////////////////////////

#define MACRO_MDU_TYPE  MACRO_MDU_ANIM
#include "macro_mdu_def.h"

#ifdef CODE_MATCH_RES_CHECK
#include "duer_mdu_def.h"
#endif

typedef enum DUERAPP_MMI_ANIM_ID_E
{
	DUERAPP_IMAGE_AMIN_NULL = ( MMI_MODULE_DUER << 16 ) | MMI_SYS_RES_FLAG,
#ifndef CODE_MATCH_RES_CHECK
#include "duer_mdu_def.h"
#endif
	DUERAPP_IMAGE_AMIN_MAX
} DUERAPP_MMI_ANIM_ID_E;

#undef MACRO_MDU_TYPE

#ifdef __cplusplus
}
#endif

#endif
