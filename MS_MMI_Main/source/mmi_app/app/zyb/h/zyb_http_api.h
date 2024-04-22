/****************************************************************************
** File Name:      mmiphs_app.h                                                *
** Author:                                                                 *
** Date:           03/22/2006                                              *
** Copyright:      2006 TLT, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the PHS                   *
/****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 03/2006       Jianshengqi         Create
** 
****************************************************************************/
#ifndef  _MMI_ZYBHTTP_API_H_    
#define  _MMI_ZYBHTTP_API_H_   

#ifdef __cplusplus
    extern "C"
    {
#endif
/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmk_app.h"
#include "mn_type.h"
#include "mmi_nv.h"
#include "sci_types.h"
#include "mmi_common.h"
#include <stdio.h>
#include "nvitem.h"
#ifdef _RTOS
#include "sci_api.h" /*@tony.yao  replase sci_mem.h with sci_api.h*/
#else
#include "tasks_id.h"
#endif
#include "mn_api.h"
#include "mn_events.h"
#include "mn_type.h"
#include "nv_item_id.h"
#include "sio.h"
#include "sig_code.h"
#include "os_api.h"
#include "mmi_signal_ext.h"
#include "mmipdp_export.h"
#include "mmiconnection_export.h"
#include "http_api.h"

#define ZYB_HTTP_LOG            ZYB_HTTP_Trace

#define ZYBHTTP_MAX_URL_LEN 1024

typedef int (*ZYBHTTPRCVHANDLER) (BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 file_len,uint32 err_id);

typedef struct _ZYB_HTTP_RCV_DATA_t
{
    uint8          * pRcv;
    uint32         len;
} ZYB_HTTP_RCV_DATA_T;

typedef struct _ZYB_HTTP_RCV_NODE_t
{
       ZYB_HTTP_RCV_DATA_T  data;
       struct _ZYB_HTTP_RCV_NODE_t * next_ptr;
} ZYB_HTTP_RCV_NODE_T;


typedef struct
{
    BOOLEAN is_get;
    uint8 * ip_str;
    uint8 * str;
    uint32 str_len;
    uint32 file_len;
    uint32 file_start_len;
    uint32 file_end_len;
    uint16 type;
    uint8 times;
    uint8 priority;
    uint32 timeout;
    ZYBHTTPRCVHANDLER rcv_handle;
}ZYB_HTTP_DATA_T;

typedef struct
{
    SIGNAL_VARS
    ZYB_HTTP_DATA_T * p_http_data;
} ZYB_HTTP_SIG_T;

typedef struct MMI_GPRS_RSP_t
{
    BOOLEAN  need_stop; 	// Ê£Óà·¢ËÍ´ÎÊý
    BOOLEAN  is_sucess;
    uint32   err_id;
} ZYB_HTTP_RSP_T;

typedef struct
{
	SIGNAL_VARS
       ZYB_HTTP_RSP_T rsp;
} ZYB_HTTP_RSP_SIG_T ;

typedef struct _ZYB_HTTP_DATA_LINK_t
{
    ZYB_HTTP_DATA_T  * p_data;
    struct _ZYB_HTTP_DATA_LINK_t * next_ptr;
} ZYB_HTTP_DATA_LINK_NODE_T;

typedef struct
{
    uint32                  refresh_time;           //refresh time: seconds
    uint8  url_arr[ZYBHTTP_MAX_URL_LEN+1];
}ZYBHTTP_REFRESH_IND_T;

typedef struct
{
    uint8  * url_arr;
    uint32   refresh_time;           //refresh time: seconds
}ZYBHTTP_REFRESH_DATA_T;

typedef struct
{
    SIGNAL_VARS
    ZYBHTTP_REFRESH_DATA_T * p_http_data;
} ZYBHTTP_REFRESH_SIG_T;

PUBLIC BOOLEAN  MMIZYB_HTTP_SendSigTo_APP(ZYB_APP_SIG_E sig_id, ZYB_HTTP_DATA_T * p_http_data);

PUBLIC BOOLEAN MMIZYB_HTTP_Init(void);
PUBLIC BOOLEAN MMIZYB_HTTP_AppSend(BOOLEAN is_get,char * ip_str,uint8 *str,uint32 str_len,uint16 type,uint8 priority,uint8 repeat_times,uint32 timeout,uint32 file_len, uint32 file_start_len,uint32 file_end_len,ZYBHTTPRCVHANDLER rcv_handle);
PUBLIC BOOLEAN MMIZYB_HTTP_IsSending(void);
PUBLIC BOOLEAN MMIZYB_HTTP_AllowClose(void);
PUBLIC BOOLEAN MMIZYB_HTTP_AppStop(void);

PUBLIC BOOLEAN  MMIZYB_HTTP_Handle_Send(DPARAM param);
PUBLIC BOOLEAN  MMIZYB_HTTP_Handle_ReSend(DPARAM param);
PUBLIC BOOLEAN  MMIZYB_HTTP_Handle_Rcv(DPARAM param);
PUBLIC BOOLEAN  MMIZYB_HTTP_Handle_SendOver(DPARAM param);

PUBLIC void MMIZYB_HTTP_PopSend_Result(BOOLEAN need_stop, uint32 error_id);

PUBLIC int MMIZYB_HTTP_GetlCurAllUrl(uint8 **ppStr);
PUBLIC int MMIZYB_HTTP_GetCurUrl(uint8 **ppStr);
PUBLIC uint32 MMIZYB_HTTP_GetCurData(uint8 **ppStr);
PUBLIC uint16 MMIZYB_HTTP_GetCurType(void);
PUBLIC uint32  MMIZYB_HTTP_GetCurFileLen(void);
PUBLIC uint32  MMIZYB_HTTP_GetCurFileStartLen(void);
PUBLIC  uint32  MMIZYB_HTTP_GetCurFileEndLen(void);
PUBLIC ZYB_HTTP_DATA_T * MMIZYB_HTTP_GetCurAll(void);

PUBLIC BOOLEAN ZYB_HTTP_RCV_Add(uint8 * pRcv, uint32 rcv_len);
PUBLIC BOOLEAN ZYB_HTTP_RCV_AddExt(uint8 * pRcv, uint32 rcv_len);
PUBLIC BOOLEAN  ZYB_HTTP_RCV_Get(ZYB_HTTP_RCV_DATA_T * pUDPData);

#ifdef __cplusplus
    }
#endif

#endif
