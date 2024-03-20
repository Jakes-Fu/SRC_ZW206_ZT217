/******************************************************************************
 ** File Name:      bt_prod.h                                                 *
 ** Author:         Liangwen.Zhen                                             *
 ** DATE:           07/26/2007                                                *
 ** Copyright:      2007 Spreadtrum, Incoporated. All Rights Reserved.        *
 ** Description:    This file defines the basic operation interfaces of BT    *
 **					about product
 **                                                                           *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 01/11/2008     Liangwen.Zhen    Create.                         *
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/ 


/**---------------------------------------------------------------------------*
 **                         Debugging Flag                                    *
 **---------------------------------------------------------------------------*/
#ifndef _BT_PROD_H_
#define _BT_PROD_H_

#include "bt_abs.h"

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    extern   "C" 
    {
#endif
 

/**---------------------------------------------------------------------------*
 **                         Constant Variables                                *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                     Local Function Prototypes                             *
 **---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description:    This function is used to restore system clock about BT
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC void BTI_RestoreSystemClk(uint8 type);

/*****************************************************************************/
//  Description:    This function is used to change system clock about BT
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC void BTI_ChangeSystemClk(uint8 type);

/*****************************************************************************/
//  Description:    This function is used to save bd address
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC void BTI_SaveBdAddr(const BT_ADDRESS *addr);

/*****************************************************************************/
//  Description:    This function is used to save xtal ftrim
//  Author:         Liangwen.Zhen
//  Note:           
/*****************************************************************************/
PUBLIC void BTI_SaveXtalFtrim(uint16 ftrim);

/*****************************************************************************/
//  Description:    This function is used to get BT configure information
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC void BT_GetConfigInfo(BT_CONFIG *config);

/*****************************************************************************/
//  Description:    This function is used to start up BT chip    
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC void BTI_StartupChip(void);

/*****************************************************************************/
//  Description:    This function is used to shut down BT chip
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC void BTI_ShutdownChip(void);

#ifdef BT_HL_PATCH_SUPPORT

/*****************************************************************************/
//  Description:    This function is HL BT init for charge mode
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC uint8 BTI_EarlyPatch(void);

/*****************************************************************************/
//  Description:    This function is HL BT init for normal mode
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC uint8 BTI_LaterPatch(void);

/*****************************************************************************/
//  Description:    This function is HL BT init for Eut mode
//  Author:         
//  Note:           
/*****************************************************************************/
PUBLIC uint8 BTI_EutPatch(void);

#endif

/*****************************************************************************/
//  Description:    
//  Author:         
//  Note:           
/*****************************************************************************/
BOOLEAN BTI_GSMTable_GetSchedStatus(void);

/*****************************************************************************/
//  Description:    
//  Author:         
//  Note:           
/*****************************************************************************/
void BTI_GSMTable_SendMsgToSchedTask(uint32 event, void *msg_ptr);

/*****************************************************************************/
//  Description:    
//  Author:         
//  Note:           
/*****************************************************************************/
BOOLEAN BTI_IsSystemBusy(void);

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
    
#endif  // End of bt_prod.h

#endif  // end of _BT_PROD_H_
