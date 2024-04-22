/******************************************************************************
 ** File Name:      global_ctl_hal.c                                             *
 ** Author:         Yong.Li                                                  *
 ** DATE:           12/22/2011                                                *
 ** Copyright:      2007 Spreadtrum, Incoporated. All Rights Reserved.         *
 ** Description:    This file defines the basic function for ldo management.  *
 ******************************************************************************/

/******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 12/22/2011     Yong.Li          Create.                                   *
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "os_api.h"
#include "chip_plf_export.h"
#include "global_ctl_drvapi.h"



/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
extern   "C"
{
#endif

/**---------------------------------------------------------------------------*
 **                         Local variables                                  *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                         Global variables                                  *
 **---------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*
 **                         Function Declaration                              *
 **---------------------------------------------------------------------------*/


/*****************************************************************************/
//  Description:  Global Ctl
//  Global resource dependence:
//  Author: Yong.Li
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN Global_CTL (GLB_MODULE_E module, GLB_CMD_E cmd, uint32 *arg)
{
	BOOLEAN ret = SCI_TRUE;
	
	switch(cmd)
	{
		case GLB_CMD_PORT_SEL:  // port select
			GlB_CTL_PORT_SEL(module, (uint32)*arg);
		break;
		
		default:
			SCI_ASSERT(0); /*assert to do*/ 
		break;
	}
	
	return ret;
}


/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
}
#endif

