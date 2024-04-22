/*
 * bds_client_param.h
 *
 *  Created on: 2020��7��15��
 *      Author: liweigao
 */

#ifndef INCLUDE_BDS_CLIENT_PARAM_H_
#define INCLUDE_BDS_CLIENT_PARAM_H_

#include "bds_client_context.h"

typedef struct {
    char sn[LENGTH_SN];
    uint16_t pam_len;
    char pam[];
} bdsc_asr_params_t;

typedef struct {
    char sn[LENGTH_SN];
    uint16_t pam_len;
    char pam[];
} bdsc_eventupload_params_t;

typedef struct {
    char sn[LENGTH_SN];
    LC_PROTOCOL protocol;
    uint32_t pid;
    char key[LENGTH_KEY];
    char cuid[LENGTH_CUID];
    char host[LENGTH_HOST];
    int port;
    uint16_t pam_len;
    char pam[];
} bdsc_push_params_t;

bdsc_asr_params_t* bdsc_asr_params_create(char *sn, uint16_t pam_len, char *pam);

void bdsc_asr_params_destroy(bdsc_asr_params_t *params);

bdsc_eventupload_params_t* bdsc_event_params_create(char *sn, uint16_t pam_len, char *pam);

void bdsc_event_params_destroy(bdsc_eventupload_params_t *params);

bdsc_push_params_t* bdsc_push_params_create(char *sn, LC_PROTOCOL protocol, uint32_t pid,
        char *key, char *cuid, char* host, int port, uint16_t pam_len, char *pam);

void bdsc_push_params_destroy(bdsc_push_params_t *params);

#endif /* INCLUDE_BDS_CLIENT_PARAM_H_ */
