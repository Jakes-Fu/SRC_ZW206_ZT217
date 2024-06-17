
#ifndef  _MMI_ZDTHTTP_API_C  
#define _MMI_ZDTHTTP_API_C  

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "zdthttp_app.h"
#include "zdthttp_api.h"
#include "zdt_net.h"
#include "cJSON.h"
#include "zdt_nv.h"
#include "mbedtls/md5.h"
#include "watch_common.h"

LOCAL BOOLEAN                  s_zdt_http_is_init = FALSE;
LOCAL uint8                       s_zdt_http_send_timer_id = 0;
LOCAL uint8                       s_zdt_http_re_send_timer_id = 0;

LOCAL BOOLEAN s_http_is_poping_data = FALSE;
LOCAL ZDT_HTTP_DATA_T *        s_http_cur_pop_pdata = PNULL;

LOCAL ZDT_HTTP_DATA_LINK_NODE_T  *    s_http_data_link_head = PNULL;
LOCAL ZDT_HTTP_RCV_NODE_T  * m_http_rcv_head = PNULL;

char         g_http_phone_imei[HTTP_IMEI_LEN+1] = {0};
char         g_http_phone_imei_2[HTTP_IMEI_LEN+1] = {0};
char         g_http_sim_imsi[HTTP_IMSI_LEN+1] = {0};
char         g_http_sim_iccid[HTTP_ICCID_LEN+1] = {0};

void MMIZDT_HTTP_PopSend_Result(BOOLEAN need_stop, uint32 error_id);
LOCAL BOOLEAN MMIZDT_HTTP_PopSend(void);

uint16  MMIHTTP_Net_GetIMEI(MN_DUAL_SYS_E dual_sys,char * imei_ptr)
{
	MN_IMEI_T imei;
	char tmp_buf1[20] = {0};
	uint8 i=0, j=0;

	MNNV_GetIMEIEx(dual_sys, imei);		

	for(i = 0, j = 0; i < 8; i++)
	{
		tmp_buf1[j++] = (imei[i] & 0x0F) + '0';
		tmp_buf1[j++] = ((imei[i] >> 4) & 0x0F) + '0';
	}

	for( i=0; i<HTTP_IMEI_LEN; i++ )
	{
		imei_ptr[i] = tmp_buf1[i+1];
	}
    
	imei_ptr[HTTP_IMEI_LEN] = 0x00;

	return HTTP_IMEI_LEN;
}

uint16  MMIHTTP_Get_SIM_IMSI(MN_DUAL_SYS_E dual_sys,char * imsi_ptr)
{
	uint8 i=0, j=0;
	MN_IMSI_T imsi = {0};
       char imsi_buf[HTTP_IMSI_LEN+1] = {0};
       
	imsi = MNSIM_GetImsiEx(dual_sys);

	if(imsi.imsi_val == NULL )
		return 0;

	for(i = 0, j = 0; i < 8; i++)
	{
		imsi_buf[j++] = (imsi.imsi_val[i] & 0x0F) + '0';
		imsi_buf[j++] = ((imsi.imsi_val[i] >> 4) & 0x0F) + '0';
	}
    
	for( i=0; i<HTTP_IMSI_LEN; i++ )
	{
		imsi_ptr[i] = imsi_buf[i+1];
	}
	imsi_ptr[HTTP_IMSI_LEN] = 0x00;
	
	return HTTP_IMSI_LEN;
}

uint16  MMIHTTP_Get_SIM_ICCID(MN_DUAL_SYS_E dual_sys,char * iccid_ptr)
{
    uint16 len = 0;
    ERR_MNDATAMAG_CODE_E err_code = 0;
    char iccid_buf[HTTP_ICCID_LEN+1] = {0};
    MNSIM_ICCID_T iccid_t = {0};

    err_code = MNSIM_GetICCIDEx(dual_sys,&iccid_t);
    ZDTHTTP_LOG("MMIZDT_Get_SIM_ICCID dual_sys =%d,err_code = %d,id_num=0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",dual_sys,err_code,iccid_t.id_num[9],iccid_t.id_num[8],iccid_t.id_num[7],iccid_t.id_num[6],iccid_t.id_num[5],iccid_t.id_num[4],iccid_t.id_num[3],iccid_t.id_num[2],iccid_t.id_num[1],iccid_t.id_num[0]);
    //MMIAPICOM_BcdToStr(PACKED_LSB_FIRST, iccid_t.id_num, HTTP_ICCID_LEN, iccid_buf);
    MMIAPICOM_BcdICCIDToStr(iccid_t.id_num, HTTP_ICCID_LEN, iccid_buf);
    len = strlen((char *)iccid_buf);
    ZDTHTTP_LOG("MMIZDT_Get_SIM_ICCID STR = %s",iccid_buf);
    SCI_MEMCPY(iccid_ptr,iccid_buf,len);
    
    return len;
}

uint16  MMIHTTP_Get_IMSI_Str(MN_IMSI_T imsi,char * imsi_ptr)
{
	uint8 i=0, j=0;
       char imsi_buf[HTTP_IMSI_LEN+1] = {0};
       
	for(i = 0, j = 0; i < 8; i++)
	{
		imsi_buf[j++] = (imsi.imsi_val[i] & 0x0F) + '0';
		imsi_buf[j++] = ((imsi.imsi_val[i] >> 4) & 0x0F) + '0';
	}
    
	for( i=0; i<HTTP_IMSI_LEN; i++ )
	{
		imsi_ptr[i] = imsi_buf[i+1];
	}
	imsi_ptr[HTTP_IMSI_LEN] = 0x00;
	
	return HTTP_IMSI_LEN;
}

/*
* 生成 api 验证的 sign 
* sign = md5(app_key + app_secret + timestamp)
* time_stamp 调用 GetCurrentTimeStampString() 去获取 #include "watch_common.h"
* 记得free 掉
*/
PUBLIC uint8 *makeSignString(uint8 *app_id,uint8 *app_secret,uint8 *time_stamp)
{
    mbedtls_md5_context md5_ctx = {0};
    uint8 digest[16] = {0};
    uint16 i = 0;
    uint8 sign[128] = {0};
    uint8 *md5 = (uint8 *)SCI_ALLOC_APPZ(33);
    sprintf(sign,"%s%s%s",app_id,app_secret,time_stamp);
#ifndef WIN32
    mbedtls_md5_init( &md5_ctx );
    mbedtls_md5_starts( &md5_ctx );
    mbedtls_md5_update( &md5_ctx, sign, strlen(sign));
    mbedtls_md5_finish( &md5_ctx, digest);
    mbedtls_md5_free( &md5_ctx );
#endif
   for(i = 0; i < 16; i++)
   {
        sprintf(md5+(i*2),"%02x",digest[i]);
   }
   ZDTHTTP_LOG("ZDT_HTTP makeSignString md5:%s",md5);
   return md5;
}

/*
* 生成带sign 要求的url 参数
* sign=xx&timestamp=xx&app_id=xxx&model=xx&SoftwareVersion=xx&deviceId=xxx
* 记得free
*/
PUBLIC uint8 *makeBaseQueryUrlString(uint8 *app_id,uint8 *app_secret)
{
    uint8 *query = (uint8 *)SCI_ALLOC_APPZ(256);
    uint8 *timestamp = GetCurrentTimeStampString();
    uint8 *sign = makeSignString(app_id,app_secret,timestamp);
    sprintf(query,"sign=%s&timestamp=%s&app_id=%s&model=%s&SoftwareVersion=%s&deviceId=%s",sign
        , timestamp, app_id, WATCHCOM_GetDeviceModel(),WATCHCOM_GetSoftwareVersion(),g_http_phone_imei);
    ZDTHTTP_LOG("ZDT_HTTP makeBaseQueryUrlString query:%s",query);
    SCI_Free(sign);
    SCI_Free(timestamp);
    return query;
}

/*
* 生成带sign 要求的header 参数
* sign:xx\r\ntimestamp=xx\r\napp_id:xxx\r\nmodel:xx\r\nSoftwareVersion:xx\r\ndeviceId:xxx
* 记得 free
*/
PUBLIC uint8 *makeHttpHeaderString(uint8 *app_id,uint8 *app_secret)
{
    uint8 *header = (uint8 *)SCI_ALLOC_APPZ(256);
    uint8 *timestamp = GetCurrentTimeStampString();
    uint8 *sign = makeSignString(app_id,app_secret,timestamp);   
    sprintf(header,"sign:%s\r\ntimestamp:%s\r\napp_id:%s\r\nmodel:%s\r\nSoftwareVersion:%s\r\ndeviceId:%s\r\n",sign
        , timestamp, app_id, WATCHCOM_GetDeviceModel(),WATCHCOM_GetSoftwareVersion(),g_http_phone_imei);
    SCI_Free(sign);
    SCI_Free(timestamp);
    return header;
}

PUBLIC BOOLEAN  MMIZDT_HTTP_SendSigTo_APP(ZDTHTTP_APP_SIG_E sig_id, ZDT_HTTP_DATA_T * p_http_data)
{
    uint8 * pstr = PNULL;
    ZDT_HTTP_SIG_T * psig = PNULL;

    //send signal to AT to write uart
    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,sig_id,sizeof(ZDT_HTTP_SIG_T),SCI_IdentifyThread());
    psig->p_http_data = p_http_data;
    
    SCI_SEND_SIGNAL((xSignalHeaderRec*)psig,P_APP);
    return TRUE;
}

LOCAL void ZDT_HTTP_SendTimer_Handle(
                                uint8 timer_id,
                                uint32 param
                                )
{
    MN_DUAL_SYS_E sys_sim = 0;
    BOOLEAN       sim_ok =  FALSE;

    if(timer_id == s_zdt_http_send_timer_id && 0 != s_zdt_http_send_timer_id)
    {
        MMK_StopTimer(s_zdt_http_send_timer_id);
        s_zdt_http_send_timer_id = 0;
    }
    MMIZDT_HTTP_PopSend_Result(TRUE, HTTP_ERROR_TIMEOUT);
}

PUBLIC void ZDT_HTTP_SendTimer_Start(uint32 time_ms)
{
    if(0 != s_zdt_http_send_timer_id)
    {
        MMK_StopTimer(s_zdt_http_send_timer_id);
        s_zdt_http_send_timer_id = 0;
    }
    s_zdt_http_send_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        ZDT_HTTP_SendTimer_Handle, 
                                                                        PNULL, 
                                                                        FALSE);
}

PUBLIC void ZDT_HTTP_SendTimer_Stop(void)
{
    if(0 != s_zdt_http_send_timer_id)
    {
        MMK_StopTimer(s_zdt_http_send_timer_id);
        s_zdt_http_send_timer_id = 0;
    }
}
LOCAL void ZDT_HTTP_ResendTimer_Handle(
                                uint8 timer_id,
                                uint32 param
                                )
{
    MN_DUAL_SYS_E sys_sim = 0;
    BOOLEAN       sim_ok =  FALSE;

    if(timer_id == s_zdt_http_re_send_timer_id && 0 != s_zdt_http_re_send_timer_id)
    {
        MMK_StopTimer(s_zdt_http_re_send_timer_id);
        s_zdt_http_re_send_timer_id = 0;
    }
    MMIZDT_HTTP_SendSigTo_APP(ZDTHTTP_APP_SIGNAL_HTTP_RESEND,PNULL);
}

PUBLIC void ZDT_HTTP_ResendTimer_Start(uint32 time_ms)
{
    if(0 != s_zdt_http_re_send_timer_id)
    {
        MMK_StopTimer(s_zdt_http_re_send_timer_id);
        s_zdt_http_re_send_timer_id = 0;
    }
    s_zdt_http_re_send_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        ZDT_HTTP_ResendTimer_Handle, 
                                                                        PNULL, 
                                                                        FALSE);
}

PUBLIC void ZDT_HTTP_ResendTimer_Stop(void)
{
    if(0 != s_zdt_http_re_send_timer_id)
    {
        MMK_StopTimer(s_zdt_http_re_send_timer_id);
        s_zdt_http_re_send_timer_id = 0;
    }
}


BOOLEAN ZDT_HTTP_RCV_Add(uint8 * pRcv, uint32 rcv_len)
{
    ZDT_HTTP_RCV_NODE_T  * p1 = NULL;
    ZDT_HTTP_RCV_NODE_T  * p2 = NULL;
    uint32 len = rcv_len;

    if(len == 0)
    {
        return FALSE;
    }
    
    p1=(ZDT_HTTP_RCV_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZDT_HTTP_RCV_NODE_T));
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

    if(m_http_rcv_head == NULL)
    {
        m_http_rcv_head = p1;
    }
    else
    {
        p2 = m_http_rcv_head;
        while(p2->next_ptr != NULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

BOOLEAN ZDT_HTTP_RCV_AddExt(uint8 * pRcv, uint32 rcv_len)
{
    ZDT_HTTP_RCV_NODE_T  * p1 = NULL;
    ZDT_HTTP_RCV_NODE_T  * p2 = NULL;
    uint32 len = rcv_len;

    if(len == 0)
    {
        return FALSE;
    }
    
    p1=(ZDT_HTTP_RCV_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZDT_HTTP_RCV_NODE_T));
    if(p1 == NULL)
    {
        return FALSE;
    }
    p1->data.pRcv = pRcv;
    p1->data.len = len;
    
    p1->next_ptr = NULL;

    if(m_http_rcv_head == NULL)
    {
        m_http_rcv_head = p1;
    }
    else
    {
        p2 = m_http_rcv_head;
        while(p2->next_ptr != NULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

BOOLEAN  ZDT_HTTP_RCV_Get(ZDT_HTTP_RCV_DATA_T * pUDPData)
{
    ZDT_HTTP_RCV_NODE_T  * p1 = NULL;
    
    p1 = m_http_rcv_head;
    
    if(p1 != NULL)
    {
      *pUDPData = p1->data;
        m_http_rcv_head = p1->next_ptr;
        SCI_FREE(p1);
        return TRUE;
    }
        
    return FALSE;
}

uint32 ZDT_HTTP_RCV_GetAll(ZDT_HTTP_RCV_DATA_T * pAllData)
{
    uint32 len = 0;
    uint32 cur_len = 0;
    ZDT_HTTP_RCV_DATA_T OneData = {0};
    ZDT_HTTP_RCV_NODE_T  * p1 = NULL;
    ZDT_HTTP_RCV_NODE_T  * p2 = NULL;
    
    if(pAllData == NULL)
    {
        return 0;
    }
    p2 = m_http_rcv_head;
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
        while(ZDT_HTTP_RCV_Get(&OneData) && OneData.pRcv != NULL)
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

BOOLEAN  ZDT_HTTP_RCV_DelAll(void)
{
    ZDT_HTTP_RCV_NODE_T  * p1 = NULL;
    
    while(m_http_rcv_head != NULL)
    {
        p1 = m_http_rcv_head;
        m_http_rcv_head = p1->next_ptr;
        if(p1->data.pRcv != 0)
        {
            SCI_FREE(p1->data.pRcv);
        }
        SCI_FREE(p1);
    }
    return TRUE;
}

//不 分配内存
LOCAL BOOLEAN ZDT_HTTP_Link_Add(ZDT_HTTP_DATA_T * p_data)
{
    ZDT_HTTP_DATA_LINK_NODE_T  * p1 = PNULL;
    ZDT_HTTP_DATA_LINK_NODE_T  * p2 = PNULL;
    
    if(p_data == PNULL 
        || p_data->ip_str == PNULL
        )
    {
        return FALSE;
    }
    
    p1=(ZDT_HTTP_DATA_LINK_NODE_T *)SCI_ALLOC_APPZ(sizeof(ZDT_HTTP_DATA_LINK_NODE_T));
    if(p1 == PNULL)
    {
        return FALSE;
    }
    
    p1->p_data = p_data;    
    p1->next_ptr = PNULL;
    
    if(s_http_data_link_head == PNULL)
    {
        s_http_data_link_head = p1;
    }
    else
    {
        p2 = s_http_data_link_head;
        while(p2->next_ptr != PNULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

//不释放内存
LOCAL BOOLEAN   ZDT_HTTP_Link_Get(ZDT_HTTP_DATA_T ** pp_data)
{
    ZDT_HTTP_DATA_LINK_NODE_T  * p1 = PNULL;
    ZDT_HTTP_DATA_LINK_NODE_T  * p2 = PNULL;
    BOOLEAN res = FALSE;
    
    p2 = p1 = s_http_data_link_head;
    
    while(p1 != PNULL)
    {
        if(p1->p_data != PNULL && p1->p_data->ip_str != PNULL)
        {
            *pp_data = p1->p_data;
            if(p1 == s_http_data_link_head)
            {
                s_http_data_link_head = p1->next_ptr;
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

LOCAL BOOLEAN  ZDT_HTTP_Link_GetByPri(ZDT_HTTP_DATA_T ** pPostData,uint8 priority)
{
    ZDT_HTTP_DATA_LINK_NODE_T  * p1 = NULL;
    ZDT_HTTP_DATA_LINK_NODE_T  * p2 = NULL;
    BOOLEAN res = FALSE;
    
    p2 = p1 = s_http_data_link_head;
    
    while(p1 != NULL)
    {
        if(p1->p_data != PNULL && p1->p_data->ip_str != PNULL && p1->p_data->priority == priority)
        {
            *pPostData = p1->p_data;
            
            if(p1 == s_http_data_link_head)
            {
                s_http_data_link_head = p1->next_ptr;
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

LOCAL BOOLEAN  ZDT_HTTP_Link_GetMaxPri(ZDT_HTTP_DATA_T ** pPostData)
{
    ZDT_HTTP_DATA_LINK_NODE_T  * p1 = NULL;
    ZDT_HTTP_DATA_LINK_NODE_T  * p2 = NULL;
    uint8 max_pri = 0;
    BOOLEAN res = FALSE;
    
    p2 = p1 = s_http_data_link_head;
    
    while(p1 != NULL)
    {
        if(p1->p_data != PNULL && p1->p_data->ip_str != PNULL && p1->p_data->priority > max_pri)
        {
            max_pri = p1->p_data->priority;
        }
        p2 = p1;
        p1 = p2->next_ptr;
    }
    
    res = ZDT_HTTP_Link_GetByPri(pPostData,max_pri);
    return res;
}

LOCAL BOOLEAN  ZDT_HTTP_Link_DelAll(void)
{
    ZDT_HTTP_DATA_LINK_NODE_T  * p1 = PNULL;
    while(s_http_data_link_head != PNULL)
    {
        p1 = s_http_data_link_head;
        s_http_data_link_head = p1->next_ptr;
        if(p1->p_data != PNULL)
        {
            if(p1->p_data->file_fullname != PNULL)
            {
                SCI_FREE(p1->p_data->file_fullname);
            }
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

LOCAL uint16  ZDT_HTTP_Link_Count(void)
{
    uint16 num = 0;
    ZDT_HTTP_DATA_LINK_NODE_T  * p1 = s_http_data_link_head;
    while(p1 != PNULL)
    {
        num++;
        p1 = p1->next_ptr;
    }
    return num;
}

LOCAL ZDT_HTTP_DATA_T *  ZDT_HTTP_Link_PopData(void)
{
    uint8 * pstr = PNULL;
    ZDT_HTTP_DATA_T * p_http_data = PNULL;
    BOOLEAN res = FALSE;
    
    res = ZDT_HTTP_Link_GetMaxPri(&p_http_data);
    
    if(res == FALSE || p_http_data == NULL)
    {
        ZDTHTTP_LOG("ZDT_HTTP_Link_PopData Empty !!!");
        return PNULL;
    }
    
    if(
        p_http_data->ip_str == PNULL
        )
    {
        if(p_http_data->file_fullname != PNULL)
        {
            SCI_FREE(p_http_data->file_fullname);
        }
        if(p_http_data->str != PNULL)
        {
            SCI_FREE(p_http_data->str);
        }
        if(p_http_data->ip_str != PNULL)
        {
            SCI_FREE(p_http_data->ip_str);
        }
        SCI_FREE(p_http_data);
        ZDTHTTP_LOG("ZDT_HTTP_Link_PopData ERR Len");
        return PNULL;
    }
    
    #if 1
        ZDTHTTP_LOG("ZDT_HTTP_Link_PopData OK p_http_data=0x%x,ip_str=%s,type=%d,str_len=%d",p_http_data,p_http_data->ip_str,p_http_data->type,p_http_data->str_len);
    #endif
   return p_http_data;
}

LOCAL uint8 * Brw_urlencode(const char *s, int len, int *new_length)
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

int MMIZDT_HTTP_GetlCurAllUrl(uint8 **ppStr)
{
    uint8 *all_str = PNULL;
    uint32 ip_len = 0;
    uint32 data_len = 0;
    uint32 all_len = 0;
    uint32 len = 0;
    int ret_len = 0;
    uint8 * p_utr = PNULL;

    if(s_http_is_poping_data)
    {
        if(s_http_cur_pop_pdata != PNULL)
        {
            if(s_http_cur_pop_pdata->ip_str != PNULL)
            {
                ip_len = strlen(s_http_cur_pop_pdata->ip_str);
            }
            if(s_http_cur_pop_pdata->str != PNULL && s_http_cur_pop_pdata->str_len > 0)
            {
                data_len = s_http_cur_pop_pdata->str_len;
            }
            all_len = ip_len + data_len;
            if(all_len > 0)
            {
                all_str = (uint8 *) SCI_ALLOCAZ(all_len+1);
                if(all_str != PNULL)
                {
                    if(ip_len > 0)
                    {
                        SCI_MEMCPY(all_str,s_http_cur_pop_pdata->ip_str,ip_len);
                        len += ip_len;
                    }
                    if(data_len > 0)
                    {
                        SCI_MEMCPY(all_str+len,s_http_cur_pop_pdata->str,data_len);
                        len += data_len;
                    }
                    p_utr = Brw_urlencode(all_str,len,&ret_len);
                    *ppStr = p_utr;
                    SCI_FREE(all_str);
                }
            }
        }
    }
    return ret_len;
}

int MMIZDT_HTTP_GetCurUrl(uint8 **ppStr)
{
    uint8 *all_str = PNULL;
    uint32 ip_len = 0;
    uint32 all_len = 0;
    uint32 len = 0;
    int ret_len = 0;
    uint8 * p_utr = PNULL;

    if(s_http_is_poping_data)
    {
        if(s_http_cur_pop_pdata != PNULL)
        {
            if(s_http_cur_pop_pdata->ip_str != PNULL)
            {
                ip_len = strlen(s_http_cur_pop_pdata->ip_str);
            }
            all_len = ip_len;
            if(all_len > 0)
            {
                all_str = (uint8 *) SCI_ALLOCAZ(all_len+1);
                if(all_str != PNULL)
                {
                    if(ip_len > 0)
                    {
                        SCI_MEMCPY(all_str,s_http_cur_pop_pdata->ip_str,ip_len);
                        len += ip_len;
                    }
                    p_utr = Brw_urlencode(all_str,len,&ret_len);
                    *ppStr = p_utr;
                    SCI_FREE(all_str);
                }
            }
        }
    }
    return ret_len;
}

uint32 MMIZDT_HTTP_GetCurData(uint8 **ppStr)
{
    if(s_http_cur_pop_pdata != PNULL)
    {
        *ppStr = s_http_cur_pop_pdata->str;
        return s_http_cur_pop_pdata->str_len;
    }
    else
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_GetCurData Err");
        return 0;
    }
}

uint16 MMIZDT_HTTP_GetCurType(void)
{
    uint16 type = 0;
    if(s_http_cur_pop_pdata != PNULL)
    {
        type = s_http_cur_pop_pdata->type;
    }
    ZDTHTTP_LOG("MMIZDT_HTTP_GetCurType type = %d",type);
    return type;
}

uint8 *  MMIZDT_HTTP_GetCurFileFullname(void)
{
    if(s_http_cur_pop_pdata != PNULL)
    {
        return s_http_cur_pop_pdata->file_fullname;
    }
    return PNULL;
}

uint8 MMIZDT_HTTP_GetCurFileStatus(void)
{
    if(s_http_cur_pop_pdata != PNULL)
    {
        if(s_http_cur_pop_pdata->file_fullname != PNULL)
        {
            if(s_http_cur_pop_pdata->file_fullname[0] == 'E')
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
    }
    return 0;
}

uint32  MMIZDT_HTTP_GetCurFileLen(void)
{
    if(s_http_cur_pop_pdata != PNULL)
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_GetCurFileLen = %d",s_http_cur_pop_pdata->file_len);
        return s_http_cur_pop_pdata->file_len;
    }
    ZDTHTTP_LOG("MMIZDT_HTTP_GetCurFileLen = 0");
    return 0;
}


LOCAL void MMIZDT_HTTP_DelCurData(void)
{
    if(s_http_cur_pop_pdata != PNULL)
    {
        if(s_http_cur_pop_pdata->ip_str != PNULL)
        {
            SCI_FREE(s_http_cur_pop_pdata->ip_str);
        }

        if(s_http_cur_pop_pdata->str != PNULL)
        {
            SCI_FREE(s_http_cur_pop_pdata->str);
        }
        if(s_http_cur_pop_pdata->file_fullname != PNULL)
        {
            SCI_FREE(s_http_cur_pop_pdata->file_fullname);
        }
        SCI_FREE(s_http_cur_pop_pdata);
        s_http_cur_pop_pdata = PNULL;
    }
    return;
}

BOOLEAN MMIZDT_HTTP_IsGetType(void)
{
    if(s_http_cur_pop_pdata != PNULL)
    {
        return s_http_cur_pop_pdata->is_get;
    }
    return TRUE;
}

BOOLEAN MMIZDT_HTTP_Init(void)
{
    if(s_zdt_http_is_init == FALSE)
    {
        MMIHTTP_Net_GetIMEI(MN_DUAL_SYS_1,g_http_phone_imei);
        MMIHTTP_Net_GetIMEI(MN_DUAL_SYS_2,g_http_phone_imei_2);

        s_zdt_http_is_init = TRUE;
    }
    return TRUE;
}

void MMIZDT_HTTP_PopSend_Result(BOOLEAN need_stop, uint32 error_id)
{
    uint8 * pstr = NULL;
    ZDT_HTTP_RSP_SIG_T * psig = PNULL;
    ZDTHTTP_LOG("MMIZDT_HTTP_PopSend_Result need_stop = %d, error_id = %d",need_stop,error_id);

    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,ZDTHTTP_APP_SIGNAL_HTTP_SEND_OVER,sizeof(ZDT_HTTP_RSP_SIG_T),SCI_IdentifyThread());
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

BOOLEAN MMIZDT_HTTP_Send_Start(char * url,MN_DUAL_SYS_E dual_sys)
{
    #if 0 //def WIN32
        MMIZDT_HTTP_PopSend_Result(FALSE,HTTP_SUCCESS);
        res = TRUE;
    #else
            MMIHTTP_Net_Open();
    #endif
    return TRUE;
}

LOCAL BOOLEAN MMIZDT_HTTP_PopSend(void)
{
    ZDT_HTTP_DATA_T * p_http_data = PNULL;
    BOOLEAN res = FALSE;
    int url_len = 0;
    if(s_http_is_poping_data)
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_PopSend ERR Busy is_poping_data=%d, init=%d, net=%d",s_http_is_poping_data,s_zdt_http_is_init,MMIHTTP_Net_IsInit());
        return FALSE;
    }
    ZDT_HTTP_ResendTimer_Stop();
    p_http_data = ZDT_HTTP_Link_PopData();
    if(p_http_data != PNULL)
    {
        s_http_is_poping_data = TRUE;
        s_http_cur_pop_pdata = p_http_data;
        ZDT_HTTP_RCV_DelAll();
        if(MMIZDT_HTTP_Send_Start(PNULL,MMIHTTP_Net_GetActiveSys()))
        {
            ZDT_HTTP_SendTimer_Start(20000);
        }
        else
        {
            ZDTHTTP_LOG("MMIZDT_HTTP_PopSend ERR Send Start");
            MMIZDT_HTTP_PopSend_Result(FALSE, HTTP_ERROR_FAILED);
#ifndef WIN32
            //SCI_HTTP_EnableDeepSleep(TRUE);
#endif
        }
    }
    return res;
}

PUBLIC BOOLEAN MMIZDT_HTTP_IsSending(void)
{
    return s_http_is_poping_data;
}

PUBLIC BOOLEAN MMIZDT_HTTP_AllowClose(void)
{
    if(s_http_is_poping_data == FALSE && ZDT_HTTP_Link_Count() == 0)
    {
        return TRUE;
    }
    return FALSE;
}

PUBLIC BOOLEAN MMIZDT_HTTP_Close(void)
{
    MMIHTTP_File_Close();
    if(s_http_is_poping_data == FALSE && ZDT_HTTP_Link_Count() == 0)
    {
        return TRUE;
    }
    return FALSE;
}

PUBLIC BOOLEAN MMIZDT_HTTP_AppSend(BOOLEAN is_get,char * ip_str,uint8 *str,uint32 str_len,uint16 type,uint8 priority,uint8 repeat_times,uint32 timeout,uint8 *file_fullname,uint32 file_len, HTTPRCVHANDLER rcv_handle)
{
    ZDT_HTTP_DATA_T * p_http_data = PNULL;
    BOOLEAN res = FALSE;
    uint32 ip_addr = 0;
    int ip_err;
    uint16 ip_len = 0;
    uint16 name_len = 0;

    if(s_zdt_http_is_init == FALSE)
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_AppSend ERR NOT INIT");
        return FALSE;
    }
    
    #if 0
    if(MMIHTTP_Net_IsInit() == FALSE)
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_AppSend ERR No Net");
        return FALSE;
    }
    #endif
    
    if((str == PNULL || str_len == 0) && ip_str == PNULL)
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_AppSend ERR NO Data");
        return FALSE;
    }
    
    p_http_data = SCI_ALLOC_APPZ(sizeof(ZDT_HTTP_DATA_T));
    if (p_http_data == PNULL)
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_AppSend ERR ALLOC");
        return FALSE;
    }

    if(file_fullname != PNULL)
    {
        name_len = SCI_STRLEN(file_fullname);
        p_http_data->file_fullname = SCI_ALLOC_APPZ(name_len+1);//free it in AT task
        if (p_http_data->file_fullname == PNULL)
        {
            ZDTHTTP_LOG("MMIZDT_HTTP_AppSend ERR ALLOC 1");
            SCI_FREE(p_http_data);
            return FALSE;
        }
        SCI_MEMCPY(p_http_data->file_fullname,file_fullname,name_len);
    }
    
    p_http_data->is_get = is_get;
    if(str != PNULL && str_len > 0)
    {
        str_len = strlen(str);
        p_http_data->str = SCI_ALLOC_APPZ(str_len+1);//free it in AT task
        if (p_http_data->str == PNULL)
        {
            ZDTHTTP_LOG("MMIZDT_HTTP_AppSend ERR ALLOC 2");
            if(p_http_data->file_fullname != PNULL)
            {
                SCI_FREE(p_http_data->file_fullname);
            }
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
            ZDTHTTP_LOG("MMIZDT_HTTP_AppSend ERR ALLOC 3");
            if(p_http_data->file_fullname != PNULL)
            {
                SCI_FREE(p_http_data->file_fullname);
            }
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

    res = MMIZDT_HTTP_SendSigTo_APP(ZDTHTTP_APP_SIGNAL_HTTP_SEND,p_http_data);
    if(res == FALSE)
    {
        if(p_http_data->file_fullname != PNULL)
        {
            SCI_FREE(p_http_data->file_fullname);
        }
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

BOOLEAN  MMIZDT_HTTP_Handle_Send(DPARAM param)
{
    BOOLEAN res = FALSE;
    ZDT_HTTP_SIG_T * pp_getdata = (ZDT_HTTP_SIG_T *)(param);
    ZDT_HTTP_DATA_T * p_getdata = pp_getdata->p_http_data;
    if(p_getdata == PNULL)
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_Handle_Send ERR NULL DATA");
        return FALSE;
    }
    
    res = ZDT_HTTP_Link_Add(p_getdata);
    if(res == FALSE)
    {
        if(p_getdata->file_fullname != NULL)
        {
            SCI_FREE(p_getdata->file_fullname);
        }
        if(p_getdata->ip_str != NULL)
        {
            SCI_FREE(p_getdata->ip_str);
        }
        if(p_getdata->str != NULL)
        {
            SCI_FREE(p_getdata->str);
        }
        SCI_FREE(p_getdata);
        ZDTHTTP_LOG("MMIZDT_HTTP_Handle_Send ERR LINK ADD");
    }
        
    ZDTHTTP_LOG("MMIZDT_HTTP_Handle_Send is_poping_data=%d",s_http_is_poping_data);
    if(s_http_is_poping_data == FALSE)
    {
        MMIZDT_HTTP_PopSend();
    }

    return TRUE;
}
BOOLEAN  MMIZDT_HTTP_Handle_ReSend(DPARAM param)
{
    BOOLEAN res = FALSE;
    char * url_ptr = PNULL;
    int url_len = 0;
    
    ZDT_HTTP_ResendTimer_Stop();
    if(s_http_cur_pop_pdata != NULL)
    {
        s_http_is_poping_data = TRUE;
        ZDT_HTTP_RCV_DelAll();
        if(MMIZDT_HTTP_Send_Start(url_ptr,MMIHTTP_Net_GetActiveSys()))
        {
            ZDT_HTTP_SendTimer_Start(20000);
        }
        else
        {
            ZDTHTTP_LOG("MMIZDT_HTTP_Handle_ReSend ERR Send Start");
            MMIZDT_HTTP_PopSend_Result(FALSE, HTTP_ERROR_FAILED);
        }
    }
    else
    {
        MMIZDT_HTTP_PopSend_Result(FALSE, HTTP_ERROR_NO_MEMORY);
    }
    return TRUE;
}

BOOLEAN  MMIZDT_HTTP_Handle_Rcv(DPARAM param)
{
    BOOLEAN res = FALSE;
    ZDT_HTTP_SIG_T * pp_getdata = (ZDT_HTTP_SIG_T *)(param);
    ZDT_HTTP_DATA_T * p_http_data = pp_getdata->p_http_data;
    
    if(p_http_data != PNULL)
    {
        ZDTHTTP_LOG("MMIZDT_HTTP_Handle_Rcv p_http_data=0x%x,ip_str=%s,type=%d,str_len=%d",p_http_data,p_http_data->ip_str,p_http_data->type,p_http_data->str_len);
        //Trace_Log_Buf_Data((char *)p_http_data->str,p_http_data->str_len);
        //Trace_Log_Buf_Data("\r\n",2);
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

BOOLEAN  MMIZDT_HTTP_Handle_SendSuccess(DPARAM param)
{
    ZDT_HTTP_RSP_T * p_getdata = (ZDT_HTTP_RSP_T *)param;
    uint16 http_link_coundt = ZDT_HTTP_Link_Count();
    
    ZDTHTTP_LOG("MMIZDT_HTTP_Handle_SendOver is_sending = %d, is_sucess=%d,err_id=%d",s_http_is_poping_data,p_getdata->is_sucess,p_getdata->err_id);
    
    if(p_getdata->need_stop)
    {
        MMIHTTP_Net_Close();
    }
    
    if(p_getdata->is_sucess)
    {
        //成功
        ZDTHTTP_LOG("MMIZDT_HTTP_Handle_SendOver OK,is_poping =%d,p_getdata=0x%x",s_http_is_poping_data,s_http_cur_pop_pdata);
        if(s_http_is_poping_data)
        {
            if(s_http_cur_pop_pdata != PNULL)
            {
                if(s_http_cur_pop_pdata->ip_str != PNULL)
                {
                    SCI_FREE(s_http_cur_pop_pdata->ip_str);
                }

                if(s_http_cur_pop_pdata->str != PNULL)
                {
                    SCI_FREE(s_http_cur_pop_pdata->str);
                }
                SCI_FREE(s_http_cur_pop_pdata);
            }
            s_http_is_poping_data = FALSE;
            if(http_link_coundt > 0)
            {
                MMIZDT_HTTP_PopSend();
            }
        }

    }
    else
    {
        //失败
        ZDTHTTP_LOG("MMIZDT_HTTP_Handle_SendOver FAIL, is_poping =%d,p_getdata=0x%x",s_http_is_poping_data,s_http_cur_pop_pdata);
        if(s_http_is_poping_data)
        {
            if(s_http_cur_pop_pdata != PNULL)
            {
                if(s_http_cur_pop_pdata->ip_str != PNULL)
                {
                    SCI_FREE(s_http_cur_pop_pdata->ip_str);
                }

                if(s_http_cur_pop_pdata->str != PNULL)
                {
                    SCI_FREE(s_http_cur_pop_pdata->str);
                }
                SCI_FREE(s_http_cur_pop_pdata);
            }
            s_http_is_poping_data = FALSE;
            if(http_link_coundt > 0)
            {
                MMIZDT_HTTP_PopSend();
            }
        }
    }

    #ifndef WIN32
        //SCI_HTTP_EnableDeepSleep(TRUE);
    #endif

    return TRUE;
}

BOOLEAN  MMIZDT_HTTP_Handle_SendFail(DPARAM param)
{
    ZDT_HTTP_RSP_T * p_getdata = (ZDT_HTTP_RSP_T *)param;
    uint16 http_link_coundt = ZDT_HTTP_Link_Count();
    
    ZDTHTTP_LOG("MMIZDT_HTTP_Handle_SendOver is_sending = %d, is_sucess=%d,err_id=%d",s_http_is_poping_data,p_getdata->is_sucess,p_getdata->err_id);
    
    if(p_getdata->need_stop)
    {
        //ZDT_HTTP_Browser_Stop();
    }
    
    if(p_getdata->is_sucess)
    {
        //成功
        ZDTHTTP_LOG("MMIZDT_HTTP_Handle_SendOver OK,is_poping =%d,p_getdata=0x%x",s_http_is_poping_data,s_http_cur_pop_pdata);
        if(s_http_is_poping_data)
        {
            if(s_http_cur_pop_pdata != PNULL)
            {
                if(s_http_cur_pop_pdata->ip_str != PNULL)
                {
                    SCI_FREE(s_http_cur_pop_pdata->ip_str);
                }

                if(s_http_cur_pop_pdata->str != PNULL)
                {
                    SCI_FREE(s_http_cur_pop_pdata->str);
                }
                SCI_FREE(s_http_cur_pop_pdata);
            }
            s_http_is_poping_data = FALSE;
            if(http_link_coundt > 0)
            {
                MMIZDT_HTTP_PopSend();
            }
        }

    }
    else
    {
        //失败
        ZDTHTTP_LOG("MMIZDT_HTTP_Handle_SendOver FAIL, is_poping =%d,p_getdata=0x%x",s_http_is_poping_data,s_http_cur_pop_pdata);
        if(s_http_is_poping_data)
        {
            if(s_http_cur_pop_pdata != PNULL)
            {
                if(s_http_cur_pop_pdata->ip_str != PNULL)
                {
                    SCI_FREE(s_http_cur_pop_pdata->ip_str);
                }

                if(s_http_cur_pop_pdata->str != PNULL)
                {
                    SCI_FREE(s_http_cur_pop_pdata->str);
                }
                SCI_FREE(s_http_cur_pop_pdata);
            }
            s_http_is_poping_data = FALSE;
            if(http_link_coundt > 0)
            {
                MMIZDT_HTTP_PopSend();
            }
        }
    }

    #ifndef WIN32
        //SCI_HTTP_EnableDeepSleep(TRUE);
    #endif

    return TRUE;
}

LOCAL BOOLEAN MMIZDT_HTTP_RcvCallBack(BOOLEAN is_ok,uint32 err_id)
{
    ZDT_HTTP_RCV_DATA_T rcvdata = {0};
    ZDTHTTP_LOG("MMIZDT_HTTP_RcvCallBack is_ok = %d, err_id= %d",is_ok,err_id);
    if(ZDT_HTTP_RCV_GetAll(&rcvdata))
    {
        if(s_http_cur_pop_pdata != PNULL)
        {
            if(s_http_cur_pop_pdata->rcv_handle != PNULL)
            {
                s_http_cur_pop_pdata->rcv_handle(is_ok,rcvdata.pRcv,rcvdata.len,err_id);
            }
        }

        if(rcvdata.pRcv != PNULL)
        {
            SCI_FREE(rcvdata.pRcv);
        }
    }
    else
    {
        if(s_http_cur_pop_pdata != PNULL)
        {
            if(s_http_cur_pop_pdata->rcv_handle != PNULL)
            {
                s_http_cur_pop_pdata->rcv_handle(is_ok,PNULL,0,err_id);
            }
        }
    }
    return TRUE;
}


BOOLEAN  MMIZDT_HTTP_Handle_SendOver(DPARAM param)
{
    ZDT_HTTP_RSP_SIG_T * pp_getdata = (ZDT_HTTP_RSP_SIG_T *)param;
    ZDT_HTTP_RSP_T * p_getdata = &(pp_getdata->rsp);
    uint16 http_link_coundt = ZDT_HTTP_Link_Count();
    
    ZDTHTTP_LOG("MMIZDT_HTTP_Handle_SendOver is_sending = %d, is_sucess=%d,err_id=%d,need_stop=%d",s_http_is_poping_data,p_getdata->is_sucess,p_getdata->err_id,p_getdata->need_stop);
    ZDT_HTTP_SendTimer_Stop();
    if(p_getdata->need_stop)
    {
        //MMIHTTP_Net_Close();
    }
    
    //ZDT_File_Remove("D:\\http.txt");
    //ZDT_File_RemoveDirFile("D:\\http");
    if(p_getdata->is_sucess)
    {
        //成功
        ZDTHTTP_LOG("MMIZDT_HTTP_Handle_SendOver OK,is_poping =%d,p_getdata=0x%x",s_http_is_poping_data,s_http_cur_pop_pdata);
        if(s_http_is_poping_data)
        {
            MMIZDT_HTTP_RcvCallBack(TRUE,p_getdata->err_id);
            MMIZDT_HTTP_DelCurData();
            s_http_is_poping_data = FALSE;
#ifndef WIN32
            //SCI_HTTP_EnableDeepSleep(TRUE);
#endif

            if(http_link_coundt > 0)
            {
                MMIZDT_HTTP_PopSend();
            }
        }
    }
    else
    {
        if(s_http_is_poping_data)
        {
            uint16 times = 0;
            uint32 timeout = 0;
            if(s_http_cur_pop_pdata != NULL)
            {
                if(p_getdata->need_stop || p_getdata->err_id == HTTP_ERROR_FILE_NO_SPACE)
                {
                    times = 0;
                }
                else
                {
                    times = s_http_cur_pop_pdata->times;
                    if(s_http_cur_pop_pdata->times > 0)
                    {
                        s_http_cur_pop_pdata->times--;
                    }
                    timeout = s_http_cur_pop_pdata->timeout;
                }
            }
            
            if(times == 0)
            {
                MMIZDT_HTTP_RcvCallBack(FALSE,p_getdata->err_id);
                MMIZDT_HTTP_DelCurData();
                s_http_is_poping_data = FALSE;
#ifndef WIN32
                //SCI_HTTP_EnableDeepSleep(TRUE);
#endif
                if(http_link_coundt > 0)
                {
                    MMIZDT_HTTP_PopSend();
                }
            }
            else
            {
                if(timeout < 200)
                {
                    ZDT_HTTP_ResendTimer_Start(200);
                }
                else
                {
                    ZDT_HTTP_ResendTimer_Start(timeout);
                }
            }
        }
        
    }
    return TRUE;
}

#if 0
PUBLIC int  MMIZDT_HTTP_Test_CB(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    ZDTHTTP_LOG("MMIZDT_HTTP_Test_CB(%d) Rcv_len=%d,%s",is_ok,Rcv_len,pRcv);
    return 0;
}

PUBLIC BOOLEAN  MMIZDT_HTTP_Test(void)
{
    static uint32 s_send_idx = 0;
    char send_buf[512] = {0};
    uint16 len = strlen(send_buf);
    s_send_idx++;
    if(s_send_idx > 99999)
    {
        s_send_idx = 0;
    }
    //http://mob.3gcare.cn/s?actType=ZTE2200&reqContent=10005|V95|89860311800250914398|A100001491D5D6
    sprintf(send_buf,"actType=ZTE2200&reqContent=%05d|AngelCare.K288.YUN.T1.00|%s|%s",s_send_idx,g_http_sim_iccid,g_http_phone_imei);
    len = strlen(send_buf);
    MMIZDT_HTTP_AppSend(TRUE,(char *)"http://oldman-care.jd.com/s?",(uint8 *)send_buf,len,1,0,0,0,MMIZDT_HTTP_Test_CB);
    return TRUE;
}
#endif

#ifdef ZDT_CYHEALTH_SUPPORT

static uint8 url_send_mask = 0;
int ZDT_Http_Get_Url_File_CB_Health(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Health(%d) Rcv_len=%d",is_ok,Rcv_len);
    url_send_mask &= ~(0x01); 
    if(is_ok)
    {
        //保存成功
        ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Health HTTP complete!");
        if(url_send_mask == 0)
        {
            MMIZDT_UpdateHealthWin(2);
        }
    }
    else
    {
        if(err_id == HTTP_ERROR_FILE_NO_SPACE)
        {
            //空间不足保存失败
            ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Health HTTP_ERROR_FILE_NO_SPACE!");
	 	}
        else
        {
             //其它错误
              ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Health HTTP other error!  err_id=%d",err_id);
        }
    }
    return 0;
}

BOOLEAN ZDT_Http_Get_Url_Pic_Health(char * url)
{
    BOOLEAN bResult = TRUE;
    uint32 fullname_len = 0;
    fullname_len = SCI_STRLEN(ZDT_HTTP_HEALTH_URL_FULLPACH);
    bResult = MMIZDT_HTTP_AppSend(TRUE,url,PNULL,0,101,0,1,0,ZDT_HTTP_HEALTH_URL_FULLPACH,fullname_len,ZDT_Http_Get_Url_File_CB_Health);
    if(bResult)
    {
        url_send_mask |= 0x01;
    }
    else
    {
        url_send_mask &= ~(0x01); 
    }
    return bResult;
}

int ZDT_Http_Get_Url_File_CB_Nucleic(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Nucleic(%d) Rcv_len=%d",is_ok,Rcv_len);
    url_send_mask &= ~(0x02); 
    if(is_ok)
    {
        //保存成功
        ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Nucleic HTTP complete!");
        if(url_send_mask == 0)
        {
            MMIZDT_UpdateHealthWin(2);
        }
    }
    else
    {
        if(err_id == HTTP_ERROR_FILE_NO_SPACE)
        {
            //空间不足保存失败
            ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Nucleic HTTP_ERROR_FILE_NO_SPACE!");
	 	}
        else
        {
             //其它错误
              ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Nucleic HTTP other error!  err_id=%d",err_id);
        }
    }
    return 0;
}

BOOLEAN ZDT_Http_Get_Url_Pic_Nucleic(char * url)
{
    BOOLEAN bResult = TRUE;
    uint32 fullname_len = 0;
    fullname_len = SCI_STRLEN(ZDT_HTTP_NUCLEIC_URL_FULLPACH);
    bResult = MMIZDT_HTTP_AppSend(TRUE,url,PNULL,0,101,0,1,0,ZDT_HTTP_NUCLEIC_URL_FULLPACH,fullname_len,ZDT_Http_Get_Url_File_CB_Nucleic);
    if(bResult)
    {
        url_send_mask |= 0x02;
    }
    else
    {
        url_send_mask &= ~(0x02); 
    }
    return bResult;
}

int ZDT_Http_Get_Url_File_CB_Acc(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Acc(%d) Rcv_len=%d",is_ok,Rcv_len);
    url_send_mask &= ~(0x04); 
    if(is_ok)
    {
        //保存成功
        ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Acc HTTP complete!");
        if(url_send_mask == 0)
        {
            MMIZDT_UpdateHealthWin(2);
        }
    }
    else
    {
        if(err_id == HTTP_ERROR_FILE_NO_SPACE)
        {
            //空间不足保存失败
            ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Acc HTTP_ERROR_FILE_NO_SPACE!");
	 	}
        else
        {
             //其它错误
              ZDTHTTP_LOG("ZDT_Http_Get_Url_File_CB_Acc HTTP other error!  err_id=%d",err_id);
        }
    }
    return 0;
}

BOOLEAN ZDT_Http_Get_Url_Pic_Acc(char * url)
{
    BOOLEAN bResult = TRUE;
    uint32 fullname_len = 0;
    fullname_len = SCI_STRLEN(ZDT_HTTP_ACC_URL_FULLPACH);
    bResult = MMIZDT_HTTP_AppSend(TRUE,url,PNULL,0,101,0,1,0,ZDT_HTTP_ACC_URL_FULLPACH,fullname_len,ZDT_Http_Get_Url_File_CB_Acc);
    if(bResult)
    {
        url_send_mask |= 0x04;
    }
    else
    {
        url_send_mask &= ~(0x04); 
    }
    return bResult;
}

BOOLEAN ZDT_Http_Delall_Url_Pic(void)
{
    ZDT_HTTP_File_Remove(ZDT_HTTP_HEALTH_URL_FULLPACH);
    ZDT_HTTP_File_Remove(ZDT_HTTP_NUCLEIC_URL_FULLPACH);
    ZDT_HTTP_File_Remove(ZDT_HTTP_ACC_URL_FULLPACH);
    return TRUE;
}

#if 1
static uint8 type1_send_mask = 0;
int ZDT_Http_Get_Type1_File_CB_Health(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Health(%d) Rcv_len=%d",is_ok,Rcv_len);
    type1_send_mask &= ~(0x01); 
    if(is_ok)
    {
        //保存成功
        ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Health HTTP complete!");
        if(type1_send_mask == 0)
        {
            MMIZDT_UpdateHealthWin(1);
        }
    }
    else
    {
        if(err_id == HTTP_ERROR_FILE_NO_SPACE)
        {
            //空间不足保存失败
            ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Health HTTP_ERROR_FILE_NO_SPACE!");
	 	}
        else
        {
             //其它错误
              ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Health HTTP other error!  err_id=%d",err_id);
        }
    }
    return 0;
}

BOOLEAN ZDT_Http_Get_Type1_Pic_Health(char * type1)
{
    BOOLEAN bResult = TRUE;
    uint32 fullname_len = 0;
    fullname_len = SCI_STRLEN(ZDT_HTTP_HEALTH_TYPE1_FULLPACH);
    bResult = MMIZDT_HTTP_AppSend(TRUE,type1,PNULL,0,101,0,1,0,ZDT_HTTP_HEALTH_TYPE1_FULLPACH,fullname_len,ZDT_Http_Get_Type1_File_CB_Health);
    if(bResult)
    {
        type1_send_mask |= 0x01;
    }
    else
    {
        type1_send_mask &= ~(0x01); 
    }
    return bResult;
}

int ZDT_Http_Get_Type1_File_CB_Nucleic(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Nucleic(%d) Rcv_len=%d",is_ok,Rcv_len);
    type1_send_mask &= ~(0x02); 
    if(is_ok)
    {
        //保存成功
        ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Nucleic HTTP complete!");
        if(type1_send_mask == 0)
        {
            MMIZDT_UpdateHealthWin(1);
        }
    }
    else
    {
        if(err_id == HTTP_ERROR_FILE_NO_SPACE)
        {
            //空间不足保存失败
            ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Nucleic HTTP_ERROR_FILE_NO_SPACE!");
	 	}
        else
        {
             //其它错误
              ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Nucleic HTTP other error!  err_id=%d",err_id);
        }
    }
    return 0;
}

BOOLEAN ZDT_Http_Get_Type1_Pic_Nucleic(char * type1)
{
    BOOLEAN bResult = TRUE;
    uint32 fullname_len = 0;
    fullname_len = SCI_STRLEN(ZDT_HTTP_NUCLEIC_TYPE1_FULLPACH);
    bResult = MMIZDT_HTTP_AppSend(TRUE,type1,PNULL,0,101,0,1,0,ZDT_HTTP_NUCLEIC_TYPE1_FULLPACH,fullname_len,ZDT_Http_Get_Type1_File_CB_Nucleic);
    if(bResult)
    {
        type1_send_mask |= 0x02;
    }
    else
    {
        type1_send_mask &= ~(0x02); 
    }
    return bResult;
}

int ZDT_Http_Get_Type1_File_CB_Acc(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Acc(%d) Rcv_len=%d",is_ok,Rcv_len);
    type1_send_mask &= ~(0x04); 
    if(is_ok)
    {
        //保存成功
        ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Acc HTTP complete!");
        if(type1_send_mask == 0)
        {
            MMIZDT_UpdateHealthWin(1);
        }
    }
    else
    {
        if(err_id == HTTP_ERROR_FILE_NO_SPACE)
        {
            //空间不足保存失败
            ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Acc HTTP_ERROR_FILE_NO_SPACE!");
	 	}
        else
        {
             //其它错误
              ZDTHTTP_LOG("ZDT_Http_Get_Type1_File_CB_Acc HTTP other error!  err_id=%d",err_id);
        }
    }
    return 0;
}

BOOLEAN ZDT_Http_Get_Type1_Pic_Acc(char * type1)
{
    BOOLEAN bResult = TRUE;
    uint32 fullname_len = 0;
    fullname_len = SCI_STRLEN(ZDT_HTTP_ACC_TYPE1_FULLPACH);
    bResult = MMIZDT_HTTP_AppSend(TRUE,type1,PNULL,0,101,0,1,0,ZDT_HTTP_ACC_TYPE1_FULLPACH,fullname_len,ZDT_Http_Get_Type1_File_CB_Acc);
    if(bResult)
    {
        type1_send_mask |= 0x04;
    }
    else
    {
        type1_send_mask &= ~(0x04); 
    }
    return bResult;
}

BOOLEAN ZDT_Http_Delall_Type1_Pic(void)
{
    ZDT_HTTP_File_Remove(ZDT_HTTP_HEALTH_TYPE1_FULLPACH);
    ZDT_HTTP_File_Remove(ZDT_HTTP_NUCLEIC_TYPE1_FULLPACH);
    ZDT_HTTP_File_Remove(ZDT_HTTP_ACC_TYPE1_FULLPACH);
    return TRUE;
}

#endif

BOOLEAN ZDT_Http_Type2_Base64_Save(uint8  * data_ptr, uint32 data_len)
{
    ZDT_HTTP_File_Remove(ZDT_HTTP_HEALTH_URL_BASE64_FULLPACH);
    return ZDT_HTTP_File_Save(ZDT_HTTP_HEALTH_URL_BASE64_FULLPACH,data_ptr,data_len,FALSE);
}

#endif


/*****************************************************************************/
/* Defines:                                                                  */
/*****************************************************************************/
// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4
// The number of 32 bit words in a key.
#define Nk 4
// Key length in bytes [128 bit]
#define KEYLEN 16
// The number of rounds in AES Cipher.
#define Nr 10

// jcallan@github points out that declaring Multiply as a function 
// reduces code size considerably with the Keil ARM compiler.
// See this link for more information: https://github.com/kokke/tiny-AES128-C/pull/3
#ifndef MULTIPLY_AS_A_FUNCTION
  #define MULTIPLY_AS_A_FUNCTION 0
#endif


/*****************************************************************************/
/* Private variables:                                                        */
/*****************************************************************************/
// state - array holding the intermediate results during decryption.
typedef uint8 state_t[4][4];
LOCAL state_t* state;

// The array that stores the round keys.
LOCAL uint8 RoundKey[176];

// The Key input to the AES Program
LOCAL const uint8* Key;

  // Initial Vector used only for CBC mode
LOCAL uint8* Iv;

// The lookup-tables are marked const so they can be placed in read-only storage instead of RAM
// The numbers below can be computed dynamically trading ROM for RAM - 
// This can be useful in (embedded) bootloader applications, where ROM is often limited.
LOCAL const uint8 sbox[256] =   {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

LOCAL const uint8 rsbox[256] =
{ 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };


// The round constant word array, Rcon[i], contains the values given by 
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
// Note that i starts at 1, not 0).
LOCAL const uint8 Rcon[255] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 
  0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 
  0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 
  0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 
  0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 
  0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 
  0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 
  0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 
  0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 
  0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 
  0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 
  0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 
  0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 
  0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 
  0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 
  0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb  };


/*****************************************************************************/
/* Private functions:                                                        */
/*****************************************************************************/
LOCAL uint8 getSBoxValue(uint8 num)
{
  return sbox[num];
}

LOCAL uint8 getSBoxInvert(uint8 num)
{
  return rsbox[num];
}

// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states. 
LOCAL void KeyExpansion(void)
{
  uint32 i, j, k;
  uint8 tempa[4]; // Used for the column/row operations
  
  // The first round key is the key itself.
  for(i = 0; i < Nk; ++i)
  {
    RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
    RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
    RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
    RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
  }

  // All other round keys are found from the previous round keys.
  for(; (i < (Nb * (Nr + 1))); ++i)
  {
    for(j = 0; j < 4; ++j)
    {
      tempa[j]=RoundKey[(i-1) * 4 + j];
    }
    if (i % Nk == 0)
    {
      // This function rotates the 4 bytes in a word to the left once.
      // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

      // Function RotWord()
      {
        k = tempa[0];
        tempa[0] = tempa[1];
        tempa[1] = tempa[2];
        tempa[2] = tempa[3];
        tempa[3] = k;
      }

      // SubWord() is a function that takes a four-byte input word and 
      // applies the S-box to each of the four bytes to produce an output word.

      // Function Subword()
      {
        tempa[0] = getSBoxValue(tempa[0]);
        tempa[1] = getSBoxValue(tempa[1]);
        tempa[2] = getSBoxValue(tempa[2]);
        tempa[3] = getSBoxValue(tempa[3]);
      }

      tempa[0] =  tempa[0] ^ Rcon[i/Nk];
    }
    else if (Nk > 6 && i % Nk == 4)
    {
      // Function Subword()
      {
        tempa[0] = getSBoxValue(tempa[0]);
        tempa[1] = getSBoxValue(tempa[1]);
        tempa[2] = getSBoxValue(tempa[2]);
        tempa[3] = getSBoxValue(tempa[3]);
      }
    }
    RoundKey[i * 4 + 0] = RoundKey[(i - Nk) * 4 + 0] ^ tempa[0];
    RoundKey[i * 4 + 1] = RoundKey[(i - Nk) * 4 + 1] ^ tempa[1];
    RoundKey[i * 4 + 2] = RoundKey[(i - Nk) * 4 + 2] ^ tempa[2];
    RoundKey[i * 4 + 3] = RoundKey[(i - Nk) * 4 + 3] ^ tempa[3];
  }
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
LOCAL void AddRoundKey(uint8 round)
{
  uint8 i,j;
  for(i=0;i<4;++i)
  {
    for(j = 0; j < 4; ++j)
    {
      (*state)[i][j] ^= RoundKey[round * Nb * 4 + i * Nb + j];
    }
  }
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
LOCAL void SubBytes(void)
{
  uint8 i, j;
  for(i = 0; i < 4; ++i)
  {
    for(j = 0; j < 4; ++j)
    {
      (*state)[j][i] = getSBoxValue((*state)[j][i]);
    }
  }
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
LOCAL void ShiftRows(void)
{
  uint8 temp;

  // Rotate first row 1 columns to left  
  temp           = (*state)[0][1];
  (*state)[0][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[3][1];
  (*state)[3][1] = temp;

  // Rotate second row 2 columns to left  
  temp           = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;

  temp       = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;

  // Rotate third row 3 columns to left
  temp       = (*state)[0][3];
  (*state)[0][3] = (*state)[3][3];
  (*state)[3][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[1][3];
  (*state)[1][3] = temp;
}

LOCAL uint8 xtime(uint8 x)
{
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

// MixColumns function mixes the columns of the state matrix
LOCAL void MixColumns(void)
{
  uint8 i;
  uint8 Tmp,Tm,t;
  for(i = 0; i < 4; ++i)
  {  
    t   = (*state)[i][0];
    Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3] ;
    Tm  = (*state)[i][0] ^ (*state)[i][1] ; Tm = xtime(Tm);  (*state)[i][0] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][1] ^ (*state)[i][2] ; Tm = xtime(Tm);  (*state)[i][1] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][2] ^ (*state)[i][3] ; Tm = xtime(Tm);  (*state)[i][2] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][3] ^ t ;        Tm = xtime(Tm);  (*state)[i][3] ^= Tm ^ Tmp ;
  }
}

// Multiply is used to multiply numbers in the field GF(2^8)
#if MULTIPLY_AS_A_FUNCTION
LOCAL uint8 Multiply(uint8 x, uint8 y)
{
  return (((y & 1) * x) ^
       ((y>>1 & 1) * xtime(x)) ^
       ((y>>2 & 1) * xtime(xtime(x))) ^
       ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
       ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))));
  }
#else
#define Multiply(x, y)                                \
      (  ((y & 1) * x) ^                              \
      ((y>>1 & 1) * xtime(x)) ^                       \
      ((y>>2 & 1) * xtime(xtime(x))) ^                \
      ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^         \
      ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))   \

#endif

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the inexperienced.
// Please use the references to gain more information.
LOCAL void InvMixColumns(void)
{
  int i;
  uint8 a,b,c,d;
  for(i=0;i<4;++i)
  { 
    a = (*state)[i][0];
    b = (*state)[i][1];
    c = (*state)[i][2];
    d = (*state)[i][3];

    (*state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
    (*state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
    (*state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
    (*state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
  }
}


// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
LOCAL void InvSubBytes(void)
{
  uint8 i,j;
  for(i=0;i<4;++i)
  {
    for(j=0;j<4;++j)
    {
      (*state)[j][i] = getSBoxInvert((*state)[j][i]);
    }
  }
}

LOCAL void InvShiftRows(void)
{
  uint8 temp;

  // Rotate first row 1 columns to right  
  temp=(*state)[3][1];
  (*state)[3][1]=(*state)[2][1];
  (*state)[2][1]=(*state)[1][1];
  (*state)[1][1]=(*state)[0][1];
  (*state)[0][1]=temp;

  // Rotate second row 2 columns to right 
  temp=(*state)[0][2];
  (*state)[0][2]=(*state)[2][2];
  (*state)[2][2]=temp;

  temp=(*state)[1][2];
  (*state)[1][2]=(*state)[3][2];
  (*state)[3][2]=temp;

  // Rotate third row 3 columns to right
  temp=(*state)[0][3];
  (*state)[0][3]=(*state)[1][3];
  (*state)[1][3]=(*state)[2][3];
  (*state)[2][3]=(*state)[3][3];
  (*state)[3][3]=temp;
}


// Cipher is the main function that encrypts the PlainText.
LOCAL void Cipher(void)
{
  uint8 round = 0;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(0); 
  
  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for(round = 1; round < Nr; ++round)
  {
    SubBytes();
    ShiftRows();
    MixColumns();
    AddRoundKey(round);
  }
  
  // The last round is given below.
  // The MixColumns function is not here in the last round.
  SubBytes();
  ShiftRows();
  AddRoundKey(Nr);
}

LOCAL void InvCipher(void)
{
  uint8 round=0;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(Nr); 

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for(round=Nr-1;round>0;round--)
  {
    InvShiftRows();
    InvSubBytes();
    AddRoundKey(round);
    InvMixColumns();
  }
  
  // The last round is given below.
  // The MixColumns function is not here in the last round.
  InvShiftRows();
  InvSubBytes();
  AddRoundKey(0);
}

LOCAL void BlockCopy(uint8* output, const uint8* input)
{
  uint8_t i;
  for (i=0;i<KEYLEN;++i)
  {
    output[i] = input[i];
  }
}



/*****************************************************************************/
/* Public functions:                                                         */
/*****************************************************************************/
LOCAL void XorWithIv(uint8* buf)
{
  uint8_t i;
  for(i = 0; i < KEYLEN; ++i)
  {
    buf[i] ^= Iv[i];
  }
}

PUBLIC void AES128_CBC_encrypt_buffer(uint8* output, uint8* input, uint32 length, const uint8* key, const uint8* iv)
{
  //uintptr_t i;
  uint32 i;
  uint8 remainders = length % KEYLEN; /* Remaining bytes in the last non-full block */

  BlockCopy(output, input);
  state = (state_t*)output;

  // Skip the key expansion if key is passed as 0
  if(0 != key)
  {
    Key = key;
    KeyExpansion();
  }

  if(iv != 0)
  {
    Iv = (uint8_t*)iv;
  }

  for(i = 0; i < length; i += KEYLEN)
  {
    XorWithIv(input);
    BlockCopy(output, input);
    state = (state_t*)output;
    Cipher();
    Iv = output;
    input += KEYLEN;
    output += KEYLEN;
  }

  if(remainders)
  {
    BlockCopy(output, input);
    //memset(output + remainders, 0, KEYLEN - remainders); /* add 0-padding */
    state = (state_t*)output;
    Cipher();
  }
}

#ifdef ZTE_WATCH
PUBLIC int  MMIZDT_HTTP_ZTE_DEVICE_REPORT_CB(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    ZDTHTTP_LOG("MMIZDT_HTTP_Test_CB(%d) Rcv_len=%d,%s",is_ok,Rcv_len,pRcv);
    if(is_ok)
    {
        cJSON *json = PNULL;
        cJSON *item_json = PNULL;
        json = cJSON_Parse(pRcv);
        if (json) 
        {
            item_json = cJSON_GetObjectItem(json,"result");
            if(item_json)
            {
                if(cJSON_GetObjectInt(item_json) == 0)
                {
                    ZDTHTTP_LOG("MMIZDT_HTTP_Test_CB success");
                    MMIZDT_NV_Set_Device_Report_Status(1);
                }
            }
            cJSON_Delete(json);
        }
    }
    return 0;
}

PUBLIC BOOLEAN  MMIZDT_HTTP_ZTE_Device_Report(void)
{
    char send_buf[256] = {0};
    char url_buf[512] = {0};
    uint16 len = 0;
    uint8_t in[17] = {0};    
	uint8_t out[256] = {0};	
	uint8_t aes_key_1[17];	  
	uint8_t iv[17]={0}; 
	uint8_t outStr[512] = {0};
    uint16 source_len = 0;
	int i,aseLen=0;
    int align_hex = 0; //16位对齐 
	char aes_key[17] ="www.ztemt.com.cn";
    char iv_key[17] ="f0ffc66146c8a96c";
    char source_data[129] = {0}; //"{}imei\":\"862495071000433\"}      ";
    sprintf(source_data, "{\"imei\":\"%s\",\"phone_model\":\"%s\",\"soft_version\":\"%s\"}",g_zdt_phone_imei_1,WATCHCOM_GetDeviceModel(),WATCHCOM_GetSoftwareVersion()); 
    source_len = strlen(source_data);
    align_hex = 16 - source_len%16;
    if(align_hex < 16) //AES NOPADDING 16位对齐
    {
        uint8 j=0;
        uint8 size = strlen(source_data);
        for(j;j<align_hex; j++)
        {
            source_data[size+j] = 32;//空格补齐
        }
    }
    memcpy(aes_key_1, aes_key, strlen(aes_key));
    memcpy(iv, iv_key, 16);
    SCI_TRACE_LOW("source data:%s",source_data);
    AES128_CBC_encrypt_buffer(out, source_data, source_len, aes_key_1, iv);
    for(i=0;i < (source_len+align_hex);i++)
    {
    	aseLen+=sprintf(outStr+aseLen,"%.2x",out[i]); //转成16进制
    }
    SCI_TRACE_LOW("encrypt data:%s",outStr);
    sprintf(send_buf,"data=%s",outStr);
    len = strlen(send_buf);
    sprintf(url_buf,"%s%s","https://report.server.nubia.cn/mobile/report.zte?",send_buf);
    SCI_TRACE_LOW("url:%s",url_buf);
    MMIZDT_HTTP_AppSend(FALSE,(char *)url_buf,"",len,1,0,0,0,PNULL,0,MMIZDT_HTTP_ZTE_DEVICE_REPORT_CB);
    return TRUE;
}

#endif

#endif
