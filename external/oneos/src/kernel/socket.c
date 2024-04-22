/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        socket.c
 *
 * @brief       Implement socket functions
 *
 * @revision
 * Date         Author          Notes
 * 2020-03-21   OneOS Team      First Version
 ***********************************************************************************************************************
 */
#include "os_types.h"
#include "os_errno.h"
#include "sys/socket.h"
#include "netdb.h"
#include "app_tcp_if.h"
#include "ssl_api.h"
#include "os_api.h"
#include "mmi_module.h"
#include "mmipdp_export.h"
#include "IN_Message.h"

typedef struct
{
    SIGNAL_VARS
    // uint32 decrypt_buf;
    uint32 decrypt_len;
} os_sock_sig;

typedef struct
{
    uint32 app_moduleId;  //Input: reference mmi_res_prj_def.h,
    int sock_handle;  //Output
    struct sci_sockaddr sock_addr; //Output
    // struct sci_sockaddr6 sock_addr6;
    SSL_HANDLE ssl_handle;//Output
    BLOCK_ID thread_id;
    // int simId;   //Input: 0 -sim 1
    // uint32 net_id;  //Output,Network Interface, come from PDP actived, we can get  IP/IP mask, DNS
    // BOOLEAN application_send_done;
    BOOLEAN ssl_handshake_done; //Output:TRUE - TLS handshake has succeeded.
    // BOOLEAN ServerIsIPv6; //TRUE: ipv6
    char* recvbuf; //Application layer
    uint32 recvlen;
}mmidemo_ssl_recorder_t;
LOCAL mmidemo_ssl_recorder_t g_mmidemo_ssl_recorder = {0};

// init
void os_sock_init(void)
{
    // SSL_Init();
    // memset(&g_sock_list, 0, sizeof(os_sock_list_t));
    memset(&g_mmidemo_ssl_recorder, 0, sizeof(mmidemo_ssl_recorder_t));
    //MMI has many app module, we just use MMI_MODULE_COMMON for test,
    //Note, maybe we must create a new app.
    g_mmidemo_ssl_recorder.app_moduleId = MMI_MODULE_COMMON;

    g_mmidemo_ssl_recorder.sock_handle = -1;
    g_mmidemo_ssl_recorder.ssl_handle = -1;
    g_mmidemo_ssl_recorder.ssl_handshake_done = FALSE;
}

int os_socket(int domain, int type, int protocol)
{
    int sci_domain = domain;
    int sci_type = type;
    int sci_protocol = protocol;
    //TCP : protocol must set to 0 for 8910ff socket
    if (sci_protocol == IPPROTO_TCP)
    {
        sci_protocol = 0;
    }
    os_sock_init();
    int fd = sci_sock_socket(sci_domain, sci_type, sci_protocol, (TCPIP_NETID_T)cmiot_net_get_netid());
    g_mmidemo_ssl_recorder.sock_handle = fd;
    g_mmidemo_ssl_recorder.thread_id = SCI_IdentifyThread();
    SCI_TRACE_LOW("oneos:sci_domain=%d, sci_type=%d, sci_protocol=%d, fd=0x%x", sci_domain, sci_type, sci_protocol, fd);
    return fd;
}

int os_closesocket(int fd)
{
    TCPIP_SOCKET_T sci_so = (TCPIP_SOCKET_T)fd;
    SCI_TRACE_LOW("oneos:os_closesocket fd=0x%x", fd);
    SSL_HANDLE sslh = g_mmidemo_ssl_recorder.ssl_handle; //os_sock_get_sslhandler(fd);
    SSL_Close(sslh, SSL_ASYNC);
    // os_sock_ssldel(fd);
    return sci_sock_socketclose(sci_so);
}

void sktPostMessage(void* handle, uint32 msg)
{
    if(NULL == handle)
    {
        SCI_TRACE_LOW("oneos:sktPostMessage parameters fail");
        return ;
    }
    SCI_TRACE_LOW("oneos:sktPostMessage handle=0x%x, msg=%u", (uint32)handle, msg);
    switch (msg)
    {
        case SSL_SEND_MESSAGE_HANDSHAKE_SUCC:  // 握手成功
            {
                g_mmidemo_ssl_recorder.ssl_handshake_done = TRUE;
                //http handle this case according to its state
                SCI_TRACE_LOW("oneos:sktPostMessage HANDSHAKE_SUCC");
            }
            break;

        case SSL_SEND_MESSAGE_FAIL: //发送失败
            {
                //http handle this case according to its state
                SCI_TRACE_LOW("oneos:sktPostMessage SEND_MESSAGE_FAIL");
            }
            break;

        case SSL_RECV_MESSAGE_SEND_SUCC: /*up layer has send encrypted data succ*/
            // SSL_EncryptPasser(SSL_HANDLE ssl_handle, uint8 * data_ptr, uint32 len)
            SCI_TRACE_LOW("oneos:sktPostMessage RECV_MESSAGE_SEND_SUCC, TLS send ok");
            break;

        case SSL_RECV_MESSAGE_RECV_SUCC:  //接收成功
            // SSL_DecryptPasser((HX_SALE_SSL_INSTANCE *)httpmachine_ptr->hSSL, RecvBuf, RecvDateLenn);
            SCI_TRACE_LOW("oneos:sktPostMessage RECV_MESSAGE_RECV_SUCC");
            break;

        case SSL_SEND_MESSAGE_CLOSE_BY_SERVER:   //服务器发送关闭
            {
                SCI_TRACE_LOW("oneos:sktPostMessage SEND_MESSAGE_CLOSE_BY_SERVER");
            }
            break;

        case SSL_SEND_MESSAGE_CANCLED_BY_USER: // 证书出现问题
            {
                //http handle this case according to its state
                SCI_TRACE_LOW("oneos:sktPostMessage SSEND_MESSAGE_CANCLED_BY_USER");
            }
            break;

        default:
            SCI_TRACE_LOW("oneos:sktPostMessage default");
            break;
    }
}

void sktDecryptDataOutput(void* handle, uint8* buf, uint32 len)
{

    SCI_TRACE_LOW("[oneos: len=%u------------>]%s       %d\n", len, __FUNCTION__,__LINE__);
    SCI_TRACE_LOW("buf=%s", buf);

    if (g_mmidemo_ssl_recorder.ssl_handshake_done == FALSE)
    {
        SCI_TRACE_LOW("[oneos: tls handshaking....]%s     %d\n",__FUNCTION__,__LINE__);
        return;
    }

    os_sock_sig *signal_ptr = NULL;
    signal_ptr = malloc(sizeof(os_sock_sig));
    if (signal_ptr != NULL)
    {
        if (g_mmidemo_ssl_recorder.recvbuf)
        {
            signal_ptr->decrypt_len = len > g_mmidemo_ssl_recorder.recvlen ? g_mmidemo_ssl_recorder.recvlen : len;
            memset(g_mmidemo_ssl_recorder.recvbuf, 0, g_mmidemo_ssl_recorder.recvlen);
            memcpy(g_mmidemo_ssl_recorder.recvbuf, (void *)buf, signal_ptr->decrypt_len);
        }
        signal_ptr->SignalCode = 1;
        signal_ptr->SignalSize = sizeof(os_sock_sig);
        signal_ptr->Sender = SCI_IdentifyThread();
        signal_ptr->Pre = NULL;
        signal_ptr->Suc = NULL;

        SCI_SendSignal((xSignalHeader)signal_ptr, g_mmidemo_ssl_recorder.thread_id);
    }
    else
    {
        SCI_TRACE_LOW("oneos:sktDecryptDataOutput malloc fail");
    }

    // 发送成功，给上层发送消息，释放SSL的资源
    SSL_AsyncMessageProc(g_mmidemo_ssl_recorder.ssl_handle, SSL_RECV_MESSAGE_RECV_SUCC, len);
    SCI_TRACE_LOW("[oneos: <------------]%s       %d\n",__FUNCTION__,__LINE__);
}

int32 sktEncryptDataOutput(void* handle, uint8* buf, uint32 len)
{
    int sci_len = 0;

    if (PNULL == handle || PNULL == buf || 0 == len)
    {
        SCI_TRACE_LOW("[oneos: input erro]%s      %d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    TCPIP_SOCKET_T sci_so = g_mmidemo_ssl_recorder.sock_handle;

    if (sci_so != -1)
    {
        sci_len = sci_sock_send(sci_so, (char *)buf, (int)len, 0);
        SCI_TRACE_LOW("[oneos:callback send data to server, data_len is %d, sendbyte is %d]%s       %d\n",len, sci_len,__FUNCTION__,__LINE__);
        {
            // 发送成功，给上层发送消息，释放SSL的资源
            SSL_AsyncMessageProc(g_mmidemo_ssl_recorder.ssl_handle, SSL_RECV_MESSAGE_SEND_SUCC, sci_len);
        }

        SCI_TRACE_LOW("oneos:sktEncryptDataOutput sci_so=0x%x send done", (uint32)sci_so);
    }

    return sci_len;
}
void sktShowCertInfo(void* handle)
{
    if(NULL == handle)
    {
        SCI_TRACE_LOW("sktShowCertInfo parameters fail");
        return ;
    }
    SCI_TRACE_LOW("[oneos:callback debug ------------>]%s       %d\n",__FUNCTION__,__LINE__);
    if (g_mmidemo_ssl_recorder.ssl_handle > 0)
    {
        //Now, we just continue.
        SSL_UserCnfCert(g_mmidemo_ssl_recorder.ssl_handle, TRUE);
    }
}

/*==========================================================
 * Function     : os_skt_sslshake
 * Description : ssl 创建及连接交互
 * Parameter    : s: skt fd
 * Return        : 0:success, -1:fail
 ==========================================================*/
int os_skt_sslshake(long fd, struct sci_sockaddr sock_addr)
{
    SSL_HANDLE ssl_handle = 0;
    SSL_CALLBACK_T os_skt_cb;
    char *addr_str = NULL;
    int sock_port= 0;
    SSL_RESULT_E  shake_result = 0;

    ssl_handle = SSL_Create((void *)&g_mmidemo_ssl_recorder.app_moduleId, fd, SSL_ASYNC);
    if(!ssl_handle)
    {
        SCI_TRACE_LOW("oneos:os_skt_sslshake ssl create fail");
        return -1;
    }
    g_mmidemo_ssl_recorder.ssl_handle = ssl_handle;

    os_skt_cb.decryptout_cb = sktDecryptDataOutput;
    os_skt_cb.encryptout_cb = sktEncryptDataOutput;
    os_skt_cb.postmessage_cb = sktPostMessage;
    os_skt_cb.showcert_cb = sktShowCertInfo;
    SSL_AsyncRegCallback(ssl_handle, &os_skt_cb);

    //select ssl protocol
    SSL_ProtocolChoose(ssl_handle, SSL_PROTOCOLTLS_1_2, SSL_ASYNC);

    addr_str = inet_ntoa(sock_addr.ip_addr);
    sock_port = sock_addr.port;
    shake_result= SSL_HandShake(ssl_handle, addr_str, sock_port, SSL_ASYNC);

    SCI_TRACE_LOW("oneos:os_skt_sslshake result =%d", shake_result);
    void *tmp = malloc(2048);
    int len =0;
    while (1)
    {
        memset(tmp, 0, 2048);
        if (g_mmidemo_ssl_recorder.ssl_handshake_done == TRUE)
        {
            break;
        }
        if (os_select(fd, 1, 0, 0, 100) > 0)
        {
            len = sci_sock_recv(fd, tmp, 2048, 0);
            if (len > 0)
            {
                SCI_TRACE_LOW("oneos:os_skt_sslshake len=%d", len);
                SSL_DecryptPasser(ssl_handle, (uint8 *)tmp, len);
            }
        }

        SCI_Sleep(100);
    }

    free(tmp);

    return shake_result;
}

int os_connect(int fd, const struct sockaddr *name, socklen_t namelen)
{
    TCPIP_SOCKET_T sci_so = (TCPIP_SOCKET_T)fd;
    int ret = -1;

    struct sockaddr_in *sa1 =  (struct sockaddr_in *)name;
    struct sci_sockaddr sci_sock = {0};
    sci_sock.family = sa1->sin_family;
    sci_sock.port = sa1->sin_port;
    sci_sock.ip_addr = sa1->sin_addr.s_addr;
    SCI_TRACE_LOW("oneos:family=%d, port=%d, addr=%s", sci_sock.family, sci_sock.port, inet_ntoa(sci_sock.ip_addr));

    SCI_MEMSET(&(g_mmidemo_ssl_recorder.sock_addr), 0, sizeof(struct sci_sockaddr));
    SCI_MEMCPY(&(g_mmidemo_ssl_recorder.sock_addr), &sci_sock, sizeof(sci_sock));

    ret = sci_sock_connect(sci_so, (struct sci_sockaddrext*)&sci_sock, namelen);
    if (ret == 0)
    {
        ret = os_skt_sslshake(sci_so, sci_sock);
    }
    return ret;
}

int os_send(int fd, const void *data, size_t size, int flags)
{
    int sci_len = 0;
    char* sci_buf = (char*)data;
    int sci_flag = flags;
    int sock_error_code;
    SSL_HANDLE sslh = g_mmidemo_ssl_recorder.ssl_handle;

    if (fd == -1 || data == NULL || size == 0)
    {
        SCI_TRACE_LOW("[oneos: input error]%s    %d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    SSL_EncryptPasser(sslh, (uint8 *)data, size);

    // no need to wait, just return
    return size;
}

int os_recv(int fd, void *mem, size_t len, int flags)
{
    int sci_len = -1;
    int errcode;
    TCPIP_SOCKET_T sci_so = (TCPIP_SOCKET_T)fd;
    char* sci_buf = (char*)mem;
    int sci_flag = flags;
    SCI_TRACE_LOW("[oneos------------>]%s       %d\n",__FUNCTION__,__LINE__);

    g_mmidemo_ssl_recorder.recvbuf = mem;
    g_mmidemo_ssl_recorder.recvlen = len;

    sci_len = sci_sock_recv(sci_so, sci_buf, (int)len, sci_flag);
    if (sci_len <= 0) {
        errcode = sci_sock_errno(sci_so);
        SCI_TRACE_LOW("oneos:os_recv fail: code=%d, len=%d\r\n", errcode, sci_len);
    }
    else
    {
        SCI_TRACE_LOW("oneos:os_recv fd=0x%x sci_len=%d", fd, sci_len);
        SSL_DecryptPasser(g_mmidemo_ssl_recorder.ssl_handle, (uint8 *)sci_buf, sci_len);

        // wait for semphore or msg that indicate data copyed to mem.
        os_sock_sig *sig_ptr = SCI_NULL;
        // os_sock_sig *sig_ptr = (os_sock_sig *)SCI_GetSignal(g_mmidemo_ssl_recorder.thread_id);
        for (int i = 0; i < 5; i++)
        {
            sig_ptr = (os_sock_sig *)SCI_PeekSignal(g_mmidemo_ssl_recorder.thread_id);
            if (sig_ptr != PNULL)
            {
                break;
            }
            SCI_Sleep(10);
        }
        sci_len = 0;
        if (sig_ptr != SCI_NULL)
        {
            SCI_TRACE_LOW("oneos:os_recv signal=%u, sender=0x%x", sig_ptr->SignalCode, sig_ptr->Sender);
            if (sig_ptr->SignalCode == 1)
            {
                sci_len = sig_ptr->decrypt_len;
            }
            free(sig_ptr);
        }
    }
    SCI_TRACE_LOW("[oneos sci_len %d<------------]%s       %d\n", sci_len, __FUNCTION__,__LINE__);
    return sci_len;
}
/*
isreadset: 1,need set readset
iswriteset: 1,need set iswriteset
isexceptset: 1,need set isexceptset
timeout_ms: timeout, ms
*/
int os_select(int fd, os_bool_t isreadset, os_bool_t iswriteset, os_bool_t isexceptset, int timeout_ms)
{
    struct sci_fd_set sci_readset;
    struct sci_fd_set* in = OS_NULL;
    struct sci_fd_set sci_writeset;
    struct sci_fd_set* out = OS_NULL;
    struct sci_fd_set sci_exceptset;
    struct sci_fd_set* ex = OS_NULL;

    long tv = timeout_ms / 100;

    if (isreadset == OS_TRUE)
    {
        SCI_FD_ZERO(&sci_readset);
        SCI_FD_SET(fd, &sci_readset);
        in = &sci_readset;
        // SCI_TRACE_LOW("oneos:fd=0x%x, isreadset=%d", fd, SCI_FD_ISSET(fd, &sci_readset));
    }
    if (iswriteset == OS_TRUE)
    {
        SCI_FD_ZERO(&sci_writeset);
        SCI_FD_SET(fd, &sci_writeset);
        out = &sci_writeset;
    }
    if (isexceptset == OS_TRUE)
    {
        SCI_FD_ZERO(&sci_exceptset);
        SCI_FD_SET(fd, &sci_exceptset);
        ex = &sci_exceptset;
    }

    return sci_sock_select(in, out, ex, tv);
}
int os_setsockopt(int fd, int level, int optname, void *optval, socklen_t optlen)
{
    return sci_sock_setsockopt((TCPIP_SOCKET_T)fd, optname, optval);

}
struct hostent *os_gethostbyname(char *name)
{
    return sci_gethostbyname(name);
}
