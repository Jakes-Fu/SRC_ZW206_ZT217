#include "os_api.h"
#include "sci_types.h"
#include "IN_Message.h"
#include "sci_api.h"
#include "socket_types.h"
#include "socket_api.h"
#include "os_types.h"
#include "os_util.h"
#include "os_errno.h"
// #include "os_sem.h"
#include "sys/socket.h"
#include "cmiot_type.h"
#include "cmiot_main.h"
#include "cmiot_thread.h"

#define CMIOT_THREAD_STACK_SIZE   (3*1024)     // 线程占用的堆栈大小，设置不合理可能会引发异常如assert
#define CMIOT_THREAD_QUEUE_NUM    8          //线程消息队列数

typedef struct CMIOT_SIG_MSG_tag
{
    xSignalHeaderRec header;
    void* msgDataEx;
} CMIOT_SIG_MSG;

static BLOCK_ID cmiot_thread_id = SCI_INVALID_BLOCK_ID;    // 线程ID
#ifdef CMIOT_GET_HOSTNAME_AYNC
static SCI_SEMAPHORE_PTR gs_gethost_sem = OS_NULL;

static TCPIP_SOCKET_T     gs_hostname_ipaddr;
static struct sci_hostent gs_hostent = {0};
static char *gs_ip_addr[2] = {NULL, NULL};

os_err_t cmiot_wait_sem(void)
{
    return SCI_GetSemaphore(gs_gethost_sem, MAX_ASYNC_GETHOST_WAIT_TIME);
}

struct sci_hostent* get_saved_hostent(void)
{
    return &gs_hostent;
}

// gethostbyname handler
void cmiot_gethostbyname_handler(void* msg)
{
    int error_code = 0;
    int addr_len=0;
    int i = 0;
    struct sci_hostent *phost = NULL;
    ASYNC_GETHOSTBYNAME_CNF_SIG_T *dns_ind = (ASYNC_GETHOSTBYNAME_CNF_SIG_T*)msg;

    error_code = ((ASYNC_GETHOSTBYNAME_CNF_SIG_T*)msg)->error_code;
    if (0 != error_code)
    {
        SCI_TRACE_LOW("cmiot:%s, error_code %d\n", __func__, error_code);
        return;
    }
    // {
    //     memcpy(&gs_hostent, &(((ASYNC_GETHOSTBYNAME_CNF_SIG_T*)msg)->hostent), sizeof(struct sci_hostent));
    //     memcpy((char *)&gs_hostname_ipaddr, gs_hostent.h_addr_list[0], 4);

    //     gs_ip_addr[0] = (char*)&gs_hostname_ipaddr;
    //     gs_hostent.h_addr_list = (char **)gs_ip_addr;
    //     SCI_TRACE_LOW("cmiot:=========%s-%d enter, gs_hostent: 0x%x, %s=========\r\n",
    //                 __func__, __LINE__, *(TCPIP_SOCKET_T *)gs_hostent.h_addr_list[0], inet_ntoa(gs_hostname_ipaddr));
    //     SCI_PutSemaphore(gs_gethost_sem);
    // }
    SCI_TRACE_LOW("cmiot_gethostbyname_handler: reqid:%d, errcode:%d,netid:%d", \
    dns_ind->request_id, dns_ind->error_code, dns_ind->netid);

    phost = &(dns_ind->hostent);
    if(phost == NULL)
    {
        SCI_TRACE_LOW("cmiot_gethostbyname_handler: phost null fail!!!");
        return;
    }

    SCI_TRACE_LOW("cmiot_gethostbyname_handler: phost->h_length:%d,phost->h_cntv4:%d,phost->h_cntv6:%d", phost->h_length,phost->h_cntv4,phost->h_cntv6);

    addr_len = phost->h_length;
    if(addr_len <= 0)
    {
        SCI_TRACE_LOW("cmiot_gethostbyname_handler: addr len=0, fail!!!");
        return;
    }

    if(phost->h_cntv4 > 0)
    {
        // g_adups_ip_type = AF_INET;
        addr_len = 4;

        for(i=0;i<addr_len;i++)//debug log
        {
            os_uint8_t ipval = (os_uint8_t)(*(phost->h_addr_list[0]+i));
            SCI_TRACE_LOW("cmiot_gethostbyname_handler ipv4 dns_ind->addr=%d", ipval);
        }

        // if(addr_len == 4)
        // {
        //     adups_save_download_ip((os_uint8_t)(*(phost->h_addr_list[0])),\
        //         (os_uint8_t)(*(phost->h_addr_list[0]+1)),\
        //         (os_uint8_t)(*(phost->h_addr_list[0]+2)),\
        //         (os_uint8_t)(*(phost->h_addr_list[0]+3)));
        // }
        memcpy(&gs_hostent, phost, sizeof(struct sci_hostent));
        memcpy((char *)&gs_hostname_ipaddr, gs_hostent.h_addr_list[0], 4);

        gs_ip_addr[0] = (char*)&gs_hostname_ipaddr;
        gs_hostent.h_addr_list = (char **)gs_ip_addr;
        SCI_TRACE_LOW("cmiot:=========%s-%d, gs_hostent: 0x%x, %s=========\r\n",
                    __func__, __LINE__, *(TCPIP_SOCKET_T *)gs_hostent.h_addr_list[0], inet_ntoa(gs_hostname_ipaddr));
        SCI_PutSemaphore(gs_gethost_sem);
    }
    else if(phost->h_cntv6 > 0)
    {
        // g_adups_ip_type = AF_INET6;
        addr_len = 16;

        for(i=0;i<addr_len;i++)
        {
            os_uint8_t ipval = (os_uint8_t)(*(phost->h_addr6_list[0]+i));
            SCI_TRACE_LOW("cmiot_gethostbyname_handler ipv6 dns_ind->addr=%02x", ipval);
        }

        // if(addr_len == 16)
        // {
        //     adups_save_download_ipv6((os_uint8_t)(*(phost->h_addr6_list[0])),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+1)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+2)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+3)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+4)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+5)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+6)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+7)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+8)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+9)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+10)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+11)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+12)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+13)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+14)),\
        //         (os_uint8_t)(*(phost->h_addr6_list[0]+15)));
        // }

    }
    else
    {
        SCI_TRACE_LOW("cmiot_gethostbyname_handler: no ipv4 or ipv6, fail!!!");
        return;
    }


    // if(dns_ind)
    // {
    //     SCI_TRACE_LOW("cmiot_gethostbyname_handler dns_ind->request_id=%d, g_request_id=%d", dns_ind->request_id, adups_socket_mgr.adups_request_id);

    //     if(dns_ind->request_id != adups_socket_mgr.adups_request_id)
    //     {
    //         return ;
    //     }
    // }
}
#endif

BLOCK_ID cmiot_get_thread_id()
{
    return cmiot_thread_id;
}
#ifdef CMIOT_GET_HOSTNAME_AYNC
LOCAL void cmiot_thread_entry(uint32 argc, void *argv)
{
    xSignalHeaderRec *psig;
    unsigned short signalCode;

    while(1) {
        psig = OS_NULL;

        SCI_RECEIVE_SIGNAL(psig,  cmiot_thread_id);     //阻塞获取，等待定时器发送
        signalCode = psig->SignalCode;

        SCI_TRACE_LOW("[cmiot] thread recv msg, signalCode = %d\n", signalCode);
        if(SOCKET_ASYNC_GETHOSTBYNAME_CNF == signalCode) {
            cmiot_gethostbyname_handler(psig);
        } else {
            SCI_TRACE_LOW("[cmiot] thread recv msg,  but not support \n");
        }

        SCI_FREE_SIGNAL(psig);
    }
}

static void cmiot_create_thread(void)
{
    if( gs_gethost_sem != OS_NULL ) {
        return;
    }

    gs_gethost_sem = SCI_CreateSemaphore("gethost_sem", 0);

    cmiot_thread_id = SCI_CreateAppThread("cmiot_thread", "cmiot_queue",
                                            cmiot_thread_entry, 0, NULL, CMIOT_THREAD_STACK_SIZE,
                                            CMIOT_THREAD_QUEUE_NUM, SCI_PRIORITY_LOWEST, SCI_PREEMPT, SCI_AUTO_START);
    if(SCI_INVALID_BLOCK_ID == cmiot_thread_id)
    {
        SCI_TRACE_LOW("[cmiot] create app thread failed\n");
    }

}
#endif

int cmiot_fota_process_init(void)
{
    int ret = -1;
    char ver[128] = {0};
#ifdef CMIOT_GET_HOSTNAME_AYNC
    cmiot_create_thread(); // get hostname thread
#endif

    SCI_Sleep(30000);
    if(OS_EOK == cmiot_enable_net())
    {
         SCI_TRACE_LOW("cmiot:ota flow start\n");
         SCI_Sleep(1000);
        //1. first check if need report upgrade result
        ret = cmiot_report_upgrade();
        if( ret != E_CMIOT_SUCCESS )
        {
            SCI_TRACE_LOW("cmiot:cmiot_report_upgrade result: %d\n", ret);
        }

        for (int cnt = 0; cnt < 3; cnt++) // do cnt for unexcepted network issue
        {
#ifndef _interactive_ota_flow_
            ret = cmiot_upgrade();
            SCI_TRACE_LOW("cmiot:cmiot_upgrade ret= %d", ret);
            // 2. check if exist new version
            if (ret == E_CMIOT_SUCCESS)
            {
                // if download firmware success, need reboot system to upgrade
                // POWER_Reset();
                break;
            }
#else
            ret = cmiot_check_version(ver);
            SCI_TRACE_LOW("cmiot:cmiot_check_version ret= %d ver =%s", ret, ver);
            if( ret == E_CMIOT_SUCCESS )
            {
                // 3. start download new version and report download result
                ret = cmiot_only_download();
                SCI_TRACE_LOW("cmiot:cmiot_only_download ret= %d", ret);

                // if download firmware success, need reboot system to upgrade
                if (ret == E_CMIOT_SUCCESS) {
                    POWER_Reset();
                }  else {
                    SCI_TRACE_LOW("cmiot:cmiot_upgrade failed, ret = %d\r\n", ret);
                }
            }
            else if( ret == E_CMIOT_LAST_VERSION )
            {
                SCI_TRACE_LOW("cmiot:No new firmware for upgrade\n");
                ret = 0;
                break;
            }
#endif
        }
    }

    return ret;
}
