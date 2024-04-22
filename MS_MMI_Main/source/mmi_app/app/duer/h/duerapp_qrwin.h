
/*****************************************************************************/
#ifndef _DUERAPP_QRWIN_H_
#define _DUERAPP_QRWIN_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 
#include "mmi_appmsg.h"
#include "sci_types.h"
#include "mmk_type.h"
#include "qrencode.h"
#include "baidu_json.h"
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

/*----------------------------------------------------------------------------*/
/*                         Function Declare                                   */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/
LOCAL int create_qrencoder(baidu_json *json_data);
LOCAL void display_qrencoder(const char *data);
LOCAL void MMIDUERAPP_DisplayQrImage(QRcode *code);
PUBLIC char *get_login_status_url();
PUBLIC char *get_auth_or_pay_url();
PUBLIC void MMIDUERAPP_QrLoginSuccess();
PUBLIC void MMIDUERAPP_QrPaySuccess();
PUBLIC MMI_HANDLE_T MMIDUERAPP_CreateQrWin(ADD_DATA data);
PUBLIC void duerapp_free_qr_info();
PUBLIC void StartLoginReqTimer(void);
PUBLIC void StopLoginReqTimer(void);
#ifdef _cplusplus
	}
#endif

#endif//_DUERAPP_QRWIN_H_