
#ifndef  _MMI_ZYBHTTP_API_C  
#define _MMI_ZYBHTTP_API_C  

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "zyb_app.h"
#include "zyb_http_api.h"

LOCAL BOOLEAN                  s_zyb_http_is_init = FALSE;
LOCAL uint8                       s_zyb_http_send_timer_id = 0;
LOCAL uint8                       s_zyb_http_re_send_timer_id = 0;

LOCAL BOOLEAN s_zyb_http_is_poping_data = FALSE;
LOCAL ZYB_HTTP_DATA_T *        s_zyb_http_cur_pop_pdata = PNULL;

LOCAL ZYB_HTTP_DATA_LINK_NODE_T  *    s_zyb_http_data_link_head = PNULL;
LOCAL ZYB_HTTP_RCV_NODE_T  * s_zyb_http_rcv_head = PNULL;

LOCAL BOOLEAN MMIZYB_HTTP_PopSend(void);

PUBLIC BOOLEAN  MMIZYB_HTTP_SendSigTo_APP(ZYB_APP_SIG_E sig_id, ZYB_HTTP_DATA_T * p_http_data)
{
    uint8 * pstr = PNULL;
    ZYB_HTTP_SIG_T * psig = PNULL;

    //send signal to AT to write uart
    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,sig_id,sizeof(ZYB_HTTP_SIG_T),SCI_IdentifyThread());
    psig->p_http_data = p_http_data;
    
    SCI_SEND_SIGNAL((xSignalHeaderRec*)psig,P_APP);
    return TRUE;
}

PUBLIC BOOLEAN  MMIZYB_HTTP_SendSig_Refresh(ZYBHTTP_REFRESH_DATA_T * p_http_data)
{
    uint8 * pstr = PNULL;
    ZYBHTTP_REFRESH_SIG_T * psig = PNULL;
            
    //send signal to AT to write uart
    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,ZYBHTTP_APP_SIGNAL_HTTP_REFRESH,sizeof(ZYBHTTP_REFRESH_SIG_T),SCI_IdentifyThread());
    psig->p_http_data = p_http_data;
    
    SCI_SEND_SIGNAL((xSignalHeaderRec*)psig,P_APP);
    return TRUE;
}

LOCAL void ZYB_HTTP_SendTimer_Handle(
                                uint8 timer_id,
                                uint32 param
                                )
{
    MN_DUAL_SYS_E sys_sim = 0;
    BOOLEAN       sim_ok =  FALSE;

    if(timer_id == s_zyb_http_send_timer_id && 0 != s_zyb_http_send_timer_id)
    {
        MMK_StopTimer(s_zyb_http_send_timer_id);
        s_zyb_http_send_timer_id = 0;
    }
    MMIZYB_HTTP_PopSend_Result(TRUE, HTTP_ERROR_TIMEOUT);
}

PUBLIC void ZYB_HTTP_SendTimer_Start(uint32 time_ms)
{
    if(0 != s_zyb_http_send_timer_id)
    {
        MMK_StopTimer(s_zyb_http_send_timer_id);
        s_zyb_http_send_timer_id = 0;
    }
    s_zyb_http_send_timer_id = MMK_CreateTimerCallback(time_ms, ZYB_HTTP_SendTimer_Handle, PNULL, FALSE);
    ZYB_HTTP_LOG("ZYB_HTTP_SendTimer_Start timeId:%d",s_zyb_http_send_timer_id);
}

PUBLIC void ZYB_HTTP_SendTimer_Stop(void)
{
    if(0 != s_zyb_http_send_timer_id)
    {
        MMK_StopTimer(s_zyb_http_send_timer_id);
        s_zyb_http_send_timer_id = 0;
    }
}
LOCAL void ZYB_HTTP_ResendTimer_Handle(
                                uint8 timer_id,
                                uint32 param
                                )
{
    MN_DUAL_SYS_E sys_sim = 0;
    BOOLEAN       sim_ok =  FALSE;

    if(timer_id == s_zyb_http_re_send_timer_id && 0 != s_zyb_http_re_send_timer_id)
    {
        MMK_StopTimer(s_zyb_http_re_send_timer_id);
        s_zyb_http_re_send_timer_id = 0;
    }
    MMIZYB_HTTP_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_RESEND,PNULL);
}

PUBLIC void ZYB_HTTP_ResendTimer_Start(uint32 time_ms)
{
    if(0 != s_zyb_http_re_send_timer_id)
    {
        MMK_StopTimer(s_zyb_http_re_send_timer_id);
        s_zyb_http_re_send_timer_id = 0;
    }
    s_zyb_http_re_send_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        ZYB_HTTP_ResendTimer_Handle, 
                                                                        PNULL, 
                                                                        FALSE);
}

PUBLIC void ZYB_HTTP_ResendTimer_Stop(void)
{
    if(0 != s_zyb_http_re_send_timer_id)
    {
        MMK_StopTimer(s_zyb_http_re_send_timer_id);
        s_zyb_http_re_send_timer_id = 0;
    }
}


BOOLEAN ZYB_HTTP_RCV_Add(uint8 * pRcv, uint32 rcv_len)
{
    ZYB_HTTP_RCV_NODE_T  * p1 = NULL;
    ZYB_HTTP_RCV_NODE_T  * p2 = NULL;
    uint32 len = rcv_len;

    if(len == 0)
    {
        return FALSE;
    }
    
    p1=(ZYB_HTTP_RCV_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZYB_HTTP_RCV_NODE_T));
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

    if(s_zyb_http_rcv_head == NULL)
    {
        s_zyb_http_rcv_head = p1;
    }
    else
    {
        p2 = s_zyb_http_rcv_head;
        while(p2->next_ptr != NULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

BOOLEAN ZYB_HTTP_RCV_AddExt(uint8 * pRcv, uint32 rcv_len)
{
    ZYB_HTTP_RCV_NODE_T  * p1 = NULL;
    ZYB_HTTP_RCV_NODE_T  * p2 = NULL;
    uint32 len = rcv_len;

    if(len == 0)
    {
        return FALSE;
    }
    
    p1=(ZYB_HTTP_RCV_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZYB_HTTP_RCV_NODE_T));
    if(p1 == NULL)
    {
        return FALSE;
    }
    p1->data.pRcv = pRcv;
    p1->data.len = len;
    
    p1->next_ptr = NULL;

    if(s_zyb_http_rcv_head == NULL)
    {
        s_zyb_http_rcv_head = p1;
    }
    else
    {
        p2 = s_zyb_http_rcv_head;
        while(p2->next_ptr != NULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

BOOLEAN  ZYB_HTTP_RCV_Get(ZYB_HTTP_RCV_DATA_T * pUDPData)
{
    ZYB_HTTP_RCV_NODE_T  * p1 = NULL;
    
    p1 = s_zyb_http_rcv_head;
    
    if(p1 != NULL)
    {
      *pUDPData = p1->data;
        s_zyb_http_rcv_head = p1->next_ptr;
        SCI_FREE(p1);
        return TRUE;
    }
        
    return FALSE;
}

uint32 ZYB_HTTP_RCV_GetAll(ZYB_HTTP_RCV_DATA_T * pAllData)
{
    uint32 len = 0;
    uint32 cur_len = 0;
    ZYB_HTTP_RCV_DATA_T OneData = {0};
    ZYB_HTTP_RCV_NODE_T  * p1 = NULL;
    ZYB_HTTP_RCV_NODE_T  * p2 = NULL;
    
    if(pAllData == NULL)
    {
        return 0;
    }
    p2 = s_zyb_http_rcv_head;
    while(p2 != NULL)
    {
        p1 = p2;
        p2 = p1->next_ptr;
        if(p1->data.pRcv != 0 && p1->data.len > 0)
        {
            len += p1->data.len;
        }
    }
    
    if(len > 0)
    {
        pAllData->pRcv = SCI_ALLOC_APPZ(len+1);
        if(pAllData->pRcv == NULL)
        {
            return 0;
        }
        while(ZYB_HTTP_RCV_Get(&OneData) && OneData.pRcv != NULL)
        {        
            SCI_MEMCPY(pAllData->pRcv+cur_len,OneData.pRcv,OneData.len);
            cur_len += OneData.len;
            if(OneData.pRcv != NULL)
            {
                SCI_FREE(OneData.pRcv);
            }
        }
        pAllData->len = cur_len;
    }
    return len;
}

BOOLEAN  ZYB_HTTP_RCV_DelAll(void)
{
    ZYB_HTTP_RCV_NODE_T  * p1 = NULL;
    
    while(s_zyb_http_rcv_head != NULL)
    {
        p1 = s_zyb_http_rcv_head;
        s_zyb_http_rcv_head = p1->next_ptr;
        if(p1->data.pRcv != 0)
        {
            SCI_FREE(p1->data.pRcv);
        }
        SCI_FREE(p1);
    }
    return TRUE;
}

//不 分配内存
LOCAL BOOLEAN ZYB_HTTP_Link_Add(ZYB_HTTP_DATA_T * p_data)
{
    ZYB_HTTP_DATA_LINK_NODE_T  * p1 = PNULL;
    ZYB_HTTP_DATA_LINK_NODE_T  * p2 = PNULL;
    
    if(p_data == PNULL 
        || p_data->ip_str == PNULL
        )
    {
        return FALSE;
    }
    
    p1=(ZYB_HTTP_DATA_LINK_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZYB_HTTP_DATA_LINK_NODE_T));
    if(p1 == PNULL)
    {
        return FALSE;
    }
    
    p1->p_data = p_data;    
    p1->next_ptr = PNULL;
    
    if(s_zyb_http_data_link_head == PNULL)
    {
        s_zyb_http_data_link_head = p1;
    }
    else
    {
        p2 = s_zyb_http_data_link_head;
        while(p2->next_ptr != PNULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

//不释放内存
LOCAL BOOLEAN   ZYB_HTTP_Link_Get(ZYB_HTTP_DATA_T ** pp_data)
{
    ZYB_HTTP_DATA_LINK_NODE_T  * p1 = PNULL;
    ZYB_HTTP_DATA_LINK_NODE_T  * p2 = PNULL;
    BOOLEAN res = FALSE;
    
    p2 = p1 = s_zyb_http_data_link_head;
    
    while(p1 != PNULL)
    {
        if(p1->p_data != PNULL && p1->p_data->ip_str != PNULL)
        {
            *pp_data = p1->p_data;
            if(p1 == s_zyb_http_data_link_head)
            {
                s_zyb_http_data_link_head = p1->next_ptr;
            }
            else
            {
                p2->next_ptr = p1->next_ptr;
            }
            res = TRUE;
            SCI_FREE(p1);
            break;
        }
        p2 = p1;
        p1 = p2->next_ptr;
    }
    return res;
}

LOCAL BOOLEAN  ZYB_HTTP_Link_GetByPri(ZYB_HTTP_DATA_T ** pPostData,uint8 priority)
{
    ZYB_HTTP_DATA_LINK_NODE_T  * p1 = NULL;
    ZYB_HTTP_DATA_LINK_NODE_T  * p2 = NULL;
    BOOLEAN res = FALSE;
    
    p2 = p1 = s_zyb_http_data_link_head;
    
    while(p1 != NULL)
    {
        if(p1->p_data != PNULL && p1->p_data->ip_str != PNULL && p1->p_data->priority == priority)
        {
            *pPostData = p1->p_data;
            
            if(p1 == s_zyb_http_data_link_head)
            {
                s_zyb_http_data_link_head = p1->next_ptr;
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
    return res;
}

LOCAL BOOLEAN  ZYB_HTTP_Link_GetMaxPri(ZYB_HTTP_DATA_T ** pPostData)
{
    ZYB_HTTP_DATA_LINK_NODE_T  * p1 = NULL;
    ZYB_HTTP_DATA_LINK_NODE_T  * p2 = NULL;
    uint8 max_pri = 0;
    BOOLEAN res = FALSE;
    
    p2 = p1 = s_zyb_http_data_link_head;
    
    while(p1 != NULL)
    {
        if(p1->p_data != PNULL && p1->p_data->ip_str != PNULL && p1->p_data->priority > max_pri)
        {
            max_pri = p1->p_data->priority;
        }
        p2 = p1;
        p1 = p2->next_ptr;
    }
    
    res = ZYB_HTTP_Link_GetByPri(pPostData,max_pri);
    return res;
}

LOCAL BOOLEAN  ZYB_HTTP_Link_DelAll(void)
{
    ZYB_HTTP_DATA_LINK_NODE_T  * p1 = PNULL;
    while(s_zyb_http_data_link_head != PNULL)
    {
        p1 = s_zyb_http_data_link_head;
        s_zyb_http_data_link_head = p1->next_ptr;
        if(p1->p_data != PNULL)
        {
            if(p1->p_data->str != PNULL)
            {
                SCI_FREE(p1->p_data->str);
            }
            if(p1->p_data->ip_str != PNULL)
            {
                SCI_FREE(p1->p_data->ip_str);
            }
            SCI_FREE(p1->p_data);
        }
        SCI_FREE(p1);
    }
    return TRUE;
}

LOCAL uint16  ZYB_HTTP_Link_Count(void)
{
    uint16 num = 0;
    ZYB_HTTP_DATA_LINK_NODE_T  * p1 = s_zyb_http_data_link_head;
    while(p1 != PNULL)
    {
        num++;
        p1 = p1->next_ptr;
    }
    return num;
}

LOCAL ZYB_HTTP_DATA_T *  ZYB_HTTP_Link_PopData(void)
{
    uint8 * pstr = PNULL;
    ZYB_HTTP_DATA_T * p_http_data = PNULL;
    BOOLEAN res = FALSE;
    
    res = ZYB_HTTP_Link_GetMaxPri(&p_http_data);
    
    if(res == FALSE || p_http_data == NULL)
    {
        ZYB_HTTP_LOG("ZYB_HTTP_Link_PopData Empty !!!");
        return PNULL;
    }
    
    if(
        p_http_data->ip_str == PNULL
        )
    {
        if(p_http_data->str != PNULL)
        {
            SCI_FREE(p_http_data->str);
        }
        SCI_FREE(p_http_data);
        ZYB_HTTP_LOG("ZYB_HTTP_Link_PopData ERR Len");
        return PNULL;
    }
    
    #if 1
        ZYB_HTTP_LOG("ZYB_HTTP_Link_PopData OK p_http_data=0x%x,ip_str=%s,type=%d,str_len=%d",p_http_data,p_http_data->ip_str,p_http_data->type,p_http_data->str_len);
    #endif
   return p_http_data;
}

LOCAL uint8 * zyb_urlencode(const char *s, int len, int *new_length)
{
    uint8 *from = PNULL; 
    uint8 *end = PNULL; 
    uint8 *start = PNULL;
    uint8 *to = PNULL;
    uint8 hexchars[] = "0123456789ABCDEF";
    uint8 c = 0;
    
    start = to = (uint8 *) SCI_ALLOCAZ(3 * len + 1);
    from = s;
    end = s + len;

    while (from < end)
    {
        c = *from++;
        #if 0
        if (c == ' ')
        {
            *to++ = '+';
        }
        else if ((c < '0' && c != '-' && c != '.')
        ||(c < 'A' && c > '9')
        ||(c > 'Z' && c < 'a' && c != '_')
        ||(c > 'z'))
        {
            to[0] = '%';
            to[1] = hexchars[c >> 4];
            to[2] = hexchars[c & 15];
            to += 3;
        }
        #else
        if (c == '#')
        {
            *to++ = 'p';
        }
        #endif
        else
        {
            *to++ = c;
        }
    }
    
    *to = 0;
    if (new_length)
    {
        *new_length = to - start;
    }
    
    return (char *) start;
}

int MMIZYB_HTTP_GetlCurAllUrl(uint8 **ppStr)
{
    uint8 *all_str = PNULL;
    uint32 ip_len = 0;
    uint32 data_len = 0;
    uint32 all_len = 0;
    uint32 len = 0;
    int ret_len = 0;
    uint8 * p_utr = PNULL;

    if(s_zyb_http_is_poping_data)
    {
        if(s_zyb_http_cur_pop_pdata != PNULL)
        {
            if(s_zyb_http_cur_pop_pdata->ip_str != PNULL)
            {
                ip_len = strlen(s_zyb_http_cur_pop_pdata->ip_str);
            }
            if(s_zyb_http_cur_pop_pdata->str != PNULL && s_zyb_http_cur_pop_pdata->str_len > 0)
            {
                data_len = s_zyb_http_cur_pop_pdata->str_len;
            }
            all_len = ip_len + data_len;
            if(all_len > 0)
            {
                all_str = (uint8 *) SCI_ALLOCAZ(all_len+1);
                if(all_str != PNULL)
                {
                    if(ip_len > 0)
                    {
                        SCI_MEMCPY(all_str,s_zyb_http_cur_pop_pdata->ip_str,ip_len);
                        len += ip_len;
                    }
                    if(data_len > 0)
                    {
                        SCI_MEMCPY(all_str+len,s_zyb_http_cur_pop_pdata->str,data_len);
                        len += data_len;
                    }
                    p_utr = zyb_urlencode(all_str,len,&ret_len);
                    *ppStr = p_utr;
                    SCI_FREE(all_str);
                }
            }
        }
    }
    return ret_len;
}

int MMIZYB_HTTP_GetCurUrl(uint8 **ppStr)
{
    uint8 *all_str = PNULL;
    uint32 ip_len = 0;
    uint32 all_len = 0;
    uint32 len = 0;
    int ret_len = 0;
    uint8 * p_utr = PNULL;

    if(s_zyb_http_is_poping_data)
    {
        if(s_zyb_http_cur_pop_pdata != PNULL)
        {
            if(s_zyb_http_cur_pop_pdata->ip_str != PNULL)
            {
                ip_len = strlen(s_zyb_http_cur_pop_pdata->ip_str);
            }
            all_len = ip_len;
            if(all_len > 0)
            {
                all_str = (uint8 *) SCI_ALLOCAZ(all_len+1);
                if(all_str != PNULL)
                {
                    if(ip_len > 0)
                    {
                        SCI_MEMCPY(all_str,s_zyb_http_cur_pop_pdata->ip_str,ip_len);
                        len += ip_len;
                    }
                    p_utr = zyb_urlencode(all_str,len,&ret_len);
                    *ppStr = p_utr;
                    SCI_FREE(all_str);
                }
            }
        }
    }
    return ret_len;
}

uint32 MMIZYB_HTTP_GetCurData(uint8 **ppStr)
{
    if(s_zyb_http_cur_pop_pdata != PNULL)
    {
        *ppStr = s_zyb_http_cur_pop_pdata->str;
        return s_zyb_http_cur_pop_pdata->str_len;
    }
    else
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurData Err");
        return 0;
    }
}

uint16 MMIZYB_HTTP_GetCurType(void)
{
    uint16 type = 0;
    if(s_zyb_http_cur_pop_pdata != PNULL)
    {
        type = s_zyb_http_cur_pop_pdata->type;
    }
    ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurType type = %d",type);
    return type;
}

uint32  MMIZYB_HTTP_GetCurFileLen(void)
{
    if(s_zyb_http_cur_pop_pdata != PNULL)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurFileLen = %d",s_zyb_http_cur_pop_pdata->file_len);
        return s_zyb_http_cur_pop_pdata->file_len;
    }
    ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurFileLen = 0");
    return 0;
}

uint32  MMIZYB_HTTP_GetCurFileStartLen(void)
{
    if(s_zyb_http_cur_pop_pdata != PNULL)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurFileStartLen = %d",s_zyb_http_cur_pop_pdata->file_start_len);
        return s_zyb_http_cur_pop_pdata->file_start_len;
    }
    ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurFileStartLen = 0");
    return 0;
}

uint32  MMIZYB_HTTP_GetCurFileEndLen(void)
{
    if(s_zyb_http_cur_pop_pdata != PNULL)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurFileEndLen = %d",s_zyb_http_cur_pop_pdata->file_end_len);
        return s_zyb_http_cur_pop_pdata->file_end_len;
    }
    ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurFileEndLen = 0");
    return 0;
}

ZYB_HTTP_DATA_T * MMIZYB_HTTP_GetCurAll(void)
{
    if(s_zyb_http_cur_pop_pdata != PNULL)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurAll OK");
        return s_zyb_http_cur_pop_pdata;
    }
    ZYB_HTTP_LOG("MMIZYB_HTTP_GetCurAll ERR");
    return PNULL;
}

LOCAL void MMIZYB_HTTP_DelCurData(void)
{
    if(s_zyb_http_cur_pop_pdata != PNULL)
    {
        if(s_zyb_http_cur_pop_pdata->ip_str != PNULL)
        {
            SCI_FREE(s_zyb_http_cur_pop_pdata->ip_str);
        }

        if(s_zyb_http_cur_pop_pdata->str != PNULL)
        {
            SCI_FREE(s_zyb_http_cur_pop_pdata->str);
        }
        SCI_FREE(s_zyb_http_cur_pop_pdata);
        s_zyb_http_cur_pop_pdata = PNULL;
    }
    return;
}

BOOLEAN MMIZYB_HTTP_IsGetType(void)
{
    if(s_zyb_http_cur_pop_pdata != PNULL)
    {
        return s_zyb_http_cur_pop_pdata->is_get;
    }
    return TRUE;
}

BOOLEAN MMIZYB_HTTP_Init(void)
{
    if(s_zyb_http_is_init == FALSE)
    {
        s_zyb_http_is_init = TRUE;
    }
    return TRUE;
}

void MMIZYB_HTTP_PopSend_Result(BOOLEAN need_stop, uint32 error_id)
{
    uint8 * pstr = NULL;
    ZYB_HTTP_RSP_SIG_T * psig = PNULL;
    ZYB_HTTP_LOG("MMIZYB_HTTP_PopSend_Result need_stop = %d, error_id = %d",need_stop,error_id);

    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,ZYBHTTP_APP_SIGNAL_HTTP_SEND_OVER,sizeof(ZYB_HTTP_RSP_SIG_T),SCI_IdentifyThread());
    if(error_id == 0)
    {
        psig->rsp.is_sucess = TRUE;
    }
    else
    {
        psig->rsp.is_sucess = FALSE;
    }
    psig->rsp.err_id = error_id;
    psig->rsp.need_stop = need_stop;
    SCI_SEND_SIGNAL((xSignalHeaderRec*)psig,P_APP);
    
    return;
}

BOOLEAN MMIZYB_HTTP_Send_Start(char * url,MN_DUAL_SYS_E dual_sys)
{
    #if 0 //def WIN32
        MMIZYB_HTTP_PopSend_Result(FALSE,HTTP_SUCCESS);
        res = TRUE;
    #else
            ZYBHTTP_Net_Open();
    #endif
    return TRUE;
}

LOCAL BOOLEAN MMIZYB_HTTP_PopSend(void)
{
    ZYB_HTTP_DATA_T * p_http_data = PNULL;
    BOOLEAN res = FALSE;
    int url_len = 0;
    if(s_zyb_http_is_poping_data)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_PopSend ERR Busy is_poping_data=%d, init=%d, net=%d",s_zyb_http_is_poping_data,s_zyb_http_is_init,ZYBHTTP_Net_IsInit());
        return FALSE;
    }
    ZYB_HTTP_ResendTimer_Stop();
    p_http_data = ZYB_HTTP_Link_PopData();
    if(p_http_data != PNULL)
    {
        s_zyb_http_is_poping_data = TRUE;
        s_zyb_http_cur_pop_pdata = p_http_data;
        ZYB_HTTP_RCV_DelAll();
        if(MMIZYB_HTTP_Send_Start(PNULL,ZYBHTTP_Net_GetActiveSys()))
        {
            ZYB_HTTP_SendTimer_Start(20000);
        }
        else
        {
            ZYB_HTTP_LOG("MMIZYB_HTTP_PopSend ERR Send Start");
            MMIZYB_HTTP_PopSend_Result(FALSE, HTTP_ERROR_FAILED);
        }
    }
    return res;
}

PUBLIC BOOLEAN MMIZYB_HTTP_IsSending(void)
{
    return s_zyb_http_is_poping_data;
}

PUBLIC BOOLEAN MMIZYB_HTTP_AllowClose(void)
{
    if(s_zyb_http_is_poping_data == FALSE && ZYB_HTTP_Link_Count() == 0)
    {
        return TRUE;
    }
    return FALSE;
}

PUBLIC BOOLEAN MMIZYB_HTTP_Close(void)
{
    if(s_zyb_http_is_poping_data == FALSE && ZYB_HTTP_Link_Count() == 0)
    {
        return TRUE;
    }
    return FALSE;
}

PUBLIC BOOLEAN MMIZYB_HTTP_AppStop(void)
{
    BOOLEAN res = FALSE;
    if(s_zyb_http_is_poping_data)
    {
        if(FALSE == ZybHttp_StopRequest())
        {
            MMIZYB_HTTP_PopSend_Result(TRUE, HTTP_ERROR_FILE_NO_SPACE);
        }
        res = TRUE;
    }
    ZYB_HTTP_Link_DelAll();
    return res;
}

PUBLIC BOOLEAN MMIZYB_HTTP_AppSend(BOOLEAN is_get,char * ip_str,uint8 *str,uint32 str_len,uint16 type,uint8 priority,uint8 repeat_times,uint32 timeout,uint32 file_len, uint32 file_start_len,uint32 file_end_len,ZYBHTTPRCVHANDLER rcv_handle)
{
    ZYB_HTTP_DATA_T * p_http_data = PNULL;
    BOOLEAN res = FALSE;
    uint32 ip_addr = 0;
    int ip_err;
    uint16 ip_len = 0;
    uint16 name_len = 0;

    if(ZYB_SIM_Exsit() == FALSE)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_AppSend ERR NO SIM");
        return FALSE;
    }
    
    if((str == PNULL || str_len == 0) && ip_str == PNULL)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_AppSend ERR NO Data");
        return FALSE;
    }
    
    p_http_data = SCI_ALLOC_APPZ(sizeof(ZYB_HTTP_DATA_T));
    if (p_http_data == PNULL)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_AppSend ERR ALLOC");
        return FALSE;
    }

    p_http_data->is_get = is_get;
    if(str != PNULL && str_len > 0)
    {
        p_http_data->str = SCI_ALLOC_APPZ(str_len+1);//free it in AT task
        if (p_http_data->str == PNULL)
        {
            ZYB_HTTP_LOG("MMIZYB_HTTP_AppSend ERR ALLOC 2");
            SCI_FREE(p_http_data);
            return FALSE;
        }
        SCI_MEMCPY(p_http_data->str,str,str_len);
        p_http_data->str_len = str_len;
    }
    p_http_data->type = type;

    if(ip_str != PNULL)
    {
        ip_len = strlen(ip_str);
        p_http_data->ip_str = SCI_ALLOC_APPZ(ip_len+1);
        if (p_http_data->ip_str == PNULL)
        {
            ZYB_HTTP_LOG("MMIZYB_HTTP_AppSend ERR ALLOC 3");
            if(p_http_data->str != PNULL)
            {
                SCI_FREE(p_http_data->str);
            }
            SCI_FREE(p_http_data);
            return FALSE;
        }
        SCI_MEMCPY(p_http_data->ip_str,ip_str,ip_len);
    }
    p_http_data->times = repeat_times;
    p_http_data->timeout  =timeout;
    p_http_data->priority = priority;
    p_http_data->rcv_handle = rcv_handle;
    p_http_data->file_len = file_len;
    p_http_data->file_start_len = file_start_len;
    p_http_data->file_end_len = file_end_len;
    
    res = MMIZYB_HTTP_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_SEND,p_http_data);
    if(res == FALSE)
    {
        if(p_http_data->ip_str!= PNULL)
        {
            SCI_FREE(p_http_data->ip_str);
        }
        if(p_http_data->str != PNULL)
        {
            SCI_FREE(p_http_data->str);
        }
        SCI_FREE(p_http_data);
    }
    return res;

}

BOOLEAN  MMIZYB_HTTP_Handle_Send(DPARAM param)
{
    BOOLEAN res = FALSE;
    ZYB_HTTP_SIG_T * pp_getdata = (ZYB_HTTP_SIG_T *)(param);
    ZYB_HTTP_DATA_T * p_getdata = pp_getdata->p_http_data;
    if(p_getdata == PNULL)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_Send ERR NULL DATA");
        return FALSE;
    }

    #if 0
        //需要即时响应
        res = ZYB_HTTP_Link_Add(p_getdata);
        if(res == FALSE)
        {
            if(p_getdata->ip_str != NULL)
            {
                SCI_FREE(p_getdata->ip_str);
            }
            if(p_getdata->str != NULL)
            {
                SCI_FREE(p_getdata->str);
            }
            SCI_FREE(p_getdata);
            ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_Send ERR LINK ADD");
        }
            
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_Send is_poping_data=%d",s_zyb_http_is_poping_data);
        if(s_zyb_http_is_poping_data == FALSE)
        {
            MMIZYB_HTTP_PopSend();
        }
        else
        {
            MMIZYB_HTTP_AppStop();
        }
    #else
    res = ZYB_HTTP_Link_Add(p_getdata);
    if(res == FALSE)
    {
        if(p_getdata->ip_str != NULL)
        {
            SCI_FREE(p_getdata->ip_str);
        }
        if(p_getdata->str != NULL)
        {
            SCI_FREE(p_getdata->str);
        }
        SCI_FREE(p_getdata);
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_Send ERR LINK ADD");
    }
        
    ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_Send is_poping_data=%d",s_zyb_http_is_poping_data);
    if(s_zyb_http_is_poping_data == FALSE)
    {
        MMIZYB_HTTP_PopSend();
    }
    #endif
    return TRUE;
}

BOOLEAN  MMIZYB_HTTP_Handle_ReSend(DPARAM param)
{
    BOOLEAN res = FALSE;
    char * url_ptr = PNULL;
    int url_len = 0;
    
    ZYB_HTTP_ResendTimer_Stop();
    if(s_zyb_http_cur_pop_pdata != NULL)
    {
        s_zyb_http_is_poping_data = TRUE;
        ZYB_HTTP_RCV_DelAll();
        if(MMIZYB_HTTP_Send_Start(url_ptr,ZYBHTTP_Net_GetActiveSys()))
        {
            ZYB_HTTP_SendTimer_Start(20000);
        }
        else
        {
            ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_ReSend ERR Send Start");
            MMIZYB_HTTP_PopSend_Result(FALSE, HTTP_ERROR_FAILED);
        }
    }
    else
    {
        MMIZYB_HTTP_PopSend_Result(FALSE, HTTP_ERROR_NO_MEMORY);
    }
    return TRUE;
}

BOOLEAN  MMIZYB_HTTP_Handle_Rcv(DPARAM param)
{
    BOOLEAN res = FALSE;
    ZYB_HTTP_SIG_T * pp_getdata = (ZYB_HTTP_SIG_T *)(param);
    ZYB_HTTP_DATA_T * p_http_data = pp_getdata->p_http_data;
    
    if(p_http_data != PNULL)
    {
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_Rcv p_http_data=0x%x,ip_str=%s,type=%d,str_len=%d",p_http_data,p_http_data->ip_str,p_http_data->type,p_http_data->str_len);
        if(p_http_data->ip_str != NULL)
        {
            SCI_FREE(p_http_data->ip_str);
        }
        if(p_http_data->str != NULL)
        {
            SCI_FREE(p_http_data->str);
        }
        SCI_FREE(p_http_data);
    }
    return TRUE;
}

BOOLEAN  MMIZYB_HTTP_Handle_SendSuccess(DPARAM param)
{
    ZYB_HTTP_RSP_T * p_getdata = (ZYB_HTTP_RSP_T *)param;
    uint16 http_link_coundt = ZYB_HTTP_Link_Count();
    
    ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_SendSuccess is_sending = %d, is_sucess=%d,err_id=%d",s_zyb_http_is_poping_data,p_getdata->is_sucess,p_getdata->err_id);
    
    if(p_getdata->need_stop)
    {
        //ZYBHTTP_Net_Close();
    }
    
    if(p_getdata->is_sucess)
    {
        //成功
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_SendSuccess OK,is_poping =%d,p_getdata=0x%x",s_zyb_http_is_poping_data,s_zyb_http_cur_pop_pdata);
        if(s_zyb_http_is_poping_data)
        {
            if(s_zyb_http_cur_pop_pdata != PNULL)
            {
                if(s_zyb_http_cur_pop_pdata->ip_str != PNULL)
                {
                    SCI_FREE(s_zyb_http_cur_pop_pdata->ip_str);
                }

                if(s_zyb_http_cur_pop_pdata->str != PNULL)
                {
                    SCI_FREE(s_zyb_http_cur_pop_pdata->str);
                }
                SCI_FREE(s_zyb_http_cur_pop_pdata);
            }
            s_zyb_http_is_poping_data = FALSE;
            if(http_link_coundt > 0)
            {
                MMIZYB_HTTP_PopSend();
            }
        }

    }
    else
    {
        //失败
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_SendOver FAIL, is_poping =%d,p_getdata=0x%x",s_zyb_http_is_poping_data,s_zyb_http_cur_pop_pdata);
        if(s_zyb_http_is_poping_data)
        {
            if(s_zyb_http_cur_pop_pdata != PNULL)
            {
                if(s_zyb_http_cur_pop_pdata->ip_str != PNULL)
                {
                    SCI_FREE(s_zyb_http_cur_pop_pdata->ip_str);
                }

                if(s_zyb_http_cur_pop_pdata->str != PNULL)
                {
                    SCI_FREE(s_zyb_http_cur_pop_pdata->str);
                }
                SCI_FREE(s_zyb_http_cur_pop_pdata);
            }
            s_zyb_http_is_poping_data = FALSE;
            if(http_link_coundt > 0)
            {
                MMIZYB_HTTP_PopSend();
            }
        }
    }
    return TRUE;
}

BOOLEAN  MMIZYB_HTTP_Handle_SendFail(DPARAM param)
{
    ZYB_HTTP_RSP_T * p_getdata = (ZYB_HTTP_RSP_T *)param;
    uint16 http_link_coundt = ZYB_HTTP_Link_Count();
    
    ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_SendOver is_sending = %d, is_sucess=%d,err_id=%d",s_zyb_http_is_poping_data,p_getdata->is_sucess,p_getdata->err_id);
    
    if(p_getdata->need_stop)
    {
        //ZYB_HTTP_Browser_Stop();
    }
    
    if(p_getdata->is_sucess)
    {
        //成功
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_SendOver OK,is_poping =%d,p_getdata=0x%x",s_zyb_http_is_poping_data,s_zyb_http_cur_pop_pdata);
        if(s_zyb_http_is_poping_data)
        {
            if(s_zyb_http_cur_pop_pdata != PNULL)
            {
                if(s_zyb_http_cur_pop_pdata->ip_str != PNULL)
                {
                    SCI_FREE(s_zyb_http_cur_pop_pdata->ip_str);
                }

                if(s_zyb_http_cur_pop_pdata->str != PNULL)
                {
                    SCI_FREE(s_zyb_http_cur_pop_pdata->str);
                }
                SCI_FREE(s_zyb_http_cur_pop_pdata);
            }
            s_zyb_http_is_poping_data = FALSE;
            if(http_link_coundt > 0)
            {
                MMIZYB_HTTP_PopSend();
            }
        }

    }
    else
    {
        //失败
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_SendOver FAIL, is_poping =%d,p_getdata=0x%x",s_zyb_http_is_poping_data,s_zyb_http_cur_pop_pdata);
        if(s_zyb_http_is_poping_data)
        {
            if(s_zyb_http_cur_pop_pdata != PNULL)
            {
                if(s_zyb_http_cur_pop_pdata->ip_str != PNULL)
                {
                    SCI_FREE(s_zyb_http_cur_pop_pdata->ip_str);
                }
                if(s_zyb_http_cur_pop_pdata->str != PNULL)
                {
                    SCI_FREE(s_zyb_http_cur_pop_pdata->str);
                }
                SCI_FREE(s_zyb_http_cur_pop_pdata);
            }
            s_zyb_http_is_poping_data = FALSE;
            if(http_link_coundt > 0)
            {
                MMIZYB_HTTP_PopSend();
            }
        }
    }
    return TRUE;
}

LOCAL BOOLEAN MMIZYB_HTTP_RcvCallBack(BOOLEAN is_ok,uint32 err_id)
{
    ZYB_HTTP_RCV_DATA_T rcvdata = {0};
    ZYB_HTTP_LOG("MMIZYB_HTTP_RcvCallBack is_ok = %d, err_id= %d",is_ok,err_id);
    if(ZYB_HTTP_RCV_GetAll(&rcvdata))
    {
        if(s_zyb_http_cur_pop_pdata != PNULL)
        {
            if(s_zyb_http_cur_pop_pdata->rcv_handle != PNULL)
            {
                s_zyb_http_cur_pop_pdata->rcv_handle(is_ok,rcvdata.pRcv,rcvdata.len,0,err_id);
            }
        }

        if(rcvdata.pRcv != PNULL)
        {
            SCI_FREE(rcvdata.pRcv);
        }
    }
    else
    {
        if(s_zyb_http_cur_pop_pdata != PNULL)
        {
            if(s_zyb_http_cur_pop_pdata->rcv_handle != PNULL)
            {
                s_zyb_http_cur_pop_pdata->rcv_handle(is_ok,PNULL,0,0,err_id);
            }
        }
    }
    return TRUE;
}


BOOLEAN  MMIZYB_HTTP_Handle_SendOver(DPARAM param)
{
    ZYB_HTTP_RSP_SIG_T * pp_getdata = (ZYB_HTTP_RSP_SIG_T *)param;
    ZYB_HTTP_RSP_T * p_getdata = &(pp_getdata->rsp);
    uint16 http_link_coundt = ZYB_HTTP_Link_Count();
    
    ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_SendOver is_sending = %d, is_sucess=%d,err_id=%d,need_stop=%d",s_zyb_http_is_poping_data,p_getdata->is_sucess,p_getdata->err_id,p_getdata->need_stop);
    ZYB_HTTP_SendTimer_Stop();
    if(p_getdata->need_stop)
    {
        //ZYBHTTP_Net_Close();
    }
    
    if(p_getdata->is_sucess)
    {
        //成功
        ZYB_HTTP_LOG("MMIZYB_HTTP_Handle_SendOver OK,is_poping =%d,p_getdata=0x%x",s_zyb_http_is_poping_data,s_zyb_http_cur_pop_pdata);
        if(s_zyb_http_is_poping_data)
        {
            MMIZYB_HTTP_RcvCallBack(TRUE,p_getdata->err_id);
            MMIZYB_HTTP_DelCurData();
            s_zyb_http_is_poping_data = FALSE;

            if(http_link_coundt > 0)
            {
                MMIZYB_HTTP_PopSend();
            }
        }
    }
    else
    {
        if(s_zyb_http_is_poping_data)
        {
            uint16 times = 0;
            uint32 timeout = 0;
            if(s_zyb_http_cur_pop_pdata != NULL)
            {
                if(p_getdata->need_stop || p_getdata->err_id == HTTP_ERROR_FILE_NO_SPACE || p_getdata->err_id == HTTP_ERROR_SERVER_CLOSE)
                {
                    times = 0;
                }
                else
                {
                    times = s_zyb_http_cur_pop_pdata->times;
                    if(s_zyb_http_cur_pop_pdata->times > 0)
                    {
                        s_zyb_http_cur_pop_pdata->times--;
                    }
                    timeout = s_zyb_http_cur_pop_pdata->timeout;
                }
            }
            
            if(times == 0)
            {
                MMIZYB_HTTP_RcvCallBack(FALSE,p_getdata->err_id);
                MMIZYB_HTTP_DelCurData();
                s_zyb_http_is_poping_data = FALSE;
                if(http_link_coundt > 0)
                {
                    MMIZYB_HTTP_PopSend();
                }
            }
            else
            {
                if(timeout < 200)
                {
                    ZYB_HTTP_ResendTimer_Start(200);
                }
                else
                {
                    ZYB_HTTP_ResendTimer_Start(timeout);
                }
            }
        }
        
    }
    return TRUE;
}

PUBLIC int  MMIZYB_HTTP_Test_CB(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 file_len,uint32 err_id)
{
    ZYB_HTTP_LOG("MMIZYB_HTTP_Test_CB(%d) file_len=%d,Rcv_len=%d,%s",is_ok,file_len,Rcv_len,pRcv);
    return 0;
}

PUBLIC BOOLEAN  MMIZYB_HTTP_Test(void)
{
    char send_buf[512] = {0};
    sprintf(send_buf,"https://iotcdn.cdnjtzy.com/iot-server/system/voice/2023/02/14/c4b1fa62da02412ea05502c45aace7a1887.mp3");
    //MMIZYB_HTTP_AppSend(TRUE,(char *)send_buf,(uint8 *)PNULL,0,1,0,0,0,0,0,0,MMIZYB_HTTP_Test_CB);
    ZYB_StreamPlayer_Start(send_buf);
    return TRUE;
}

PUBLIC BOOLEAN  MMIZYB_HTTP_TestPlay(void)
{
    char send_buf[512] = {0};
    //sprintf(send_buf,"https://iotcdn.cdnjtzy.com/iot-server/system/voice/2023/02/14/c4b1fa62da02412ea05502c45aace7a1887.mp3");
    sprintf(send_buf,"https://img.zuoyebang.cc/zyb-image/5a3ec309-4443-4d71-94eb-623a99562726.mp3");
    ZYB_StreamPlayer_Start(send_buf);
    return TRUE;
}

PUBLIC BOOLEAN  MMIZYB_HTTP_TestLongPlay(void)
{
    char send_buf[512] = {0};
    //sprintf(send_buf,"https://zyb-vip.cdnjtzy.com/vip-fighting/20210707/96c5a34bc4bb0db03cc0199a5036882f.mp3");
    sprintf(send_buf,"https://iotcdn.cdnjtzy.com/iot-server/system/voice/2023/04/21/bb1f8d4df3c841da9da8f1a270e341f3470.mp3");
    ZYB_StreamPlayer_Start(send_buf);
    return TRUE;
}

#endif
