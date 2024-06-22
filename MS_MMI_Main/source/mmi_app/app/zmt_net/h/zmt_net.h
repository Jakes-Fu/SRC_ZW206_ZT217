#ifndef _ZMT_EXPORT_H_
#define _ZMT_EXPORT_H_

#include "sci_types.h"
#include "os_api.h"
#include "mmk_type.h"
#include "mmi_module.h"
#include "std_header.h"
#include "mmk_app.h"
#include "mmk_timer.h"
#include "mmi_text.h"
#include "mmi_common.h"
#include "window_parse.h"
#include "guitext.h"
#include "guilcd.h"
#include "mmi_menutable.h"
#include "guilistbox.h"
#include "mmi_image.h"
#include "guiedit.h"
#include "mmipub.h"
#include "mmi_appmsg.h"
#include "sig_code.h"
#include "mmiset_export.h"
#include "mmienvset_export.h"
#include "mmipdp_export.h"
#include "mmiconnection_export.h"
#include "IN_message.h"
#include "socket_types.h"
#include "socket_api.h"
#include "cjson.h"

#ifdef __cplusplus
extern   "C"
{
#endif


#define ZMT_TCP_RCV_SIZE 10240
#define ZMT_TCP_RET_OK 0
#define ZMT_TCP_RET_ERR 1


typedef enum _ZMT_TCP_TASK_SIGNAL
{
	SIG_ZMT_TCP_START = 0xF501,
	SIG_ZMT_TCP_CONNET,
	SIG_ZMT_TCP_WRITE,
	SIG_ZMT_TCP_RCV,
	SIG_ZMT_TCP_MAX
}ZMT_TCP_TASK_SIG_E;

typedef enum
{
    TCP_SUCCESS = 0,
    TCP_ERROR,
    TCP_ERR_REGEXIST, //???
    TCP_ERR_NOUIM, //??
    TCP_ERR_NOPS,   //???
    TCP_ERR_SOCKET,
    TCP_ERR_DISCONNECT,
    TCP_ERR_NET,
    TCP_ERR_NET_EMFILE,
    TCP_ERR_NET_NOSYS,
    TCP_ERR_NET_WRITE,
    TCP_ERR_NET_READ,
    TCP_ERR_LINKFULL,
    TCP_ERR_MEMFULL,
    TCP_ERR_NOREG,
    TCP_ERR_NOOPEN,
    TCP_ERR_NODATA,
    TCP_ERR_IP,
    TCP_ERR_TIMEOUT,
    TCP_ERR_READ,
    TCP_ERR_DATA,
    TCP_ERR_CLEAR,
    TCP_ERR_INCALL,
    TCP_ERR_MAX
}ZMT_TCP_ERR_TYPE_E;

typedef enum
{
    TCP_CB_NULL,
    TCP_CB_REG,
    TCP_CB_SEND,
    TCP_CB_RCV,
    TCP_CB_UNREG,
    TCP_CB_DISCONNECTED,
    TCP_CB_CONNECTED,
    TCP_CB_MAX
}ZMT_TCP_CB_TYPE_E;

typedef enum _ZMT_TCP_SOCKET_STATE_e
{
    TCP_SOCKET_STATE_NULL,
    TCP_SOCKET_STATE_OPEN,  //?????Sockefd
    TCP_SOCKET_STATE_CONNECTING,    //?ûÇ??
    TCP_SOCKET_STATE_CONNECTED, //??¯Að©
    TCP_SOCKET_STATE_WRITING,    //?ûÇ???
    TCP_SOCKET_STATE_DISCONNECTED  //?????????Socketfd
}ZMTTCP_SOCKET_STATE_E;


typedef int (*ZMTTCPCBHANDLER) (void *pUser,ZMT_TCP_CB_TYPE_E cb_type,uint16 wParam, void * dwParam);
typedef int (*ZMTTCPRCVHANDLER) (void *pUser,uint8 * pRcv,uint32 Rcv_len);
typedef uint32 (*ZMTCMDCHECKHANDLER)(uint8 * pData, int Data_len,uint8 **ppCmdBuf, uint32 * cmd_len);

typedef struct _ZMT_TCP_RCV_DATA_t
{
    void *pMe;
    uint8          * pRcv;
    uint32         len;
} ZMT_TCP_RCV_DATA_T;

typedef struct _ZMT_TCP_RCV_NODE_t
{
       ZMT_TCP_RCV_DATA_T  data;
       struct _ZMT_TCP_RCV_NODE_t * next_ptr;
} ZMT_TCP_RCV_NODE_T;

typedef struct _ZMT_TCP_REG_DATA_t
{
    void *pMe;
    char          * conn_ip;
    uint16         conn_port;
    ZMTTCPCBHANDLER callback;
} ZMT_TCP_REG_DATA_T;

typedef struct _ZMT_TCP_LINK_DATA_t
{
    void * pMe;
    uint8          * str;
    uint32         len;
    uint8 times;
    uint8 priority;
    uint32 timeout;
    ZMTTCPRCVHANDLER rcv_handle;
} ZMT_TCP_LINK_DATA_T;


typedef struct _ZMT_TCP_LINK_t
{
       ZMT_TCP_LINK_DATA_T *  data;
       struct _ZMT_TCP_LINK_t * next_ptr;
} ZMT_TCP_LINK_NODE_T;


typedef struct _ZMT_TCP_DATA_t
{
    uint16 type;
    void* data_p;
} ZMT_TCP_DATA_T;

typedef struct
{
    SIGNAL_VARS
    ZMT_TCP_DATA_T * p_data;
} ZMT_TCP_DATA_SIG_T;

typedef struct _ZMT_TCP_TASK_DATA_t
{
    void * pMe;
    uint8 * str;
    uint32 str_len;
    uint16 port;
    uint32 ip_addr;
    uint8  * ip_str;
} ZMT_TCP_TASK_DATA_T;

typedef struct
{
    SIGNAL_VARS
    ZMT_TCP_TASK_DATA_T * p_task_data;
} ZMT_TCP_TASK_SIG_T;

typedef struct _ZMT_TCP_INTERFACE_t 
{
    BLOCK_ID m_tcp_task_id;
    uint16 m_tcp_interface_idx;
    BOOLEAN m_tcp_net_is_init;
    ZMTCMDCHECKHANDLER m_tcp_cmd_check_handle;
    ZMTTCP_SOCKET_STATE_E  m_tcp_socket_status;
    TCPIP_SOCKET_T            m_tcp_pISocket;             // Pointer to socket
    TCPIP_SOCKET_T            m_tcp_pISocket_close;             // Pointer to socket
    uint16                m_tcp_last_err;

    BOOLEAN		   m_tcp_need_connect;
    BOOLEAN            m_tcp_socket_is_connected;
    char *               m_tcp_ip_url;
    uint32               m_tcp_conn_ip;
    uint16                m_tcp_conn_port;
    uint8		        m_tcp_reconn_times;
    BOOLEAN		        m_tcp_need_read;

    ZMT_TCP_LINK_DATA_T * m_tcp_cur_data;
    uint8 *               m_tcp_buf_get;
    uint32               m_tcp_get_len;          // length of pszMsg (calculated when set)
    uint8 *                m_tcp_buf_rcv;
    uint32                 m_tcp_rcv_len;              // Index used for sending TCP data
    BOOLEAN                 m_tcp_is_reading;
    uint32               m_tcp_get_curlen;              // Index used for sending TCP data

    uint8				m_tcp_connwrite;
    uint8				m_tcp_closewrite;
    uint8				m_tcp_closeconn;

    BOOLEAN             m_tcp_is_sending;
    BOOLEAN             m_tcp_need_rcv;

    BOOLEAN             m_tcp_is_reg;
    ZMT_TCP_RCV_NODE_T  * m_tcp_rcv_head;
    ZMT_TCP_LINK_NODE_T  * m_tcp_link_head;
    uint32                 m_tcp_rcv_needlen;
    ZMTTCPCBHANDLER   m_tcp_callback;

    uint8                       m_tcp_pdp_timer_id;
    uint8                       m_tcp_connect_timer_id;
    uint8                       m_tcp_write_timer_id;
    uint8                       m_tcp_rewrite_timer_id;
    uint8                       m_tcp_read_timer_id;
    BOOLEAN                  m_tcp_wait_net;
    ZMT_TCP_REG_DATA_T * m_tcp_reg_data;
}ZMT_TCP_INTERFACE_T;

extern MMI_RESULT_E  MMIZMTTCP_Handle_AppMsg (PWND app_ptr, uint16 msg_id,DPARAM param);
extern BOOLEAN ZMTTCP_NetOpen(ZMT_TCP_INTERFACE_T ** ppMe,ZMTCMDCHECKHANDLER cmd_handle,uint16 rcv_len);
extern BOOLEAN ZMTTCP_NetClose(ZMT_TCP_INTERFACE_T * pMe);
extern void ZMTTCP_CloseAllTimer(ZMT_TCP_INTERFACE_T *pMe);
extern int ZMTTCP_Reset(ZMT_TCP_INTERFACE_T *pMe);
extern int TZMT_API_TCP_Reg(ZMT_TCP_INTERFACE_T *pMe,char * str_ip_url,uint16 conn_port,ZMTTCPCBHANDLER callback);
extern int TZMT_API_TCP_UnReg(ZMT_TCP_INTERFACE_T *pMe);
extern uint32 TZMT_API_TCP_SendData(ZMT_TCP_INTERFACE_T *pMe,uint8 * pData,uint16 Data_len,uint8 priority,uint8 repeat_times,uint32 timeout,ZMTTCPRCVHANDLER rcv_handle);
extern int TZMTAPI_TCP_SendDataNow(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_REG_DATA_T * pData);
extern int ZMTTCP_CleanSendLink(ZMT_TCP_INTERFACE_T *pMe);
extern BOOLEAN TZMT_API_TCP_AllowClose(ZMT_TCP_INTERFACE_T *pMe);
extern BOOLEAN  MMIZMT_TCP_Handle_PDPActiveOK(void);
extern BOOLEAN  MMIZMT_TCP_Handle_PDPActiveFail(void);
extern BOOLEAN  MMIZMT_TCP_Handle_PDPRespond(void);
extern void ZMT_Tcp_ForceCloseAll(void);

PUBLIC MN_DUAL_SYS_E MMIZMT_Net_GetActiveSys(void);
PUBLIC BOOLEAN MMIZMT_Net_Open(void);
PUBLIC BOOLEAN MMIZMT_Net_Close(void);
PUBLIC void MMIZmt_AppInit(void);

extern ZMT_TCP_INTERFACE_T * m_zmt_tcp_reg_interface;

typedef struct
{
    uint8 *        str;
    uint32         len; 
} MMI_ZMT_DATA_T;

typedef struct
{
    SIGNAL_VARS
    MMI_ZMT_DATA_T data;
} MMI_ZMT_SIG_T;

#ifdef   __cplusplus
    }
#endif

#endif