#ifndef __LEBAO_NETWORK_H
#define __LEBAO_NETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sci_types.h"
#include "mn_type.h"

int lebao_network_connect(void);
MN_DUAL_SYS_E lebao_get_active_sim(void);
int lebao_get_net_id(void);
BOOLEAN lebao_gprs_is_opened(void);
BOOLEAN lebao_active_network(void);
BOOLEAN lebao_deactive_network(void);
BOOLEAN lebao_network_is_connected(void);
void lebao_network_enter_setting(void);

#ifdef __cplusplus
}
#endif

#endif
