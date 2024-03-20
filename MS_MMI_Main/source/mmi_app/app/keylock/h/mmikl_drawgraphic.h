/*****************************************************************************
** File Name:      mmikl_drawgraphic.h                                       *
** Author:         jian.ma                                                   *
** Date:           12/12/2011                                                *
** Copyright:      2011 Spreadtrum, Incoporated. All Rights Reserved.        *
** Description:    This file is used to describe drag icon or ring unlock    *
**                 function                                                  *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 12/2011        jian.ma	       Create
******************************************************************************/

#ifndef _MMIKL_DRAWGRAPHIC_H_
#define _MMIKL_DRAWGRAPHIC_H_ /*perl robot add*/

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmk_type.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif
		
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/******************************************************************************/
//  Description : handle kl display window msg of type4.
//  Global resource dependence : none
//  Author: 
//  Note: Pattern unlock style
/*****************************************************************************/
PUBLIC BOOLEAN MMIKL_HandleDrawGraphicWinMsg(
                                            MMI_WIN_ID_T    win_id, 
                                            uint16          msg_id, 
                                            DPARAM          param
                                            );
/*****************************************************************************/
//  Description : 打开设置图形加密窗口
//  Global resource dependence : 
//  Author:
//  Date:
/*****************************************************************************/
PUBLIC void MMIKL_OpenSetKLPassward(void);
/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif
