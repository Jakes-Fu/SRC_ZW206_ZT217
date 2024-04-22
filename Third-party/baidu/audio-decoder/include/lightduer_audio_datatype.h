/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// Author: Huang Jianfeng(huangjianfeng02@baidu.com)
//
// Description: Definition of audio data buff

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DATATYPE_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DATATYPE_H

#include <stdint.h>
#include <stddef.h>
#include "lightduer_audio_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_BUFFER_SIZE   (2048)
#define AUDIO_PACKET_SIZE   (2048)
#ifdef ORIGINAL_AAC_DECODE_LIB
#define AUDIO_FRAME_SIZE    (1152 * 2 * 2 * 2)
#else
#define AUDIO_FRAME_SIZE    (1152 * 2 * 2)
#endif

typedef enum {
    AUDIO_TYPE_MP3,
    AUDIO_TYPE_AAC,
    AUDIO_TYPE_M4A,
    AUDIO_TYPE_WAV,
    AUDIO_TYPE_TS,
    AUDIO_TYPE_HLS,
    AUDIO_TYPE_PCM,
    AUDIO_TYPE_UNSPECIFIED,
    AUDIO_TYPE_UNKNOWN,
} duer_audio_type_t;

typedef struct duer_audio_data_buff_s {
    uint8_t *head;
    uint8_t *data;
    uint8_t *tail;
    uint8_t *end;
} duer_audio_data_buff_t;

/**
 * DESC:
 * This function is used to init the audio data buffer, malloc memory and set pointers
 *
 * PARAM:
 * adb, the pointer of the audio data buffer
 * data_size, the data length of the malloc memory
 *
 * @RETURN:
 * AUDIO_ERROR_NONE, if success
 * others, if failed
 */
duer_audio_error_t duer_audio_init_adb(duer_audio_data_buff_t *adb, size_t data_size);

/**
 * DESC:
 * This function is used to pull the data pointer of target audio data buffer
 *
 * PARAM:
 * adb, the pointer of the audio data buffer
 * data_size, how many bytes the data pointer will be move forward
 *
 * @RETURN:
 * AUDIO_ERROR_NONE, if success
 * others, if failed
 */
duer_audio_error_t duer_audio_adb_pull(duer_audio_data_buff_t *adb, size_t len);

/**
 * DESC:
 * This function is used to push the data pointer of target audio data buffer
 *
 * PARAM:
 * adb, the pointer of the audio data buffer
 * data_size, how many bytes the data pointer will be move backward
 *
 * @RETURN:
 * AUDIO_ERROR_NONE, if success
 * others, if failed
 */
duer_audio_error_t duer_audio_adb_push(duer_audio_data_buff_t *adb, size_t len);

/**
 * DESC:
 * This function is used to move the tail pointer of target audio data buffer
 *
 * PARAM:
 * adb, the pointer of the audio data buffer
 * data_size, how many bytes the tail pointer will be move forward
 *
 * @RETURN:
 * AUDIO_ERROR_NONE, if success
 * others, if failed
 */
duer_audio_error_t duer_audio_adb_put(duer_audio_data_buff_t *adb, size_t len);

/**
 * DESC:
 * This function is used to clear the audio data buffer
 *
 * PARAM:
 * adb, the pointer of the audio data buffer
 *
 * @RETURN:
 * AUDIO_ERROR_NONE, if success
 * others, if failed
 */
duer_audio_error_t duer_audio_adb_clear(duer_audio_data_buff_t *adb);

/**
 * DESC:
 * This function is used to get data size of the audio data buffer
 *
 * PARAM:
 * adb, the pointer of the audio data buffer
 *
 * @RETURN:
 * the size of data
 */
int duer_audio_adb_get_data_size(duer_audio_data_buff_t *adb);

/**
 * DESC:
 * This function is used to uninit the audio data buffer, free memorys and reset pointers
 *
 * PARAM:
 * adb, the pointer of the audio data buffer
 *
 * @RETURN:
 * AUDIO_ERROR_NONE, if success
 * others, if failed
 */
duer_audio_error_t duer_audio_uninit_adb(duer_audio_data_buff_t *adb);

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DATATYPE_H
