/*
 * bds_client.h
 *
 *  Created on: 2020/7/15
 *      Author: liweigao
 */

#ifndef INCLUDE_BDS_CLIENT_H_
#define INCLUDE_BDS_CLIENT_H_

#include <stdint.h>

#include "bds_client_context.h"
#include "bds_client_event.h"
#include "bds_client_command.h"
#include "bds_client_param.h"

typedef void *bds_client_handle;

typedef int32_t (*bds_client_event_listener_f)(bds_client_event_t *event,
        void *custom);

bds_client_handle bds_client_create(bds_client_context_t *context);

void bds_client_set_event_listener(bds_client_handle handle,
        bds_client_event_listener_f listener, void *custom);

int32_t bds_client_start(bds_client_handle handle);

int32_t bds_client_send(bds_client_handle handle,
        bds_client_command_t *command);

int32_t bds_client_stop(bds_client_handle handle);

int bds_client_destroy(bds_client_handle handle);

char* bds_client_get_version();

int bds_client_get_sn(char *buffer, int length);

/**
 *  0:E  1:W  2:I 3:D 4:V
 */
void bds_client_set_log_level(int level);

#endif /* INCLUDE_BDS_CLIENT_H_ */
