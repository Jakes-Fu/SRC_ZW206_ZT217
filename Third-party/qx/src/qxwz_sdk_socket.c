#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* #include <unistd.h> */
/* #include <fcntl.h> */
/* #include <netdb.h> */
/* #include <sys/ioctl.h> */
/* #include <netinet/in.h> */
/* #include <sys/types.h> */         /* See NOTES */
/* #include <sys/socket.h> */

/* According to POSIX.1-2001, POSIX.1-2008 */
/* #include <sys/select.h> */

/* According to earlier standards */
/* #include <sys/time.h> */

#define FUNC_ENTER(module)  do{\
                    QX_LOG(DEBUG,module,"    enter\n");\
                    }while(0);
#define FUNC_EXIT(module)  do{\
                    QX_LOG(DEBUG,module,"    exit\n");\
                    }while(0);
					
#include "socket_types.h"

//#include "qxwz_sdk_type.h"
#include "qxwz_sdk_socket.h"
//#include "qxwz_sdk_log.h"
#include "qx_supl_sdk.h"
#include "errno.h"

#include "os_api.h"
#include "tcpip_api.h"
#include "app_tcp_if.h"
#define MAX_RETRY_TIMES 100

#ifdef MODULE_NAME 
#undef MODULE_NAME
#define MODULE_NAME "QXWZ_SOCKET"
#else
#define MODULE_NAME "QXWZ_SOCKET"
#endif
/*TODO: Implement by User*/
#define QX_LOG(level,module, fmt, args...) SCI_TRACE_LOW("%s "fmt"\n",module,##args)
static qx_socket_t socket_config = {0};
void qx_socket_config_setting(int id)
{
    socket_config.netid = id;
}
int qx_socket(int domain, int type, int protocol, int nonblock)
{

    int sock_fd;
    int f, t;
    FUNC_ENTER(MODULE_NAME)
    if(domain == QX_SOCKET_AF_INET){
        f = AF_INET;
    } else if (domain == QX_SOCKET_AF_INET6) {
        f = AF_INET6;
    } else
        return QX_RET_ERR;
    if(type == QX_SOCKET_STREAM){
        t = SOCK_STREAM;
    }else{
        t = SOCK_STREAM;
    }
    sock_fd = sci_sock_socket(f, t, protocol,socket_config.netid);
    if(sock_fd == -1){
        QX_LOG(DEBUG,MODULE_NAME,"qx_socket create error:%d\n",sci_sock_errno(sock_fd));
        return QX_RET_ERR;
    }
#if 1
    if(nonblock == QX_SOCKET_UBLOCK){ 
        if ( -1 == sci_sock_setsockopt(sock_fd, SO_NBIO, NULL) )
        {
            QX_LOG(DEBUG,MODULE_NAME,"set so %x to non-block failed - error %d", sock_fd, sci_sock_errno(sock_fd));
            sock_fd = QX_RET_ERR;
        }
        
    }
#endif
    FUNC_EXIT(MODULE_NAME)

    return sock_fd;
}
int qx_socket_gethostbyname(char *host, char *strip, int *af) 
{
#define MAX_GETHOSTBYNAME_TIMES	2
    int rc = 0;

    int i = 0;
    struct sci_hostent* hostent = NULL;

    for(i = 0; i < MAX_GETHOSTBYNAME_TIMES; ++i) {
        hostent = sci_gethostbyname_ext( host ,socket_config.netid);
        if (hostent)
            break;
    }
    if (!hostent)
        goto failure;
    if (hostent->h_cntv4 > 0) {
        if (NULL == inet_ntop(AF_INET, hostent->h_addr_list[0], strip, 47)) {
            QX_LOG(ERROR,MODULE_NAME,"inet_ntop(family = %d, addr_ptr = %p, str_ptr = %p, len = %d) = %p",
                    AF_INET, hostent->h_addr_list[0], strip, 47, NULL);
            goto failure;
        }
        if (af)
            *af = QX_SOCKET_AF_INET;
    } else if (hostent->h_cntv6 > 0) {
        if (NULL == inet_ntop(AF_INET6, hostent->h_addr6_list[0], strip, 47)) {
            QX_LOG(ERROR,MODULE_NAME,"inet_ntop(family = %d, addr_ptr = %p, str_ptr = %p, len = %d) = %p",
                    AF_INET6, hostent->h_addr6_list[0], strip, 47, NULL);
            goto failure;
    }
        if (af)
            *af = QX_SOCKET_AF_INET6;
    } else
        goto failure;

    goto success;
exit:
    return rc;
success:
    rc = 0;
    goto cleanup;
failure:
    rc = -1;
    goto cleanup;
cleanup:
    goto exit;
}
int qx_socket_gethostbyname1(char *host, char *strip)
{

    TCPIP_IPADDR_T      hostip = 0;
    int                 ret = 0;
    int                 i = 0;
    FUNC_ENTER(MODULE_NAME)
    for( i = 0 ; i < MAX_RETRY_TIMES ; i++ )
    {
        //need to confirm which netid is correct
        ret = sci_parse_host_ext(host, &hostip, 0,socket_config.netid);
        if( ENP_SEND_PENDING == ret )
        {   //already sent DNS request, wait response
            qx_os_msleep(100);
        }
        else {
            break;
        }
    }
    if( 0 == ret )
    {
        QX_LOG(DEBUG,MODULE_NAME,"ip of URL %s is %s", host, inet_ntoa(hostip));
        strcpy(strip, inet_ntoa(hostip));
    }
    else
    {
        QX_LOG(DEBUG,MODULE_NAME,"sci_parse_host err - %d", ret);
    }
    FUNC_EXIT(MODULE_NAME)

    return ret;
    
}
int qx_socket_connect(int sockfd, void *host, int port, int qxaf) 
{

    int                 ret = 0;
    int rc = 0;
    struct sci_sockaddrext addr_ext;
    struct sci_sockaddr *addr = NULL;
    struct sci_sockaddr6 *addr6 = NULL;
    int af = 0;
    FUNC_ENTER(MODULE_NAME)

    /* build the server's Internet address */
    /*bzero((char *) &serveraddr, sizeof(serveraddr));*/
    qx_os_memset(&addr_ext, 0, sizeof(addr_ext));
    addr = (struct sci_sockaddr*) &addr_ext;
    addr6 = (struct sci_sockaddr6*) &addr_ext;
    if (qxaf == QX_SOCKET_AF_INET) {
        af = AF_INET;
        addr->family = af;
        addr->port = htons(port);
        rc = inet_pton(af, host, &addr->ip_addr);
        QX_LOG(DEBUG,MODULE_NAME,"inet_pton(af = %d, host = %s, addr_ptr = %p) = %d",
                af, (char const*) host, &addr->ip_addr, rc);
        if (rc != 0)
            goto failure;
        QX_LOG(DEBUG,MODULE_NAME,"ip [%d.%d.%d.%d]",
                ((unsigned char const*) &addr->ip_addr)[0],
                ((unsigned char const*) &addr->ip_addr)[1],
                ((unsigned char const*) &addr->ip_addr)[2],
                ((unsigned char const*) &addr->ip_addr)[3]
                );
    } else if (qxaf == QX_SOCKET_AF_INET6) {
        af = AF_INET6;
        addr6->sin6_family = af;
        addr6->sin6_port = htons(port);
        rc = inet_pton(af, host, &addr6->sin6_addr);
        QX_LOG(DEBUG,MODULE_NAME,"inet_pton(af = %d, host = %s, addr_ptr = %p) = %d",
                af, (char const*) host, &addr6->sin6_addr, rc);
        if (rc != 0)
            goto failure;
    } else {
        QX_LOG(ERROR,MODULE_NAME,"af not in (AF_INET, AF_INET6)");
        goto failure;
    }
    
    ret = sci_sock_connect(sockfd, &addr_ext, sizeof(addr_ext));
    
    if( QX_RET_ERR == ret ) {
        if(EINPROGRESS == sci_sock_errno(sockfd)) {
            // except EINPROGRESS, others are fatal error
            ret = 0;
        }
    }
    goto success;
exit:
    FUNC_EXIT(MODULE_NAME)

    return ret;
success:
    ret = 0;
    goto cleanup;
failure:
    ret = -1;
    goto cleanup;
cleanup:
    goto exit;
}

int qx_socket_send(int sockfd, const void *buf, int len, int flags)
{
    FUNC_ENTER(MODULE_NAME)

    return sci_sock_send(sockfd, buf, len, flags);
}

int qx_socket_recv(int sockfd, void *buf, int len, int flags)
{
    FUNC_ENTER(MODULE_NAME)

    return  sci_sock_recv(sockfd, buf, len, flags);
}

/*
timeout:ms
0->read
1->write
*/
int qx_socket_select(int sockfd, long timeout, QX_SOCKET_MONITOR rw)
{

    int ret = 0;
    sci_fd_set rfds, wfds;  
    FUNC_ENTER(MODULE_NAME)
      
    /* set select() time out */  
    /* tv.tv_sec = timeout/1000;   */
    /* tv.tv_usec = (timeout%1000)*1000;  */
    /* initialize watch list*/
    SCI_FD_ZERO(&rfds);
    SCI_FD_ZERO(&wfds);  
    if(rw == QX_SOCK_MONITOR_READ){
        SCI_FD_SET(sockfd, &rfds);  
    }else if(rw == QX_SOCK_MONITOR_WRITE){
        SCI_FD_SET(sockfd, &wfds);  
    }else{
        QX_LOG(DEBUG,MODULE_NAME,"qx_socket_select invalid param\n");
        return QX_RET_SOCKET_SELECT_ERROR;
    }
    do{
        if(rw == QX_SOCK_MONITOR_READ){
            ret = sci_sock_select(&rfds, NULL, NULL, timeout/100);    
        }else if(rw == QX_SOCK_MONITOR_WRITE){
            ret = sci_sock_select(NULL, &wfds, NULL, timeout/100);    
        }
        QX_LOG(DEBUG,MODULE_NAME,"select ret=%d,sockfd=%d\n",ret,sockfd);
        if (ret == QX_RET_ERR)  {
            ret = QX_RET_SOCKET_SELECT_ERROR;
            QX_LOG(DEBUG,MODULE_NAME,"select error\n");  
            break;  
        }else if(ret == 0){
            QX_LOG(DEBUG,MODULE_NAME,"select time out\n");  
            ret = QX_RET_SOCKET_TIMEOUT;  
            break;  
        }else{   
            if (SCI_FD_ISSET(sockfd, &rfds) || SCI_FD_ISSET(sockfd, &wfds)){  
               QX_LOG(DEBUG,MODULE_NAME,"time to read and write\n");
               ret = QX_RET_OK;
               break;
            }
        }  
    }while(ret > 0);
    FUNC_EXIT(MODULE_NAME)
    return ret;
}
int qx_socket_close(int sockfd)
{
    FUNC_ENTER(MODULE_NAME)
    return sci_sock_socketclose(sockfd);
}
