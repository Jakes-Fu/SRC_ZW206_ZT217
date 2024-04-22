#ifndef _DUERAPP_TEXT_H_
#define _DUERAPP_TEXT_H_

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

typedef enum DUERAPP_MMI_TEXT_ID_E
{
	DUERAPP_TXT_NULL = ( MMI_MODULE_DUER << 16 ) | MMI_SYS_RES_FLAG,
#ifndef CODE_MATCH_RES_CHECK
	#include "duer_mdu_def.h"
#endif
	DUERAPP_TXT_MAX
} DUERAPP_MMI_TEXT_ID_E;

#undef MACRO_MDU_TYPE
#ifdef __cplusplus
}
#endif

#endif // APPSAMPLE_MMI_TEXT_H_
