#ifndef __CMIOT_THREAD_H__
#define __CMIOT_THREAD_H__

#include "sci_types.h"
#include "os_types.h"

#define MAX_ASYNC_GETHOST_WAIT_TIME 20000

BLOCK_ID cmiot_get_thread_id();
os_uint32_t cmiot_net_get_netid();
os_err_t cmiot_wait_sem(void);
struct sci_hostent* get_saved_hostent(void);
int cmiot_enable_net(void);

#endif //__CMIOT_THREAD_H__
