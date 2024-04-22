/****************************************************************************
** File Name:      socket.c                                              *
** Author:         	                                          *
** Date:           15/10/2019                                              *
** Copyright:                                                              *
** Description:      socket                                        *
****************************************************************************/
/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#if 1//unisoc added
// MACOR platform interface
#include "socket_types.h"
#include "sci_types.h"
#include "os_api.h"
#include "socket_api.h"
#include "sfs.h"
#endif
#include "socket.h" ///for up layer
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**
   * Marco
   */
#define SOCKET_DEBUG
#ifdef SOCKET_DEBUG
#define DEBUG  SCI_TRACE_LOW
#else
#define DEBUG
#endif

#define POLLIN		01              /* There is data to read.  */
#define POLLPRI		02              /* There is urgent data to read.  */
#define POLLOUT		04              /* Writing now will not block.  */


/**
  * global value
  */
LOCAL unsigned int g_socket_nitId = 0;
LOCAL BOOLEAN g_socket_data_save = FALSE;

PUBLIC void socketSaveDataToFile(char* buff_ptr, int32 len);

/**
  * static funciton
  */
LOCAL int socket_ErrorMap(int socket_error)
{
	int error_no = socket_error;
	
	switch(socket_error)
	{
		case ENOBUFS:
			error_no = 9917;
			break;

		case ETIMEDOUT:
			error_no = 9938;
			break;

		case EISCONN:
			error_no = 9904;
			break;

		case EOPNOTSUPP:
			error_no = 9929;
			break;

		case ECONNABORTED:
			error_no = 9906;
			break;

		case EWOULDBLOCK:
			error_no = 9930;
			break;

		case ECONNREFUSED:
			error_no = 9908;
			break;

		case ECONNRESET:
			error_no = 9909;
			break;

		case ENOTCONN:
			error_no = 9925;
			break;

		case EALREADY:
			error_no = 9907;
			break;

		case EINVAL:
			error_no = 9943;
			break;

		case EMSGSIZE:
			error_no = 9913;
			break;

		case EPIPE:
			error_no = 9950;
			break;

		case EDESTADDRREQ:
			error_no = 9910;
			break;

		//case ESHUTDOWN:
			//error_no = 15;
			//break;

		case ENOPROTOOPT:
			error_no = 9921;
			break;

		//case EHAVEOOB:
			//error_no = 17;
			//break;

		case ENOMEM:
			error_no = 9971;
			break;

		case EADDRNOTAVAIL:
			error_no = 9903;
			break;

		case EADDRINUSE:
			error_no = 9902;
			break;

		case EAFNOSUPPORT:
			error_no = 9901;
			break;

		case EINPROGRESS:
			error_no = 9928;
			break;

		//case ELOWER:
			//error_no = 23;
			//break;

		case ENOTSOCK:
			error_no = 9923;
			break;

		//case EIEIO:
			//error_no = 27;
			//break;

		//case ETOOMANYREFS:
			//error_no = 28;
			//break;

		case EFAULT:
			error_no = 9948;
			break;

		case ENETUNREACH:
			error_no = 9916;
			break;
		default:
			break;
	}
	DEBUG("socket [%s:%d] socket error: %d map to %d\n ",__FUNCTION__,__LINE__,socket_error, error_no);
	return error_no;
}

/**
  *	@Note:Up layer interface. 
  */
#ifndef LWM2M_SUPPORT_4
PUBLIC void socket_SetNetId(unsigned int id)
{
    g_socket_nitId = id;
}


PUBLIC unsigned int socket_getNetId(void)
{
    return g_socket_nitId;
}


struct hostent *gethostbynameBlock(const char *hostname)
{
    struct hostent *host_ptr = PNULL;
    int count = 12;
    DEBUG("[socket debug %s       %d]\n",__FUNCTION__,__LINE__);
    while (count--)
    {
        host_ptr = gethostbyname_block(hostname, TCPIPNETIF_GetNetifWithNetid(g_socket_nitId));
        if (host_ptr != NULL)
        {
            DEBUG("[socket debug %s       %d] family: %d\n",__FUNCTION__,__LINE__, host_ptr->h_addrtype);
            break;
        }
    }

    return host_ptr;
}

#endif

#ifdef LWM2M_SUPPORT_4
/*
 * SPLIBC_he2ai()
 *
 * This function returns a pointer to the first element of a newly allocated
 * addrinfo struct linked list filled with the data of a given hostent.
 * addrinfo is meant to work like the addrinfo struct does for a IPv6
 * stack, but usable also for IPv4, all hosts and environments.
 *
 * The memory allocated by this function *MUST* be free'd later on calling
 * Curl_freeaddrinfo().  For each successful call to this function there
 * must be an associated call later to Curl_freeaddrinfo().
 *
 *   addrinfo defined in "lib/addrinfo.h"
 *
 *     struct addrinfo {
 *       int                   ai_flags;
 *       int                   ai_family;
 *       int                   ai_socktype;
 *       int                   ai_protocol;
 *       curl_socklen_t        ai_addrlen;   * Follow rfc3493 struct addrinfo *
 *       char                 *ai_canonname;
 *       struct sockaddr      *ai_addr;
 *       struct addrinfo *ai_next;
 *     };
 *
 *   hostent defined in <netdb.h>
 *
 *     struct hostent {
 *       char    *h_name;
 *       char    **h_aliases;
 *       int     h_addrtype;
 *       int     h_length;
 *       char    **h_addr_list;
 *     };
 *
 *   for backward compatibility:
 *
 *     #define h_addr  h_addr_list[0]
 */

static struct addrinfo *threax_he2ai(const struct hostent *he, int port)
{
    struct addrinfo *ai = NULL;
    struct addrinfo *prevai = NULL;
    struct addrinfo *firstai = NULL;
    struct sockaddr_in *addr;
#ifdef ENABLE_IPV6
    struct sockaddr_in6 *addr6;
#endif
    int result = 0;
    int i;
    char *curr;

    if(!he)
    /* no input == no output! */
    return NULL;

    if ((he->h_name == NULL) || (he->h_addr_list == NULL))
    {
        DEBUG("[socket debug  ] [%s  %d ]]",__FUNCTION__,__LINE__);
        return NULL;
    }

    //SCI_ASSERT((he->h_name != NULL) && (he->h_addr_list != NULL));

    for(i = 0; (curr = he->h_addr_list[i]) != NULL; i++) {
        size_t ss_size;
        size_t namelen = strlen(he->h_name) + 1; /* include zero termination */
#ifdef ENABLE_IPV6
        if(he->h_addrtype == AF_INET6)
          ss_size = sizeof(struct sockaddr_in6);
        else
#endif
          ss_size = sizeof(struct sockaddr_in);

        /* allocate memory to hold the struct, the address and the name */
        ai = SCI_ALLOCAZ( sizeof(struct addrinfo) + ss_size + namelen);
        if(!ai) {
          result = 27;//CURLE_OUT_OF_MEMORY;
         DEBUG("[socket debug] calloc error");
          break;
        }
        /* put the address after the struct */
        ai->ai_addr = (void *)((char *)ai + sizeof(struct addrinfo));
        /* then put the name after the address */
        ai->ai_canonname = (char *)ai->ai_addr + ss_size;
        memcpy(ai->ai_canonname, he->h_name, namelen);

        if(!firstai)
          /* store the pointer we want to return from this function */
          firstai = ai;

        if(prevai)
          /* make the previous entry point to this */
          prevai->ai_next = ai;

        ai->ai_family = he->h_addrtype;

        /* we return all names as STREAM, so when using this address for TFTP
           the type must be ignored and conn->socktype be used instead! */
        ai->ai_socktype = SOCK_STREAM;

        ai->ai_addrlen = (int)ss_size;

        /* leave the rest of the struct filled with zero */

        switch(ai->ai_family) {
            case AF_INET:
              addr = (void *)ai->ai_addr; /* storage area for this info */

              memcpy(&addr->sin_addr, curr, sizeof(struct in_addr));
              addr->sin_family = (int)(he->h_addrtype);
              addr->sin_port = htons((unsigned short)port);
              DEBUG("[socket debug] ip4:%s", inet_ntoa(addr->sin_addr.s_addr));
              break;

#ifdef ENABLE_IPV6
            case AF_INET6:
              addr6 = (void *)ai->ai_addr; /* storage area for this info */

              memcpy(&addr6->sin6_addr, curr, sizeof(struct in6_addr));
              addr6->sin6_family = (int)(he->h_addrtype);
              addr6->sin6_port = htons((unsigned short)port);
              break;
#endif
            default:
                break;
        }

        prevai = ai;
    }

    if(result) {
        freeaddrinfo(firstai);
        firstai = NULL;
    }
    DEBUG("[socket debug] result:%d, firstai:0x%x", result, firstai);

    return firstai;
}
#endif

/**
  * @Note: don't change API name /input/ ouput 
*/

/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
   Returns a file descriptor for the new socket, or -1 for errors.  */
PUBLIC int dm_socket (int __domain, int __type, int __protocol)  
{
    int sock_id = -1;
    DEBUG("[socket debug %s     %d]>>>>>>>>  __domain: %d, __type: %d, __protocol: %d, netid: %d\n",
        __FUNCTION__,__LINE__, __domain, __type, __protocol, socket_getNetId());
    if (__domain == 0)
    {
        __domain = AF_INET;
    }
    __protocol = 0; //unisoc added
    if (__type == SOCK_STREAM)
    {
        sock_id = sci_sock_socket(__domain, __type, __protocol, socket_getNetId());
        if (sock_id  != -1)
        {
            DEBUG("[socket ok %s     %d]sci_sock_asyncselect start\n",__FUNCTION__,__LINE__);
            //sci_sock_asyncselect(sock_id, SCI_IdentifyThread(), AS_READ | AS_WRITE | AS_CONNECT | AS_CLOSE);
            DEBUG("[socket ok %s     %d]sci_sock_asyncselect end\n",__FUNCTION__,__LINE__);
        }
        else
        {
            DEBUG("[socket error %s     %d]\n",__FUNCTION__,__LINE__);
        }
    }
    else if (__type == SOCK_DGRAM)
    {
        sock_id = sci_sock_socket(__domain, __type, __protocol, socket_getNetId());
        if (sock_id != -1)
        {
            //sci_sock_asyncselect(sock_id, SCI_IdentifyThread(), AS_READ | AS_WRITE);
            DEBUG("[socket ok %s     %d]\n",__FUNCTION__,__LINE__);
        }
        else
        {
            DEBUG("[socket error %s     %d]\n",__FUNCTION__,__LINE__);
        }
    }
    //set the socket is block mod
    //if (sci_sock_setsockopt(sock_id, SO_BIO, PNULL) == -1)
    //{
        //DEBUG("[socket error %s     %d]\n",__FUNCTION__,__LINE__);
       // sci_sock_socketclose(sock_id);
    //}
    DEBUG("[socket debug %s     %d]sock_id is 0x%x , fd:0x%x <<<<<<<<<  \n", __FUNCTION__, __LINE__, sock_id, TO_SOCKET_FD(sock_id));
    return TO_SOCKET_FD(sock_id);
}

int dm_bind(int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len)
{
    DEBUG("[socket debug %s     %d  __fd:0x%x, id:0x%x >>>>>>>>  \n",__FUNCTION__,__LINE__, __fd, TO_SCI_SOCKET_ID(__fd));
    return sci_sock_bind(TO_SCI_SOCKET_ID(__fd), __addr, __len);
}



/* Open a connection on socket FD to peer at ADDR (which LEN bytes long).
   For connectionless socket types, just set the default address to send to
   and the only address from which to accept transmissions.
   Return 0 on success, -1 for errors.

   This function is a cancellation point and therefore not marked with
    .  */
PUBLIC int dm_connect (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len)
{
    DEBUG("[socket debug %s     %d  __fd:0x%x, id:0x%x >>>>>>>>  \n",__FUNCTION__,__LINE__, __fd, TO_SCI_SOCKET_ID(__fd));
    return sci_sock_connect(TO_SCI_SOCKET_ID(__fd), __addr, __len);
}

/* Read N bytes into BUF through socket FD.
   If ADDR is not NULL, fill in *ADDR_LEN bytes of it with tha address of
   the sender, and store the actual size of the address in *ADDR_LEN.
   Returns the number of bytes read or -1 for errors.

   This function is a cancellation point and therefore not marked with
    .  */
PUBLIC ssize_t dm_recvfrom (int __fd, void *__restrict __buf, size_t __n,
			 int __flags, __SOCKADDR_ARG __addr,
			 socklen_t *__restrict __addr_len)
{
    DEBUG("[socket debug %s     %d  __fd:0x%x, id:0x%x >>>>>>>>  \n",__FUNCTION__,__LINE__, __fd, TO_SCI_SOCKET_ID(__fd));
    return sci_sock_recvfrom(TO_SCI_SOCKET_ID(__fd), __buf, __n, __flags,  __addr);
}

/* Send N bytes of BUF on socket FD to peer at address ADDR (which is
   ADDR_LEN bytes long).  Returns the number sent, or -1 for errors.

   This function is a cancellation point and therefore not marked with
    .  */
PUBLIC ssize_t dm_sendto (int __fd, const void *__buf, size_t __n,
		       int __flags, __CONST_SOCKADDR_ARG __addr,
		       socklen_t __addr_len)
{
    DEBUG("[socket debug %s     %d  __fd:0x%x, id:0x%x >>>>>>>>  \n",__FUNCTION__,__LINE__, __fd, TO_SCI_SOCKET_ID(__fd));
    return sci_sock_sendto(TO_SCI_SOCKET_ID(__fd), __buf, __n, __flags, __addr);
}

PUBLIC int dm_close(int __fd)
{
    DEBUG("[socket debug %s     %d  __fd:0x%x, id:0x%x >>>>>>>>  \n",__FUNCTION__,__LINE__, __fd, TO_SCI_SOCKET_ID(__fd));
    return sci_sock_socketclose(TO_SCI_SOCKET_ID(__fd));
}
#ifdef   __cplusplus
    }
#endif

