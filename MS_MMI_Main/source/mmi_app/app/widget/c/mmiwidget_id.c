/*************************************************************************
 ** File Name:      mmiwidget_id.c                                          *
 ** Author:         haiyang.hu                                           *
 ** Date:           2006/09/18                                           *
 ** Copyright:      2006 Spreadtrum, Incorporated. All Rights Reserved.  *
 ** Description:     This file defines the function about nv             *
 *************************************************************************
 *************************************************************************
 **                        Edit History                                  *
 ** ---------------------------------------------------------------------*
 ** DATE           NAME             DESCRIPTION                          *
 ** 2006/09/18     haiyang.hu       Create.                              *
*************************************************************************/

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
//#include "std_header.h"
//#include "sci_types.h"
#include "mmi_module.h"
#include "mmi_modu_main.h"
#include "mmiwidget_id.h"
#define WIN_ID_DEF(win_id)          #win_id

LOCAL const uint8 mmiwidget_id_name_arr[][MMI_WIN_ID_NAME_MAX_LENGTH] =
{
    #include "mmiwidget_id.def"    
};

#undef WIN_ID_DEF

/*****************************************************************************/
//  Description : Register acc menu group
//  Global resource dependence : none
//  Author: haiyang.hu
//  Note:
/*****************************************************************************/
PUBLIC void MMIWIDGET_RegWinIdNameArr(void)
{
    MMI_RegWinIdNameArr(MMI_MODULE_WIDGET, mmiwidget_id_name_arr);        /*lint !e64*/
}


