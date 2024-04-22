
#ifndef  _MMI_HTTP_NET_C  
#define _MMI_HTTP_NET_C  

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "zyb_app.h"
#include "zyb_http_api.h"
#include "zyb_http_net.h"
#include "http_api.h"

#define ZYB_HTTP_APPFLAG (0xac9b)

#define ZYBHTTP_PDP_ACTIVE_TIMEOUT       30000  
#define ZYBHTTP_PDP_RETRY_TIMEOUT        2000 //重新尝试连接pdp，换卡连接

#define ZYBHTTP_PDP_DEFAULT_SIM_SYS MN_DUAL_SYS_1

LOCAL MN_DUAL_SYS_E       s_zyb_http_net_set_sys   = ZYBHTTP_PDP_DEFAULT_SIM_SYS;
LOCAL MN_DUAL_SYS_E       s_zyb_http_net_cur_sys   = MN_DUAL_SYS_1;
LOCAL uint8                       s_zyb_http_net_pdp_timer_id      = 0;
LOCAL uint8                       s_zyb_http_net_pdp_retry_timer_id = 0;
LOCAL uint32                      s_zyb_http_net_id            = 0;
LOCAL BOOLEAN                  s_zyb_http_net_pdp_state_is_ok        = FALSE;
LOCAL BOOLEAN                  s_zyb_http_net_is_init        = FALSE;
LOCAL BOOLEAN                  s_zyb_http_net_need_init        = TRUE;
LOCAL BOOLEAN                  s_zyb_http_net_gprs_status[MN_DUAL_SYS_MAX+1] = {0};
LOCAL BOOLEAN                  s_zyb_http_net_is_activing        = FALSE;
LOCAL BOOLEAN                  s_zyb_http_net_is_closing        = FALSE;

LOCAL BOOLEAN                  s_zyb_http_net_need_active_close        = FALSE;
LOCAL BOOLEAN                  s_zyb_http_net_need_close_active        = FALSE;

LOCAL BOOLEAN  ZYBHTTP_Net_PDPTry(MN_DUAL_SYS_E *  p_cur_sys);
LOCAL BOOLEAN ZYBHTTP_Net_PdpDeactive(void);
LOCAL BOOLEAN ZYBHTTP_Net_PdpActive(void);

LOCAL BOOLEAN ZYBHTTP_Net_Send_CallBack(BOOLEAN is_ok,uint32 err_id)
{
    if(MMIZYB_HTTP_IsSending())
    {
        if(is_ok)
        {
            MMIZYB_HTTP_PopSend_Result(FALSE,HTTP_SUCCESS);
        }
        else
        {
            MMIZYB_HTTP_PopSend_Result(FALSE,err_id);
        }
    }
}

#if 1
LOCAL void ZybHttp_GetSignalStruct(DPARAM param, void *signal_struct_ptr, uint16 struct_size);

#define ZYBHTTP_ACCEPT_STRING "text/vnd.wap.wml,application/vnd.wap.xhtml+xml,application/xhtml+xml,\
image/vnd.wap.wbmp,image/gif,image/jpg,image/jpeg,image/png,image/bmp,text/html,\
application/vnd.oma.drm.message,application/vnd.oma.drm.content,application/vnd.oma.drm.rights+xml,application/vnd.oma.drm.rights+wbxml,\
*/*;q=0.1"

/********************/
#define ZYBHTTP_ACCEPT_CHARSET_STRING "UTF-8,ISO-8859-1,US-ASCII,windows-1252,windows-1251"

//#define ZYBHTTP_ACCEPT_STRING "text/plain,text/html"
#define ZYBHTTP_ACCEPT_LANGUAGE_STRING  "zh-cn, en" //表示接受中文英文
#define ZYBHTTP_USER_AGENT_STRING       "Nokia 220 4G/2.0(p)"
//#define ZYBHTTP_ACCEPT_CHARSET_STRING   "UTF-8"

#define ZYBHTTP_HTTP_SIGNAL_ANALYTIC_EX(param,signal,app_param,http_context_ptr,app_module_id,sig_type) do{\
	    ZybHttp_GetSignalStruct(param, &signal, sizeof(sig_type));\
	    if(signal.module_id != app_module_id)\
	    {\
	    	return MMI_RESULT_FALSE;\
	    }\
	    app_param = signal.app_param;\
	    http_context_ptr->context_id   = signal.context_id;\
	    http_context_ptr->app_instance = 0;\
	    http_context_ptr->request_id = 0;\
	    http_context_ptr->module_id    = signal.module_id;\
}while(0)\

#define ZYBHTTP_HTTP_SIGNAL_ANALYTIC(param,signal,http_context_ptr,sig_type) do{\
	ZybHttp_GetSignalStruct(param, &signal, sizeof(sig_type));\
	if(signal.context_id != http_context_ptr->context_id)\
	{\
	    	return MMI_RESULT_FALSE;\
	}\
}while(0)\

typedef struct __ZybHttp
{
    BOOLEAN flags;
    MN_DUAL_SYS_E sim_sys;
    uint32 net_id;
    BOOLEAN need_refresh;
}ZybHttp;

typedef struct __ZYBHTTP_HTTP_CONTEXT
{
    HTTP_APP_MODULE_ID_T module_id;
    HTTP_CONTEXT_ID_T    context_id;
    HTTP_REQUEST_ID_T    request_id;
    HTTP_APP_INSTANCE_T  app_instance; //用于区分不同的HTTP请求。同时只有一个HTTP请求时可以为任意值。
}ZYBHTTP_HTTP_CONTEXT;

LOCAL BOOLEAN g_cur_zybhttp_is_init = FALSE;
LOCAL uint32 s_cur_zybhttp_content_len = 0;
LOCAL uint32 g_cur_zybhttp_file_len = 0;
LOCAL uint32 g_cur_zybhttp_file_start_len = 0;
LOCAL uint32 g_cur_zybhttp_file_end_len = 0;
LOCAL uint32 g_cur_zybhttp_rcv_count = 0;
LOCAL ZYBHTTP_HTTP_CONTEXT   s_zybhttp_http_context      = {0};
LOCAL ZybHttp s_zybhttphttp={0};
LOCAL uint8 *  s_zyb_p_cur_url = PNULL;
LOCAL uint8 *  s_zyb_p_refresh_url = PNULL;

void ZybHttp_GetSignalStruct(DPARAM param, void *signal_struct_ptr, uint16 struct_size)
{
    if((PNULL != param)&&(PNULL != signal_struct_ptr)&&(struct_size >= sizeof(xSignalHeaderRec)))
    {
        SCI_MEMCPY((uint8 *)((uint32)signal_struct_ptr), param, (struct_size ));
    }
}
#define ZYB_HTTP_RANGE_HEADER_LEN  60
#define ZYB_HTTP_RANGE_HEADER_PREFIX  "bytes="

LOCAL MMI_RESULT_E ZybHttp_HandleGetRequest(ZYBHTTP_HTTP_CONTEXT * http_context_ptr, DPARAM param)
{
    int base_len = 0;
    MMI_RESULT_E        result = MMI_RESULT_FALSE;
    HTTP_INIT_CNF_SIG_T signal = {0};
    HTTP_APP_PARAM_T app_param = 0;
    HTTP_GET_PARAM_T* get_param_ptr= NULL;
    int file_read = 0;
    int len = 0;
    uint32 tickcount = 0;
    int url_len = 0;
    if(http_context_ptr == NULL)
    {
        return MMI_RESULT_FALSE;
    }

    if(s_zyb_p_cur_url != PNULL)
    {
        SCI_FREE(s_zyb_p_cur_url);
        s_zyb_p_cur_url = PNULL;
    }

    url_len = MMIZYB_HTTP_GetlCurAllUrl((uint8 **)&s_zyb_p_cur_url);
    if(url_len <= 0)
    {
        if(s_zyb_p_cur_url != NULL)
        {
            SCI_FREE(s_zyb_p_cur_url);
            s_zyb_p_cur_url = PNULL;
        }
        ZYB_HTTP_LOG("ZybHttp_HandleGetRequest Err URL Malloc");
        return MMI_RESULT_FALSE;
    }
    ZYB_HTTP_LOG("ZybHttp_HandleGetRequest");
    ZYB_HTTPSendTrace(s_zyb_p_cur_url,strlen(s_zyb_p_cur_url));

    ZYBHTTP_HTTP_SIGNAL_ANALYTIC_EX(param, signal, app_param, http_context_ptr,MMI_MODULE_ZYB,HTTP_INIT_CNF_SIG_T);
    
    get_param_ptr = (HTTP_GET_PARAM_T*)SCI_ALLOC_APPZ(sizeof(HTTP_GET_PARAM_T));

	if(get_param_ptr == NULL)
	{
		return MMI_RESULT_FALSE;
	}	

	get_param_ptr->connection = HTTP_CONNECTION_KEEP_ALIVE;
	get_param_ptr->need_net_prog_ind = FALSE;
	len=strlen(ZYBHTTP_ACCEPT_STRING);
	get_param_ptr->accept.accept_ptr = SCI_ALLOC(len+1);
	memset(get_param_ptr->accept.accept_ptr,0,len+1);
	strcpy(get_param_ptr->accept.accept_ptr,ZYBHTTP_ACCEPT_STRING);
#if 1
	len=strlen(ZYBHTTP_ACCEPT_LANGUAGE_STRING);
	get_param_ptr->accept_language.accept_language_ptr = SCI_ALLOC(len+1);
	memset(get_param_ptr->accept_language.accept_language_ptr,0,len+1);
	strcpy(get_param_ptr->accept_language.accept_language_ptr,ZYBHTTP_ACCEPT_LANGUAGE_STRING);
    
	len=strlen(ZYBHTTP_USER_AGENT_STRING);
	get_param_ptr->user_agent.user_agent_ptr = SCI_ALLOC(len+1);
	memset(get_param_ptr->user_agent.user_agent_ptr,0,len+1);
	strcpy(get_param_ptr->user_agent.user_agent_ptr,ZYBHTTP_USER_AGENT_STRING);	
#endif
    	//拼接URL
      get_param_ptr->uri.uri_ptr = (char *)s_zyb_p_cur_url;
    
      len = strlen(ZYBHTTP_ACCEPT_CHARSET_STRING);
      get_param_ptr->accept_charset.accept_charset_ptr = SCI_ALLOC(len + 1);
      memset(get_param_ptr->accept_charset.accept_charset_ptr,0,len + 1);
      strcpy(get_param_ptr->accept_charset.accept_charset_ptr,ZYBHTTP_ACCEPT_CHARSET_STRING);
      // Rang 相关
    {
        char begin_bytes[ZYB_HTTP_RANGE_HEADER_LEN+1] = {0};
        uint32 start_pos = g_cur_zybhttp_file_start_len;
        uint32 end_pos = g_cur_zybhttp_file_end_len;
        uint16 len = 0;
        
        if(start_pos != 0 || end_pos != 0)
        {
            get_param_ptr->extern_header.other_header_ptr = (HTTP_OTHER_HEADER_T *)SCI_ALLOCAZ(sizeof(HTTP_OTHER_HEADER_T));
            if(PNULL !=  get_param_ptr->extern_header.other_header_ptr)
            {
                len = strlen("Range");
                get_param_ptr->extern_header.other_header_ptr->header_name_ptr = SCI_ALLOCAZ(len + 1);
                if(PNULL ==  get_param_ptr->extern_header.other_header_ptr->header_name_ptr)
                {
                    SCI_FREE(get_param_ptr->extern_header.other_header_ptr);
                }
                else
                {
                    sprintf(begin_bytes, "%ld-%ld", start_pos, end_pos);
                    len = strlen(ZYB_HTTP_RANGE_HEADER_PREFIX) + strlen(begin_bytes);
                    get_param_ptr->extern_header.other_header_ptr->header_value_ptr = SCI_ALLOCAZ(len + 1);
                    if(PNULL ==  get_param_ptr->extern_header.other_header_ptr->header_value_ptr)
                    {
                        SCI_FREE(get_param_ptr->extern_header.other_header_ptr->header_name_ptr);
                        SCI_FREE(get_param_ptr->extern_header.other_header_ptr);
                    }
                    else
                    {
                        strcpy(get_param_ptr->extern_header.other_header_ptr->header_name_ptr, "Range");
                        strcpy(get_param_ptr->extern_header.other_header_ptr->header_value_ptr, ZYB_HTTP_RANGE_HEADER_PREFIX);
                        strcat(get_param_ptr->extern_header.other_header_ptr->header_value_ptr, begin_bytes);
                        get_param_ptr->extern_header.header_num = 1;
                        ZYB_HTTP_LOG("ZybHttp_HandleGetRequest ADD HEAD, name=%s, value=%s",get_param_ptr->extern_header.other_header_ptr->header_name_ptr,get_param_ptr->extern_header.other_header_ptr->header_value_ptr);
                    }
                }
            }
        }
    }
#ifdef HTTP_SUPPORT
	if(!HTTP_GetRequest(http_context_ptr->context_id,get_param_ptr, http_context_ptr->app_instance))
	{
		result = MMI_RESULT_TRUE;
	}
#endif
    SCI_FREE(get_param_ptr->accept.accept_ptr);
    SCI_FREE(get_param_ptr->accept_charset.accept_charset_ptr);
    SCI_FREE(get_param_ptr->accept_language.accept_language_ptr);
    SCI_FREE(get_param_ptr->user_agent.user_agent_ptr);
    //SCI_FREE(get_param_ptr->uri.uri_ptr);
    if (PNULL != get_param_ptr->extern_header.other_header_ptr)
    {
        if (PNULL != get_param_ptr->extern_header.other_header_ptr->header_name_ptr)
        {
            SCI_FREE(get_param_ptr->extern_header.other_header_ptr->header_name_ptr);
        }

        if (PNULL != get_param_ptr->extern_header.other_header_ptr->header_value_ptr)
        {
            SCI_FREE(get_param_ptr->extern_header.other_header_ptr->header_value_ptr);
        }

        SCI_FREE(get_param_ptr->extern_header.other_header_ptr);
    }
    SCI_FREE(get_param_ptr);
    return	result;	
}

LOCAL MMI_RESULT_E ZybHttp_HandlePostRequest(ZYBHTTP_HTTP_CONTEXT * http_context_ptr, DPARAM param)
{
    int base_len = 0;
    MMI_RESULT_E        result = MMI_RESULT_FALSE;
    HTTP_INIT_CNF_SIG_T signal = {0};
    HTTP_APP_PARAM_T app_param = 0;
    HTTP_POST_PARAM_T* post_param_ptr= NULL;
    int file_read = 0;
    int len = 0;
    uint32 tickcount = 0;
    int url_len = 0;
    if(http_context_ptr == NULL)
    {
        return MMI_RESULT_FALSE;
    }

    if(s_zyb_p_cur_url != PNULL)
    {
        SCI_FREE(s_zyb_p_cur_url);
        s_zyb_p_cur_url = PNULL;
    }

    url_len = MMIZYB_HTTP_GetCurUrl((uint8 **)&s_zyb_p_cur_url);
    if(url_len <= 0)
    {
        if(s_zyb_p_cur_url != NULL)
        {
            SCI_FREE(s_zyb_p_cur_url);
            s_zyb_p_cur_url = PNULL;
        }
        ZYB_HTTP_LOG("ZybHttp_HandlePostRequest Err URL Malloc");
        return MMI_RESULT_FALSE;
    }
    ZYB_HTTP_LOG("ZybHttp_HandlePostRequest %s",s_zyb_p_cur_url);
    ZYBHTTP_HTTP_SIGNAL_ANALYTIC_EX(param, signal, app_param, http_context_ptr,MMI_MODULE_ZYB,HTTP_INIT_CNF_SIG_T);
    
    post_param_ptr = (HTTP_POST_PARAM_T*)SCI_ALLOC_APPZ(sizeof(HTTP_POST_PARAM_T));

	if(post_param_ptr == NULL)
	{
		return MMI_RESULT_FALSE;
	}	

	post_param_ptr->connection = HTTP_CONNECTION_KEEP_ALIVE;
	post_param_ptr->need_net_prog_ind = FALSE;
	len=strlen(ZYBHTTP_ACCEPT_STRING);
	post_param_ptr->accept.accept_ptr = SCI_ALLOC(len+1);
	memset(post_param_ptr->accept.accept_ptr,0,len+1);
	strcpy(post_param_ptr->accept.accept_ptr,ZYBHTTP_ACCEPT_STRING);
#if 1
	len=strlen(ZYBHTTP_ACCEPT_LANGUAGE_STRING);
	post_param_ptr->accept_language.accept_language_ptr = SCI_ALLOC(len+1);
	memset(post_param_ptr->accept_language.accept_language_ptr,0,len+1);
	strcpy(post_param_ptr->accept_language.accept_language_ptr,ZYBHTTP_ACCEPT_LANGUAGE_STRING);
    
	len=strlen(ZYBHTTP_USER_AGENT_STRING);
	post_param_ptr->user_agent.user_agent_ptr = SCI_ALLOC(len+1);
	memset(post_param_ptr->user_agent.user_agent_ptr,0,len+1);
	strcpy(post_param_ptr->user_agent.user_agent_ptr,ZYBHTTP_USER_AGENT_STRING);	
#endif
    	//拼接URL
      post_param_ptr->uri.uri_ptr = (char *)s_zyb_p_cur_url;
    
      len = strlen(ZYBHTTP_ACCEPT_CHARSET_STRING);
      post_param_ptr->accept_charset.accept_charset_ptr = SCI_ALLOC(len + 1);
      memset(post_param_ptr->accept_charset.accept_charset_ptr,0,len + 1);
      strcpy(post_param_ptr->accept_charset.accept_charset_ptr,ZYBHTTP_ACCEPT_CHARSET_STRING);
      
    post_param_ptr->post_body.u.post_buffer.is_copied_by_http = FALSE;
    post_param_ptr->post_body.is_use_file = FALSE;
    post_param_ptr->post_body.body_type_ptr = "application/json;charset=utf-8";
    post_param_ptr->post_body.u.post_buffer.buffer_len = MMIZYB_HTTP_GetCurData(&post_param_ptr->post_body.u.post_buffer.buffer_ptr);
    post_param_ptr->is_use_post_after_redirect = TRUE;
    ZYB_HTTPSendTrace(post_param_ptr->post_body.u.post_buffer.buffer_ptr, post_param_ptr->post_body.u.post_buffer.buffer_len);
#ifdef HTTP_SUPPORT
	if(!HTTP_PostRequest(http_context_ptr->context_id,post_param_ptr, http_context_ptr->app_instance))
	{
		result = MMI_RESULT_TRUE;
	}
#endif
    SCI_FREE(post_param_ptr->accept.accept_ptr);
    SCI_FREE(post_param_ptr->accept_charset.accept_charset_ptr);
    SCI_FREE(post_param_ptr->accept_language.accept_language_ptr);
    SCI_FREE(post_param_ptr->user_agent.user_agent_ptr);
    //SCI_FREE(get_param_ptr->uri.uri_ptr);
    SCI_FREE(post_param_ptr);
    return result;
}

LOCAL MMI_RESULT_E ZybHttp_HandleRequestIdInd(ZYBHTTP_HTTP_CONTEXT *http_context_ptr, DPARAM param)
{
    MMI_RESULT_E result = MMI_RESULT_FALSE;
    HTTP_REQUEST_ID_IND_SIG_T signal = {0};
	HTTP_REQUEST_ID_IND_SIG_T *cnf = (HTTP_REQUEST_ID_IND_SIG_T*)param;
   //当request_id为BILLING_REQUEST_ID_DEFAULT_VALUE时,则没被初始化过,属于本模块的消息
    ZYBHTTP_HTTP_SIGNAL_ANALYTIC(param,signal,http_context_ptr,HTTP_REQUEST_ID_IND_SIG_T);
	ZYB_HTTP_LOG("ZybHttp_HandleRequestIdInd receive HTTP_SIG_REQUEST_ID_IND");
	ZYB_HTTP_LOG("ZybHttp_HandleRequestIdInd receive http_context_ptr->context_id:%d",http_context_ptr->context_id);
    http_context_ptr->context_id = signal.context_id;
    http_context_ptr->app_instance = signal.app_instance;
    http_context_ptr->request_id = signal.request_id;
    ZYB_HTTP_LOG("ZybHttp_HandleRequestIdInd request_id:%ld,http_context_ptr->context_id:%d",signal.request_id,http_context_ptr->context_id);
    result = MMI_RESULT_TRUE;

    return result;
}

LOCAL MMI_RESULT_E ZybHttp_HandleDataInd(ZYBHTTP_HTTP_CONTEXT * http_context_ptr, DPARAM param)
{
    //---1--- 
    ZYB_HTTP_DATA_T * pCurData = MMIZYB_HTTP_GetCurAll();
    MMI_RESULT_E        result = MMI_RESULT_TRUE;
    HTTP_DATA_IND_SIG_T signal = {0};
    ZYBHTTP_HTTP_SIGNAL_ANALYTIC(param,signal,http_context_ptr,HTTP_DATA_IND_SIG_T);
    g_cur_zybhttp_rcv_count += signal.data_len;
    ZYB_HTTP_LOG("ZybHttp_HandleDataInd receive len=%d,g_all_http_need_rcv_len=%d",signal.data_len,g_cur_zybhttp_rcv_count);
    //ZYB_HTTPRcvTraceU8(signal.data_ptr, signal.data_len);
    if(g_cur_zybhttp_file_len > 0)
    {
        ZYB_HTTP_RCV_Add(signal.data_ptr,signal.data_len);
    }
    else
    {
        if(s_cur_zybhttp_content_len != 0 && g_cur_zybhttp_file_start_len >= s_cur_zybhttp_content_len)
        {
            //文件已接收完成
        }
        else
        {
            if(s_zybhttphttp.flags && pCurData != PNULL && pCurData->rcv_handle != PNULL)
            {
                pCurData->rcv_handle(TRUE,signal.data_ptr,signal.data_len,g_cur_zybhttp_rcv_count,HTTP_SUCCESS);
            }
        }
    }
    ZYB_HTTP_SendTimer_Start(20000);
    http_context_ptr->request_id = signal.request_id;
    http_context_ptr->app_instance = signal.app_instance;
    return result;
}

LOCAL MMI_RESULT_E ZybHttp_HandleGetCnf(ZYBHTTP_HTTP_CONTEXT * http_context_ptr, DPARAM param)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
#ifdef HTTP_SUPPORT
    HTTP_GET_CNF_SIG_T signal = {0};
    ZYBHTTP_HTTP_SIGNAL_ANALYTIC(param,signal,http_context_ptr,HTTP_GET_CNF_SIG_T);
    ZYB_HTTP_LOG("ZybHttp_HandleGetCnf code=%d,need_refresh=%d",signal.rsp_header_info.response_code,s_zybhttphttp.need_refresh);
    if(s_zybhttphttp.need_refresh)
    {
        ZYB_HTTP_RCV_DelAll();
        MMIZYB_HTTP_SendSig_Refresh(PNULL);
        s_zybhttphttp.need_refresh = FALSE;
    }
    else
    {
        HTTP_CloseRequest(http_context_ptr->context_id);
    }
    
#endif
    return result;
}

LOCAL MMI_RESULT_E ZybHttp_HandlePostCnf(ZYBHTTP_HTTP_CONTEXT * http_context_ptr, DPARAM param)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
#ifdef HTTP_SUPPORT
    HTTP_POST_CNF_SIG_T signal = {0};
    ZYBHTTP_HTTP_SIGNAL_ANALYTIC(param,signal,http_context_ptr,HTTP_POST_CNF_SIG_T);
    ZYB_HTTP_LOG("ZybHttp_HandlePostCnf code=%d",signal.rsp_header_info.response_code);

    if(s_zybhttphttp.need_refresh)
    {
        ZYB_HTTP_RCV_DelAll();
        MMIZYB_HTTP_SendSig_Refresh(PNULL);
        s_zybhttphttp.need_refresh = FALSE;
    }
    else
    {
        HTTP_CloseRequest(http_context_ptr->context_id);
    }
#endif
    return result;
}

LOCAL MMI_RESULT_E ZybHttp_HandleCloseCnf(ZYBHTTP_HTTP_CONTEXT *http_context_ptr, DPARAM param)
{
    MMI_RESULT_E result = MMI_RESULT_FALSE;
    HTTP_CLOSE_CNF_SIG_T signal = {0};
    BOOLEAN nv_value = 1;
    uint32 cur_file_len = 0;
    uint32 err_id = HTTP_ERROR_HANDSHAKE;
    
    ZybHttp_GetSignalStruct(param, &signal, sizeof(HTTP_CLOSE_CNF_SIG_T));
    if(http_context_ptr->context_id == signal.context_id)
    {
        ZYB_HTTP_LOG("ZybHttp_HandleCloseCnf receive HTTP_SIG_CLOSE_CNF g_cur_zybhttp_rcv_count=%d",g_cur_zybhttp_rcv_count);
        http_context_ptr->context_id = 0;
        http_context_ptr->request_id = 0;
        result = MMI_RESULT_TRUE;
#if defined(MMI_GPRS_SUPPORT) || defined(WIFI_SUPPORT)
        //ZYBHTTP_Net_Close(); 不关闭网络
#endif
        g_cur_zybhttp_is_init = FALSE;
        if(s_cur_zybhttp_content_len != 0 && g_cur_zybhttp_file_start_len >= s_cur_zybhttp_content_len)
        {
            //文件已接收完成
            s_zybhttphttp.flags = FALSE;
            err_id = HTTP_ERROR_SERVER_CLOSE;
        }

        ZYBHTTP_Net_Send_CallBack(s_zybhttphttp.flags,err_id);
        ZYB_HTTP_LOG("ZybHttp_HandleCloseCnf send again=== ");
   }
    return result;
}
    
MMI_RESULT_E Zyb_HandleHttpMsg(PWND app_ptr, uint16 msg_id, DPARAM param)
{
    //当result为true时，后续注册的app不需要再响应http的消息
    MMI_RESULT_E result = MMI_RESULT_FALSE;
    void    *original_param = (void*)((uint32)param);
    ZYBHTTP_HTTP_CONTEXT *http_context_ptr = &s_zybhttp_http_context;
    HTTP_SSL_CERT_UNTRUST_IND_SIG_T* cert_ptr = PNULL;
    uint8 * cur_filename = PNULL;
	ZYB_HTTP_LOG("Zyb_HandleHttpMsg == msg_id=0x%x ",msg_id);	
	if(msg_id == HTTP_SIG_INIT_CNF)
	{
	      HTTP_INIT_CNF_SIG_T * cnf = (HTTP_INIT_CNF_SIG_T*)original_param;
	      if(cnf->module_id != MMI_MODULE_ZYB || ZYB_HTTP_APPFLAG != cnf->app_param)
            {
                return MMI_RESULT_FALSE;
            }
            s_cur_zybhttp_content_len = 0;
            g_cur_zybhttp_rcv_count = 0;
            g_cur_zybhttp_file_len = MMIZYB_HTTP_GetCurFileLen();
            g_cur_zybhttp_file_start_len = MMIZYB_HTTP_GetCurFileStartLen();
            g_cur_zybhttp_file_end_len = MMIZYB_HTTP_GetCurFileEndLen();
            ZYB_HTTP_LOG(" HTTP_SIG_INIT_CNF file_len=%d,%d-%d",g_cur_zybhttp_file_len,g_cur_zybhttp_file_start_len,g_cur_zybhttp_file_end_len);	
            if(s_zyb_p_refresh_url != PNULL)
            {
                SCI_FREE(s_zyb_p_refresh_url);
                s_zyb_p_refresh_url = PNULL;
            }
            s_zybhttphttp.need_refresh = FALSE;
            s_zybhttphttp.flags = FALSE;
            if(MMIZYB_HTTP_IsGetType())
            {
        	      result = ZybHttp_HandleGetRequest(http_context_ptr, param);
            }
            else
            {
        	      result = ZybHttp_HandlePostRequest(http_context_ptr, param);
            }
            g_cur_zybhttp_is_init = TRUE;
            if(result == MMI_RESULT_FALSE)
            {
                ZYBHTTP_Net_Send_CallBack(FALSE,HTTP_ERROR_HANDSHAKE);
            }
	      return result;//后续app不用再响应
	 }
	 //第一次请求后，http分配给app的id
	 if(msg_id == HTTP_SIG_REQUEST_ID_IND)
	 {
	      result = ZybHttp_HandleRequestIdInd(http_context_ptr, param);
	      return result;
	 }	 
	 switch (msg_id)
	 {
	 case HTTP_SIG_HEADER_IND:
	 	{
#ifdef HTTP_SUPPORT
	 		HTTP_HEADER_IND_SIG_T signal = {0};
	 		ZYBHTTP_HTTP_SIGNAL_ANALYTIC(param,signal,http_context_ptr,HTTP_HEADER_IND_SIG_T);
	 		ZYB_HTTP_LOG("receive HTTP_SIG_HEADER_IND");
	 		ZYB_HTTP_LOG("response_code:%d",signal.rsp_header_info.response_code);
	 		ZYB_HTTP_LOG("context_id:%ld,app_instance:%ld,request_id:%ld",
	 						http_context_ptr->context_id,http_context_ptr->app_instance,http_context_ptr->request_id);
                   if(signal.header_ptr != PNULL)
                   {
                        s_cur_zybhttp_content_len = signal.rsp_header_info.content_length;
                       ZYB_HTTP_LOG("content_length=%d",s_cur_zybhttp_content_len);
                       ZYB_HTTPTraceCR(signal.header_ptr,strlen(signal.header_ptr));
                   }
	 		if(signal.rsp_header_info.response_code == 200 || signal.rsp_header_info.response_code == 206)
	 		{	 			
	 			result=HTTP_HeaderResponse(http_context_ptr->context_id,http_context_ptr->app_instance,
	 								http_context_ptr->request_id,HTTP_DATA_TRANS_STYLE_BUFFER,PNULL,0);
                          
                          if(signal.rsp_header_info.refresh_url_ptr != NULL)
                          {
                                uint32 len = strlen(signal.rsp_header_info.refresh_url_ptr);
                                if(s_zyb_p_refresh_url != PNULL)
                                {
                                    SCI_FREE(s_zyb_p_refresh_url);
                                    s_zyb_p_refresh_url = PNULL;
                                }
                                s_zyb_p_refresh_url = SCI_ALLOC_APPZ(len+1);
                                if(s_zyb_p_refresh_url != PNULL)
                                {
                                    strcpy((char*)s_zyb_p_refresh_url,(const char*)(signal.rsp_header_info.refresh_url_ptr));
                                }
                                s_zybhttphttp.need_refresh = TRUE;
                                ZYB_HTTP_LOG("refresh time=%d, url_ptr:%s",signal.rsp_header_info.refresh_time,signal.rsp_header_info.refresh_url_ptr);
                                
                          }
                          else
                          {
                                s_zybhttphttp.flags = TRUE;
                          }
	 		}
	 		else
	 		{
                       if(signal.rsp_header_info.response_code == 416)
                       {
                            ZYBHTTP_Net_Send_CallBack(FALSE,HTTP_ERROR_SERVER_CLOSE);
                       }
                       else
                       {
                            ZYBHTTP_Net_Send_CallBack(FALSE,HTTP_ERROR_NET_DATA_ERROR);
                       }
                       
	 		    HTTP_CloseRequest(http_context_ptr->context_id);
	 		}
#endif
	 		ZYB_HTTP_LOG("result:%d",result);
	 		result = MMI_RESULT_TRUE;
	 	}
	 	break;
        
	 case HTTP_SIG_ERROR_IND:
	 	{
                    HTTP_ERROR_IND_SIG_T signal = {0};
                    ZYBHTTP_HTTP_SIGNAL_ANALYTIC(param,signal,http_context_ptr,HTTP_ERROR_IND_SIG_T);
                    ZYB_HTTP_LOG("receive HTTP_SIG_ERROR_IND err_id=%d",signal.result);
                    HTTP_CloseRequest(http_context_ptr->context_id);
	 	}
	 	break;	
	 case HTTP_SIG_DATA_IND:
	 	{
                    ZYB_HTTP_LOG("receive HTTP_SIG_DATA_IND");
	             result=ZybHttp_HandleDataInd(http_context_ptr,param);
	 	}
	 	break;
	 case HTTP_SIG_GET_CNF:
	 	result =ZybHttp_HandleGetCnf(http_context_ptr,param);
             
	 	break;
	 case HTTP_SIG_POST_CNF:
	 	result =ZybHttp_HandlePostCnf(http_context_ptr,param);
	 	break;
   	 case HTTP_SIG_CLOSE_CNF://HTTP_SIG_CLOSE_REQ消息的回复,告知app,http服务关闭
        	result = ZybHttp_HandleCloseCnf(http_context_ptr, param);	
        	break;
     default:
     	break;
	 }
	 return result;
}

LOCAL BOOLEAN  ZybHttp_StartRequest()
{
    HTTP_INIT_PARAM_T init_param = {0};
    init_param.is_cookie_enable = FALSE;
    init_param.is_cache_enable = FALSE;
    init_param.net_id = s_zyb_http_net_id;
    ZYB_HTTP_LOG("ZybHttp_StartRequest init_param_ptr->is_cookie_enable = FALSE; \n");
#ifdef HTTP_SUPPORT
    if (HTTP_SUCCESS == HTTP_InitRequest(MMI_MODULE_ZYB, ZYB_HTTP_APPFLAG, &init_param))
#endif
    {
        return TRUE;
    }
    return FALSE;
}

BOOLEAN  ZybHttp_StopRequest()
{
    BOOLEAN res = FALSE;
    if(g_cur_zybhttp_is_init)
    {
        HTTP_CloseRequest(s_zybhttp_http_context.context_id);
        res = TRUE;
    }
    return res;
}

MMI_RESULT_E ZybHttp_StartRequestRefresh(ZYBHTTP_HTTP_CONTEXT * http_context_ptr, char * url)
{
      int base_len = 0;
	MMI_RESULT_E        result = MMI_RESULT_FALSE;
	HTTP_GET_PARAM_T* get_param_ptr= NULL;
	int len = 0;

        if(http_context_ptr == NULL || url == PNULL)
        {
            return MMI_RESULT_FALSE;
        }

	ZYB_HTTP_LOG("ZybHttp_StartRequestRefresh url =%s",url);
	get_param_ptr = (HTTP_GET_PARAM_T*)SCI_ALLOC_APPZ(sizeof(HTTP_GET_PARAM_T));

	if(get_param_ptr == NULL)
	{
		return MMI_RESULT_FALSE;
	}	
	memset(get_param_ptr,0,sizeof(HTTP_GET_PARAM_T));
	
	get_param_ptr->connection = HTTP_CONNECTION_KEEP_ALIVE;
	get_param_ptr->need_net_prog_ind = FALSE;
	len=strlen(ZYBHTTP_ACCEPT_STRING);
	get_param_ptr->accept.accept_ptr = SCI_ALLOC(len+1);
	memset(get_param_ptr->accept.accept_ptr,0,len+1);
	strcpy(get_param_ptr->accept.accept_ptr,ZYBHTTP_ACCEPT_STRING);
	len=strlen(ZYBHTTP_ACCEPT_LANGUAGE_STRING);
	get_param_ptr->accept_language.accept_language_ptr = SCI_ALLOC(len+1);
	memset(get_param_ptr->accept_language.accept_language_ptr,0,len+1);
	strcpy(get_param_ptr->accept_language.accept_language_ptr,ZYBHTTP_ACCEPT_LANGUAGE_STRING);	
	len=strlen(ZYBHTTP_USER_AGENT_STRING);
	get_param_ptr->user_agent.user_agent_ptr = SCI_ALLOC(len+1);
	memset(get_param_ptr->user_agent.user_agent_ptr,0,len+1);
	strcpy(get_param_ptr->user_agent.user_agent_ptr,ZYBHTTP_USER_AGENT_STRING);	

	//拼接URL
	len=strlen(url);
	get_param_ptr->uri.uri_ptr= SCI_ALLOC(len+1);
	memset(get_param_ptr->uri.uri_ptr,0,len+1);
	strcpy(get_param_ptr->uri.uri_ptr,url);
	ZYB_HTTP_LOG("ZybHttp_StartRequestRefresh uri.uri_ptr=%s,len=%d ",get_param_ptr->uri.uri_ptr,len);
    
      len = strlen(ZYBHTTP_ACCEPT_CHARSET_STRING);
      get_param_ptr->accept_charset.accept_charset_ptr = SCI_ALLOC(len + 1);
      memset(get_param_ptr->accept_charset.accept_charset_ptr,0,len + 1);
      strcpy(get_param_ptr->accept_charset.accept_charset_ptr,ZYBHTTP_ACCEPT_CHARSET_STRING);
      
#ifdef HTTP_SUPPORT
	if(!HTTP_GetRequest(http_context_ptr->context_id,get_param_ptr, http_context_ptr->app_instance))
	{
		result = MMI_RESULT_TRUE;
	}
#endif
    SCI_FREE(get_param_ptr->accept.accept_ptr);
    SCI_FREE(get_param_ptr->accept_charset.accept_charset_ptr);
    SCI_FREE(get_param_ptr->accept_language.accept_language_ptr);
    SCI_FREE(get_param_ptr->user_agent.user_agent_ptr);
    SCI_FREE(get_param_ptr->uri.uri_ptr);
    SCI_Free(get_param_ptr);
    return	result;	
}

BOOLEAN MMIZYB_HTTP_Handle_RefreshInd(DPARAM param)
{
    #if 1
        if(MMI_RESULT_FALSE == ZybHttp_StartRequestRefresh(&s_zybhttp_http_context,s_zyb_p_refresh_url))
        {
            HTTP_CloseRequest(s_zybhttp_http_context.context_id);
        }
    #else
    ZYBHTTP_REFRESH_DATA_T ** pp_getdata = (ZYBHTTP_REFRESH_DATA_T **)(param);
    ZYBHTTP_REFRESH_DATA_T * ind_ptr = *pp_getdata;
    uint32  url_len = 0;
    if(ind_ptr == PNULL)
    {
        ZYB_HTTP_LOG("MMIZYBHTTP_HTTP_CTREG_HandleRefreshInd ERR NULL DATA");
        return FALSE;
    }
    if(ind_ptr->url_arr != PNULL)
    {
        url_len = SCI_STRLEN((char*)ind_ptr->url_arr);
    }
    
    if (0 == url_len)
    {
        ZYB_HTTP_LOG("[HTTP]MMIZYBHTTP_HTTP_CTREG_HandleRefreshInd: url len == 0");
        SCI_FREE(ind_ptr);
        return FALSE;
    }

    if (ind_ptr->refresh_time > 0)
    {
        MMK_CreateTimerCallback(ind_ptr->refresh_time * 1000, (MMI_TIMER_FUNC)ZYB_HttpRefreshTimerCallback, (uint32)ind_ptr->url_arr, FALSE);
    }
    else
    {
        HandleCTRegHttpGetRefresh(&s_zybhttp_http_context,ind_ptr->url_arr); 
        if(ind_ptr->url_arr != NULL)
        {
            SCI_FREE(ind_ptr->url_arr);
        }
    }        
    SCI_FREE(ind_ptr);
    #endif
    return TRUE;
}
#endif
/*****************************************************************************/
//  Discription: Get apn str 
//  Global resource dependence: None
//  Author: Gaily.Wang
//  Note : 
/*****************************************************************************/
LOCAL char* HTTP_Net_GetApnStr(MN_DUAL_SYS_E dual_sys)
{
    MMICONNECTION_LINKSETTING_DETAIL_T* linksetting = PNULL;
    char*   apn_str = PNULL;
    uint8   index   = MMIAPIBROWSER_GetNetSettingIndex(dual_sys);

    linksetting = MMIAPICONNECTION_GetLinkSettingItemByIndex(dual_sys, index);

    if(PNULL != linksetting && 0 != linksetting->apn_len)
    {
        apn_str = (char*)linksetting->apn;
    }
    if(apn_str != PNULL)
    {
        ZYB_HTTP_LOG("HTTP NET Get APN %s",apn_str);
    }
    else
    {
        ZYB_HTTP_LOG("HTTP NET Get APN NULL");
    }
    return apn_str;
}

/*****************************************************************************/
//  Discription: Get apn str 
//  Global resource dependence: None
//  Author: Gaily.Wang
//  Note : 
/*****************************************************************************/
LOCAL char* HTTP_Net_GetUserNameStr(MN_DUAL_SYS_E dual_sys)
{
    MMICONNECTION_LINKSETTING_DETAIL_T* linksetting = PNULL;
    char*   user_name_str = PNULL;
    uint8   index   = 0;

    linksetting = MMIAPICONNECTION_GetLinkSettingItemByIndex(dual_sys, index);

    if(PNULL != linksetting && 0 != linksetting->username_len)
    {
        user_name_str = (char*)linksetting->username;
    }
    if(user_name_str != PNULL)
    {
        ZYB_HTTP_LOG("HTTP NET Get UserName %s",user_name_str);
    }
    else
    {
        ZYB_HTTP_LOG("HTTP NET Get UserName NULL");
    }
    return user_name_str;
    
}

/*****************************************************************************/
//  Discription: Get apn str 
//  Global resource dependence: None
//  Author: Gaily.Wang
//  Note : 
/*****************************************************************************/
LOCAL char* HTTP_Net_GetPasswordStr(MN_DUAL_SYS_E dual_sys)
{
    MMICONNECTION_LINKSETTING_DETAIL_T* linksetting = PNULL;
    char*   password_str = PNULL;
    uint8   index   = 0;

    linksetting = MMIAPICONNECTION_GetLinkSettingItemByIndex(dual_sys, index);

    if(PNULL != linksetting && 0 != linksetting->password_len)
    {
        password_str = (char*)linksetting->password;
    }
    if(password_str != PNULL)
    {
        ZYB_HTTP_LOG("HTTP NET Get Password %s",password_str);
    }
    else
    {
        ZYB_HTTP_LOG("HTTP NET Get Password NULL");
    }
    return password_str;
}

/*****************************************************************************/
//  Description : handle socket connect timeout
//  Global resource dependence : none
//  Author: Gaily.Wang
//  Note:
/*****************************************************************************/
LOCAL void HTTP_Net_HandlePdpActiveTimer(
                                            uint8 timer_id,
                                            uint32 param
                                            )
{
    ZYB_HTTP_LOG("HTTP NET PdpActive ERR TimeOut");
    ZYBHTTP_Net_PdpDeactive();
    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_FAIL,PNULL,0);
}

/*****************************************************************************/
//  Discription: Start socket connect timer
//  Global resource dependence: None
//  Author: Gaily.Wang
//  Note : 
/*****************************************************************************/
LOCAL void HTTP_Net_StartPdpActiveTimer(void)
{
    if(0 != s_zyb_http_net_pdp_timer_id)
    {
        MMK_StopTimer(s_zyb_http_net_pdp_timer_id);
        s_zyb_http_net_pdp_timer_id = 0;
    }
    
    s_zyb_http_net_pdp_timer_id = MMK_CreateTimerCallback(ZYBHTTP_PDP_ACTIVE_TIMEOUT, 
                                                                        HTTP_Net_HandlePdpActiveTimer, 
                                                                        PNULL, 
                                                                        FALSE);
}
/*****************************************************************************/
//  Discription: Start socket connect timer
//  Global resource dependence: None
//  Author: Gaily.Wang
//  Note : 
/*****************************************************************************/
LOCAL void HTTP_Net_StopPdpActiveTimer(void)
{
    if(0 != s_zyb_http_net_pdp_timer_id)
    {
        MMK_StopTimer(s_zyb_http_net_pdp_timer_id);
        s_zyb_http_net_pdp_timer_id = 0;
    }
}

/*****************************************************************************
//  Description : handle pdp msg 
//  Global resource dependence : none
//  Author: Gaily.Wang
//  Note:
*****************************************************************************/
LOCAL void HTTP_Net_HandlePdpMsg(MMIPDP_CNF_INFO_T *msg_ptr)
{
    if(PNULL == msg_ptr)
    {
        return;
    }
    
    ZYB_HTTP_LOG("HTTP NET HandlePdpMsg msg_id=%d,result=%d",msg_ptr->msg_id,msg_ptr->result);
    
    switch(msg_ptr->msg_id) 
    {
    case MMIPDP_ACTIVE_CNF:     //PDP激活成功
        HTTP_Net_StopPdpActiveTimer();
        
        if(MMIPDP_RESULT_SUCC == msg_ptr->result)
        {
            s_zyb_http_net_id = msg_ptr->nsapi;
            ZYB_HTTP_LOG("HTTP NET MMIPDP_ACTIVE_CNF OK net_id=0x%x",s_zyb_http_net_id);
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_SUCCESS,PNULL,0);
        }
        else if (MMIPDP_RESULT_FAIL == msg_ptr->result)
        {
            ZYB_HTTP_LOG("HTTP NET MMIPDP_ACTIVE_CNF FAIL");
            ZYBHTTP_Net_PdpDeactive();
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_FAIL,PNULL,0);
        }
        else 
        {
            ZYB_HTTP_LOG("HTTP NET MMIPDP_ACTIVE_CNF FAIL Other");
            ZYBHTTP_Net_PdpDeactive();
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_FAIL,PNULL,0);
        }        
        break;
        
    case MMIPDP_DEACTIVE_CNF:   //PDP去激活成功。
            ZYB_HTTP_LOG("HTTP NET MMIPDP_DEACTIVE_CNF");
            s_zyb_http_net_id = 0;
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_CONNECT_FAIL,PNULL,0);
        break;
        
    case MMIPDP_DEACTIVE_IND:   //PDP被网络端去激活。
            ZYB_HTTP_LOG("HTTP NET MMIPDP_DEACTIVE_IND");
            s_zyb_http_net_id = 0;
            ZYBHTTP_Net_PdpDeactive();
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_CONNECT_FAIL,PNULL,0);
        break;
        
    default:
        break;
    }
    
}
/*****************************************************************************/
//  Description : Pdp Active, 
//  Global resource dependence : none
//  Author: Gaily.Wang
//  Note:
/*****************************************************************************/
#if 1
LOCAL BOOLEAN  ZYBHTTP_Net_PDPTry(MN_DUAL_SYS_E *  p_cur_sys)
{
#if defined(MMI_GPRS_SUPPORT)
    BOOLEAN                 return_val      = FALSE;
    MMIPDP_ACTIVE_INFO_T    active_info     = {0};
    MMICONNECTION_APNTYPEINDEX_T *apn_idx_arr;
    MMICONNECTION_LINKSETTING_DETAIL_T* linksetting = PNULL;
    MN_DUAL_SYS_E sim_sys;
        
    ZYB_HTTP_LOG("HTTP NET ConnectPDP");

    if (FALSE == MMIAPIPHONE_GetDataServiceSIM(&sim_sys))
    {
        SCI_TraceLow("HTTP NET ConnectPDP [SIM ERROR]");
        return FALSE;
    }
    *p_cur_sys = sim_sys;
    active_info.app_handler         = MMI_MODULE_ZYB;
    active_info.dual_sys            = sim_sys;
    active_info.apn_ptr             = NULL;
    active_info.user_name_ptr       = NULL;
    active_info.psw_ptr             = NULL;
    active_info.priority            = 3;
    active_info.ps_service_rat      = MN_TD_PREFER;
    active_info.ps_interface        = MMIPDP_INTERFACE_GPRS;
    active_info.handle_msg_callback = HTTP_Net_HandlePdpMsg;
    active_info.ps_service_type     = BROWSER_E;
    active_info.storage             = MN_GPRS_STORAGE_ALL;

    apn_idx_arr = MMIAPICONNECTION_GetApnTypeIndex(sim_sys);
    if (apn_idx_arr == NULL)
    {
        ZYB_HTTP_LOG("HTTP NET ConnectPDP [APN IDX ERROR]");
        return FALSE;
    }
    ZYB_HTTP_LOG("HTTP NET  ConnectPDP [sim = %d, Net Setting = %d]", sim_sys, apn_idx_arr->internet_index[sim_sys].index);
    linksetting = MMIAPICONNECTION_GetLinkSettingItemByIndex(sim_sys, apn_idx_arr->internet_index[sim_sys].index);

    if(PNULL != linksetting && 0 != linksetting->apn_len)
    {
        active_info.apn_ptr = (char*)linksetting->apn;
        active_info.user_name_ptr = (char*)linksetting->username;
        active_info.psw_ptr = (char*)linksetting->password;
#ifdef IPVERSION_SUPPORT_V4_V6
        active_info.ip_type   = linksetting->ip_type;
#endif
        if(MMIAPIPDP_Active(&active_info))
        {
            ZYB_HTTP_LOG("HTTP NET PdpActive GPRS sim_sys=%d",sim_sys);
            HTTP_Net_StartPdpActiveTimer();
            return_val = TRUE;
        }
    }
    return return_val;
#endif
    return FALSE;
}
#else
LOCAL BOOLEAN  ZYBHTTP_Net_PDPTry(MN_DUAL_SYS_E sim_sys)
{
    MMIPDP_ACTIVE_INFO_T    active_info     = {0};
    BOOLEAN                 return_val      = FALSE;
    
    active_info.app_handler         = MMI_MODULE_ZYB;
    active_info.dual_sys            = sim_sys;
    active_info.apn_ptr             = HTTP_Net_GetApnStr(sim_sys);
    active_info.user_name_ptr       = HTTP_Net_GetUserNameStr(sim_sys);
    active_info.psw_ptr             = HTTP_Net_GetPasswordStr(sim_sys);
    active_info.priority            = 3;
    active_info.ps_service_rat      = MN_UNSPECIFIED;
    active_info.ps_interface        = MMIPDP_INTERFACE_GPRS;
    active_info.handle_msg_callback = HTTP_Net_HandlePdpMsg;
    active_info.ps_service_type = BROWSER_E;
    active_info.storage = MN_GPRS_STORAGE_ALL;

    if(MMIAPIPDP_Active(&active_info))
    {
        ZYB_HTTP_LOG("HTTP NET PdpActive GPRS sim_sys=%d",sim_sys);
        HTTP_Net_StartPdpActiveTimer();
        return_val = TRUE;
    }
    return return_val;
}
#endif

LOCAL BOOLEAN ZYBHTTP_Net_PdpActive(void)
{
    BOOLEAN                 return_val      = FALSE;
    MN_DUAL_SYS_E           dual_sys        = MN_DUAL_SYS_MAX;
    MMIPDP_ACTIVE_INFO_T    active_info     = {0};
    
    ZYB_HTTP_LOG("HTTP NET PdpActive Start is_activing=%d,is_ok=%d",s_zyb_http_net_is_activing,s_zyb_http_net_pdp_state_is_ok);
    if(s_zyb_http_net_is_activing)
    {
        s_zyb_http_net_need_active_close = FALSE;
        return TRUE;
    }

    s_zyb_http_net_is_activing = TRUE;
    
    if(s_zyb_http_net_pdp_state_is_ok)
    {
        MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_SUCCESS,PNULL,0);
        return TRUE;
    }

    if(MMIAPISET_GetFlyMode() 
      || (MMIAPIPHONE_GetSimOkNum(NULL,0)==0)
      /*||(MMICC_GetCallNum()>0)*/
    )
    {
        ZYB_HTTP_LOG("HTTP NET PdpActive ERR FLY");
        MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_FAIL,PNULL,0);
        return FALSE;
    }

#if 0 //def WIFI_SUPPORT    
    //优先wifi
    if(s_zyb_http_net_set_sys == MN_DUAL_SYS_MAX && MMIWIFI_STATUS_CONNECTED == MMIAPIWIFI_GetStatus())
    {
        active_info.app_handler         = MMI_MODULE_ZYB;
        active_info.handle_msg_callback = HTTP_Net_HandlePdpMsg;
        active_info.ps_interface        = MMIPDP_INTERFACE_WIFI;
        if(MMIAPIPDP_Active(&active_info))
        {
            ZYB_HTTP_LOG("HTTP NET PdpActive OK WIFI");
            HTTP_Net_StartPdpActiveTimer();
            return_val = TRUE;
        }
        else
        {
            ZYB_HTTP_LOG("HTTP NET PdpActive ERR WIFI");
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_FAIL,PNULL,0);
            return FALSE;
        }
    }
    else
#endif
    {
        uint32 sim_ok_num = 0;
        uint16 sim_ok = 0;
        
        sim_ok_num = MMIAPIPHONE_GetSimAvailableNum(&sim_ok,1);/*lint !e64*/
        if(0 == sim_ok_num)
        {
            ZYB_HTTP_LOG("HTTP NET PdpActive ERR NO SIM");
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_FAIL,PNULL,0);
            return FALSE;
        }

        return_val = ZYBHTTP_Net_PDPTry(&s_zyb_http_net_cur_sys);//第一张有效卡
        if(FALSE == return_val)
        {
            ZYB_HTTP_LOG("HTTP NET PdpActive ERR cur_sys(%d/%d),",s_zyb_http_net_cur_sys,sim_ok_num);
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_FAIL,PNULL,0);
        }
        else
        {
            ZYBHTTP_Net_NV_SetActiveSys(s_zyb_http_net_cur_sys);
        }
    }

    return return_val;
}

/*****************************************************************************/
//  Description : Deactive Pdp
//  Global resource dependence : none
//  Author: Gaily.Wang
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN ZYBHTTP_Net_PdpDeactive(void)
{
    BOOLEAN return_val = FALSE;
    ZYB_HTTP_LOG("ZYBHTTP_Net_PdpDeactive");
    return_val = MMIAPIPDP_Deactive(MMI_MODULE_ZYB);

    return return_val;
}

LOCAL BOOLEAN ZybHttp_Net_Init(void)
{
    MN_DUAL_SYS_E dual_sys = ZYBHTTP_Net_GetActiveSys();
    ZYB_HTTP_LOG("ZybHttp_Net_Init is_init=%d, dual_sys=%d",s_zyb_http_net_is_init,dual_sys);
    if(s_zyb_http_net_is_init == FALSE)
    {
        s_zyb_http_net_is_init = TRUE;
    }
    return TRUE;
}

LOCAL BOOLEAN ZybHttp_Net_Open(void)
{
    ZYB_HTTP_LOG("ZybHttp_Net_Open state=%d,is_activing=%d,is_closing=%d",s_zyb_http_net_pdp_state_is_ok,s_zyb_http_net_is_activing,s_zyb_http_net_is_closing);
    if(s_zyb_http_net_is_closing)
    {
        s_zyb_http_net_need_close_active = TRUE;
    }
    else
    {
        ZYBHTTP_Net_PdpActive();
    }
    return TRUE;
}

LOCAL BOOLEAN ZybHttp_Net_CloseEx(void)
{
    if(s_zyb_http_net_is_closing)
    {
        s_zyb_http_net_need_close_active = FALSE;
        return TRUE;
    }
    s_zyb_http_net_is_closing = TRUE;
    if(s_zyb_http_net_pdp_state_is_ok)
    {
        if(FALSE == ZYBHTTP_Net_PdpDeactive())
        {
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_CONNECT_FAIL,PNULL,0);
        }
    }
    else
    {
        ZYBHTTP_Net_PdpDeactive();
        MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_PDP_CONNECT_FAIL,PNULL,0);
    }
}

LOCAL BOOLEAN ZybHttp_Net_Close(void)
{
    ZYB_HTTP_LOG("ZybHttp_Net_Close state=%d,is_activing=%d,is_closing=%d",s_zyb_http_net_pdp_state_is_ok,s_zyb_http_net_is_activing,s_zyb_http_net_is_closing);
    if(s_zyb_http_net_is_activing)
    {
        s_zyb_http_net_need_active_close = TRUE;
    }
    else
    {
        ZybHttp_Net_CloseEx();
    }
    return TRUE;
}

LOCAL BOOLEAN ZybHttp_Net_Reset(void)
{
    ZYB_HTTP_LOG("ZybHttp_Net_Reset state=%d",s_zyb_http_net_pdp_state_is_ok);
    ZYBHTTP_Net_Close();
    ZYBHTTP_Net_Open();
    return TRUE;
}

LOCAL BOOLEAN ZybHttp_Net_CallBack(BOOLEAN is_ok)
{
    if(MMIZYB_HTTP_IsSending())
    {
        if(is_ok)
        {
            if(FALSE == ZybHttp_StartRequest())
            {
                MMIZYB_HTTP_PopSend_Result(FALSE,HTTP_ERROR_CONNECT);
            }
        }
        else
        {
            MMIZYB_HTTP_PopSend_Result(FALSE,HTTP_ERROR_CONNECT);
        }
    }
}


LOCAL BOOLEAN ZybHttp_Net_PdpStateChange(BOOLEAN is_ok)
{
    ZYB_HTTP_LOG("HTTP NET PDPStateChange %d",is_ok);
    return TRUE;
}

LOCAL void ZybHttp_Net_PdpStateUpdate(BOOLEAN is_ok,BOOLEAN is_active)
{
    ZYB_HTTP_LOG("ZybHttp_Net_PdpStateUpdate is_ok=%d,is_active=%d,activing=%d,closing=%d",is_ok,is_active,s_zyb_http_net_is_activing,s_zyb_http_net_is_closing);
    if(s_zyb_http_net_pdp_state_is_ok != is_ok)
    {
        s_zyb_http_net_pdp_state_is_ok = is_ok;
        ZybHttp_Net_PdpStateChange(is_ok);
    }
    
    if(is_active)
    {
        if(s_zyb_http_net_is_activing)
        {
            s_zyb_http_net_is_activing = FALSE;
            
            if(s_zyb_http_net_need_active_close)
            {
                ZYB_HTTP_LOG("ZybHttp_Net_PdpStateUpdate Active Close");
                s_zyb_http_net_need_active_close = FALSE;
                ZYBHTTP_Net_Close();
                ZybHttp_Net_CallBack(FALSE);
            }
            else
            {
                if(is_ok)
                {
                    ZybHttp_Net_CallBack(TRUE);
                }
            }
        }
    }
    else
    {   
        s_zyb_http_net_is_activing = FALSE;
        if(s_zyb_http_net_is_closing)
        {
            s_zyb_http_net_is_closing = FALSE;
            if(s_zyb_http_net_need_close_active)
            {
                ZYB_HTTP_LOG("ZybHttp_Net_PdpStateUpdate Close Active");
                s_zyb_http_net_need_close_active = FALSE;
                ZYBHTTP_Net_Open();
            }
        }
    }
    return;
}

PUBLIC uint32 ZYBHTTP_Net_GetNetID(void)
{
    return s_zyb_http_net_id;
}

PUBLIC BOOLEAN ZYBHTTP_Net_PdpStateGet(void)
{
    return s_zyb_http_net_pdp_state_is_ok;
}

PUBLIC BOOLEAN ZYBHTTP_Net_IsActiving(void)
{
    return s_zyb_http_net_is_activing;
}

PUBLIC BOOLEAN ZYBHTTP_Net_IsClosing(void)
{
    return s_zyb_http_net_is_closing;
}

PUBLIC BOOLEAN ZYBHTTP_Net_IsInit(void)
{
    return s_zyb_http_net_is_init;
}

PUBLIC BOOLEAN ZYBHTTP_Net_Init(void)
{
    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_NET_INIT,PNULL,0);
    return TRUE;
}

PUBLIC BOOLEAN ZYBHTTP_Net_Reset(void)
{
    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_NET_RESET,PNULL,0);
    return TRUE;
}

PUBLIC BOOLEAN ZYBHTTP_Net_Open(void)
{
    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_NET_OPEN,PNULL,0);
    return TRUE;
}

PUBLIC BOOLEAN ZYBHTTP_Net_Close(void)
{
    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_NET_CLOSE,PNULL,0);
    return TRUE;
}

LOCAL MN_DUAL_SYS_E ZYBHTTP_Net_NV_GetActiveSys(void)
{
    return s_zyb_http_net_set_sys;
}

PUBLIC BOOLEAN  ZYBHTTP_Net_NV_SetActiveSys(MN_DUAL_SYS_E set_sys) 
{
    uint8 net_sys = set_sys;
    if(s_zyb_http_net_set_sys != set_sys)
    {
        s_zyb_http_net_set_sys = set_sys;
        return TRUE;
    }
    return FALSE;
}

PUBLIC MN_DUAL_SYS_E ZYBHTTP_Net_GetActiveSys(void)
{
    return s_zyb_http_net_set_sys;
}

LOCAL BOOLEAN  ZYBHTTP_Net_Handle_Init(DPARAM param)
{
    ZYB_HTTP_LOG("ZYBHTTP_Net_Handle_Init");
    ZybHttp_Net_Init();
    return TRUE;
}

LOCAL BOOLEAN  ZYBHTTP_Net_Handle_Open(DPARAM param)
{
    ZYB_HTTP_LOG("ZYBHTTP_Net_Handle_Open");
    ZybHttp_Net_Open();
    return TRUE;
}

LOCAL BOOLEAN  ZYBHTTP_Net_Handle_Close(DPARAM param)
{
    ZYB_HTTP_LOG("ZYBHTTP_Net_Handle_Close");
    ZybHttp_Net_Close();
    return TRUE;
}

LOCAL BOOLEAN  ZYBHTTP_Net_Handle_Reset(DPARAM param)
{
    ZYB_HTTP_LOG("ZYBHTTP_Net_Handle_Reset");
    ZybHttp_Net_Reset();
    return TRUE;
}

LOCAL BOOLEAN  ZYBHTTP_Net_Handle_PDPActiveOK(DPARAM param)
{
    ZYB_HTTP_LOG("ZYBHTTP_Net_Handle_PDPActiveOK net_is_activing=%d",s_zyb_http_net_is_activing);
    ZybHttp_Net_PdpStateUpdate(TRUE,TRUE);
    return TRUE;
}

LOCAL BOOLEAN  ZYBHTTP_Net_Handle_PDPActiveFail(DPARAM param)
{
    ZYB_HTTP_LOG("ZYBHTTP_Net_Handle_PDPActiveFail net_is_activing=%d",s_zyb_http_net_is_activing);
    ZybHttp_Net_PdpStateUpdate(FALSE,TRUE);
    return TRUE;
}

LOCAL BOOLEAN  ZYBHTTP_Net_Handle_PDPRespond(DPARAM param)
{
    ZYB_HTTP_LOG("HTTP NET Handle_PDP_RSP net_is_activing=%d",s_zyb_http_net_is_activing);
    ZybHttp_Net_PdpStateUpdate(FALSE,FALSE);
    return TRUE;
}

PUBLIC MMI_RESULT_E ZYBHTTP_Net_Handle_AppMsg(PWND app_ptr, uint16 msg_id, DPARAM param)
{
    MMI_RESULT_E res = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case ZYBHTTP_APP_SIGNAL_NET_INIT:
            ZYBHTTP_Net_Handle_Init(param);
        break;
        
    case ZYBHTTP_APP_SIGNAL_NET_OPEN:
            ZYBHTTP_Net_Handle_Open(param);
        break;
        
    case ZYBHTTP_APP_SIGNAL_NET_CLOSE:
            ZYBHTTP_Net_Handle_Close(param);
        break;
        
    case ZYBHTTP_APP_SIGNAL_NET_RESET:
            ZYBHTTP_Net_Handle_Reset(param);
        break;
        
    case ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_SUCCESS:
            ZYBHTTP_Net_Handle_PDPActiveOK(param);
        break;
        
    case ZYBHTTP_APP_SIGNAL_PDP_ACTIVE_FAIL:
            ZYBHTTP_Net_Handle_PDPActiveFail(param);
        break;
        
    case ZYBHTTP_APP_SIGNAL_PDP_CONNECT_FAIL:
            ZYBHTTP_Net_Handle_PDPRespond(param);
        break;
    
    default:
            res = MMI_RESULT_FALSE;
        break;
    }

    if(res == MMI_RESULT_FALSE)
    {
    }
    return res;
}
#endif
