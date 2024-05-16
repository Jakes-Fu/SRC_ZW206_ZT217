/*****************************************************************************
** File Name:      zmt_dial_text.h                                            *
** Author:           fys                                                        *
** Date:           2024/05/08                                                 *
** Copyright:       *
** Description:                       *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** Create by Spreadtrum Resource Editor tool                                 *
******************************************************************************/

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
#include "zmt_dial_mdu_def.h"
#endif

typedef enum ZMT_DIAL_MMI_TEXT_ID_E
{
	ZMT_DIAL_TXT_NULL = ( MMI_MODULE_ZMT_DIAL << 16 ) | MMI_SYS_RES_FLAG,

#include "zmt_dial_mdu_def.h"

	ZMT_DIAL_TXT_MAX
} ZMT_DIAL_MMI_TEXT_ID_E;

#undef MACRO_MDU_TYPE

#ifdef __cplusplus
}
#endif

