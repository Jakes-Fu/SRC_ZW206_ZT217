/*
 * bds_client_event.h
 *
 *  Created on: 2020��7��15��
 *      Author: liweigao
 */

#ifndef INCLUDE_BDS_CLIENT_EVENT_H_
#define INCLUDE_BDS_CLIENT_EVENT_H_

#include <stdbool.h>
#include <stdint.h>

#include "bds_client_context.h"

typedef enum {
    EVENT_ASR_BEGIN = 1000,
    EVENT_ASR_RESULT,
    EVENT_ASR_EXTERN_DATA,
    EVENT_ASR_TTS_DATA,
    EVENT_ASR_END,
    EVENT_ASR_CANCEL,
    EVENT_ASR_ERROR,
    EVENT_ASR_MIDDLE_RESULT,
    EVENT_EVENTUPLOAD_BEGIN = 2000,
    EVENT_EVENTUPLOAD_END,
    EVENT_EVENTUPLOAD_DATA,
    EVENT_EVENTUPLOAD_TTS,
    EVENT_EVENTUPLOAD_CANCEL,
    EVENT_EVENTUPLOAD_ERROR,
    EVENT_PUSH_DATA = 3000,
    EVENT_PUSH_ERROR,
    EVENT_PUSH_DISCONNECTED,
    EVENT_PUSH_CONNECTED
} bdsc_event_key_t;

// content may be basetype or struct, can custom
typedef struct {
    bdsc_event_key_t key;
    void *content;
    uint16_t content_length;
} bds_client_event_t;

typedef struct {
    char sn[LENGTH_SN];
    int32_t code;
    uint16_t info_length;
    char info[];
} bdsc_event_error_t;

typedef struct {
    char sn[LENGTH_SN];
    uint16_t buffer_length;
    uint8_t buffer[];
} bdsc_event_data_t;

typedef struct {
    char sn[LENGTH_SN];
} bdsc_event_process_t;

bdsc_event_error_t* bdsc_event_error_create(char *sn, int32_t code,
        uint16_t info_length, char *info);

void bdsc_event_error_destroy(bdsc_event_error_t *error);

bdsc_event_data_t* bdsc_event_data_create(char *sn, uint16_t buffer_length, uint8_t *buffer);

void bdsc_event_data_destroy(bdsc_event_data_t *data);

bds_client_event_t* bdsc_event_create(bdsc_event_key_t key,
        void *content, uint16_t content_length);

bool bdsc_deepcopy_event(bds_client_event_t *dst, bds_client_event_t *src);

bds_client_event_t* bdsc_clone_event(bds_client_event_t *src);

void bdsc_deepdestroy_event(bds_client_event_t *event);

bdsc_event_process_t* bdsc_event_process_create(char *sn);

void bdsc_event_process_destroy(bdsc_event_process_t *process);

#endif /* INCLUDE_BDS_CLIENT_EVENT_H_ */
