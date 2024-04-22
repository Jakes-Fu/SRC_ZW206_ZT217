/*****************************************************************************
** File Name:      wifisupp_eut.h                                            *
** Author:         bruce.chi                                                 *
** Date:           5/18/2010                                                 *
** Copyright:      2003 Spreatrum, Incoporated. All Rights Reserved.         *
** Description:    This file is used for wifi eut api definition             *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 5/18/2010     bruce.chi         Create                                     *
******************************************************************************/

#ifndef _WIFISUPP_EUT_H_
#define _WIFISUPP_EUT_H_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "os_api.h"
#include "wifisupp_api.h"

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
 **                         typedef                                          *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          Enum                                             */
/*---------------------------------------------------------------------------*/
typedef enum
{
	WIFI_MODE_DSSS_BPSK = 0,
	WIFI_MODE_DSSS_DQPSK,
	WIFI_MODE_DSSS_CCK,
	WIFI_MODE_OFDM_BPSK,
	WIFI_MODE_OFDM_DQPSK,
	WIFI_MODE_16QAM,
	WIFI_MODE_64QAM,
	WIFI_MODE_MAX
}WIFI_MODE_E;

/*---------------------------------------------------------------------------*/
/*                          Struct                                           */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                          Global variables                                 */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                          EXTERNAL FUNCTION DECLARE                        */
/*---------------------------------------------------------------------------*/
/****************************************************************************/
//Description : WIFI Eut init
//Global resource dependence : 
//Author: bruce.chi
//Note: 
/****************************************************************************/
PUBLIC BOOLEAN WIFISUPP_EUTInit(void);

#ifdef   __cplusplus
    }
#endif


#endif 


