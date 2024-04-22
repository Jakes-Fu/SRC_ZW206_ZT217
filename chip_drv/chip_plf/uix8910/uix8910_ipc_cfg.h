/******************************************************************************
 ** File Name:    uix8910_ipc_cfg.c                                            *
 ** Author:       dejun.wei                                                 *
 ** DATE:         07/09/2018                                                  *
 ** Copyright:    2010  Spreadtrum, Incoporated. All Rights Reserved.           *
 ** Description:                                                              *
 ******************************************************************************/
/******************************************************************************
 **                   Edit    History                                         *
 **---------------------------------------------------------------------------*
 ** DATE          NAME            DESCRIPTION                                 *
 ** 06/05/2010    Steve.zhan      Create.                                     *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **------------------------------------------------------------------------- */
#include "sci_types.h"
#include "chip_plf_export.h"

/**---------------------------------------------------------------------------*
 **                             Compiler Flag                                 *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
extern   "C"
{
#endif

/**---------------------------------------------------------------------------*
**                               Micro Define                                **
**---------------------------------------------------------------------------*/
#if defined(CHIP_VER_UIS8910C) || defined(CHIP_VER_UIS8910A) || defined(CHIP_VER_UIX8910) || defined(CHIP_VER_UWS6121E) 
#define SM_BASE 0x800F3800	
#define SM_LEN	0x100800	// 1MB
#define UL_BIG_BUF_CNT	(128+100)
#define UL_LIT_BUF_CNT	(256+256)
#define DL_BIG_BUF_CNT	256
#define DL_LIT_BUF_CNT	256
#define MD_VERSION_OFF	0x1006D8
#define MD_EXEC_CAUSE_OFF	0x100700
#else
#define SM_BASE 0x80105400	
#define SM_LEN	0x17DC00	// 1.5MB
#define UL_BIG_BUF_CNT	256
#define UL_LIT_BUF_CNT	512
#define DL_BIG_BUF_CNT	512
#define DL_LIT_BUF_CNT	512
#define MD_VERSION_OFF	0x17DAD8
#define MD_EXEC_CAUSE_OFF	0x17DB00
#endif
/**----------------------------------------------------------------------------*
**                         Compiler Flag                                      **
**----------------------------------------------------------------------------*/
#ifdef   __cplusplus
}
#endif
/**---------------------------------------------------------------------------*/
