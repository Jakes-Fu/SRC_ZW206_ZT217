/*****************************************************************************
** File Name:      mmidropdownwin_nv.c                                       *
** Author:                                                                   *
** Date:           01/07/2011                                                *
** Copyright:      2011 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe dropdown win                *
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 07/2011       Paul.Huang          Create                                  *
******************************************************************************/

#define _MMIDROPDOWNWIN_NV_C_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#ifdef PDA_UI_DROPDOWN_WIN
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmi_modu_main.h"
#include "mmidropdownwin_export.h"

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*
 **                         LOCAL DEFINITION                                 *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DECLARE                                 *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/
static const uint16 s_dropdown_notify_nv_len[] =
{
    DROPDOWN_NOTIFY_MAX*sizeof(MMIDROPDOWN_NOTIFY_ITEM_T)
};

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : register module nv len and max item
//  Global resource dependence : none
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_RegNv(void)
{
    MMI_RegModuleNv(MMI_MODULE_DROPDOWNWIN,s_dropdown_notify_nv_len,sizeof(s_dropdown_notify_nv_len)/sizeof(uint16));
}
#endif
