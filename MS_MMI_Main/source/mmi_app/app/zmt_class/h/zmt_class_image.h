/*****************************************************************************
** File Name:      zmt_class_image.h                                               *
** Author:         fys                                               *
** Date:           2024/06/27                                                *
******************************************************************************/

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

#ifdef CODE_MATCH_RES_CHECK
#include "zmt_class_mdu_def.h"
#endif

typedef enum ZMT_CLASS_MMI_IMAGE_ID_E
{
	ZMT_HANZI_IMAGE_NULL = ( MMI_MODULE_ZMT_CLASS << 16 ) | MMI_SYS_RES_FLAG,
#ifndef CODE_MATCH_RES_CHECK
#include "zmt_class_mdu_def.h"
#endif
	ZMT_CLASS_IMAGE_MAX_ID
} ZMT_CLASS_MMI_IMAGE_ID_E;

#undef MACRO_MDU_TYPE

#ifdef __cplusplus
}
#endif

