/*****************************************************************************
** File Name:      zmt_class_text.h                                               *
** Author:         fys                                               *
** Date:           2024/06/27                                                *
******************************************************************************/
#ifndef ZMT_CLASS_TEXT_H
#define ZMT_CLASS_TEXT_H

#include "sci_types.h"
#include "mmi_module.h"

#ifdef __cplusplus
extern   "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Text Resource ID
///////////////////////////////////////////////////////////////////////////////

#define MACRO_MDU_TYPE  MACRO_MDU_TEXT
#include "macro_mdu_def.h"

#ifdef CODE_MATCH_RES_CHECK
#include "zmt_class_mdu_def.h"
#endif

typedef enum ZMT_CLASS_MMI_TEXT_ID_E
{
	ZMT_CLASS_TXT_NULL = ( MMI_MODULE_ZMT_CLASS << 16 ) | MMI_SYS_RES_FLAG,

#include "zmt_class_mdu_def.h"

	ZMT_CLASS_TXT_MAX
} ZMT_CLASS_MMI_TEXT_ID_E;

#undef MACRO_MDU_TYPE

#ifdef __cplusplus
}
#endif

#endif