
/*****************************************************************************/
#ifndef _DUERAPP_OPERATE_H_
#define _DUERAPP_OPERATE_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 
#include "mmi_appmsg.h"
#include "sci_types.h"
#include "mmk_type.h"
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/ 

#ifdef _cplusplus
	extern   "C"
    {
#endif
/*----------------------------------------------------------------------------*/
/*                         MACRO DEFINITION                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/
typedef enum
{
    NATIVE_OP_JUMP_TO_SECOND        = 1,
    NATIVE_OP_JUMP_TO_SOUND_ITEM    = 2,
    NATIVE_OP_JUMP_TO_WRITTING      = 3,
    NATIVE_OP_GET_ACTIVITY_LONG_PIC = 4,
    NATIVE_OP_SHOW_COMMON_QR        = 5,
} native_operate_type_e;
/*----------------------------------------------------------------------------*/
/*                         Function Declare                                   */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/
PUBLIC void MMIDUERAPP_CreateOperateWin(void);
PUBLIC void MMIDUERAPP_CreateOpLongpicWin(void);
PUBLIC void MMIDUERAPP_OpLongpicDownload(void);

PUBLIC void MMIDUERAPP_OperateActionUrl(char *url);

#ifdef _cplusplus
	}
#endif

#endif//_DUERAPP_OPERATE_H_