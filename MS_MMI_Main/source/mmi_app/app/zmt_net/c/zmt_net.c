
#include "zmt_net_export.h"
#include "zmt_net_main.h"
#include "zmt_net_http.h"
#include "mmidc_gui.h"
#include "sci_types.h"

#define ZMT_UPLOAD_HOST "117.50.185.119"
#define ZMT_UPLOAD_HOST_PORT 8080

#define ZMT_UPLOAD_AUDIO_URL "/front/voice/asr"
#define ZMT_UPLOAD_IMG_URL "/front/voice/asr"

#define ZMT_UPLOAD_AUDIO_CONTENT_TYPE "application/octet-stream"

static char zmt_upload_head[] = 
	"POST %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Connection: keep-alive\r\n"
	"Content-Length: %d\r\n"
    "Cache-Control: no-cache\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n"
	"Content-Type: multipart/form-data; boundary=%s\r\n"
    "Accept: */*\r\n"
    "Accept-Language: en-US,en;q=0.8,zh-CN;q=0.6,zh;q=0.4,zh-TW;q=0.2,es;q=0.2\r\n"
    "\r\n";

static char zmt_upload_parameters[] = 
	"Content-Disposition: form-data;\r\n\r\n";

static char zmt_upload_speech[] = 
    "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
    "Content-Type: %s\r\n\r\n";


LOCAL BOOLEAN  ZMT_Net_PDPTry(MN_DUAL_SYS_E sim_sys);
LOCAL BOOLEAN ZMT_Net_PdpDeactive(void);


LOCAL uint32 s_zmt_net_id = 0;
BOOLEAN m_zmt_net_is_open;
BOOLEAN m_zmt_net_is_init;
BOOLEAN m_zmt_is_reg;

BOOLEAN is_send_file_success  = FALSE;

ZMT_DATA_CONTENT_T * zmt_chatdata;

PUBLIC void ZMT_Net_TCP_Close(void)
{
    ZMT_Tcp_ForceCloseAll();
    MMIZMT_Net_Close();
}

static void ZMT_Net_SendCB(void *pUser,uint8 * pRcv,uint32 Rcv_len)
{
    uint16 flag= 0;
    int i = 0;
    int j = 0;
    char json_str[4096]={0};
    cJSON *rcvJsonPtr;
    char * result;

    result=SCI_ALLOCA(Rcv_len+1);
    SCI_MEMSET(result,0,Rcv_len+1);
    SCI_MEMCPY(result,pRcv,Rcv_len);

    for (i = 0 ,j = 0;i < Rcv_len ; i++)
    {
        if(flag!=0)
        {
            if(result[i]!='\n'&&result[i]!='\r'&&result[i]!='\0')
            {
                json_str[j++]=result[i];
            }
        }
        if(result[i] == '{'&&flag==0)
        {
            flag = i;
            json_str[j++]=result[i];
        }
        else if(result[i] == '}')
        {
            json_str[j++]='\0';
            break;
        }
    }

    SCI_TRACE_LOW("[ZMT_NET]  %s: len = %d", __FUNCTION__, strlen(json_str));
    //SCI_TRACE_LOW("[ZMT_NET]  %s: json_str = %s", __FUNCTION__, json_str);
    
    SCI_FREE(result);
}

LOCAL void get_rand_str(char s[], int number)
{
    char *str = "567895678945678994567893456789345678834567893456";
    int i,lstr;
    char ss[2] = {0};
	 uint32 time = SCI_GetTickCount();
    lstr = strlen(str);
    
       srand(time);
    for(i = 1; i <= number; i++){
       sprintf(ss,"%c",str[(rand()%lstr)]);
       strcat(s,ss);
    }
}

PUBLIC uint32 ZMT_Net_TCPSendFile(ZMT_DATA_CONTENT_T * chatdata)
{
    ZMT_TCP_LINK_DATA_T * cur_data = NULL;
    uint8 *fileData = NULL; 
    uint32 len = 0;
    uint32 head_len = 0;
    uint32 read_len = 0;
    uint32 file_len = 0;
    uint32 all_len = 0;
    char *boundary_header = "------AiWiFiBoundary";
    char* end = "\r\n";
    char* twoHyphens = "--";
    char s[20] = {0};
    char *boundary =PNULL;
    char firstBoundary[128]={0};
    char secondBoundary[128]={0};
    char endBoundary[128]={0};
    char speech_tmp[1024]={0};
    char *parameter_data=PNULL;
    int content_length =0;
    char header_data[4096] = {0};
    int ret = 0;

    get_rand_str(s,19);
    SCI_TRACE_LOW("[ZMT_NET] %s: s = %s", __FUNCTION__, s);
    boundary = SCI_ALLOC_APPZ(strlen(boundary_header)+strlen(s) +1);
    memset(boundary, 0, strlen(boundary_header)+strlen(s) +1);
    strcat(boundary, boundary_header);
    strcat(boundary, s);
    SCI_TRACE_LOW("[ZMT_NET] %s: boundary = %s", __FUNCTION__, boundary);
    sprintf(firstBoundary, "%s%s%s", twoHyphens, boundary, end);
    sprintf(secondBoundary, "%s%s%s%s", end, twoHyphens, boundary, end);
    sprintf(endBoundary, "%s%s%s%s%s", end, twoHyphens, boundary, twoHyphens, end);

    file_len = chatdata->data_len;
    SCI_TRACE_LOW("[ZMT_NET] %s: file_len = %d", __FUNCTION__, file_len);

    if(chatdata->data_type == ZMT_DATA_AUDIO)
    {
        sprintf(speech_tmp,zmt_upload_speech,"file","test.amr",ZMT_UPLOAD_AUDIO_CONTENT_TYPE);
    }
    else
    {
        sprintf(speech_tmp,zmt_upload_speech,"file","test.jpg",ZMT_UPLOAD_AUDIO_CONTENT_TYPE);
    }

    parameter_data = SCI_ALLOC_APPZ(strlen(zmt_upload_parameters) + strlen(boundary) + strlen(end)*2 + strlen(twoHyphens) +1);
    strcpy(parameter_data, zmt_upload_parameters);
    strcat(parameter_data, secondBoundary); 

    content_length = len+ strlen(boundary)*2 + strlen(parameter_data) + strlen(speech_tmp) + strlen(end)*3 + strlen(twoHyphens)*3 + file_len;
    if(chatdata->data_type == ZMT_DATA_AUDIO)
    {
        ret = _snprintf(header_data,4096, zmt_upload_head, ZMT_UPLOAD_AUDIO_URL, ZMT_UPLOAD_HOST, content_length, boundary);
    }
    else
    {
        ret = _snprintf(header_data,4096, zmt_upload_head, ZMT_UPLOAD_AUDIO_URL, ZMT_UPLOAD_HOST, content_length, boundary);
    }

    strcat(header_data,firstBoundary);

    SCI_TRACE_LOW("[ZMT_NET] %s: firstBoundary = %s", __FUNCTION__, firstBoundary);
    strcat(header_data,parameter_data);   
    strcat(header_data,speech_tmp);

    head_len = strlen(header_data);

    SCI_TRACE_LOW("[ZMT_NET] %s: head_len = %d ", __FUNCTION__, head_len);

    cur_data=(ZMT_TCP_LINK_DATA_T *)SCI_ALLOC_APPZ(sizeof(ZMT_TCP_LINK_DATA_T));
    if(cur_data == NULL)
    {
        SCI_TRACE_LOW("[ZMT_NET] %s: ERR Malloc cur_data", __FUNCTION__);
        return 0;
    }

    cur_data->str= (uint8 *)SCI_ALLOC_APPZ( head_len+file_len+200);
    if(cur_data->str == NULL)
    {
        SCI_TRACE_LOW("[ZMT_NET] %s: ERR Malloc cur_data->str",  __FUNCTION__);
        SCI_FREE(cur_data);
        return 0;
    }
    	

    SCI_MEMCPY(cur_data->str,header_data,head_len);
    fileData = cur_data->str+head_len;
    SCI_MEMCPY(fileData,chatdata->data,file_len);
    read_len = file_len;
    
    len = strlen(endBoundary);
    SCI_TRACE_LOW("[ZMT_NET] %s: file_len=%d ,read_len=%d endBoundaryLen=%d",  __FUNCTION__, file_len, read_len, len);
    SCI_MEMCPY(cur_data->str+head_len+read_len,endBoundary,len);
    all_len = head_len + read_len + len;

    cur_data->pMe = m_zmt_tcp_reg_interface;
    cur_data->len = all_len;
    cur_data->times = 0;
    cur_data->timeout = 0;
    cur_data->priority = 0;
    if(chatdata->rec_handle != NULL){
        cur_data->rcv_handle = chatdata->rec_handle;
    }else{
        cur_data->rcv_handle = ZMT_Net_SendCB;
    }
    SCI_TRACE_LOW("[ZMT_NET] %s: cur_data->str len=%d ", __FUNCTION__, SCI_STRLEN(cur_data->str));
    ZMT_TCP_PostEvtTo_APP(ZMT_APP_SIGNAL_TCP_WRITE_ADD,cur_data,all_len);

    SCI_FREE(boundary);
    SCI_FREE(parameter_data);

    SCI_TRACE_LOW("[ZMT_NET] %s: Handle=0x%x", __FUNCTION__, (uint32)cur_data);
    return (uint32)cur_data;
}
 
static int ZMT_Net_Reg_CallBack(void * pUser, ZMT_TCP_CB_TYPE_E cb_type, uint16 wParam, uint32 dwParam)
{
    ZMT_TCP_INTERFACE_T *pTcp = (ZMT_TCP_INTERFACE_T *)pUser;
    SCI_TRACE_LOW("[ZMT_NET]  %s: cb_type=%d,Result=%d,Data=0x%x", __FUNCTION__, cb_type,wParam,dwParam);

    switch(cb_type)
    {
        case TCP_CB_DISCONNECTED:
            SCI_TRACE_LOW("[ZMT_NET]  %s: TCP_CB_DISCONNECTED", __FUNCTION__);
            break;
        case TCP_CB_REG:
            SCI_TRACE_LOW("[ZMT_NET]  %s: TCP_CB_REG", __FUNCTION__);	
            break;
        case TCP_CB_SEND:
            {
                uint16 ret = wParam;
                ZMT_TCP_LINK_DATA_T * SendData = (ZMT_TCP_LINK_DATA_T*)dwParam;
                SCI_TRACE_LOW("[ZMT_NET]  %s: TCP_CB_SEND ret = %d, Handle = 0x%x", __FUNCTION__, ret, SendData);
                if(ret == TCP_SUCCESS)
                {
                    SCI_TRACE_LOW("[ZMT_NET]  %s: TCP_CB_SEND Success Handle=0x%x", __FUNCTION__, SendData);
                    if(SendData != NULL && SendData->str != NULL)
                    {		
                        //SCI_TRACE_LOW("[ZMT_NET]  %s: SendData->str = %s", __FUNCTION__, SendData->str);
                    }
                }
            }
            break;
        case TCP_CB_RCV:
            {
                uint16 ret = wParam;
                ZMT_TCP_RCV_DATA_T * RcvData = (ZMT_TCP_RCV_DATA_T *) dwParam;
                if(ret == TCP_SUCCESS)
                {
                    SCI_TRACE_LOW("[ZMT_NET]  %s: TCP_CB_RCV Over", __FUNCTION__);
                }
                else
                {
                    SCI_TRACE_LOW("[ZMT_NET]  %s: TCP_CB_RCV FAIL", __FUNCTION__);
                }
            }
            break;                
        default:
            break;
    }
    return 0;
}

BOOLEAN ZMT_Net_IsInit(void)
{
    return m_zmt_net_is_init;
}

PUBLIC BOOLEAN ZMT_Net_Is_SendFile_Success(void)
{
    return is_send_file_success;
}


BOOLEAN  MMIZMT_SendSigTo_APP(ZMT_APP_SIG_E sig_id, uint8 * data_ptr, uint32 data_len)
{
    uint8 * pstr = NULL;
    MMI_ZMT_SIG_T * psig = PNULL;
    SCI_TRACE_LOW("[ZMT_NET] %s: start ", __FUNCTION__);
    if(data_ptr != PNULL && data_len != 0)
    {
        pstr = SCI_ALLOCA(data_len);//free it in AT task
        if (pstr == PNULL)
        {
                SCI_PASSERT(0, ("[ZMT_NET] MMIZMT_SendSigTo_APP Alloc  %ld FAIL",data_len));
                return FALSE;
        }
        SCI_MEMCPY(pstr,data_ptr,data_len);
    }
    
    //send signal to AT to write uart
    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,sig_id,sizeof(MMI_ZMT_SIG_T),SCI_IdentifyThread());
    psig->data.len = data_len;
    psig->data.str = pstr;
		
    SCI_SEND_SIGNAL((xSignalHeaderRec*)psig,P_APP);
    SCI_TRACE_LOW("[ZMT_NET] %s: end", __FUNCTION__);
    return TRUE;
}


int ZMT_Net_Reg(void)
{
    SCI_TRACE_LOW("[ZMT_NET] ZMT_Net_Reg ");
    if(FALSE == ZMT_Net_IsInit())
    {
        return 1;
    }

    ZMT_API_TCP_UnReg(m_zmt_tcp_reg_interface);
    
    {
        ZMT_API_TCP_Reg(m_zmt_tcp_reg_interface,ZMT_UPLOAD_HOST,ZMT_UPLOAD_HOST_PORT,ZMT_Net_Reg_CallBack);
    }

    return 0;
}


extern BOOLEAN ZMT_Net_IsOpen()
{
    return m_zmt_net_is_open;
}

extern int ZMT_Net_Open()
{
    SCI_TRACE_LOW("[ZMT_NET] ZMT_Net_Open");
    m_zmt_is_reg = FALSE;
    ZMT_Net_Reg();
    m_zmt_net_is_open = TRUE;
    return 0;
}

extern int ZMT_Net_Close()
{
    SCI_TRACE_LOW("[ZMT_NET] ZMT_Net_Close");
    m_zmt_is_reg = FALSE;
    if(ZMT_Net_IsInit())
    {
        ZMT_API_TCP_UnReg(m_zmt_tcp_reg_interface);
    }
    m_zmt_net_is_open = FALSE;
   return 0;
}


extern BOOLEAN ZMT_Net_IsAllow()
{
    if( m_zmt_net_is_init)
    {
        return TRUE;
    }
    SCI_TRACE_LOW("[ZMT_NET] ZMT_Net_IsAllow: FALSE");
    return FALSE;
}


int ZMT_Net_Init(void)
{
    SCI_TRACE_LOW("[ZMT_NET] ZMT_Net_Init");
    if(ZMTTCP_NetOpen(&(m_zmt_tcp_reg_interface),PNULL,10240))
    {
        SCI_TRACE_LOW("[ZMT_NET] ZMT_Net_Init Reg OK ");
    }
  
    m_zmt_net_is_init = TRUE;
    if(ZMT_Net_IsAllow())
    {
        ZMT_Net_Open();
    }
    return 0;
}


extern int ZMT_Net_Reset()
{
    SCI_TRACE_LOW("[ZMT_NET] ZMT_Net_Reset");
    if(FALSE == ZMT_Net_IsInit())
    {
        return 1;
    }
    if(ZMT_Net_IsAllow())
    {
       ZMT_Net_Open();
    }
    else
    {
       ZMT_Net_Close();
    }
    return 0;
}

#define ZMT_PDP_ACTIVE_TIMEOUT       30000  
#define ZMT_PDP_RETRY_TIMEOUT        2000 

#define ZMT_PDP_DEFAULT_SIM_SYS  MN_DUAL_SYS_1

LOCAL MN_DUAL_SYS_E       s_zmt_net_set_sys   = ZMT_PDP_DEFAULT_SIM_SYS;
LOCAL MN_DUAL_SYS_E       s_zmt_net_cur_sys   = MN_DUAL_SYS_1;
LOCAL uint8                       s_zmt_net_pdp_timer_id      = 0;
LOCAL uint8                       s_zmt_net_pdp_retry_timer_id = 0;

LOCAL BOOLEAN                  s_zmt_net_pdp_state_is_ok        = FALSE;
LOCAL BOOLEAN                  s_zmt_net_is_init        = FALSE;
LOCAL BOOLEAN                  s_zmt_net_need_init        = TRUE;
LOCAL BOOLEAN                  s_zmt_net_gprs_status[MN_DUAL_SYS_MAX+1] = {0};
LOCAL BOOLEAN                  s_zmt_net_is_activing        = FALSE;
LOCAL BOOLEAN                  s_zmt_net_is_closing        = FALSE;

LOCAL BOOLEAN                  s_zmt_net_need_active_close        = FALSE;
LOCAL BOOLEAN                  s_zmt_net_need_close_active        = FALSE;



/*****************************************************************************/
//  Description : handle socket connect timeout
//  Global resource dependence : none
//  Author: Gaily.Wang
//  Note:
/*****************************************************************************/
LOCAL void ZMT_Net_HandlePdpActiveTimer(
                                            uint8 timer_id,
                                            uint32 param
                                            )
{
    SCI_TRACE_LOW("[ZMT_NET] PdpActive ERR TimeOut");
    ZMT_Net_PdpDeactive();
    MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
}

/*****************************************************************************/
//  Discription: Start socket connect timer
//  Global resource dependence: None
//  Author: Gaily.Wang
//  Note : 
/*****************************************************************************/
LOCAL void ZMT_Net_StartPdpActiveTimer(void)
{
    if(0 != s_zmt_net_pdp_timer_id)
    {
        MMK_StopTimer(s_zmt_net_pdp_timer_id);
        s_zmt_net_pdp_timer_id = 0;
    }
    
    s_zmt_net_pdp_timer_id = MMK_CreateTimerCallback(ZMT_PDP_ACTIVE_TIMEOUT,ZMT_Net_HandlePdpActiveTimer,PNULL,FALSE);
}
/*****************************************************************************/
//  Discription: Start socket connect timer
//  Global resource dependence: None
//  Author: Gaily.Wang
//  Note : 
/*****************************************************************************/
LOCAL void ZMT_Net_StopPdpActiveTimer(void)
{
    if(0 != s_zmt_net_pdp_timer_id)
    {
        MMK_StopTimer(s_zmt_net_pdp_timer_id);
        s_zmt_net_pdp_timer_id = 0;
    }
}

LOCAL void ZMT_Net_HandlePdpRetryTimer(
                                uint8 timer_id,
                                uint32 param
                                )
{
    MN_DUAL_SYS_E sys_sim = 0;
    BOOLEAN       sim_ok =  FALSE;

    if(timer_id == s_zmt_net_pdp_retry_timer_id && 0 != s_zmt_net_pdp_retry_timer_id)
    {
        MMK_StopTimer(s_zmt_net_pdp_retry_timer_id);
        s_zmt_net_pdp_retry_timer_id = 0;
    }
    s_zmt_net_cur_sys++;
    if(s_zmt_net_cur_sys < MMI_DUAL_SYS_MAX)
    {
        for(sys_sim = s_zmt_net_cur_sys; sys_sim < MMI_DUAL_SYS_MAX; sys_sim++)
        {
            if(MMIPHONE_IsSimOk(sys_sim))
            {
                s_zmt_net_cur_sys = sys_sim;
                sim_ok = TRUE;
                break;
            }
        }
        if(sim_ok)
        {
            if(!ZMT_Net_PDPTry(s_zmt_net_cur_sys))
            {
                SCI_TRACE_LOW("[ZMT_NET] PdpActive Retry ERR cur_sys(%d),",s_zmt_net_cur_sys);
        	    s_zmt_net_pdp_retry_timer_id = MMK_CreateTimerCallback(ZMT_PDP_RETRY_TIMEOUT,ZMT_Net_HandlePdpRetryTimer,PNULL,FALSE);
            }
        }
        else
        {
            SCI_TRACE_LOW("[ZMT_NET] PdpActive Retry ERR NO SIM");
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
        }
    }
    else
    {
        SCI_TRACE_LOW("[ZMT_NET] PdpActive Retry ERR no_sys");
        MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
    }
}

/*****************************************************************************
//  Description : handle pdp msg 
//  Global resource dependence : none
//  Author: Gaily.Wang
//  Note:
*****************************************************************************/
LOCAL void ZMT_Net_HandlePdpMsg(MMIPDP_CNF_INFO_T *msg_ptr)
{
    if(PNULL == msg_ptr)
    {
        return;
    }
    
    SCI_TRACE_LOW("[ZMT_NET] %s: msg_id=%d,result=%d", __FUNCTION__, msg_ptr->msg_id,msg_ptr->result);
    
    switch(msg_ptr->msg_id) 
    {
    case MMIPDP_ACTIVE_CNF:
        ZMT_Net_StopPdpActiveTimer();    
        if(MMIPDP_RESULT_SUCC == msg_ptr->result)
        {
            s_zmt_net_id = msg_ptr->nsapi;
            SCI_TRACE_LOW("[ZMT_NET] %s: MMIPDP_ACTIVE_CNF OK net_id=0x%x", __FUNCTION__, s_zmt_net_id);
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_SUCCESS,PNULL,0);
        }
        else if (MMIPDP_RESULT_FAIL == msg_ptr->result)
        {
            SCI_TRACE_LOW("[ZMT_NET] %s: MMIPDP_ACTIVE_CNF FAIL", __FUNCTION__);
            ZMT_Net_PdpDeactive();
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
        }
        else 
        {
            SCI_TRACE_LOW("[ZMT_NET] %s: MMIPDP_ACTIVE_CNF FAIL Other", __FUNCTION__);
            ZMT_Net_PdpDeactive();
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
        }        
        break;
        
    case MMIPDP_DEACTIVE_CNF:
            SCI_TRACE_LOW("[ZMT_NET] %s: MMIPDP_DEACTIVE_CNF", __FUNCTION__);
            s_zmt_net_id = 0;
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_CONNET_FAIL,PNULL,0);
        break;
        
    case MMIPDP_DEACTIVE_IND:
            SCI_TRACE_LOW("[ZMT_NET] %s: MMIPDP_DEACTIVE_IND", __FUNCTION__);
            s_zmt_net_id = 0;
            ZMT_Net_PdpDeactive();
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_CONNET_FAIL,PNULL,0);
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
LOCAL BOOLEAN  ZMT_Net_PDPTry(MN_DUAL_SYS_E sim_sys)
{
#if defined(MMI_GPRS_SUPPORT)
    BOOLEAN                 return_val      = FALSE;
    MMIPDP_ACTIVE_INFO_T    active_info     = {0};
    MMICONNECTION_APNTYPEINDEX_T *apn_idx_arr;
    MMICONNECTION_LINKSETTING_DETAIL_T* linksetting = PNULL;

    SCI_TRACE_LOW("[ZMT_NET] %s: ConnectPDP", __FUNCTION__);

    if (FALSE == MMIAPIPHONE_GetDataServiceSIM(&sim_sys))
    {
        SCI_TraceLow("[ZMT_NET] %s: ConnectPDP [SIM ERROR]", __FUNCTION__);
        return FALSE;
    }

    active_info.app_handler         = MMI_MODULE_ZMT_NET;
    active_info.dual_sys            = sim_sys;
    active_info.apn_ptr             = NULL;
    active_info.user_name_ptr       = NULL;
    active_info.psw_ptr             = NULL;
    active_info.priority            = 3;
    active_info.ps_service_rat      = MN_TD_PREFER;
    active_info.ps_interface        = MMIPDP_INTERFACE_GPRS;
    active_info.handle_msg_callback = ZMT_Net_HandlePdpMsg;
    active_info.ps_service_type     = BROWSER_E;
    active_info.storage             = MN_GPRS_STORAGE_ALL;

    apn_idx_arr = MMIAPICONNECTION_GetApnTypeIndex(sim_sys);
    if (apn_idx_arr == NULL)
    {
        SCI_TRACE_LOW("[ZMT_NET] %s: ConnectPDP [APN IDX ERROR]", __FUNCTION__);
        return FALSE;
    }
    SCI_TRACE_LOW("[ZMT_NET] %s: ConnectPDP [sim = %d, Net Setting = %d]", __FUNCTION__, sim_sys, apn_idx_arr->internet_index[sim_sys].index);
    linksetting = MMIAPICONNECTION_GetLinkSettingItemByIndex(sim_sys, apn_idx_arr->internet_index[sim_sys].index);

    if(PNULL != linksetting && 0 != linksetting->apn_len)
    {
        active_info.apn_ptr = (char*)linksetting->apn;
        active_info.user_name_ptr = (char*)linksetting->username;
        active_info.psw_ptr = (char*)linksetting->password;
#ifdef IPVERSION_SUPPORT_V4_V6
        active_info.ip_type = linksetting->ip_type;
#endif
        if(MMIAPIPDP_Active(&active_info))
        {
            SCI_TRACE_LOW("[ZMT_NET] %s: PdpActive GPRS sim_sys=%d", __FUNCTION__, sim_sys);
            ZMT_Net_StartPdpActiveTimer();
            return_val = TRUE;
        }
    }
    return return_val;
#endif
    return FALSE;
}

LOCAL BOOLEAN ZMT_Net_PdpActive(void)
{
    BOOLEAN                 return_val      = FALSE;
    MN_DUAL_SYS_E           dual_sys        = MN_DUAL_SYS_MAX;
    MMIPDP_ACTIVE_INFO_T    active_info     = {0};
    
    SCI_TRACE_LOW("[ZMT_NET] %s: is_activing=%d,is_ok=%d",__FUNCTION__,s_zmt_net_is_activing,s_zmt_net_pdp_state_is_ok);
    if(s_zmt_net_is_activing)
    {
        s_zmt_net_need_active_close = FALSE;
        return TRUE;
    }
    s_zmt_net_is_activing = TRUE;
    
	#if 0
    if(s_dslchat_net_id == 0 && MMIZDT_Net_GetNetID() == 0)
    {
        s_zmt_net_pdp_state_is_ok =  FALSE;
    }
    #endif
	
    if(s_zmt_net_pdp_state_is_ok)
    {
        MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_SUCCESS,PNULL,0);
        return TRUE;
    }

    {
        uint32 sim_ok_num = 0;
        uint16 sim_ok = 0;
        
        sim_ok_num = MMIAPIPHONE_GetSimAvailableNum(&sim_ok,1);/*lint !e64*/
        if(0 == sim_ok_num)
        {
            SCI_TRACE_LOW("[ZMT_NET] %s: PdpActive ERR NO SIM",__FUNCTION__);
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
            return FALSE;
        }
        
        if(s_zmt_net_is_init == FALSE)
        {
            SCI_TRACE_LOW("[ZMT_NET] %s: PdpActive ERR NO INIT",__FUNCTION__);
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
            return FALSE;
        }


        if(s_zmt_net_set_sys < MN_DUAL_SYS_MAX)
        {
            if(MMIAPIPHONE_IsSimAvailable(s_zmt_net_set_sys))
            {
                s_zmt_net_cur_sys = s_zmt_net_set_sys;
            }
            else
            {
                SCI_TRACE_LOW("[ZMT_NET] %s: PdpActive ERR set_sys(%d) NO SIM",__FUNCTION__,s_zmt_net_set_sys);
                MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
                return FALSE;
            }
        }
        else
        {
            s_zmt_net_cur_sys = sim_ok;
        }

        return_val = ZMT_Net_PDPTry(s_zmt_net_cur_sys);
        if(FALSE == return_val)
        {
            SCI_TRACE_LOW("[ZMT_NET] %s: PdpActive ERR cur_sys(%d/%d)",__FUNCTION__, s_zmt_net_cur_sys,sim_ok_num);
            {
                MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_ACTIVE_FAIL,PNULL,0);
            }
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
LOCAL BOOLEAN ZMT_Net_PdpDeactive(void)
{
    BOOLEAN return_val = FALSE;
    
    return_val = MMIAPIPDP_Deactive(MMI_MODULE_ZMT_NET);

    return return_val;
}

LOCAL BOOLEAN ZMT_PDP_Net_Init(void)
{
    MN_DUAL_SYS_E dual_sys = MMIZMT_Net_GetActiveSys();
    SCI_TRACE_LOW("[ZMT_NET] %s: is_init=%d, dual_sys=%d",__FUNCTION__,s_zmt_net_is_init,dual_sys);
    if(s_zmt_net_is_init == FALSE)
    {
        s_zmt_net_is_init = TRUE;
    }
    return TRUE;
}

// to active pdp ,connect to  pdp network
LOCAL BOOLEAN ZMT_PDP_Net_Open(void)
{
    SCI_TRACE_LOW("[ZMT_NET] %s: state=%d,is_activing=%d,is_closing=%d",__FUNCTION__,s_zmt_net_pdp_state_is_ok,s_zmt_net_is_activing,s_zmt_net_is_closing);
    if(s_zmt_net_is_closing)
    {
        s_zmt_net_need_close_active = TRUE;
    }
    else
    {
        ZMT_Net_PdpActive();
    }
    return TRUE;
}

LOCAL BOOLEAN ZMT_PDP_Net_CloseEx(void)
{
    if(s_zmt_net_is_closing)
    {
        s_zmt_net_need_close_active = FALSE;
        return TRUE;
    }
    s_zmt_net_is_closing = TRUE;
    if(s_zmt_net_pdp_state_is_ok)
    {
        if(FALSE == ZMT_Net_PdpDeactive())
        {
            MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_CONNET_FAIL,PNULL,0);
        }
    }
    else
    {
        ZMT_Net_PdpDeactive();
        MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_CONNET_FAIL,PNULL,0);
    }
}

LOCAL BOOLEAN ZMT_PDP_Net_Close(void)
{
    SCI_TRACE_LOW("[ZMT_NET] %s: state=%d,is_activing=%d,is_closing=%d",__FUNCTION__, s_zmt_net_pdp_state_is_ok,s_zmt_net_is_activing,s_zmt_net_is_closing);
    if(s_zmt_net_is_activing)
    {
        s_zmt_net_need_active_close = TRUE;
    }
    else
    {
        ZMT_PDP_Net_CloseEx();
    }
    return TRUE;
}

LOCAL BOOLEAN ZMT_PDP_Net_Reset(void)
{
    SCI_TRACE_LOW("[ZMT_NET] %s: state=%d",__FUNCTION__, s_zmt_net_pdp_state_is_ok);
    MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_CLOSE,PNULL,0);
    MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_OPEN,PNULL,0);
    return TRUE;
}

LOCAL BOOLEAN ZMT_Net_PdpStateChange(BOOLEAN is_ok)
{
    SCI_TRACE_LOW("[ZMT_NET] %s: PDPStateChange %d",__FUNCTION__,is_ok);
    return TRUE;
}

LOCAL void ZMT_Net_PdpStateUpdate(BOOLEAN is_ok,BOOLEAN is_active)
{
    SCI_TRACE_LOW("[ZMT_NET] %s: is_ok=%d,is_active=%d,activing=%d,closing=%d",__FUNCTION__,is_ok,is_active,s_zmt_net_is_activing,s_zmt_net_is_closing);
    if(s_zmt_net_pdp_state_is_ok != is_ok)
    {
        s_zmt_net_pdp_state_is_ok = is_ok;
        ZMT_Net_PdpStateChange(is_ok);
    }
    
    if(is_active)
    {
        if(s_zmt_net_is_activing)
        {
            s_zmt_net_is_activing = FALSE;
            if(s_zmt_net_need_active_close)
            {
                SCI_TRACE_LOW("[ZMT_NET] %s: Active Close",__FUNCTION__);
                s_zmt_net_need_active_close = FALSE;
                MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_CLOSE,PNULL,0);
            }
        }
    }
    else
    {   
        s_zmt_net_is_activing = FALSE;
        if(s_zmt_net_is_closing)
        {
            s_zmt_net_is_closing = FALSE;
            if(s_zmt_net_need_close_active)
            {
                SCI_TRACE_LOW("[ZMT_NET] %s: Close Active",__FUNCTION__);
                s_zmt_net_need_close_active = FALSE;
                MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_OPEN,PNULL,0);
            }
        }
    }
    return;
}

PUBLIC uint32 MMIZMT_Net_GetNetID(void)
{
    return s_zmt_net_id;
}

PUBLIC BOOLEAN MMIZMT_Net_PdpStateGet(void)
{
    return s_zmt_net_pdp_state_is_ok;
}

PUBLIC BOOLEAN MMIZMT_Net_IsActiving(void)
{
    return s_zmt_net_is_activing;
}

PUBLIC BOOLEAN MMIZMT_Net_IsClosing(void)
{
    return s_zmt_net_is_closing;
}


PUBLIC BOOLEAN MMIZMT_Net_IsInit(void)
{
    return s_zmt_net_is_init;
}

//send msg  ,to init open pdp ,
PUBLIC BOOLEAN MMIZMT_Net_Init(void)
{
    SCI_TRACE_LOW("[ZMT_NET] %s", __FUNCTION__);
    MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_INIT,PNULL,0);
    return TRUE;
}


PUBLIC BOOLEAN MMIZMT_Net_Reset(void)
{
    MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_RESET,PNULL,0);
    return TRUE;
}

//send msg  ,to init open pdp ,
PUBLIC BOOLEAN MMIZMT_Net_Open(void)
{
    MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_OPEN,PNULL,0);
    return TRUE;
}

PUBLIC BOOLEAN MMIZMT_Net_Close(void)
{
    MMIZMT_SendSigTo_APP(ZMT_APP_SIGNAL_NET_CLOSE,PNULL,0);
    return TRUE;
}

LOCAL MN_DUAL_SYS_E MMIZMT_Net_NV_GetActiveSys(void)
{
    MN_RETURN_RESULT_E      return_value = MN_RETURN_FAILURE;
    uint8 net_sys = ZMT_PDP_DEFAULT_SIM_SYS;
    s_zmt_net_set_sys = net_sys;
    return net_sys;
}

LOCAL BOOLEAN  MMIZMT_Net_NV_SetActiveSys(MN_DUAL_SYS_E set_sys) 
{
    return TRUE;
}

PUBLIC MN_DUAL_SYS_E MMIZMT_Net_GetActiveSys(void)
{
    return s_zmt_net_set_sys;
}

PUBLIC void MMIZMT_Net_SetActiveSys(MN_DUAL_SYS_E set_sys) 
{
    if(MMIZMT_Net_NV_SetActiveSys(set_sys))
    {
        if(s_zmt_net_pdp_state_is_ok)
        {
            if(set_sys < MN_DUAL_SYS_MAX && s_zmt_net_cur_sys != set_sys)
            {
                MMIZMT_Net_Reset();
            }
        }
        else
        {
            if(s_zmt_net_is_activing)
            {
                 if(set_sys < MN_DUAL_SYS_MAX && s_zmt_net_cur_sys != set_sys)
                 {
                    MMIZMT_Net_Reset();
                 }
            }
        }
    }
    return;
}

LOCAL BOOLEAN  MMIZMT_Net_Handle_Init(DPARAM param)
{
    SCI_TRACE_LOW("[ZMT_NET] %s", __FUNCTION__);
    ZMT_PDP_Net_Init();
    return TRUE;
}

LOCAL BOOLEAN  MMIZMT_Net_Handle_Open(DPARAM param)
{
    SCI_TRACE_LOW("[ZMT_NET] %s", __FUNCTION__);
    ZMT_PDP_Net_Open();
    return TRUE;
}

LOCAL BOOLEAN  MMIZMT_Net_Handle_Close(DPARAM param)
{
    SCI_TRACE_LOW("[ZMT_NET] %s", __FUNCTION__);
    ZMT_PDP_Net_Close();
    return TRUE;
}

LOCAL BOOLEAN  MMIZMT_Net_Handle_Reset(DPARAM param)
{
    SCI_TRACE_LOW("[ZMT_NET] %s", __FUNCTION__);
    ZMT_PDP_Net_Reset();
    return TRUE;
}

LOCAL BOOLEAN  MMIZMT_Net_Handle_PDPActiveOK(DPARAM param)
{
    SCI_TRACE_LOW("[ZMT_NET] %s: net_is_activing=%d", __FUNCTION__,s_zmt_net_is_activing);
    ZMT_Net_PdpStateUpdate(TRUE,TRUE);

    MMIZMT_TCP_Handle_PDPActiveOK();
    return TRUE;
}

LOCAL BOOLEAN  MMIZMT_Net_Handle_PDPActiveFail(DPARAM param)
{
    SCI_TRACE_LOW("[ZMT_NET] %s: net_is_activing=%d", __FUNCTION__,s_zmt_net_is_activing);
    ZMT_Net_PdpStateUpdate(FALSE,TRUE);

    MMIZMT_TCP_Handle_PDPActiveFail();
    return TRUE;
}

LOCAL BOOLEAN  MMIZMT_Net_Handle_PDPRespond(DPARAM param)
{
    SCI_TRACE_LOW("[ZMT_NET] %s: net_is_activing=%d", __FUNCTION__,s_zmt_net_is_activing);
    ZMT_Net_PdpStateUpdate(FALSE,FALSE);

    MMIZMT_TCP_Handle_PDPRespond();
    return TRUE;
}


PUBLIC MMI_RESULT_E MMIZMT_Net_Handle_AppMsg(PWND app_ptr, uint16 msg_id, DPARAM param)
{
    MMI_RESULT_E res = MMI_RESULT_TRUE;
    SCI_TRACE_LOW("[ZMT_NET] %s: msg_id = %d",__FUNCTION__, msg_id);

    switch(msg_id)
    {
    case ZMT_APP_SIGNAL_NET_INIT:
		{
			SCI_TRACE_LOW("[ZMT_NET] %s: ZMT_APP_SIGNAL_NET_INIT ",__FUNCTION__); 
			MMIZMT_Net_Handle_Init(param);  //init or open pdp
			break;
		}
    case ZMT_APP_SIGNAL_NET_OPEN:
		{  
			SCI_TRACE_LOW("[ZMT_NET] %s: ZMT_APP_SIGNAL_NET_OPEN ",__FUNCTION__);    
			MMIZMT_Net_Handle_Open(param);   // open pdp
			break;
		}
    case ZMT_APP_SIGNAL_NET_CLOSE:
		{
			SCI_TRACE_LOW("[ZMT_NET] %s: ZMT_APP_SIGNAL_NET_CLOSE ",__FUNCTION__);
			MMIZMT_Net_Handle_Close(param);  // detach pdp
			break;
        }
    case ZMT_APP_SIGNAL_NET_RESET:
		{
			SCI_TRACE_LOW("[ZMT_NET] %s: ZMT_APP_SIGNAL_NET_RESET ",__FUNCTION__);
			MMIZMT_Net_Handle_Reset(param);  //pdp reset
			break;
		}
    case ZMT_APP_SIGNAL_NET_ACTIVE_SUCCESS:
		{	
			SCI_TRACE_LOW("[ZMT_NET] %s: ZMT_APP_SIGNAL_NET_ACTIVE_SUCCESS ",__FUNCTION__);
			MMIZMT_Net_Handle_PDPActiveOK(param);
			ZMT_Net_Init(); // init url ip task ,should start socket thread here now
			break;
		}
    case ZMT_APP_SIGNAL_NET_ACTIVE_FAIL:
		{	
			SCI_TRACE_LOW("[ZMT_NET] %s: ZMT_APP_SIGNAL_NET_ACTIVE_FAIL ",__FUNCTION__); 
			MMIZMT_Net_Handle_PDPActiveFail(param);
			break;
		}
    case ZMT_APP_SIGNAL_NET_CONNET_FAIL:
        {
			SCI_TRACE_LOW("[ZMT_NET] %s: ZMT_APP_SIGNAL_NET_CONNET_FAIL ",__FUNCTION__); 
			MMIZMT_Net_Handle_PDPRespond(param);
			break;
		}
    default:
		{
			SCI_TRACE_LOW("[ZMT_NET] %s: default = %d",__FUNCTION__, msg_id);
			res = MMI_RESULT_FALSE;
			break;
		}
    }

	SCI_TRACE_LOW("[ZMT_NET] %s: res = %d",__FUNCTION__, res);

	if(res == MMI_RESULT_FALSE)
	{
		res = MMIZMTTCP_Handle_AppMsg(app_ptr,msg_id,param);
		SCI_TRACE_LOW("[ZMT_NET] %s: res = %d",__FUNCTION__, res);
	}

	return res;
}



