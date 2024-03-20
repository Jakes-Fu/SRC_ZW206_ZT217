/******************************************************************************
 ** File Name:      fdl_main.h                                                *
 ** Author:         Leo.Feng                                                  *
 ** DATE:           30/11/2001                                                *
 ** Copyright:      2001 Spreatrum, Incoporated. All Rights Reserved.         *
 ** Description:    This file defines the interfaces of  the FIFO for serial  *
 **                 I/O operation. The FIFO is a clic queue.Writing operation *
 *                  can overlap the data has not be reading.                  *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 30/11/2001     Leo.Feng         Create.                                   *
 **                                                                           *
 ******************************************************************************/


#ifndef _FDL_MAIN_H_
#define _FDL_MAIN_H_


/*!
        \file  sio_fifo.h
        \brief Contains the high level c api for the sio fifo.

*/
/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "sci_types.h"
#include "cmd_def.h"


/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern   "C"
{
#endif


/**---------------------------------------------------------------------------*
 **                         Data Structures                                   *
 **---------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*
**                         Constant Variables                                *
**---------------------------------------------------------------------------*/
/* This macro write to a coprocessor register */
#define         ESAL_TS_RTE_CP_WRITE(cp, op1, cp_value, crn, crm, op2)              \
                {                                                                   \
                    __asm                                                           \
                    {                                                               \
                        MCR     cp, op1, (cp_value), crn, crm, op2                  \
                    }                                                               \
                }

/**---------------------------------------------------------------------------*
**                         External Variables                                *
**---------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*
 **                         External Functions                                *
 **---------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*
 **                         Function Prototypes                               *
 **---------------------------------------------------------------------------*/


//#ifdef SECURE_BOOT_SUPPORT
#if 0
uint32 download_secure_check(uint8 *, uint8 *);
#endif

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif //_SIO_FIFO__H_
