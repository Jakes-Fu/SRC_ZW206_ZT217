/*
 * bds_client_context.h
 *
 *  Created on: 2020��7��15��
 *      Author: liweigao
 */

#ifndef INCLUDE_BDS_CLIENT_CONTEXT_H_
#define INCLUDE_BDS_CLIENT_CONTEXT_H_

#include <stdint.h>
#include <stdbool.h>

#define ERROR_BDSC_INVALID_RESOURCE             -1000
#define ERROR_BDSC_ASR_START_FAILED             -2000
#define ERROR_BDSC_ASR_CANCEL_FAILED            -2001
#define ERROR_BDSC_ASR_NET_ERROR                -2002
#define ERROR_BDSC_ASR_HD_SERVER_ERROR          -2003
#define ERROR_BDSC_ASR_TYPE_NOT_RSP             -2004
#define ERROR_BDSC_RECORDER_START_FAILED        -3000
#define ERROR_BDSC_RECORDER_READ_FAILED         -3001
#define ERROR_BDSC_EVENTUPLOAD_START_FAILED     -4000
#define ERROR_BDSC_EVENTUPLOAD_CANCEL_FAILED    -4001
#define ERROR_BDSC_EVENTUPLOAD_ENGINE_BUSY      -4002
#define ERROR_BDSC_EVENTUPLOAD_NET_ERROR        -4003
#define ERROR_BDSC_EVENTUPLOAD_HD_SERVER_ERROR  -4004
#define ERROR_BDSC_EVENTUPLOAD_TYPE_NOT_RSP     -4005
#define ERROR_BDSC_PUSH_NET_ERROR               -4006

#define LENGTH_SN                               37
#define LENGTH_KEY                              64
#define LENGTH_CUID                             65
#define LENGTH_HOST                             64
#define LENGTH_APP                              64
#define LENGTH_URI                              128
#define LENGTH_PATH                             128

#define KEY_HOST                                "host"
#define KEY_PORT                                "port"
#define KEY_PATH                                "path"
#define KEY_WS_HEAD_HOST                        "head_host"
#define KEY_ENGINE_URI                          "engine_uri"
#define KEY_CUID                                "cuid"
#define KEY_PID                                 "pid"
#define KEY_KEY                                 "key"

typedef enum {
    LC_PROTOCOL_TCP,
    LC_PROTOCOL_TLS
} LC_PROTOCOL;

typedef struct {
    int32_t flag;
    uint16_t offset;
    uint16_t buffer_length;
    uint8_t buffer[];
} bdsc_audio_t;

typedef struct {
    int32_t code;
    uint16_t info_length;
    char info[];
} bdsc_error_t;

typedef struct {
    int32_t level;
} bds_client_context_t;

bdsc_error_t* bdsc_error_create(int32_t code, uint16_t info_length, char *info);

void bdsc_error_destroy(bdsc_error_t *error);

bdsc_audio_t* bdsc_audio_create(uint16_t buffer_length);

int32_t bdsc_audio_append(bdsc_audio_t *audio, uint8_t *src, uint16_t src_length);

bool bdsc_audio_is_full(bdsc_audio_t *audio);

uint8_t* bdsc_audio_get_offset_addr(bdsc_audio_t *audio);

uint16_t bdsc_audio_get_left_length(bdsc_audio_t *audio);

void bdsc_audio_destroy(bdsc_audio_t *audio);

#endif /* INCLUDE_BDS_CLIENT_CONTEXT_H_ */
