/*
 * bds_client_command.h
 *
 *  Created on: 2020��7��15��
 *      Author: liweigao
 */

#ifndef INCLUDE_BDS_CLIENT_COMMAND_H_
#define INCLUDE_BDS_CLIENT_COMMAND_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bds_client_context.h"

typedef enum {
    CMD_ASR_START = 100,
    CMD_ASR_CANCEL,
    CMD_ASR_STOP,
    CMD_ASR_EXTERN_DATA,
    CMD_ASR_CONFIG,
    CMD_EVENTUPLOAD_START = 200,
    CMD_EVENTUPLOAD_CONFIG,
    CMD_EVENTUPLOAD_CANCEL,
    CMD_EVENTUPLOAD_DATA,
    CMD_PUSH_START = 300,
    CMD_PUSH_STOP,
    CMD_PUSH_PING,
} bdsc_cmd_key_t;

typedef struct {
    bdsc_cmd_key_t key;
    void *content;
    uint16_t content_length;
} bds_client_command_t;

typedef struct {
    char sn[LENGTH_SN];
    int32_t flag;
    uint16_t buffer_length;
    uint8_t buffer[];
} bdsc_cmd_data_t;

bdsc_cmd_data_t* bdsc_cmd_data_create(int32_t flag, uint16_t buffer_length,
        uint8_t *buffer, char *sn);

void bdsc_cmd_data_destroy(bdsc_cmd_data_t *data);

bool bdsc_deepcopy_command(bds_client_command_t *dst,
        bds_client_command_t *src);

void bdsc_deepdestroy_command(bds_client_command_t *command);

#endif /* INCLUDE_BDS_CLIENT_COMMAND_H_ */
