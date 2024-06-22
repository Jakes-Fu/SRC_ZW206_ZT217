
#include "zmt_net_export.h"
#include "zmt_net_main.h"
#include "zmt_net_http.h"
#include "zmt_net.h"

#define ZMT_TCP_INTERFACE_MAX 5
#define ZMT_TCP_PDP_CLOSE_TIMEOUT 5000

BOOLEAN  m_zmt_tcp_pdpstate = FALSE;

MMI_APPLICATION_T    mmi_zmt_app;
ZMT_TCP_INTERFACE_T * m_zmt_tcp_reg_interface;
ZMT_TCP_INTERFACE_T m_zmt_tcp_interface;

static uint8 ZMTTcp_TCP_Cancel(ZMT_TCP_INTERFACE_T * pMe);
static uint8 ZMTTcp_Connect(ZMT_TCP_INTERFACE_T * pMe);
static int ZMTTCP_Start(ZMT_TCP_INTERFACE_T *pMe);
static int ZMTTCP_ReStart(ZMT_TCP_INTERFACE_T *pMe);
static void ZMTTCP_DNSConnect(ZMT_TCP_INTERFACE_T * pMe);

LOCAL MMI_RESULT_E ZMT_Handle_AppMsg(PWND app_ptr, uint16 msg_id, DPARAM param)
{
    MMI_RESULT_E res = MMI_RESULT_TRUE;
    SCI_TRACE_LOW("[ZMT_TCP] %s: msg_id = %x", __FUNCTION__,msg_id);
    return res;
}


static MMI_RESULT_E  HandleZMTAppMsg (PWND app_ptr, 
                                    uint16 msg_id, 
                                    DPARAM param)
{
    MMI_RESULT_E res = MMI_RESULT_FALSE;
    SCI_TRACE_LOW("[ZMT_TCP] %s: zmtmsg_id %x", __FUNCTION__,msg_id);
    
    if(msg_id >= ZMT_APP_SIGNAL_START && msg_id <= ZMT_APP_SIGNAL_END)
    {
		if(MMI_RESULT_TRUE == MMIZMT_Net_Handle_AppMsg(app_ptr,msg_id,param))
		{
			return MMI_RESULT_TRUE;
		}

		if(MMI_RESULT_TRUE == ZMT_Handle_AppMsg(app_ptr,msg_id,param))
        {
			return MMI_RESULT_TRUE;
        }
    }	
    return res;
}

PUBLIC void MMIZmt_AppInit(void)
{
	mmi_zmt_app.ProcessMsg = HandleZMTAppMsg;
	m_zmt_tcp_reg_interface = &m_zmt_tcp_interface;
	SCI_TRACE_LOW("[ZMT_TCP] %s: SUCCESS", __FUNCTION__);
 }

PUBLIC BOOLEAN  ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIG_E sig_id, void* p_data, uint16 type)
{
   
    uint8 * pstr = PNULL;
    ZMT_TCP_DATA_SIG_T * psig = PNULL;    
    ZMT_TCP_DATA_T * cur_data = PNULL;
      SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_TCP_PostEvtTo_APP", __FUNCTION__);
    cur_data=(ZMT_TCP_DATA_T *)SCI_ALLOC_APPZ(sizeof(ZMT_TCP_DATA_T));
    if(cur_data == PNULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT TCP POSTEvt ERR Malloc", __FUNCTION__);
        return FALSE;
    }
    cur_data->data_p = p_data;
    cur_data->type = type;

    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,sig_id,sizeof(ZMT_TCP_DATA_SIG_T),SCI_IdentifyThread());
    psig->p_data = cur_data;
    SCI_SEND_SIGNAL((xSignalHeaderRec*)psig,P_APP);
    return TRUE;
}

static BOOLEAN  ZMT_TCP_SendSigTo_Task(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_TASK_SIG_E sig_id, ZMT_TCP_TASK_DATA_T * p_task_data)
{
    uint8 * pstr = PNULL;
    ZMT_TCP_TASK_SIG_T * psig = PNULL;
    
    if(pMe->m_tcp_task_id != SCI_INVALID_BLOCK_ID)
    {
        //send signal to AT to write uart
        SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,sig_id,sizeof(ZMT_TCP_TASK_SIG_T),SCI_IdentifyThread());
        psig->p_task_data = p_task_data;
        
        SCI_SEND_SIGNAL((xSignalHeaderRec*)psig,pMe->m_tcp_task_id);
        return TRUE;
    }
    return FALSE;
}

static void ZMT_TCP_DelCurData(ZMT_TCP_INTERFACE_T *pMe)
{
    if(pMe->m_tcp_cur_data != NULL)
    {
        if(pMe->m_tcp_cur_data->str != NULL)
        {
            SCI_FREE(pMe->m_tcp_cur_data->str);
        }
        SCI_FREE(pMe->m_tcp_cur_data);
        pMe->m_tcp_cur_data = NULL;
    }
    return;
}

static BOOLEAN ZMT_TCP_Link_AddData(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_LINK_DATA_T * pData)
{
    ZMT_TCP_LINK_NODE_T  * p1 = NULL;
    ZMT_TCP_LINK_NODE_T  * p2 = NULL;
    
    if(pData ==  NULL)
    {
        return FALSE;
    }
    
    p1=(ZMT_TCP_LINK_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZMT_TCP_LINK_NODE_T));
    if(p1 == NULL)
    {
        return FALSE;
    }
    SCI_MEMSET(p1,0,sizeof(ZMT_TCP_LINK_NODE_T));

    p1->data = pData;
    p1->next_ptr = NULL;
    
    if(pMe->m_tcp_link_head == NULL)
    {
        pMe->m_tcp_link_head = p1;
    }
    else
    {
        p2 = pMe->m_tcp_link_head;
        while(p2->next_ptr != NULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

static BOOLEAN  ZMT_TCP_Link_GetByPri(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_LINK_DATA_T ** pPostData,uint8 priority)
{
    ZMT_TCP_LINK_NODE_T  * p1 = NULL;
    ZMT_TCP_LINK_NODE_T  * p2 = NULL;
    BOOLEAN res = FALSE;
    
    p2 = p1 = pMe->m_tcp_link_head;
    
    while(p1 != NULL)
    {
        if(p1->data != NULL && p1->data->priority == priority)
        {
            *pPostData = p1->data;
            
            if(p1 == pMe->m_tcp_link_head)
            {
                pMe->m_tcp_link_head = p1->next_ptr;
            }
            else
            {
                p2->next_ptr = p1->next_ptr;
            }
            SCI_FREE(p1);
            res = TRUE;
            break;
        }
        p2 = p1;
        p1 = p2->next_ptr;
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s: res = %d", __FUNCTION__, res);
    return res;
}

static BOOLEAN  ZMT_TCP_Link_GetMaxPri(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_LINK_DATA_T ** pPostData)
{
    ZMT_TCP_LINK_NODE_T  * p1 = NULL;
    ZMT_TCP_LINK_NODE_T  * p2 = NULL;
    uint8 max_pri = 0;
    BOOLEAN res = FALSE;
    
    p2 = p1 = pMe->m_tcp_link_head;
    
    while(p1 != NULL)
    {
        if(p1->data != NULL && p1->data->priority > max_pri)
        {
            max_pri = p1->data->priority;
        }
        p2 = p1;
        p1 = p2->next_ptr;
    }
    
    res = ZMT_TCP_Link_GetByPri(pMe,pPostData,max_pri);
    return res;
}

static BOOLEAN  ZMT_TCP_Link_DelAll(ZMT_TCP_INTERFACE_T *pMe)
{
    ZMT_TCP_LINK_NODE_T  * p1 = NULL;
    
    while(pMe->m_tcp_link_head != NULL)
    {
        p1 = pMe->m_tcp_link_head;
        pMe->m_tcp_link_head = p1->next_ptr;
        if(p1->data != NULL)
        {
            if(p1->data->str != NULL)
            {
                SCI_FREE(p1->data->str);
            }
            SCI_FREE(p1->data);
        }
        SCI_FREE(p1);
    }
    return TRUE;
}

static uint16  ZMT_TCP_Link_Count(ZMT_TCP_INTERFACE_T *pMe)
{
    ZMT_TCP_LINK_NODE_T  * p1 = NULL;
    uint16 num = 0;
    
    p1 = pMe->m_tcp_link_head;
    
    while(p1 != NULL)
    {
        num++;
        p1 = p1->next_ptr;
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s: num = %d", __FUNCTION__, num);
    return num;
}

static BOOLEAN ZMT_TCP_RCV_Add(ZMT_TCP_INTERFACE_T *pMe, uint8 * pRcv, uint32 rcv_len)
{
    ZMT_TCP_RCV_NODE_T  * p1 = NULL;
    ZMT_TCP_RCV_NODE_T  * p2 = NULL;
    uint32 len = rcv_len;
    SCI_TRACE_LOW("[ZMT_TCP] %s: rcv_len = %d", __FUNCTION__, rcv_len);

    if(len == 0)
    {
        return FALSE;
    }
    
    p1=(ZMT_TCP_RCV_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZMT_TCP_RCV_NODE_T));
    if(p1 == NULL)
    {
        return FALSE;
    }
    
    p1->data.pRcv = (uint8 *)SCI_ALLOC_APPZ(len);
    if(p1->data.pRcv == NULL)
    {
        SCI_FREE(p1);
        return FALSE;
    }
    
    SCI_MEMCPY(p1->data.pRcv,pRcv,rcv_len);

    p1->data.len = len;
    
    p1->next_ptr = NULL;

    if(pMe->m_tcp_rcv_head == NULL)
    {
        pMe->m_tcp_rcv_head = p1;
    }
    else
    {
        p2 = pMe->m_tcp_rcv_head;
        while(p2->next_ptr != NULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

static BOOLEAN ZMT_TCP_RCV_AddExt(ZMT_TCP_INTERFACE_T *pMe, uint8 * pRcv, uint32 rcv_len)
{
    ZMT_TCP_RCV_NODE_T  * p1 = NULL;
    ZMT_TCP_RCV_NODE_T  * p2 = NULL;
    uint32 len = rcv_len;
    SCI_TRACE_LOW("[ZMT_TCP] %s: rcv_len = %d", __FUNCTION__, rcv_len);

    if(len == 0)
    {
        return FALSE;
    }
    
    p1=(ZMT_TCP_RCV_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZMT_TCP_RCV_NODE_T));
    if(p1 == NULL)
    {
        return FALSE;
    }
    p1->data.pRcv = pRcv;
    p1->data.len = len;
    
    p1->next_ptr = NULL;

    if(pMe->m_tcp_rcv_head == NULL)
    {
        pMe->m_tcp_rcv_head = p1;
    }
    else
    {
        p2 = pMe->m_tcp_rcv_head;
        while(p2->next_ptr != NULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

static BOOLEAN  ZMT_TCP_RCV_Get(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_RCV_DATA_T * pUDPData)
{
    ZMT_TCP_RCV_NODE_T  * p1 = NULL;
    
    p1 = pMe->m_tcp_rcv_head;
    SCI_TRACE_LOW("[ZMT_TCP] %s ", __FUNCTION__);
    if(p1 != NULL)
    {
      *pUDPData = p1->data;
        pMe->m_tcp_rcv_head = p1->next_ptr;
        SCI_FREE(p1);
        return TRUE;
    }
        
    return FALSE;
}

static BOOLEAN  ZMT_TCP_RCV_DelAll(ZMT_TCP_INTERFACE_T *pMe)
{
    ZMT_TCP_RCV_NODE_T  * p1 = NULL;
    SCI_TRACE_LOW("[ZMT_TCP] %s", __FUNCTION__);
    while(pMe->m_tcp_rcv_head != NULL)
    {
        p1 = pMe->m_tcp_rcv_head;
        pMe->m_tcp_rcv_head = p1->next_ptr;
        if(p1->data.pRcv != 0)
        {
            SCI_FREE(p1->data.pRcv);
        }
        SCI_FREE(p1);
    }
    return TRUE;
}

static void ZMTTcp_Send_Write_Resp(ZMT_TCP_INTERFACE_T *pMe,uint8 res,uint32 Errid)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s: res =%d , Errid=%d", __FUNCTION__, res, Errid);
    if(res == TCP_SUCCESS)
    {
        ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_SUCCESS,(void *)pMe,Errid);
    }
    else
    {
        ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_FAIL,(void *)pMe,Errid);
    }
}

LOCAL void ZMT_TCP_HandlePdpCloseTimer(
                                uint8 timer_id,
                                uint32 param
                                )
{
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)param;
    if(timer_id == pMe->m_tcp_pdp_timer_id && 0 != pMe->m_tcp_pdp_timer_id)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: timerout ", __FUNCTION__);
        MMK_StopTimer(pMe->m_tcp_pdp_timer_id);
        pMe->m_tcp_pdp_timer_id = 0;
    }
}

LOCAL void ZMT_TCP_StartPdpCloseTimer(ZMT_TCP_INTERFACE_T *pMe)
{
    if(0 != pMe->m_tcp_pdp_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_pdp_timer_id);
        pMe->m_tcp_pdp_timer_id = 0;
    }
    
    pMe->m_tcp_pdp_timer_id = MMK_CreateTimerCallback(ZMT_TCP_PDP_CLOSE_TIMEOUT, 
                                                                        ZMT_TCP_HandlePdpCloseTimer, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
}

PUBLIC void ZMT_TCP_StopPdpCloseTimer(ZMT_TCP_INTERFACE_T *pMe)
{
    if(0 != pMe->m_tcp_pdp_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_pdp_timer_id);
        pMe->m_tcp_pdp_timer_id = 0;
    }
}

static void ZMT_TCP_HandleConnectTimer(
                                uint8 timer_id,
                                uint32 param
                                )
{
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)param;
    if(timer_id == pMe->m_tcp_connect_timer_id && 0 != pMe->m_tcp_connect_timer_id)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: timerout ", __FUNCTION__);
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_DISCONNECT);
        MMK_StopTimer(pMe->m_tcp_connect_timer_id);
        pMe->m_tcp_connect_timer_id = 0;
    }
}

static void ZMT_TCP_StartConnectTimer(ZMT_TCP_INTERFACE_T *pMe)
{
    if(0 != pMe->m_tcp_connect_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_connect_timer_id);
        pMe->m_tcp_connect_timer_id = 0;
    }
    
    pMe->m_tcp_connect_timer_id = MMK_CreateTimerCallback(120*1000, 
                                                                        ZMT_TCP_HandleConnectTimer, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
}

static void ZMT_TCP_StopConnectTimer(ZMT_TCP_INTERFACE_T *pMe)
{
    if(0 != pMe->m_tcp_connect_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_connect_timer_id);
        pMe->m_tcp_connect_timer_id = 0;
    }
}

static void ZMT_TCP_HandleReWriteTimer(
                                uint8 timer_id,
                                uint32 param
                                )
{
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)param;
    if(timer_id == pMe->m_tcp_rewrite_timer_id && 0 != pMe->m_tcp_rewrite_timer_id)
    {
        ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_RESTART,(void *)pMe,0);
        MMK_StopTimer(pMe->m_tcp_rewrite_timer_id);
        pMe->m_tcp_rewrite_timer_id = 0;
    }
}

static void ZMT_TCP_StartReWriteTimer(ZMT_TCP_INTERFACE_T *pMe,uint32 time_ms)
{
    if(0 != pMe->m_tcp_rewrite_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_rewrite_timer_id);
        pMe->m_tcp_rewrite_timer_id = 0;
    }
    
    pMe->m_tcp_rewrite_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        ZMT_TCP_HandleReWriteTimer, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
}

static void ZMT_TCP_StopReWriteTimer(ZMT_TCP_INTERFACE_T *pMe)
{
    if(0 != pMe->m_tcp_rewrite_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_rewrite_timer_id);
        pMe->m_tcp_rewrite_timer_id = 0;
    }
}

static void ZMT_TCP_HandleWriteTimer(
                                uint8 timer_id,
                                uint32 param
                                )
{
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)param;
    if(timer_id == pMe->m_tcp_write_timer_id && 0 != pMe->m_tcp_write_timer_id)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: timerout ", __FUNCTION__);
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_TIMEOUT);
        MMK_StopTimer(pMe->m_tcp_write_timer_id);
        pMe->m_tcp_write_timer_id = 0;
    }
}

static void ZMT_TCP_StartWriteTimer(ZMT_TCP_INTERFACE_T *pMe,uint32 time_ms)
{
    if(0 != pMe->m_tcp_write_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_write_timer_id);
        pMe->m_tcp_write_timer_id = 0;
    }
    
    pMe->m_tcp_write_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        ZMT_TCP_HandleWriteTimer, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
}

static void ZMT_TCP_StopWriteTimer(ZMT_TCP_INTERFACE_T *pMe)
{
    if(0 != pMe->m_tcp_write_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_write_timer_id);
        pMe->m_tcp_write_timer_id = 0;
    }
}

static void ZMT_TCP_HandleReadTimer(
                                uint8 timer_id,
                                uint32 param
                                )
{
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)param;
    if(timer_id == pMe->m_tcp_read_timer_id && 0 != pMe->m_tcp_read_timer_id)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: timerout ", __FUNCTION__);
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_READ);
        MMK_StopTimer(pMe->m_tcp_read_timer_id);
        pMe->m_tcp_read_timer_id = 0;
    }
}

static void ZMT_TCP_StartReadTimer(ZMT_TCP_INTERFACE_T *pMe,uint32 time_ms)
{
    if(0 != pMe->m_tcp_read_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_read_timer_id);
        pMe->m_tcp_read_timer_id = 0;
    }
    
    pMe->m_tcp_read_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        ZMT_TCP_HandleReadTimer, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
}

static void ZMT_TCP_StopReadTimer(ZMT_TCP_INTERFACE_T *pMe)
{
    if(0 != pMe->m_tcp_read_timer_id)
    {
        MMK_StopTimer(pMe->m_tcp_read_timer_id);
        pMe->m_tcp_read_timer_id = 0;
    }
}

static TCPIP_SOCKET_T ZMT_TCP_SocCreate(ZMT_TCP_INTERFACE_T *pMe,uint16 bind_port, TCPIP_NETID_T net_id)
{
    TCPIP_SOCKET_T sockfd = TCPIP_SOCKET_INVALID;
    struct sci_sockaddr addr;
    char * ip_str = PNULL;
    SCI_TRACE_LOW("[ZMT_TCP] %s: bind_port=%d,net_id=0x%x", __FUNCTION__, bind_port,net_id);

    sockfd = sci_sock_socket(AF_INET,SOCK_STREAM,0, net_id);
    SCI_TRACE_LOW("[ZMT_TCP] %s: sci_sock_socket sockfd =%d", __FUNCTION__, sockfd);
    if(sockfd == TCPIP_SOCKET_INVALID)
    {
        if(pMe->m_tcp_pISocket != TCPIP_SOCKET_INVALID)
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: sci_sock_socketclose", __FUNCTION__);
            sci_sock_socketclose(pMe->m_tcp_pISocket);
            pMe->m_tcp_pISocket = TCPIP_SOCKET_INVALID;
            sockfd = sci_sock_socket(AF_INET,SOCK_STREAM,0, net_id);
            if(sockfd == TCPIP_SOCKET_INVALID)
            {
                SCI_TRACE_LOW("[ZMT_TCP] %s: SocketCreate ERR 1", __FUNCTION__);
                return TCPIP_SOCKET_INVALID;
            }
        }
        else
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: SocketCreate ERR 2", __FUNCTION__);
            return TCPIP_SOCKET_INVALID;
        }
    }

    sci_sock_setsockopt(sockfd, SO_NBIO, NULL);
    SCI_TRACE_LOW("[ZMT_TCP] %s: SocketCreate OK sockfd= %ld", __FUNCTION__, sockfd);
    return sockfd;
}

static BOOLEAN  ZMT_TCP_SocWriteEx(ZMT_TCP_INTERFACE_T *pMe)
{
    BOOLEAN res = FALSE;
    uint32 len =  pMe->m_tcp_get_len;
    int send_len = pMe->m_tcp_get_len;
    int sent_len = 0, total_sent = 0;
    SCI_TRACE_LOW("[ZMT_TCP] %s: len=%d", __FUNCTION__, len);
    if(pMe->m_tcp_pISocket == TCPIP_SOCKET_INVALID)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ERR No Socket", __FUNCTION__);
        return FALSE;
    }
    
    while( total_sent < len )
    {
        
        sent_len = sci_sock_send(pMe->m_tcp_pISocket, (pMe->m_tcp_buf_get + total_sent), send_len, 0 );
        SCI_TRACE_LOW("[ZMT_TCP] %s: sci_sock_send send_len=%d ,sent_len=%d ", __FUNCTION__, send_len, sent_len );
        if( TCPIP_SOCKET_ERROR == sent_len )
        {
            int error = sci_sock_errno(pMe->m_tcp_pISocket);
            SCI_TRACE_LOW("[ZMT_TCP] %s: sci_sock_errno=%d", __FUNCTION__, error);
            if( EWOULDBLOCK == error || ENOBUFS == error )
            { 
                SCI_Sleep(200);
            }
            else
            {
                SCI_TRACE_LOW("[ZMT_TCP] %s: send big error ,stop ", __FUNCTION__);
                break;
            }
        }
        else if( sent_len > 0 )
        {
            total_sent += sent_len;
            if(total_sent >= len)
            {
                res = TRUE;
                break;
            }
            send_len -= sent_len;
        }
        else
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: connect closed exception ", __FUNCTION__);
            break;
        }
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s: Result=%d, sockfd=%ld,Len=%d/%d", __FUNCTION__,res,pMe->m_tcp_pISocket,total_sent,len);
    return res;
}

static void ZMT_TCP_SocWrite(ZMT_TCP_INTERFACE_T *pMe)
{
    if (TRUE == ZMT_TCP_SocWriteEx(pMe))
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: Write finished", __FUNCTION__);
        if(pMe->m_tcp_cur_data != NULL && pMe->m_tcp_cur_data->rcv_handle != NULL)
        {
            pMe->m_tcp_need_rcv = TRUE;
            ZMT_TCP_StartReadTimer(pMe,120*1000);
        }
        else
        {
            ZMTTcp_Send_Write_Resp(pMe,TCP_SUCCESS,TCP_SUCCESS);
        }
    }
    else
    {
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_NET_WRITE);
    }
}
static void ZMT_TCP_SocConnected(ZMT_TCP_INTERFACE_T *pMe,uint32 conn_err)
{    
    BOOLEAN conn_ok = FALSE;
    if(conn_err == 0)
    {
        conn_ok = TRUE;
    }
    
    SCI_TRACE_LOW("[ZMT_TCP] %s: Connected(%d) Url=%s,Port=%d,IP=0x%x",__FUNCTION__, conn_ok,pMe->m_tcp_ip_url,pMe->m_tcp_conn_port,pMe->m_tcp_conn_ip);
    
    if (conn_ok)
    {
        ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_CONNECT_SUCCESS,(void *)pMe,0);
        ZMT_TCP_SocWrite(pMe);
    } 
    else
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: Connected Err",__FUNCTION__);
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_DISCONNECT);
    }
}

static BOOLEAN  ZMT_TCP_SocConnect(ZMT_TCP_INTERFACE_T *pMe,uint32 ip_addr,uint16 port)
{
    int send_res = 0;
    struct sci_sockaddr dest_addr;

    if(pMe->m_tcp_pISocket == TCPIP_SOCKET_INVALID)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ERR No Socket", __FUNCTION__);
        return FALSE;
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s: %s:%d,%d,0x%x", __FUNCTION__, pMe->m_tcp_ip_url,pMe->m_tcp_conn_port,port,ip_addr);
    SCI_MEMSET(&dest_addr,0,sizeof(dest_addr));
    dest_addr.family = AF_INET;
    dest_addr.ip_addr = ip_addr;
    dest_addr.port = htons(port);
    
    send_res = sci_sock_connect(pMe->m_tcp_pISocket, &dest_addr, sizeof(dest_addr));
    if(send_res == TCPIP_SOCKET_ERROR)
    {
        if(EINPROGRESS != sci_sock_errno(pMe->m_tcp_pISocket) )
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: ERR sockfd=%ld, errno=%d",__FUNCTION__,pMe->m_tcp_pISocket,sci_sock_errno(pMe->m_tcp_pISocket));
            return FALSE;
        }
    }
    if(pMe->m_tcp_socket_is_connected == FALSE)
    {
        ZMT_TCP_StartConnectTimer(pMe);
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s: sockfd=%ld,IP=0x%x,port=%d",__FUNCTION__,pMe->m_tcp_pISocket,ip_addr,port);
    return TRUE;
}

static BOOLEAN  ZMT_TCP_SocConnectStart(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_TASK_DATA_T * p_tcp_data)
{
    BOOLEAN res = FALSE;
        
    if(p_tcp_data == PNULL || p_tcp_data->str == PNULL || p_tcp_data->str_len == 0)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ERR No Data", __FUNCTION__);
        return FALSE;
    }
    
    if(p_tcp_data->ip_addr == 0)
    {
        if(p_tcp_data->ip_str == PNULL)
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: ERR No IP", __FUNCTION__);
            return FALSE;
        }
        else
        {
            int rv = -1;
            TCPIP_IPADDR_T send_addr = 0;
            struct sci_hostent * host = sci_gethostbyname(p_tcp_data->ip_str);
            rv = sci_parse_host_ext((char*)p_tcp_data->ip_str,&send_addr,1,MMIZMT_Net_GetNetID());
           rv = sci_parse_host((char*)p_tcp_data->ip_str,&send_addr,1);
            SCI_TRACE_LOW("[ZMT_TCP] %s:  ip_str = %s", __FUNCTION__, p_tcp_data->ip_str);
            if(host != NULL)
            {
                char * ip_str = NULL;
                if( sizeof(TCPIP_IPADDR_T) == host->h_length )
                {
                    SCI_MEMCPY( &send_addr, host->h_addr_list[0], host->h_length );
                }
                ip_str = inet_ntoa(send_addr);
                p_tcp_data->ip_addr = (uint32)send_addr;
                SCI_TRACE_LOW("[ZMT_TCP] %s: DNS %s,%s,0x%x,%d", __FUNCTION__, p_tcp_data->ip_str,ip_str,send_addr,p_tcp_data->port);
                res = ZMT_TCP_SocConnect(pMe,send_addr,p_tcp_data->port);
            }
            else
            {
                SCI_TRACE_LOW("[ZMT_TCP] %s: DNS ERR", __FUNCTION__);
                return FALSE;
            }
        }
    }    
    else
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ip_addr=0x%x,%s,%d", __FUNCTION__, p_tcp_data->ip_addr,p_tcp_data->ip_str,p_tcp_data->port);
        res = ZMT_TCP_SocConnect(pMe,p_tcp_data->ip_addr,p_tcp_data->port);
    }
    return res;
}

static BOOLEAN  TCP_Task_HandleSig_Conncet(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_TASK_DATA_T * p_task_data)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s: Connect Url:%s, Port=%d,IP=0x%x", __FUNCTION__, p_task_data->ip_str,p_task_data->port,p_task_data->ip_addr);
    if(FALSE == ZMT_TCP_SocConnectStart(pMe,p_task_data))
    {
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_DISCONNECT);
    }
    else
    {
        if(pMe->m_tcp_socket_is_connected)
        {
            ZMT_TCP_SocConnected(pMe,0);
        }
    }
    return TRUE;
}

static BOOLEAN  TCP_Task_HandleSig_Rcv(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_TASK_DATA_T * p_task_data)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s: Rcv Len=%d", __FUNCTION__, p_task_data->str_len);
    return TRUE;
}

static BOOLEAN  TCP_Task_HandleSig_Write(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_TASK_DATA_T * p_task_data)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s: Write Len=%d,is_connected=%d",__FUNCTION__, p_task_data->str_len,pMe->m_tcp_socket_is_connected);
    if(pMe->m_tcp_socket_is_connected)
    {
        ZMT_TCP_SocWrite(pMe);
    }
    else
    {
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_NET_WRITE);
    }
    return TRUE;
}

void ZMT_Tcp_ForceCloseAll()
{
    ZMT_TCP_INTERFACE_T * pMe = m_zmt_tcp_reg_interface;
    SCI_TRACE_LOW("[ZMT_TCP] %s", __FUNCTION__);     
    if(pMe)
    {
        ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_DISCONNECTED,(void *)pMe,0);
    }

}

void ZMT_TCP_TaskEntry(uint32 argc, void *argv)
{
    xSignalHeaderRec*   sig_ptr = 0; 
    int ret = 0;
    int left_len=0;
    int recv_len=0;
    uint16 ip_port = 0;
    uint32 rcv_ip_addr = 0;
    char* ip_str=NULL;
    TCPIP_SOCKET_T  so = 0;
    uint32 err = 0;
    uint32 cur_net_id = MMIZMT_Net_GetNetID();
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)argv;
    
    pMe->m_tcp_task_id = SCI_IdentifyThread();
    pMe->m_tcp_pISocket = ZMT_TCP_SocCreate(pMe, 0, cur_net_id);
    if(pMe->m_tcp_pISocket != TCPIP_SOCKET_INVALID)
    {
        ret = sci_sock_asyncselect(pMe->m_tcp_pISocket, pMe->m_tcp_task_id, AS_CONNECT|AS_READ|AS_WRITE|AS_CLOSE|AS_FULLCLOSED);
    }
    
    pMe->m_tcp_socket_is_connected = FALSE;
    
    SCI_TRACE_LOW("[ZMT_TCP] %s: taskid=0x%x, socket_id=%d, ret=%d interface=%d", __FUNCTION__, pMe->m_tcp_task_id, pMe->m_tcp_pISocket, ret,pMe->m_tcp_interface_idx);
        
    while(1)
    {
        sig_ptr = SCI_GetSignal(pMe->m_tcp_task_id);
		 SCI_TRACE_LOW("[ZMT_TCP] %s: SignalCode=%d", __FUNCTION__, sig_ptr->SignalCode);
        switch(sig_ptr->SignalCode)
        {            
            case SOCKET_FULL_CLOSED_EVENT_IND:
                {
                    so = ((SOCKET_FULL_CLOSED_EVENT_IND_SIG_T*)sig_ptr)->socket_id;
                    SCI_TRACE_LOW("[ZMT_TCP] %s: SOCKET_FULL_CLOSED_EVENT_IND so=%ld,sockid=%ld",__FUNCTION__,so,pMe->m_tcp_pISocket);
                    if( pMe->m_tcp_pISocket != TCPIP_SOCKET_INVALID && so == pMe->m_tcp_pISocket)
                    {
                        pMe->m_tcp_socket_is_connected = FALSE;
                        ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_DISCONNECTED,(void *)pMe,0);
                    }
                }
                break;
                
            case SOCKET_CONNECTION_CLOSE_EVENT_IND:
                {
                    so = ((SOCKET_CONNECTION_CLOSE_EVENT_IND_SIG_T*)sig_ptr)->socket_id;
                    SCI_TRACE_LOW("[ZMT_TCP] %s: SOCKET_CONNECTION_CLOSE_EVENT_IND so=%ld,sockid=%ld",__FUNCTION__,so,pMe->m_tcp_pISocket);
                    if( pMe->m_tcp_pISocket != TCPIP_SOCKET_INVALID && so == pMe->m_tcp_pISocket)
                    {
                        pMe->m_tcp_socket_is_connected = FALSE;
                        ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_DISCONNECTED,(void *)pMe,0);
                    }
                }
                break;
                
            case SOCKET_WRITE_EVENT_IND:
                {
                    so = ((SOCKET_WRITE_EVENT_IND_SIG_T*)sig_ptr)->socket_id;
                    SCI_TRACE_LOW("[ZMT_TCP] %s: SOCKET_WRITE_EVENT_IND so=%ld,sockid=%ld",__FUNCTION__,so,pMe->m_tcp_pISocket);
                    if( pMe->m_tcp_pISocket != TCPIP_SOCKET_INVALID && so == pMe->m_tcp_pISocket)
                    {
                    }
                }
                break;
                
            case SOCKET_CONNECT_EVENT_IND:
                {
                    ZMT_TCP_StopConnectTimer(pMe);
                    so = ((SOCKET_CONNECT_EVENT_IND_SIG_T*)sig_ptr)->socket_id;
                    err = ((SOCKET_CONNECT_EVENT_IND_SIG_T*)sig_ptr)->error_code;
                    SCI_TRACE_LOW("[ZMT_TCP] %s: SOCKET_CONNECT_EVENT_IND so=%ld,sockid=%ld,err=%ld", __FUNCTION__, so, pMe->m_tcp_pISocket, err);
                    if( pMe->m_tcp_pISocket != TCPIP_SOCKET_INVALID && so == pMe->m_tcp_pISocket)
                    {
                        if(0 == err)
                        {
                            pMe->m_tcp_socket_is_connected = TRUE;
                        }
                        ZMT_TCP_SocConnected(pMe,err);
                    }
                }
                break;
                
            case SOCKET_READ_EVENT_IND:
            case SOCKET_READ_BUFFER_STATUS_EVENT_IND:
                SCI_TRACE_LOW("[ZMT_TCP] %s: SOCKET_READ_EVENT_IND ", __FUNCTION__);
                if(sig_ptr->SignalCode == SOCKET_READ_BUFFER_STATUS_EVENT_IND)
                {
                    so = ((SOCKET_READ_BUFFER_STATUS_EVENT_IND_SIG_T*)sig_ptr)->socket_id;
                }
                else
                {
                    so = ((SOCKET_READ_EVENT_IND_SIG_T*)sig_ptr)->socket_id;
                }
                if( pMe->m_tcp_pISocket != TCPIP_SOCKET_INVALID && so == pMe->m_tcp_pISocket)
                {
                    while(1)
                    {
                        SCI_TRACE_LOW("[ZMT_TCP] %s: sci_sock_asyncrecv", __FUNCTION__);
                        recv_len = sci_sock_asyncrecv(pMe->m_tcp_pISocket,  pMe->m_tcp_buf_rcv, pMe->m_tcp_rcv_len, 0, &left_len);
                        if (recv_len > 0)
                        {
                            SCI_TRACE_LOW("[ZMT_TCP] %s: recv_len=%d, left_len=%d", __FUNCTION__,recv_len,left_len);
                            if(pMe->m_tcp_cmd_check_handle != PNULL)
                            {
                                uint32 cur_len = 0;
                                uint32 end_len = 0;
                                uint32 cmd_len = 0;
                                uint8 * pCmd = PNULL;
                                do
                                {
                                    cur_len = pMe->m_tcp_cmd_check_handle(pMe->m_tcp_buf_rcv+end_len,recv_len-end_len,&pCmd,&cmd_len);
                                    if(cur_len > 0)
                                    {
                                        ZMT_TCP_RCV_AddExt(pMe,pCmd,(uint32)cmd_len);
                                        end_len += cur_len;
                                        if(pMe->m_tcp_need_rcv)
                                        {
                                            if(pMe->m_tcp_cur_data != NULL && pMe->m_tcp_cur_data->rcv_handle != NULL)
                                            {
                                                SCI_TRACE_LOW("[ZMT_TCP] %s: SOCKET_READ_EVENT_IND rcv_handle", __FUNCTION__);
                                                if(ZMT_TCP_RET_OK == pMe->m_tcp_cur_data->rcv_handle((void *)pMe,pCmd,(uint32)cmd_len))
                                                {
                                                    ZMT_TCP_StopReadTimer(pMe);
                                                    ZMTTcp_Send_Write_Resp(pMe,TCP_SUCCESS,TCP_SUCCESS);
                                                    pMe->m_tcp_need_rcv = FALSE;
                                                }
                                            }
                                        }
                                    }
                                }
                                while(cur_len > 0);
                                    
                                if( 0 == left_len )
                                {
                                    ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_RCV_DATA,(void *)pMe,0);
                                    break;
                                }

                            }
                            else
                            {
                                ZMT_TCP_RCV_Add(pMe,pMe->m_tcp_buf_rcv,(uint32)recv_len);
                                if(pMe->m_tcp_need_rcv)
                                {
                                    if(pMe->m_tcp_cur_data != NULL && pMe->m_tcp_cur_data->rcv_handle != NULL)
                                    {
                                        SCI_TRACE_LOW("[ZMT_TCP] %s: SOCKET_READ_EVENT_IND rcv_handle 2", __FUNCTION__);
                                        if(ZMT_TCP_RET_OK == pMe->m_tcp_cur_data->rcv_handle(pMe,pMe->m_tcp_buf_rcv,(uint32)recv_len))
                                        {
                                            ZMT_TCP_StopReadTimer(pMe);
                                            ZMTTcp_Send_Write_Resp(pMe,TCP_SUCCESS,TCP_SUCCESS);
                                            pMe->m_tcp_need_rcv = FALSE;
                                        }
                                    }
                                }
                                if( 0 == left_len )
                                {
                                    ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_RCV_DATA,(void *)pMe,0);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            SCI_TRACE_LOW("[ZMT_TCP] %s: SOCKET_READ_EVENT_IND recv_len < 0", __FUNCTION__);
                            if(pMe->m_tcp_need_rcv)
                            {
                                ZMT_TCP_StopReadTimer(pMe);
                                ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_READ);
                                pMe->m_tcp_need_rcv = FALSE;
                            }
                            else
                            {
                                ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_DISCONNECTED,(void *)pMe,0);
                            }
                            break;
                        }
                    }
                }
                break;
                
            case SIG_ZMT_TCP_CONNET:
                {
                    ZMT_TCP_TASK_SIG_T* data_ptr=(ZMT_TCP_TASK_SIG_T*)sig_ptr;
                    SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_TCP_CONNET ", __FUNCTION__);
                    if(data_ptr != PNULL && data_ptr->p_task_data != PNULL)
                    {
                        TCP_Task_HandleSig_Conncet(pMe,data_ptr->p_task_data);
                        if(data_ptr->p_task_data->ip_str != NULL)
                        {
                            SCI_FREE(data_ptr->p_task_data->ip_str);
                        }
                        #if 0
                        if(data_ptr->p_task_data->str != NULL)
                        {
                            SCI_FREE(data_ptr->p_task_data->str);
                        }
                        #endif
                        SCI_FREE(data_ptr->p_task_data);
                    }
                    else
                    {
                        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_DISCONNECT);
                    }
                    break;
                }
                            
            case SIG_ZMT_TCP_RCV:
                {
                    ZMT_TCP_TASK_SIG_T* data_ptr=(ZMT_TCP_TASK_SIG_T*)sig_ptr;
                    SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_TCP_TCP_RCV ", __FUNCTION__);
                    if(data_ptr != PNULL && data_ptr->p_task_data != PNULL)
                    {
                        TCP_Task_HandleSig_Rcv(pMe,data_ptr->p_task_data);
                        if(data_ptr->p_task_data->ip_str != NULL)
                        {
                            SCI_FREE(data_ptr->p_task_data->ip_str);
                        }
                        if(data_ptr->p_task_data->str != NULL)
                        {
                            SCI_FREE(data_ptr->p_task_data->str);
                        }
                        SCI_FREE(data_ptr->p_task_data);
                    }
                    break;
                }
            case SIG_ZMT_TCP_WRITE:
                {
                    ZMT_TCP_TASK_SIG_T* data_ptr=(ZMT_TCP_TASK_SIG_T*)sig_ptr;
                    SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_TCP_TCP_WRITE ", __FUNCTION__);
                    if(data_ptr != PNULL && data_ptr->p_task_data != PNULL)
                    {
                        TCP_Task_HandleSig_Write(pMe,data_ptr->p_task_data);
                        if(data_ptr->p_task_data->ip_str != NULL)
                        {
                            SCI_FREE(data_ptr->p_task_data->ip_str);
                        }
                        #if 0
                        if(data_ptr->p_task_data->str != NULL)
                        {
                            SCI_FREE(data_ptr->p_task_data->str);
                        }
                        #endif
                        SCI_FREE(data_ptr->p_task_data);
                    }
                    break;
                }

            }
            SCI_FREE(sig_ptr);
    }
}

static void ZMT_TCP_TaskClose(ZMT_TCP_INTERFACE_T *pMe)
{
	SCI_TRACE_LOW("[ZMT_TCP] %s: taskid=0x%x", __FUNCTION__, pMe->m_tcp_task_id);
	if(SCI_INVALID_BLOCK_ID != pMe->m_tcp_task_id)
	{   
             if(pMe->m_tcp_pISocket != TCPIP_SOCKET_INVALID)
             {
                SCI_TRACE_LOW("[ZMT_TCP] %s: sci_sock_socketclose", __FUNCTION__);
        		sci_sock_socketclose(pMe->m_tcp_pISocket);
             }
		pMe->m_tcp_pISocket = TCPIP_SOCKET_INVALID;
		SCI_TerminateThread(pMe->m_tcp_task_id);
		SCI_DeleteThread(pMe->m_tcp_task_id);
		pMe->m_tcp_task_id = SCI_INVALID_BLOCK_ID;
            pMe->m_tcp_socket_is_connected = FALSE;
	}
}

static BLOCK_ID ZMT_TCP_TaskCreate(ZMT_TCP_INTERFACE_T *pMe)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s: taskid=0x%x", __FUNCTION__, pMe->m_tcp_task_id);
    if(pMe->m_tcp_task_id == SCI_INVALID_BLOCK_ID)
    {
        pMe->m_tcp_task_id = SCI_CreateThread("ZMT_TCP_THREAD_NAME",
                                                                "ZMT_TCP_QUEUE_NAME",
                                                                ZMT_TCP_TaskEntry,
                                                                NULL,
                                                                (void *)pMe,
                                                                1024*10,
                                                                50,
                                                                TX_SYSTEM_NORMALE,	// TX_SYSTEM_HIGH,
                                                                SCI_PREEMPT,
                                                                SCI_AUTO_START
                                                                );
    }
    return pMe->m_tcp_task_id;
}

static void ZMTTcp_SetSocketState(ZMT_TCP_INTERFACE_T *pMe,ZMTTCP_SOCKET_STATE_E status)
{
    switch(status)
    {
        case TCP_SOCKET_STATE_NULL:
                SCI_TRACE_LOW("[ZMT_TCP] %s: TCP_SOCKET_STATE_NULL", __FUNCTION__);
            break;
        case TCP_SOCKET_STATE_OPEN:
                SCI_TRACE_LOW("[ZMT_TCP] %s: TCP_SOCKET_STATE_OPEN", __FUNCTION__);
            break;
        case TCP_SOCKET_STATE_CONNECTING:
                SCI_TRACE_LOW("[ZMT_TCP] %s: TCP_SOCKET_STATE_CONNECTING", __FUNCTION__);
            break;
        case TCP_SOCKET_STATE_CONNECTED:
                SCI_TRACE_LOW("[ZMT_TCP] %s: TCP_SOCKET_STATE_CONNECTED", __FUNCTION__);
            break;
        case TCP_SOCKET_STATE_WRITING:
                SCI_TRACE_LOW("[ZMT_TCP] %s: TCP_SOCKET_STATE_WRITING", __FUNCTION__);
            break;
    }
    pMe->m_tcp_socket_status = status;
}

static ZMTTCP_SOCKET_STATE_E ZMTTcp_GetState(ZMT_TCP_INTERFACE_T * pMe)
{
        return pMe->m_tcp_socket_status;
}

static BOOLEAN ZMTTcp_IsConnected(ZMT_TCP_INTERFACE_T * pMe)
{
    ZMTTCP_SOCKET_STATE_E status = pMe->m_tcp_socket_status;
    BOOLEAN res = FALSE;
    
    if(status == TCP_SOCKET_STATE_CONNECTED
        || status == TCP_SOCKET_STATE_WRITING
       )
    {
        res = TRUE;
    }
    return res;
}

static BOOLEAN ZMTTcp_IsUsing(ZMT_TCP_INTERFACE_T * pMe)
{
    ZMTTCP_SOCKET_STATE_E status = pMe->m_tcp_socket_status;
    BOOLEAN res = FALSE;
    
    if(status == TCP_SOCKET_STATE_CONNECTING
        || status == TCP_SOCKET_STATE_WRITING
        )
    {
        res = TRUE;
    }
    return res;
}

static void ZMTTcp_TCPClose(ZMT_TCP_INTERFACE_T *pMe)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s: TCP Close Socket = %d",__FUNCTION__,pMe->m_tcp_pISocket);
    ZMT_TCP_TaskClose(pMe);
    return;
}

static uint8 ZMTTcp_TCP_Cancel(ZMT_TCP_INTERFACE_T *pMe)
{
    int8 cur_socket = pMe->m_tcp_pISocket;
    SCI_TRACE_LOW("[ZMT_TCP] %s: Cancel Socket = %d",__FUNCTION__,pMe->m_tcp_pISocket);
    pMe->m_tcp_need_connect = TRUE;
    return 0;
}

static uint8 ZMTTcp_Open(ZMT_TCP_INTERFACE_T *pMe)
{
    
    if(pMe->m_tcp_task_id == SCI_INVALID_BLOCK_ID)
    {
        if(SCI_INVALID_BLOCK_ID == ZMT_TCP_TaskCreate(pMe))
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s:  ZMT_TCP_TaskCreate fail",__FUNCTION__);
            return 0;
        }
        ZMTTcp_SetSocketState(pMe,TCP_SOCKET_STATE_OPEN);
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_TCP_TaskCreate success",__FUNCTION__);
    return 1;
}

static uint8 ZMTTcp_Close(ZMT_TCP_INTERFACE_T *pMe)
{
    ZMTTcp_TCP_Cancel(pMe);
    ZMTTcp_TCPClose(pMe);
    return 1;
}

static BOOLEAN ZMTTcp_Connect(ZMT_TCP_INTERFACE_T *pMe)
{
    ZMT_TCP_TASK_DATA_T * p_task_data = PNULL;
    BOOLEAN res = FALSE;
    uint32 ip_addr = 0;
    int ip_err;
    uint16 ip_len = 0;
    
    if(pMe->m_tcp_task_id == SCI_INVALID_BLOCK_ID)
    {
        return FALSE;
    }
    
    if(pMe->m_tcp_cur_data == NULL)
    {
        return FALSE;
    }
    if(pMe->m_tcp_cur_data->str == NULL || pMe->m_tcp_cur_data->len == 0)
    {
        return FALSE;
    }
    p_task_data = SCI_ALLOC_APPZ(sizeof(ZMT_TCP_TASK_DATA_T));
    if (p_task_data == PNULL)
    {
        return FALSE;
    }
    #if 1
        p_task_data->str = pMe->m_tcp_cur_data->str;
        p_task_data->str_len = pMe->m_tcp_cur_data->len;
    #else
    p_task_data->str = SCI_ALLOC_APPZ(pMe->m_tcp_cur_data->len+1);
    if(p_task_data->str == PNULL)
    {
        SCI_FREE(p_task_data);
        return FALSE;
    }
    SCI_MEMCPY(p_task_data->str,pMe->m_tcp_cur_data->str,pMe->m_tcp_cur_data->len);
    p_task_data->str_len = pMe->m_tcp_cur_data->len;
    #endif
    p_task_data->port = pMe->m_tcp_conn_port;
    
    if(pMe->m_tcp_ip_url != PNULL)
    {
        ip_len = strlen(pMe->m_tcp_ip_url);
        p_task_data->ip_str = SCI_ALLOC_APPZ(ip_len+1);
        if (p_task_data->ip_str == PNULL)
        {
            SCI_FREE(p_task_data);
            return FALSE;
        }
        SCI_MEMCPY(p_task_data->ip_str,pMe->m_tcp_ip_url,ip_len);
    }
    p_task_data->ip_addr = pMe->m_tcp_conn_ip;
    p_task_data->pMe = (void * )pMe;
    SCI_TRACE_LOW("[ZMT_TCP] %s: ip=%s:%d,ip_addr=0x%x",__FUNCTION__,p_task_data->ip_str,p_task_data->port,p_task_data->ip_addr);
   
    if(FALSE == ZMT_TCP_SendSigTo_Task(pMe,SIG_ZMT_TCP_CONNET,p_task_data))
    {
        if (p_task_data->ip_str != PNULL)
        {
            SCI_FREE(p_task_data->ip_str);
        }
        SCI_FREE(p_task_data);
        return FALSE;
    }
    return TRUE;
}

extern BOOLEAN ZMTTcp_Write(ZMT_TCP_INTERFACE_T *pMe,uint8 *str,uint16 str_len)
{
    ZMT_TCP_TASK_SIG_T * psig = PNULL;
    ZMT_TCP_TASK_DATA_T * p_task_data = PNULL;
    
    if(pMe->m_tcp_task_id == SCI_INVALID_BLOCK_ID)
    {
        return FALSE;
    }
    
    p_task_data = SCI_ALLOC_APPZ(sizeof(ZMT_TCP_TASK_DATA_T));
    if (p_task_data == PNULL)
    {
        return FALSE;
    }

#if 1
    p_task_data->str = str;
    p_task_data->str_len = str_len;
#else
    p_task_data->str = SCI_ALLOC_APPZ(str_len+1);
    if (p_task_data->str == PNULL)
    {
        SCI_FREE(p_task_data);
        return FALSE;
    }
    SCI_MEMCPY(p_task_data->str,str,str_len);
    p_task_data->str_len = str_len;
#endif
    if(FALSE == ZMT_TCP_SendSigTo_Task(pMe,SIG_ZMT_TCP_WRITE,p_task_data))
    {
        #if 0
        if (p_task_data->str != PNULL)
        {
            SCI_FREE(p_task_data->str);
        }
        #endif
        SCI_FREE(p_task_data);
        return FALSE;
    }

    return TRUE;
}

extern int ZMTTCP_Close(ZMT_TCP_INTERFACE_T *pMe)
{       
    ZMTTcp_Close(pMe);
    return 0;
}

static void ZMTTCP_DNSConnect(ZMT_TCP_INTERFACE_T *pMe)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s ", __FUNCTION__);
    if(pMe->m_tcp_task_id == SCI_INVALID_BLOCK_ID)
    {
        if(0 == ZMTTcp_Open(pMe))
        {
            ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_NOOPEN);
            
            return;
        }
    }
        
    if(pMe->m_tcp_socket_status == TCP_SOCKET_STATE_OPEN)
    {
        ZMTTcp_SetSocketState(pMe,TCP_SOCKET_STATE_DISCONNECTED);
    }
    
    if(TRUE == ZMTTcp_Connect(pMe))
    {
        ZMTTcp_SetSocketState(pMe,TCP_SOCKET_STATE_CONNECTING);
    }
    else
    {
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_DISCONNECT);
    }
}


extern int ZMTTCP_ConnWrite(ZMT_TCP_INTERFACE_T *pMe)
{
    int res = 0;
    ZMTTCP_SOCKET_STATE_E status = pMe->m_tcp_socket_status;
    SCI_TRACE_LOW("[ZMT_TCP] %s: pMe=0x%x,%s:%d,len=%d",__FUNCTION__,pMe,pMe->m_tcp_ip_url,pMe->m_tcp_conn_port,pMe->m_tcp_get_len);

    if(status == TCP_SOCKET_STATE_CONNECTING)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: CONNECTING",__FUNCTION__);
        ZMTTcp_TCP_Cancel(pMe);
    }
    else if(status == TCP_SOCKET_STATE_WRITING)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: WRITING",__FUNCTION__);
    }
    else if(status == TCP_SOCKET_STATE_CONNECTED)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: CONNECTED",__FUNCTION__);
        if(pMe->m_tcp_pISocket >= 0)
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: CONNECTED ZMTTcp_Write",__FUNCTION__);
            ZMTTcp_Write(pMe,pMe->m_tcp_buf_get,pMe->m_tcp_get_len);
            ZMT_TCP_StartWriteTimer(pMe,120*1000);
            return 0;
        }
    }
    else if(status == TCP_SOCKET_STATE_OPEN)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: OPEN",__FUNCTION__);
    }
    else if(status == TCP_SOCKET_STATE_DISCONNECTED)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: DISCONNECTED",__FUNCTION__);
        ZMTTcp_TCP_Cancel(pMe);
    }
   else
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: OTHER = %d",__FUNCTION__,pMe->m_tcp_socket_status);
        ZMTTcp_TCP_Cancel(pMe);
    }
    
    ZMTTCP_DNSConnect(pMe);// will create tcp thread
    ZMT_TCP_StartWriteTimer(pMe,120*1000);
    return res;
}

extern void ZMTTCP_CloseAllTimer(ZMT_TCP_INTERFACE_T *pMe)
{
    ZMT_TCP_StopReadTimer(pMe);
    ZMT_TCP_StopWriteTimer(pMe);
    ZMT_TCP_StopReWriteTimer(pMe);
    return;
}

extern int ZMTTCP_Reset(ZMT_TCP_INTERFACE_T *pMe)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s: pMe->m_tcp_socket_status=%d",__FUNCTION__, pMe->m_tcp_socket_status);
    ZMTTCP_CloseAllTimer(pMe);
    ZMTTCP_Close(pMe);
    ZMT_TCP_DelCurData(pMe);
    ZMT_TCP_RCV_DelAll(pMe);
    ZMT_TCP_Link_DelAll(pMe);
    pMe->m_tcp_conn_ip = 0;
    pMe->m_tcp_conn_port = 0;
    pMe->m_tcp_is_sending = FALSE;
    pMe->m_tcp_need_rcv = FALSE;
    pMe->m_tcp_buf_get = NULL;

    if(pMe->m_tcp_ip_url != NULL)
    {
        SCI_FREE(pMe->m_tcp_ip_url);
        pMe->m_tcp_ip_url = NULL;
    }
    return 0;
}

static int ZMTTCP_End(ZMT_TCP_INTERFACE_T *pMe,uint16 res,uint16 err_id)
{
    int result = 0;
    ZMT_TCP_LINK_DATA_T * GetData = pMe->m_tcp_cur_data;
        
    ZMT_TCP_StopReadTimer(pMe);
    ZMT_TCP_StopWriteTimer(pMe);
    
    SCI_TRACE_LOW("[ZMT_TCP] %s: interface_idx=%d,Handle=0x%x,res=%d,err_id=%d",__FUNCTION__,pMe->m_tcp_interface_idx,pMe->m_tcp_cur_data,res,err_id);

	if(err_id!=0)
	{
	    SCI_TRACE_LOW("[ZMT_TCP] %s: err_id = %d", __FUNCTION__, err_id);
	}

    if(GetData != NULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: Len=%d,Times=%d,timeout=%d,priority=%d",__FUNCTION__,GetData->len,GetData->times,GetData->timeout,GetData->priority);
    }

    if(res == TCP_SUCCESS)
    {
        if(pMe->m_tcp_callback != NULL)
        {
            pMe->m_tcp_callback(pMe,TCP_CB_SEND,TCP_SUCCESS,(void *)GetData);
        }
        ZMT_TCP_DelCurData(pMe);
        pMe->m_tcp_is_sending = FALSE;
        pMe->m_tcp_need_rcv = FALSE;
        if(ZMT_TCP_Link_Count(pMe))
        {
            ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_START,(void *)pMe,0);
        }
    }
    else
    {
        pMe->m_tcp_last_err = err_id;
        if(err_id == TCP_ERR_INCALL)
        {
            if(pMe->m_tcp_callback != NULL)
            {
                pMe->m_tcp_callback(pMe,TCP_CB_SEND,err_id,(void *)GetData);
            }
            ZMT_TCP_DelCurData(pMe);
            pMe->m_tcp_is_sending = FALSE;
            pMe->m_tcp_need_rcv = FALSE;
        }
        else if(err_id == TCP_ERR_LINKFULL)
        {
            if(pMe->m_tcp_callback != NULL)
            {
                pMe->m_tcp_callback(pMe,TCP_CB_SEND,err_id,NULL);
            }
            pMe->m_tcp_is_sending = FALSE;
            pMe->m_tcp_need_rcv = FALSE;
            if(ZMT_TCP_Link_Count(pMe))
            {
                ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_START,(void *)pMe,0);
            }
        }
        else
        {
            uint16 times = 0;
            uint32 timeout = 0;
            if(pMe->m_tcp_cur_data != NULL)
            {
                times = pMe->m_tcp_cur_data->times;
                if(pMe->m_tcp_cur_data->times > 0)
                {
                    pMe->m_tcp_cur_data->times--;
                }
                timeout = pMe->m_tcp_cur_data->timeout;
            }
            
            if(err_id == TCP_ERR_NET
                || err_id == TCP_ERR_NET_NOSYS
                || err_id == TCP_ERR_NET_WRITE
                || err_id == TCP_ERR_NET_READ
                )
            {
                ZMTTCP_Close(pMe);
                if(pMe->m_tcp_callback != NULL)
                {
                    pMe->m_tcp_callback(pMe,TCP_CB_DISCONNECTED,pMe->m_tcp_socket_status,NULL);
                }
                times = 0;
            }
            else if(err_id == TCP_ERR_NET_EMFILE)
            {
                ZMTTCP_Close(pMe);
                if(pMe->m_tcp_callback != NULL)
                {
                    pMe->m_tcp_callback(pMe,TCP_CB_DISCONNECTED,pMe->m_tcp_socket_status,NULL);
                }
                times = 0;
            }
            else if(err_id == TCP_ERR_DISCONNECT)
            {
                ZMTTCP_Close(pMe);
                if(pMe->m_tcp_callback != NULL)
                {
                    pMe->m_tcp_callback(pMe,TCP_CB_DISCONNECTED,pMe->m_tcp_socket_status,NULL);
                }
            }
            else if(err_id == TCP_ERR_TIMEOUT)
            {
                ZMTTCP_Close(pMe);
                if(pMe->m_tcp_callback != NULL)
                {
                    pMe->m_tcp_callback(pMe,TCP_CB_DISCONNECTED,pMe->m_tcp_socket_status,NULL);
                }
                times = 0;
            }
            else if(err_id == TCP_ERR_NOREG
                || err_id == TCP_ERR_NOPS)
            {
                times = 0;
            }
            
            if(times == 0)
            {
                if(pMe->m_tcp_callback != NULL)
                {
                    pMe->m_tcp_callback(pMe,TCP_CB_SEND,err_id,(void *)GetData);
                }
                
                ZMT_TCP_DelCurData(pMe);
                pMe->m_tcp_is_sending = FALSE;
                pMe->m_tcp_need_rcv = FALSE;
                if(ZMT_TCP_Link_Count(pMe))
                {
                    ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_START,(void *)pMe,0);
                }
            }
            else
            {
                if(timeout == 0)
                {
                    ZMT_TCP_StartReWriteTimer(pMe,200);
                }
                else
                {
                    if(timeout < 200)
                    {
                        timeout = 200;
                    }
                    ZMT_TCP_StartReWriteTimer(pMe,timeout);
                }
            }

        }
        
    }
    return result;

}

static int ZMTTCP_Start(ZMT_TCP_INTERFACE_T *pMe)
{    
    if(pMe->m_tcp_is_sending)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: TCP is Sending...", __FUNCTION__);
        return ZMT_TCP_RET_OK;
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s: ZMTTCP_Start", __FUNCTION__);    
    ZMT_TCP_StopReWriteTimer(pMe);
    ZMT_TCP_DelCurData(pMe);
    
    if(pMe->m_tcp_is_reg && ZMT_TCP_Link_GetMaxPri(pMe,&pMe->m_tcp_cur_data))
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ZMTTCP_Start 2", __FUNCTION__);  
        pMe->m_tcp_is_sending = TRUE;
        pMe->m_tcp_buf_get = pMe->m_tcp_cur_data->str;
        pMe->m_tcp_get_len = pMe->m_tcp_cur_data->len;
        if(pMe->m_tcp_buf_get == NULL || pMe->m_tcp_cur_data->len == 0)
        {
            ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_MEMFULL);
            return ZMT_TCP_RET_ERR;
        }
        #ifdef FILE_LOG_SUPPORT
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: Handle=0x%x,Len=%d", __FUNCTION__,pMe->m_tcp_cur_data,pMe->m_tcp_get_len);
            Trace_Need_Hex(TRUE);
            if(pMe->m_tcp_get_len < 2048)
            {
                Trace_Log_Buf_Data((char *)pMe->m_tcp_buf_get,pMe->m_tcp_get_len);
                Trace_Need_Hex(FALSE);
                Trace_Log_Buf_Data("\r\n",2);
            }
            else
            {
                Trace_Log_Buf_Data((char *)pMe->m_tcp_buf_get,80);
                Trace_Log_Buf_Data("...",3);
                Trace_Log_Buf_Data((char *)&pMe->m_tcp_buf_get[pMe->m_tcp_get_len-1],1);
                Trace_Log_Buf_Data("\r\n",2);
            }
        }
        #endif
        SCI_TRACE_LOW("[ZMT_TCP] %s: ZMTTCP_Start 3", __FUNCTION__);  
        ZMTTCP_ConnWrite(pMe);
    }
	else 
	{
		SCI_TRACE_LOW("[ZMT_TCP] %s: no pMe->m_tcp_is_reg && DSLCHAT_TCP_Link_GetMaxPri(pMe,&pMe->m_tcp_cur_data", __FUNCTION__);
		ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_NOREG);
	}
	
    return ZMT_TCP_RET_OK;
}

static int ZMTTCP_ReStart(ZMT_TCP_INTERFACE_T *pMe)
{        
    ZMT_TCP_StopReWriteTimer(pMe);
    if(pMe->m_tcp_is_reg && pMe->m_tcp_cur_data != NULL)
    {
        pMe->m_tcp_is_sending = TRUE;
        pMe->m_tcp_buf_get = pMe->m_tcp_cur_data->str;
        pMe->m_tcp_get_len = pMe->m_tcp_cur_data->len;
        if(pMe->m_tcp_buf_get == NULL || pMe->m_tcp_cur_data->len == 0)
        {
            ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_MEMFULL);
            return ZMT_TCP_RET_ERR;
        }
        ZMTTCP_ConnWrite(pMe);
    }
    else
    {
        ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_NOREG);
    }
    return ZMT_TCP_RET_OK;
}

static int ZMTTCP_GetUrlStr(ZMT_TCP_INTERFACE_T *pMe,char* host)
{
    uint16 len = strlen(host);
    if(len)
    {
        if(pMe->m_tcp_ip_url != NULL)
        {
            SCI_FREE(pMe->m_tcp_ip_url);
        }
        pMe->m_tcp_ip_url = SCI_ALLOC_APPZ(len+1);
        if(pMe->m_tcp_ip_url !=  NULL)
        {
            SCI_MEMCPY(pMe->m_tcp_ip_url,host,len);
        }
    }

    return ZMT_TCP_RET_OK;
}

int ZMTTCP_CleanSendLink(ZMT_TCP_INTERFACE_T *pMe)
{    
    if(pMe->m_tcp_is_reg)
    {
        ZMTTCP_CloseAllTimer(pMe);
        if(pMe->m_tcp_is_sending)
        {
            ZMTTCP_End(pMe,TCP_ERROR,TCP_ERR_NOREG);
            pMe->m_tcp_is_sending = FALSE;
        }
        ZMT_TCP_DelCurData(pMe);
        while(ZMT_TCP_Link_GetMaxPri(pMe,&pMe->m_tcp_cur_data))
        {
            if(pMe->m_tcp_callback != NULL)
            {
                pMe->m_tcp_callback(pMe,TCP_CB_SEND,TCP_ERR_CLEAR,(void *)pMe->m_tcp_cur_data);
            }
            ZMT_TCP_DelCurData(pMe);
        }
    }
    return ZMT_TCP_RET_OK;
}

static int ZMTTCP_RegStart(ZMT_TCP_INTERFACE_T *pMe,ZMT_TCP_REG_DATA_T * pData)
{   
    
    uint32 ip_addr = 0;
    int ip_err;
    SCI_TRACE_LOW("[ZMT_TCP] %s:  start thread",__FUNCTION__);
    if(ZMTTcp_Open(pMe))
    {
        pMe->m_tcp_is_reg = TRUE;
        pMe->m_tcp_conn_port = pData->conn_port;
        pMe->m_tcp_callback = pData->callback;
        ZMTTCP_GetUrlStr(pMe,pData->conn_ip);
        #if 0
        ip_err = inet_aton(pData->conn_ip, &ip_addr);
        if(ip_err == 1)
        {
            pMe->m_tcp_conn_ip= ip_addr;
        }
        else
        #endif
        {
            pMe->m_tcp_conn_ip = 0;
        }
        ZMTTcp_SetSocketState(pMe,TCP_SOCKET_STATE_OPEN);
        SCI_TRACE_LOW("[ZMT_TCP] %s: pMe=0x%x,Url:%s, Port=%d,IP=0x%x,%s",__FUNCTION__,pMe,pData->conn_ip,pData->conn_port,pMe->m_tcp_conn_ip,pMe->m_tcp_ip_url);
        if(pData->callback != NULL)
        {
            pData->callback(pMe,TCP_CB_REG,TCP_SUCCESS,NULL);
        }
    }
    else
    {
        if(pData->callback != NULL)
        {
            pData->callback(pMe,TCP_CB_REG,TCP_ERR_SOCKET,NULL);
        }
    }
    return ZMT_TCP_RET_OK;
}

static  int ZMTTCP_HandleReg(DPARAM param)
{
    int res = ZMT_TCP_RET_OK;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T*)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_REG_DATA_T * pData = NULL;
    ZMT_TCP_INTERFACE_T *pMe = PNULL;
    if(p_data != NULL)
    {
        pData = (ZMT_TCP_REG_DATA_T *)(p_data->data_p);
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s",__FUNCTION__);
    if(pData != NULL)
    {   
        pMe = (ZMT_TCP_INTERFACE_T *)pData->pMe;
        if(pMe->m_tcp_net_is_init == FALSE)
        {
            if(pData->callback != NULL)
            {
                pData->callback(pMe,TCP_CB_REG,TCP_ERROR,TCP_ERR_NOUIM);
            }
            
            if(pData->conn_ip != NULL)
            {
                SCI_FREE(pData->conn_ip);
            }
            SCI_FREE(pData);
            if(p_data != NULL)
            {
                SCI_FREE(p_data);
            }
            return ZMT_TCP_RET_ERR;
        }
        ZMT_TCP_StopPdpCloseTimer(pMe);
        if(pMe->m_tcp_is_reg)
        {
            if(pData->callback != NULL)
            {
                pData->callback(pMe,TCP_CB_REG,TCP_ERR_REGEXIST,NULL);
            }
            
            if(pData->conn_ip != NULL)
            {
                SCI_FREE(pData->conn_ip);
            }
            SCI_FREE(pData);
            if(p_data != NULL)
            {
                SCI_FREE(p_data);
            }
            return ZMT_TCP_RET_ERR;
        }
        SCI_TRACE_LOW("[ZMT_TCP] %s: m_dslchat_tcp_pdpstate=%d",__FUNCTION__, m_zmt_tcp_pdpstate );

        if(m_zmt_tcp_pdpstate)
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMTTCP_RegStart",__FUNCTION__);
            ZMTTCP_RegStart(pMe,pData); //start thread task
            if(pData->conn_ip != NULL)
            {
                SCI_FREE(pData->conn_ip);
            }
            SCI_FREE(pData);
        }
        else
        {
            pMe->m_tcp_wait_net = TRUE;
            pMe->m_tcp_reg_data = pData;
			MMIZMT_Net_Open();
        }
    }

    if(p_data != NULL)
    {
        SCI_FREE(p_data);
    }

    return res;
}

static  int ZMTTCP_HandleUnReg(DPARAM param)
{
    int res = 0;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = PNULL;
    if(p_data != PNULL)
    {
        pMe = (ZMT_TCP_INTERFACE_T *)p_data->data_p;
    }
    
    if(pMe != PNULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: UNREG OK is_reg=%d,interface_idx=%d",__FUNCTION__,pMe->m_tcp_is_reg,pMe->m_tcp_interface_idx);

        ZMTTCP_CleanSendLink(pMe);
        
        ZMTTCP_Reset(pMe);
        
        if(pMe->m_tcp_callback != NULL)
        {
            pMe->m_tcp_callback(pMe,TCP_CB_UNREG,TCP_SUCCESS,NULL);
        }
        pMe->m_tcp_callback = NULL;
        pMe->m_tcp_is_reg = FALSE;
        
       ZMT_TCP_StartPdpCloseTimer(pMe);
    }
    if(p_data != NULL)
    {
        SCI_FREE(p_data);
    }
    return ZMT_TCP_RET_OK;
}

static int ZMTTCP_HandleRcvData(DPARAM param)
{
    int res = 0;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)p_data->data_p;
    ZMT_TCP_RCV_DATA_T GetData = {0};
    while(ZMT_TCP_RCV_Get(pMe,&GetData) && GetData.pRcv != NULL)
    {
        int res = 0;
        
        if(pMe->m_tcp_callback != NULL)
        {
            res = pMe->m_tcp_callback(pMe,TCP_CB_RCV,TCP_SUCCESS,(void *)(&GetData));
        }
        if(res != 1)
        {
            if(GetData.pRcv != NULL)
            {
                SCI_FREE(GetData.pRcv);
            }
        }
    }
    
    if(p_data != NULL)
    {
        SCI_FREE(p_data);
    }
    
    return ZMT_TCP_RET_OK;
}

static  int ZMTTCP_HandleSend(DPARAM param)
{
    int res = 0;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = PNULL;
    ZMT_TCP_LINK_DATA_T * pData = PNULL;
    SCI_TRACE_LOW("[ZMT_TCP] %s: 1",__FUNCTION__);
    if(p_data != NULL)
    {
        pData = (ZMT_TCP_LINK_DATA_T *)(p_data->data_p);
        SCI_TRACE_LOW("[ZMT_TCP] %s: 2",__FUNCTION__);
        
    }
    if(pData != NULL)
    {
         SCI_TRACE_LOW("[ZMT_TCP] %s: 31");
        pMe = (ZMT_TCP_INTERFACE_T *)(pData->pMe); //bug ,pMe is null
        SCI_TRACE_LOW("[ZMT_TCP] %s: 32 pMe=0x%x %s:%d",__FUNCTION__,pMe,pMe->m_tcp_ip_url,pMe->m_tcp_conn_port);
        if(ZMT_TCP_Link_AddData(pMe,pData))
        {
            ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_START,(void *)pMe,0);
        }
        else
        {
            if(pData->str != NULL)
            {
                SCI_FREE(pData->str);
            }
            ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_LINKFULL);
            SCI_FREE(pData);
        }
    }
    if(p_data != NULL)
    {
        SCI_FREE(p_data);
    }
    return res;
}
static  int ZMTTCP_HandleSendStart(DPARAM param)
{
    int res = 0;
   
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)(p_data->data_p);
    SCI_TRACE_LOW("[ZMT_TCP] %s: 1",__FUNCTION__);
    if(pMe)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: pMe=0x%x,is_reg=%d,%s",__FUNCTION__,pMe,pMe->m_tcp_is_reg,pMe->m_tcp_ip_url);
    }
    
    
    ZMTTCP_Start(pMe);

    if(p_data != NULL)
    {
        SCI_FREE(p_data);
    }

    return ZMT_TCP_RET_OK;
}

static  int ZMTTCP_HandleSendReStart(DPARAM param)
{
    int res = 0;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)(p_data->data_p);
    SCI_TRACE_LOW("[ZMT_TCP] %s: Send ReStart is_reg=%d",__FUNCTION__,pMe->m_tcp_is_reg);
    
    ZMTTCP_ReStart(pMe);

    if(p_data != NULL)
    {
        SCI_FREE(p_data);
    }

    return ZMT_TCP_RET_OK;
}

static  int ZMTTCP_HandleSendSuccess(DPARAM param)
{
    int res = 0;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = PNULL;
    SCI_TRACE_LOW("[ZMT_TCP] %s",__FUNCTION__);

    if(p_data != NULL)
    {
        pMe = (ZMT_TCP_INTERFACE_T *)p_data->data_p;
        ZMTTCP_End(pMe,TCP_SUCCESS,p_data->type);
        SCI_FREE(p_data);
    }

    return ZMT_TCP_RET_OK;
}

static  int ZMTTCP_HandleSendFail(DPARAM param)
{
    int res = 0;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = PNULL;
    
    SCI_TRACE_LOW("[ZMT_TCP] %s", __FUNCTION__);
    
    if(p_data != NULL)
    {
        pMe = (ZMT_TCP_INTERFACE_T *)p_data->data_p;
        ZMTTCP_End(pMe,TCP_ERROR,p_data->type);
        SCI_FREE(p_data);
    }

    return ZMT_TCP_RET_OK;
}

extern int ZMTTCP_HandleConnected(DPARAM param)
{
    int res = 0;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)p_data->data_p;
    
    SCI_TRACE_LOW("[ZMT_TCP] %s: status=%d", __FUNCTION__,pMe->m_tcp_socket_status);
    
    ZMTTcp_SetSocketState(pMe,TCP_SOCKET_STATE_CONNECTED);
    if(pMe->m_tcp_need_connect)
    {
        pMe->m_tcp_need_connect = FALSE;
        
        if(pMe->m_tcp_callback != NULL)
        {
            pMe->m_tcp_callback(pMe,TCP_CB_CONNECTED,pMe->m_tcp_socket_status,NULL);
        }
    }

    if(p_data != NULL)
    {
        SCI_FREE(p_data);
    }

    return ZMT_TCP_RET_OK;
}

extern int ZMTTCP_HandleDisconnected(DPARAM param)
{
    int res = 0;
    ZMT_TCP_DATA_SIG_T * pp_getdata = (ZMT_TCP_DATA_SIG_T *)(param);
    ZMT_TCP_DATA_T * p_data = pp_getdata->p_data;
    ZMT_TCP_INTERFACE_T *pMe = (ZMT_TCP_INTERFACE_T *)p_data->data_p;
    
    SCI_TRACE_LOW("[ZMT_TCP] %s: Handle status=%d",__FUNCTION__, pMe->m_tcp_socket_status);

    ZMTTCP_Close(pMe);
    if(pMe->m_tcp_is_reg)
    {
        if(pMe->m_tcp_is_sending)
        {
            ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_NOPS);
        }
        
        if(pMe->m_tcp_callback != NULL)
        {
            pMe->m_tcp_callback(pMe,TCP_CB_DISCONNECTED,pMe->m_tcp_socket_status,NULL);
        }
    }
    
    if(p_data != NULL)
    {
        SCI_FREE(p_data);
    }

    return ZMT_TCP_RET_OK;
}

PUBLIC MMI_RESULT_E  MMIZMTTCP_Handle_AppMsg (PWND app_ptr, uint16 msg_id,DPARAM param)
{
    MMI_RESULT_E res = MMI_RESULT_TRUE;
    SCI_TRACE_LOW("[ZMT_TCP] %s: msg_id = %d",msg_id);
    switch(msg_id)
    {    
        case ZMT_APP_SIGNAL_TCP_CONNECT_SUCCESS:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_CONNECT_SUCCESS",__FUNCTION__);
                ZMTTCP_HandleConnected(param);
            return MMI_RESULT_TRUE;
            
        case ZMT_APP_SIGNAL_TCP_DISCONNECTED:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_DISCONNECTED",__FUNCTION__);
                ZMTTCP_HandleDisconnected(param);
            return MMI_RESULT_TRUE;
                        
        case ZMT_APP_SIGNAL_TCP_RCV_DATA:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_RCV_DATA",__FUNCTION__);
                ZMTTCP_HandleRcvData(param);
            return MMI_RESULT_TRUE;
            
        case ZMT_APP_SIGNAL_TCP_UNREG:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_UNREG",__FUNCTION__);
                ZMTTCP_HandleUnReg(param);
            return MMI_RESULT_TRUE;
            
        case ZMT_APP_SIGNAL_TCP_REG:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_REG",__FUNCTION__);
                ZMTTCP_HandleReg(param);
            return MMI_RESULT_TRUE;

        case ZMT_APP_SIGNAL_TCP_WRITE_ADD:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_WRITE_ADD",__FUNCTION__);
            ZMTTCP_HandleSend(param);
            return MMI_RESULT_TRUE;
            
        case ZMT_APP_SIGNAL_TCP_WRITE_START:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_WRITE_START",__FUNCTION__);
            ZMTTCP_HandleSendStart(param);
            return MMI_RESULT_TRUE;
            
        case ZMT_APP_SIGNAL_TCP_WRITE_RESTART:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_WRITE_RESTART",__FUNCTION__);
                ZMTTCP_HandleSendReStart(param);
            return MMI_RESULT_TRUE;
            
        case ZMT_APP_SIGNAL_TCP_WRITE_SUCCESS:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_WRITE_SUCCESS",__FUNCTION__);
                ZMTTCP_HandleSendSuccess(param);
            return MMI_RESULT_TRUE;
            
        case ZMT_APP_SIGNAL_TCP_WRITE_FAIL:
            SCI_TRACE_LOW("[ZMT_TCP] %s: ZMT_APP_SIGNAL_TCP_WRITE_FAIL",__FUNCTION__);
                ZMTTCP_HandleSendFail(param);
            return MMI_RESULT_TRUE;
         default:
			 SCI_TRACE_LOW("[ZMT_TCP] %s: default ",__FUNCTION__);
                res = MMI_RESULT_FALSE;
            break;
    }
    
    return res;

}

static BOOLEAN MMI_ZMT_TCP_PdpStateChange(BOOLEAN is_ok,BOOLEAN is_active)
{

    SCI_TRACE_LOW("[ZMT_TCP] %s: is_ok=%d, is_active=%d",__FUNCTION__,is_ok,is_active);
    ZMT_API_TCP_UnReg(m_zmt_tcp_reg_interface);
    return TRUE;
}



static BOOLEAN MMI_ZMT_TCP_PdpStateActive(BOOLEAN is_ok)
{
    uint16 i = 0;
    BOOLEAN res = FALSE;
    ZMT_TCP_INTERFACE_T * pMe = m_zmt_tcp_reg_interface;
    SCI_TRACE_LOW("[ZMT_TCP] %s: is_ok=%d",__FUNCTION__,is_ok);
	if(pMe->m_tcp_wait_net)
	{
		res = TRUE;
		pMe->m_tcp_wait_net = FALSE;
		if(is_ok)
		{
			if(pMe->m_tcp_reg_data != NULL)
			{
				ZMTTCP_RegStart(pMe,pMe->m_tcp_reg_data);
				if(pMe->m_tcp_reg_data->conn_ip != NULL)
				{
					SCI_FREE(pMe->m_tcp_reg_data->conn_ip);
				}
				SCI_FREE(pMe->m_tcp_reg_data);
				pMe->m_tcp_reg_data = NULL;
			}
		}
		else
		{
			if(pMe->m_tcp_reg_data != NULL)
			{
				if(pMe->m_tcp_reg_data->callback != NULL)
				{
					pMe->m_tcp_reg_data->callback(pMe,TCP_CB_REG,TCP_ERR_NOPS,NULL);
				}
				if(pMe->m_tcp_reg_data->conn_ip != NULL)
				{
					SCI_FREE(pMe->m_tcp_reg_data->conn_ip);
				}
				SCI_FREE(pMe->m_tcp_reg_data);
				pMe->m_tcp_reg_data = NULL;
			}
		}
	}
    return res;
}



static BOOLEAN MMIZMT_TCP_PdpStateUpdate(BOOLEAN is_ok,BOOLEAN is_active)
{
    BOOLEAN res = FALSE;
    SCI_TRACE_LOW("[ZMT_TCP] %s: is_active=%d",__FUNCTION__,is_active);
    if(is_active)
    {
        res = MMI_ZMT_TCP_PdpStateActive(is_ok);
    }
    if(m_zmt_tcp_pdpstate != is_ok)
    {
        m_zmt_tcp_pdpstate = is_ok;
        MMI_ZMT_TCP_PdpStateChange(is_ok,is_active);
    }
    return TRUE;
}
PUBLIC BOOLEAN  MMIZMT_TCP_Handle_PDPActiveOK(void)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s",__FUNCTION__);
    MMIZMT_TCP_PdpStateUpdate(TRUE,TRUE);
    return TRUE;
}

PUBLIC BOOLEAN  MMIZMT_TCP_Handle_PDPActiveFail(void)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s",__FUNCTION__);
    MMIZMT_TCP_PdpStateUpdate(FALSE,TRUE);
    return TRUE;
}

PUBLIC BOOLEAN  MMIZMT_TCP_Handle_PDPRespond(void)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s",__FUNCTION__);
    MMIZMT_TCP_PdpStateUpdate(FALSE,FALSE);
    return TRUE;
}


static BOOLEAN ZMTTCP_NetGetInterface(ZMT_TCP_INTERFACE_T ** ppMe)
{
    return TRUE;
}

BOOLEAN ZMTTCP_NetOpen(ZMT_TCP_INTERFACE_T ** ppMe,ZMTCMDCHECKHANDLER cmd_handle,uint16 rcv_len)
{
    if(ZMTTCP_NetGetInterface(ppMe))
    {
        ZMT_TCP_INTERFACE_T * pMe = *ppMe;
        if(pMe != PNULL && pMe->m_tcp_net_is_init == FALSE)
        {
            SCI_TRACE_LOW("[ZMT_TCP] %s: interface_idx=%d",__FUNCTION__,pMe->m_tcp_interface_idx);
            if(rcv_len == 0)
            {
                rcv_len = 1024;
            }
            pMe->m_tcp_buf_rcv = SCI_ALLOC_APPZ(rcv_len);
            if(pMe->m_tcp_buf_rcv == PNULL)
            {
                SCI_TRACE_LOW("[ZMT_TCP] %s: rcv_len =  %d",__FUNCTION__,rcv_len);
                return FALSE;
            }
            pMe->m_tcp_net_is_init = TRUE;
            pMe->m_tcp_task_id == SCI_INVALID_BLOCK_ID;
            pMe->m_tcp_pISocket = TCPIP_SOCKET_INVALID;
            pMe->m_tcp_last_err = TCP_ERROR;
            pMe->m_tcp_need_connect = TRUE;
            pMe->m_tcp_rcv_len = rcv_len;
            pMe->m_tcp_cmd_check_handle = cmd_handle;
            return TRUE;
        }
    }
    return FALSE;
}

BOOLEAN ZMTTCP_NetClose(ZMT_TCP_INTERFACE_T * pMe)
{
    if(pMe != PNULL && pMe->m_tcp_net_is_init == TRUE)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: interface_idx=%d",__FUNCTION__, pMe->m_tcp_interface_idx);
        ZMTTCP_CloseAllTimer(pMe);
        ZMTTCP_Close(pMe);
        ZMT_TCP_DelCurData(pMe);
        ZMT_TCP_RCV_DelAll(pMe);
        ZMT_TCP_Link_DelAll(pMe);
        if(pMe->m_tcp_ip_url != NULL)
        {
            SCI_FREE(pMe->m_tcp_ip_url);
            pMe->m_tcp_ip_url = NULL;
        }
        if(pMe->m_tcp_buf_rcv != NULL)
        {
            SCI_FREE(pMe->m_tcp_buf_rcv);
            pMe->m_tcp_buf_rcv = NULL;
        }
        SCI_MEMSET(pMe,0,sizeof(ZMT_TCP_INTERFACE_T));
    }
    return TRUE;
}

extern int ZMTAPI_TCP_SendDataNow(ZMT_TCP_INTERFACE_T * pMe,ZMT_TCP_LINK_DATA_T * pData)
{
    int res = 0;
    SCI_TRACE_LOW("[ZMT_TCP] %s ", __FUNCTION__);
    if(pData != NULL)
    {
        if(ZMT_TCP_Link_AddData(pMe,pData))
        {
            ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_START,(void *)pMe,0);
        }
        else
        {
            if(pData->str != NULL)
            {
                SCI_FREE(pData->str);
            }
            ZMTTcp_Send_Write_Resp(pMe,TCP_ERROR,TCP_ERR_LINKFULL);
            SCI_FREE(pData);
        }
    }
    return res;
}

extern uint32 ZMT_API_TCP_SendData(ZMT_TCP_INTERFACE_T * pMe,uint8 * pData,uint16 Data_len,uint8 priority,uint8 repeat_times,uint32 timeout,ZMTTCPRCVHANDLER rcv_handle)
{
    ZMT_TCP_LINK_DATA_T * cur_data = NULL;
    uint16 len = Data_len;
    
    if(Data_len == 0 || pData == NULL )
    {
        return 0;
    }

    cur_data=(ZMT_TCP_LINK_DATA_T *)SCI_ALLOC_APPZ(sizeof(ZMT_TCP_LINK_DATA_T));
    if(cur_data == NULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ERR Malloc", __FUNCTION__);
        return 0;
    }

    cur_data->str= (uint8 *)SCI_ALLOC_APPZ(len +1);
    if(cur_data->str == NULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ERR Malloc DataLen=%d", __FUNCTION__,len +1);
        SCI_FREE(cur_data);
        return 0;
    }
    SCI_MEMSET(cur_data->str,0,len+1);
    cur_data->len = len;
    SCI_MEMCPY(cur_data->str,pData,len);
    cur_data->pMe = (void *)pMe;
    cur_data->times = repeat_times;
    cur_data->timeout  =timeout;
    cur_data->priority = priority;
    cur_data->rcv_handle =rcv_handle;
    ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_ADD,cur_data,len);
    return (uint32)cur_data;
}

int ZMT_API_TCP_Reg(ZMT_TCP_INTERFACE_T *pMe,char * str_ip_url,uint16 conn_port,ZMTTCPCBHANDLER callback)
{
    ZMT_TCP_REG_DATA_T * cur_data = NULL;
    uint16 ip_len = 0;
    if(pMe == NULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: Err No INTERFACE", __FUNCTION__);
        return ZMT_TCP_RET_ERR;
    }
    SCI_TRACE_LOW("[ZMT_TCP] %s: pMe=0x%x", __FUNCTION__,pMe);
    if(str_ip_url == NULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: Err No ip_str", __FUNCTION__);
        return ZMT_TCP_RET_ERR;
    }

    ip_len= strlen(str_ip_url);

    if(ip_len == 0)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: Err ip_len", __FUNCTION__);
        return ZMT_TCP_RET_ERR;
    }
    
    cur_data=(ZMT_TCP_REG_DATA_T *)SCI_ALLOC_APPZ(sizeof(ZMT_TCP_REG_DATA_T));
    if(cur_data == NULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ERR Malloc", __FUNCTION__);
        return ZMT_TCP_RET_ERR;
    }
    
    cur_data->conn_ip = (char *)SCI_ALLOC_APPZ(ip_len+1);
    if(cur_data->conn_ip == NULL)
    {
        SCI_TRACE_LOW("[ZMT_TCP] %s: ERR Malloc IP Len=%d", __FUNCTION__,ip_len+1);
        SCI_FREE(cur_data);
        return ZMT_TCP_RET_ERR;
    }
    cur_data->pMe = (void *)pMe;
    SCI_MEMCPY(cur_data->conn_ip,str_ip_url,ip_len);
    cur_data->conn_port = conn_port;
    cur_data->callback = callback;
    ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_REG,(void *)cur_data,ip_len);
  
  return ZMT_TCP_RET_OK;
}

int ZMT_API_TCP_UnReg(ZMT_TCP_INTERFACE_T *pMe)
{
    SCI_TRACE_LOW("[ZMT_TCP] %s: pMe=0x%x", __FUNCTION__, pMe);
    ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_UNREG,(void *)pMe,0);
    return ZMT_TCP_RET_OK;
}

BOOLEAN ZMT_API_TCP_AllowClose(ZMT_TCP_INTERFACE_T *pMe)
{
    if(pMe->m_tcp_is_reg == FALSE)
    {
        return TRUE;
    }
    return FALSE;
}
