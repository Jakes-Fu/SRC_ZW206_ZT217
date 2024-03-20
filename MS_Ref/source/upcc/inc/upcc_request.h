/******************************************************************************
 ** File Name:    UPCC_request.h                                                *
 ** Author:       yuhua.shi                                                   *
 ** DATE:         3/23/2006                                                   *
 ** Copyright:    2006 Spreatrum, Incoporated. All Rights Reserved.           *
 ** Description:                                                              *
 ******************************************************************************/
 /*****************************************************************************
 **                   Edit    History                                         *
 **---------------------------------------------------------------------------* 
 ** DATE          NAME            DESCRIPTION                                 * 
 ** 3/23/2006     yuhua.shi       Create.                                     *
 ******************************************************************************/
#ifndef _UPCC_REQUEST_H_
    #define _UPCC_REQUEST_H_
    
/*----------------------------------------------------------------------------*
 **                         DEPENDENCIES                                      *
 **-------------------------------------------------------------------------- */           
#include "upcc_common.h"



/**---------------------------------------------------------------------------*
 **                         COMPILER FLAG                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern "C"
    {    
#endif

/**---------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                  *
 **---------------------------------------------------------------------------*/    
   
/**---------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                 *
 **---------------------------------------------------------------------------*/
 
/**---------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                               *
 **---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description: call the related function to handle the request  
//	Global resource dependence: 
//  Author: yuhua.shi
//	Note:           
/*****************************************************************************/
PUBLIC void UPCC_HandleRequest(void);
                               
/*****************************************************************************/
//  Description: process the request to video control interface   
//	Global resource dependence: 
//  Author: yuhua.shi
//	Note:           
/*****************************************************************************/
PUBLIC void UPCC_VC_Request(uint8 bDerection, uint16 wValue, \
                            uint8 bRequest, uint16 wlen);
   
/*****************************************************************************/
//  Description: init the delay timer for request
//	Global resource dependence: 
//  Author: yuhua.shi
//	Note:           
/*****************************************************************************/
void UPCC_InitRequestDelayTimer(void);

/*****************************************************************************/
//  Description: deinit the delay timer for request
//	Global resource dependence: 
//  Author: yuhua.shi
//	Note:           
/*****************************************************************************/
void UPCC_DeInitRequestDelayTimer(void);                                                   
/**---------------------------------------------------------------------------*
**                          COMPILER FLAG                                     *
**----------------------------------------------------------------------------*/    
#ifdef __cplusplus
    }
#endif      



/**---------------------------------------------------------------------------*/   
#endif //_UPCC_REQUEST_H_
//end UPCC_request.h
