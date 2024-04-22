/****************************************************************************
** File Name:      mmiphs_app.h                                                *
** Author:                                                                 *
** Date:           03/22/2006                                              *
** Copyright:      2006 TLT, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the PHS                   *
/****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 03/2006       Jianshengqi         Create
** 
****************************************************************************/
#ifndef  _MMI_ZYB_APP_H_    
#define  _MMI_ZYB_APP_H_   

#ifdef __cplusplus
    extern "C"
    {
#endif
/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "zyb_common.h"
#include "zyb_id.h"
#include "zyb_menutable.h"
#include "zyb_nv.h"
#include "zyb_text.h"
#include "zyb_image.h"
#include "zyb_anim.h"

#define ZYB_LOG            ZYB_Trace

typedef struct
{
       uint8 *        str;
	uint32         len; 
} MMI_ZYB_DATA_T;

typedef struct
{
	SIGNAL_VARS
       MMI_ZYB_DATA_T data;
} MMI_ZYB_SIG_T ;


extern void MMIZYB_AppInit(void);
extern void MMIZYB_RegWinIdNameArr(void);
extern void MMIZYB_InitModule(void);
extern void MMIZYB_RegMenuGroup(void);
extern void MMIZYB_RegNv(void);
extern BOOLEAN  MMIZYB_SendSigTo_APP(ZYB_APP_SIG_E sig_id, uint8 * data_ptr, uint32 data_len);

#ifdef __cplusplus
    }
#endif

#endif
