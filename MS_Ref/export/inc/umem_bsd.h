/******************************************************************************
 ** File Name:    umem_main.h                                                    *
 ** Author:       tao.feng                                                 *
 ** DATE:         5/05/2005                                                    *
 ** Copyright:    2009 Spreatrum, Incoporated. All Rights Reserved.           *
 ** Description:                                                              *
 ******************************************************************************/
/******************************************************************************
 **                   Edit    History                                         *
 **---------------------------------------------------------------------------* 
 ** DATE          NAME            DESCRIPTION                                 * 
 ** 5/05/2009      tao.feng       Create.                                     *
 ******************************************************************************/
#ifndef UMEM_BSD_H
    #define UMEM_BSDN_H
/*----------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **-------------------------------------------------------------------------- */


/**----------------------------------------------------------------------------*
**                               Micro Define                                 **
**----------------------------------------------------------------------------*/


/**----------------------------------------------------------------------------*
**                             Data Prototype                                 **
**----------------------------------------------------------------------------*/


/**----------------------------------------------------------------------------*
**                           Function Prototype                               **
**----------------------------------------------------------------------------*/
/********************************************************************
*	Description:  This function is used to register BSD operation.
*	Dependence: none
*	Author: tao.feng
*	Param:
*		none
*	Result:
*		None zero:the Capacity, unit is byte
*		0: Fail
********************************************************************/
PUBLIC BOOLEAN Umem_RegBSD(void);

/********************************************************************
*	Description: This function is used to unregister BSD operation.
*	Dependence:  none
*	Author: tao.feng
*	Param:
*		none
*	Result:
*		None zero:the Capacity, unit is byte
*		0: Fail
********************************************************************/

PUBLIC BOOLEAN Umem_UnRegBSD(void);

/**---------------------------------------------------------------------------*/
#endif
// End 

