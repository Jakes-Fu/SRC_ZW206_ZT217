#include "os_types.h"
#include "os_util.h"
#include "os_errno.h"

#if 1
#include "os_api.h"
#include "mn_type.h"
#include "mn_error.h"
#ifdef MN_RPC_SUPPORT
#include "mn_api_td.h"
#else
#include "mn_api.h"
#endif
#include "sci_api.h"
#include "mmipdp_export.h"
#include "mmiconnection_export.h"
#include "mmi_module.h"

#include "sys/socket.h"
#include "tcpip_types.h"
#include "socket_types.h"
#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif
#include "mmk_timer.h"

#define MMI_MODULE_CMIOT  0x800//zhanxun new code jianyi

typedef void (*cmiot_common_callback)(void);
typedef void (*cmiot_timer_callback)(void);

typedef struct
{
    os_uint32_t timerid;
    os_uint32_t delay;
    cmiot_timer_callback func_ptr;
}CMIOT_TIMER_STRUCT;

enum _CMIOT_TIMERID
{
    CMIOT_TIMERID_00,
    CMIOT_TIMERID_01,
    // CMIOT_TIMERID_02,
    // CMIOT_TIMERID_03,
    // CMIOT_TIMERID_04,
    // CMIOT_TIMERID_05,
    // CMIOT_TIMERID_06,
    // CMIOT_TIMERID_07,
    // CMIOT_TIMERID_08,
    // CMIOT_TIMERID_09,
    CMIOT_TIMERID_MAXNUM
};

static cmiot_common_callback s_cmiot_pdp_userevt;
static CMIOT_TIMER_STRUCT g_cmiot_timer[CMIOT_TIMERID_MAXNUM]={0}; /*timer*/
static os_uint8_t cmiot_pdp_active_times = 0;
static os_bool_t s_cmiot_is_dns_pdp_actived = OS_FALSE;      //status: dns pdp is actived or not

static os_uint32_t g_cmiot_net_id = 0;//5;
static SCI_SEMAPHORE_PTR gs_pdp_sem = OS_NULL;

extern void cmiot_create_thread(void);
static void cmiot_net_pdp_reactive(void);

/*wifi connect status*/
// os_bool_t cmiot_wifi_is_connected(void)
// {
// #ifdef WIFI_SUPPORT
//     if(MMIWIFI_STATUS_CONNECTED == MMIAPIWIFI_GetStatus())
//     {
//         return OS_TRUE;
//     }
// #endif
//     return OS_FALSE;
// }

void cmiot_net_pdp_userevt_exe(void)
{
    if (s_cmiot_pdp_userevt) s_cmiot_pdp_userevt();
}

void cmiot_timer_handler(os_uint8_t time_id, os_uint32_t param)
{
    uint8 idx = (uint8)param;
    cmiot_timer_callback func_ptr = NULL;

    if (idx >= CMIOT_TIMERID_MAXNUM) return;

    if (g_cmiot_timer[idx].timerid == time_id)
    {
        func_ptr = g_cmiot_timer[idx].func_ptr;
        MMK_StopTimer(time_id);
        g_cmiot_timer[idx].timerid = 0;
        g_cmiot_timer[idx].func_ptr = NULL;
        g_cmiot_timer[idx].delay = 0;
        if (func_ptr) func_ptr();
    }

    SCI_TRACE_LOW("cmiot:%s: timer idx:%d, send timer msg", __func__, idx);
}

void cmiot_start_timer(os_int32_t delayms, cmiot_timer_callback func_ptr, os_bool_t period)
{
    os_uint8_t idx = 0;

    for (idx = 0; idx < CMIOT_TIMERID_MAXNUM; idx++)
    {
        if (g_cmiot_timer[idx].func_ptr == func_ptr)
        {
            MMK_StopTimer(g_cmiot_timer[idx].timerid);
            g_cmiot_timer[idx].timerid = 0;
            g_cmiot_timer[idx].func_ptr = NULL;
            g_cmiot_timer[idx].delay = 0;
        }
    }

    for (idx = 0; idx < CMIOT_TIMERID_MAXNUM; idx++)
    {
        if(g_cmiot_timer[idx].func_ptr == NULL)
        {
            g_cmiot_timer[idx].func_ptr = func_ptr;
            break;
        }
    }
    SCI_TRACE_LOW("cmiot:%s: idx=%d", __func__, idx);

    if (idx >= CMIOT_TIMERID_MAXNUM) return;

    g_cmiot_timer[idx].delay = delayms;
    g_cmiot_timer[idx].timerid = MMK_CreateTimerCallback(delayms, cmiot_timer_handler, (uint32)idx, period);
}

void cmiot_stop_timer(cmiot_timer_callback func_ptr)
{
    os_uint8_t idx = 0;

    SCI_TRACE_LOW("cmiot:%s: timer func: 0x%08x", __func__, (os_uint32_t)func_ptr);

    for (idx = 0; idx < CMIOT_TIMERID_MAXNUM; idx++)
    {
        if (g_cmiot_timer[idx].func_ptr == func_ptr)
        {
            break;
        }
    }

    if (idx >= CMIOT_TIMERID_MAXNUM) return;

    SCI_TRACE_LOW("cmiot:%s: timer idx:%d", __func__, idx);

    if (g_cmiot_timer[idx].timerid)
    {
        MMK_StopTimer(g_cmiot_timer[idx].timerid);
        g_cmiot_timer[idx].timerid = 0;
        g_cmiot_timer[idx].func_ptr = NULL;
        g_cmiot_timer[idx].delay = 0;
    }
}

os_bool_t cmiot_net_pdp_is_active(void)
{
    return s_cmiot_is_dns_pdp_actived;
}

void cmiot_net_set_netid(os_uint32_t netid)
{
    g_cmiot_net_id = netid;
}

os_uint32_t cmiot_net_get_netid()
{
    return g_cmiot_net_id;
}

static void cmiot_net_pdp_active_msghdlr(MMIPDP_CNF_INFO_T *msg_ptr)
{
    SCI_TRACE_LOW("cmiot:cmiot_net_pdp_active_msghdlr Entry, msg_ptr=0x%x,msg_id = %d, result = %d", msg_ptr, msg_ptr->msg_id, msg_ptr->result);

    switch(msg_ptr->msg_id) {
        case MMIPDP_ACTIVE_CNF:
            if(msg_ptr->result == MMIPDP_RESULT_SUCC) {
                SCI_TRACE_LOW("cmiot:cmiot MMIPDP_ACTIVE_CNF: msg_ptr->result SUCCESS, net id=%u\n", msg_ptr->nsapi);
                cmiot_stop_timer(cmiot_net_pdp_reactive);
                cmiot_pdp_active_times = 0;
                s_cmiot_is_dns_pdp_actived = OS_TRUE;
                cmiot_net_set_netid(msg_ptr->nsapi);
                cmiot_net_pdp_userevt_exe();
            } else {
                SCI_TRACE_LOW("cmiot: pdp active failed");
                if (cmiot_pdp_active_times < 4) {
                    SCI_TRACE_LOW("cmiot:cmiot MMIPDP_ACTIVE_CNF: pdp try again, cmiot_pdp_active_times=%d\n", cmiot_pdp_active_times);
                    cmiot_pdp_active_times++;
                    cmiot_start_timer(30*1000, cmiot_net_pdp_reactive, OS_FALSE);
                } else {
                    cmiot_pdp_active_times = 0;
                    SCI_TRACE_LOW("cmiot: pdp active failed end!");
                }
            }
            break;

        case MMIPDP_DEACTIVE_CNF:
        case MMIPDP_DEACTIVE_IND:
            SCI_TRACE_LOW("cmiot:: pdp DEACTIVE!");
            s_cmiot_is_dns_pdp_actived = FALSE;
            break;
        default:
            break;
    }
}

os_uint32_t cmiot_net_pdp_active(cmiot_common_callback userevt)
{
    // os_uint8_t setting_index = 0;
    os_bool_t ret = OS_FALSE;
    MMIPDP_ACTIVE_INFO_T app_info = {0};
    MMICONNECTION_LINKSETTING_DETAIL_T* setting_item_ptr = PNULL;
    MN_DUAL_SYS_E sim_select = MN_DUAL_SYS_1;
    MMICONNECTION_APNTYPEINDEX_T *apn_idx_arr;
    MN_ATTACH_STATE_E cs_attach_state = MN_INVALID_STATE;
    MN_ATTACH_STATE_E ps_attach_state = MN_INVALID_STATE;

    s_cmiot_pdp_userevt = userevt;
    SCI_TRACE_LOW("cmiot: s_cmiot_is_dns_pdp_actived %d, sim:%d", s_cmiot_is_dns_pdp_actived, MMIAPIPHONE_IsSimOk(MN_DUAL_SYS_1));

    // PDP not activated, activate PDP firstly
    if(!s_cmiot_is_dns_pdp_actived) {
        memset(&app_info, 0, sizeof(MMIPDP_ACTIVE_INFO_T));

        // if(cmiot_wifi_is_connected())
        // {
        //     app_info.app_handler = MMI_MODULE_CMIOT;
        //     app_info.handle_msg_callback = cmiot_net_pdp_active_msghdlr;
        //     app_info.ps_interface = MMIPDP_INTERFACE_WIFI;
        //     ret = MMIAPIPDP_Active(&app_info);
        // }
        // else
        {
            // setting_index = MMIAPIBROWSER_GetNetSettingIndex(sim_select);
            apn_idx_arr = MMIAPICONNECTION_GetApnTypeIndex(sim_select);
            setting_item_ptr = MMIAPICONNECTION_GetLinkSettingItemByIndex(sim_select, apn_idx_arr->internet_index[sim_select].index);
            SCI_TRACE_LOW("cmiot: setting_index=%d,sim_select=%d", apn_idx_arr->internet_index[sim_select].index, sim_select);//debug

            // if(OS_FALSE == MMIAPIPHONE_IsNetworkAttached(sim_select)) {
            //     SCI_TRACE_LOW("cmiot: active pdp fail,network status not ready!!!");
            //     return OS_ERROR;
            // }
            MNPHONE_GetAttachStates(sim_select, &cs_attach_state, &ps_attach_state);
            SCI_TRACE_LOW("cmiot: dual_sys=%d, ps_attach=%d, cs_attach=%d", sim_select, ps_attach_state, cs_attach_state);
            if (!(MN_ATTACHED_STATE == cs_attach_state) && !(MN_ATTACHED_STATE == ps_attach_state))
            {
                SCI_TRACE_LOW("cmiot: active pdp fail,network status not ready!!!");
                return OS_ERROR;
            }

            if (setting_item_ptr != PNULL)
            {
                app_info.dual_sys = sim_select;
                app_info.app_handler = MMI_MODULE_COMMON; //MMI_MODULE_CMIOT
                app_info.auth_type = setting_item_ptr->auth_type;
                app_info.apn_ptr = (char*)setting_item_ptr->apn;
                app_info.user_name_ptr = (char*)setting_item_ptr->username;
                app_info.psw_ptr = (char*)setting_item_ptr->password;
                app_info.ps_service_type = BROWSER_E; //STREAMING_E;
                app_info.ps_service_rat = MN_UNSPECIFIED;
                app_info.ps_interface = MMIPDP_INTERFACE_GPRS;
                app_info.storage = MN_GPRS_STORAGE_ALL;
                app_info.priority = 1; //3;
                app_info.handle_msg_callback = cmiot_net_pdp_active_msghdlr;
                SCI_TRACE_LOW("cmiot callback,func:0x%x", cmiot_net_pdp_active_msghdlr);
                #if 0//def IPVERSION_SUPPORT_V4_V6
                app_info.ip_type = setting_item_ptr->ip_type;
                #endif

                // SCI_TRACE_LOW("cmiot: apn=%s, user_name=%s, password=%s,app_info.ip_type=%d", app_info.apn_ptr, app_info.user_name_ptr, app_info.psw_ptr,app_info.ip_type);
                ret = MMIAPIPDP_Active(&app_info);
            }
        }

        if (OS_FALSE == ret) {
            SCI_TRACE_LOW("cmiot: active pdp fail ret %d", ret);
            return OS_ERROR;
        }
    }
    else
    {
        SCI_TRACE_LOW("cmiot: PDP is already activated");
        return OS_PDP_ACTIVATED;
    }

    return OS_EOK;
}

static void cmiot_net_pdp_reactive(void)
{
    if(cmiot_net_pdp_is_active() == OS_FALSE && s_cmiot_pdp_userevt) {
        cmiot_net_pdp_active(s_cmiot_pdp_userevt);
    }
}
void oneos_pdp_enable(void)
{
    SCI_TRACE_LOW("cmiot:=========%s-%d enter=======gs_pdp_sem=0x%x=\r\n", __func__, __LINE__, gs_pdp_sem);

    SCI_PutSemaphore(gs_pdp_sem);
}
#endif

int oneos_socket_test(void)
{
    int                ret;
    int                sock = -1;
    int                data_len;
    char              *buf;
    struct sockaddr_in server_addr;
    const char        *data = "Hello CMCC OneOS!";
    const char        *url = "106.13.65.1"; /* fix while debug */
    int                  port = 6585;             /* fix while debug */

    // host = gethostbyname(url);
    buf = malloc(128);
    if (NULL == buf) {
        SCI_TRACE_LOW("cmiot:malloc failed!\n");
        return -1;
    }

    sock = os_socket(AF_INET, SOCK_STREAM, 0);
    if ( sock == -1 ) {
        SCI_TRACE_LOW("cmiot:=========%s-%d enter, socket failed!=========\r\n", __func__, __LINE__);
        free(buf);
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    // server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    server_addr.sin_addr.s_addr = inet_addr(url);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (os_connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) != 0) {
        SCI_TRACE_LOW("cmiot:=========%s-%d enter, connect failed!=========\r\n", __func__, __LINE__);
        free(buf);
        return -1;
    }

    ret = os_send(sock, data, strlen(data), 0);
    if (ret < 0) {
        SCI_TRACE_LOW("cmiot:=========%s-%d enter, send/write to socket data failed=========\r\n", __func__, __LINE__);
        os_closesocket(sock);
        free(buf);

    } else {
        data_len = os_recv(sock, buf, 128, 0);
        //data_len = read(sock, buf, 128);
        if (data_len <= 0)
        {
            SCI_TRACE_LOW("cmiot:=========%s-%d enter, recv/read from socket failed=========\r\n", __func__, __LINE__);
            os_closesocket(sock);
        } else {
            buf[data_len] = 0;
            SCI_TRACE_LOW("cmiot:=========%s-%d enter, recv data: %s=========\r\n", __func__, __LINE__, buf);
        }

        free(buf);
    }

    return 0;
}


int cmiot_enable_net(void)
{
    int ret;
    // struct hostent* localhost = NULL;

    if( gs_pdp_sem == OS_NULL )
    {
        gs_pdp_sem = SCI_CreateSemaphore("pdp_sem", 0);
        SCI_TRACE_LOW("cmiot:gs_pdp_sem=0x%x\r\n", gs_pdp_sem);
    }

    //1. enable pdp
    ret = cmiot_net_pdp_active(oneos_pdp_enable);
    if (OS_EOK == ret)
    {
        //2. wait enable success
        if( OS_EOK == SCI_GetSemaphore(gs_pdp_sem, 30*5*1000) )
        {
            SCI_TRACE_LOW("cmiot:%s-%d enter, pdp enable success\r\n", __func__, __LINE__);

            //3. start domain test
            // SCI_TRACE_LOW("cmiot:=========%s-%d enter=========\r\n", __func__, __LINE__);
            // oneos_socket_test();

            // localhost = (struct hostent*)os_gethostbyname("fota.os.cmiotcd.com:9686");
            // SCI_TRACE_LOW("cmiot:=========%s-%d os_gethostbyname: 0x%x, %s=========\r\n", __func__, __LINE__, *(int *)localhost->h_addr_list[0], inet_ntoa(*(TCPIP_IPADDR_T *)localhost->h_addr_list[0]));
        }
        else
        {
            ret = OS_ERROR;
        }
    }
    else if (OS_PDP_ACTIVATED == ret)
    {
        SCI_TRACE_LOW("cmiot:%s-%d pdp already activated\r\n", __func__, __LINE__);
        ret = 0;
    }

    return ret;
}
