// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_id.c
 * Auth: Liuwenshuai (liuwenshuai@baidu.com)
 * Desc: duerapp main.
 */
 /****************************************************************************/


/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmi_module.h"
#include "mmi_modu_main.h"

#define WIN_ID_DEF(win_id)  #win_id

const uint8 mmiappbaidu_id_name_arr[][MMI_WIN_ID_NAME_MAX_LENGTH] =
{
    #include "duerapp_id.def"    
};

#undef WIN_ID_DEF

/*****************************************************************************/
// 	Description : Register app samle win and win name
//	Global resource dependence : none
//  Author: sam.hua
//	Note:
/*****************************************************************************/
PUBLIC void MMIDUERAPP_RegWinIdNameArr(void)
{
    MMI_RegWinIdNameArr(MMI_MODULE_DUER, (const uint8 **)mmiappbaidu_id_name_arr);
}
