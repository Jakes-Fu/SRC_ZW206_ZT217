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
// Author: Huang Jianfeng (Huangjianfeng02@baidu.com)
//
// Description: Definition of baidu audio input layer

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_INPUT_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_INPUT_H

#include "include/lightduer_audio_error.h"
#include "include/lightduer_audio_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_INPUT_UNKNOWN,
    AUDIO_INPUT_FILE,
    AUDIO_INPUT_HTTP,
    AUDIO_INPUT_HLS,
    AUDIO_INPUT_FLASH,
    AUDIO_INPUT_STREAM,
    AUDIO_INPUT_RAM,
} duer_audio_input_type_t;

typedef struct duer_audio_input_context_s {
    void *audio_context;
    duer_audio_input_type_t type;
    const char *url; // no need to free
    uint32_t total_size;
    uint32_t position;
    void *priv_data;
    int download_stop_flag;
    duer_bool resumable;        // whether resumable type
    int download_retry_time;
    int download_connect_timeout;
    duer_bool nearly_finished;
} duer_audio_input_context_t;

typedef struct duer_audio_input_s {
    duer_audio_input_type_t type;
    duer_audio_error_t (*open)(duer_audio_input_context_t* context, const char *path);
    duer_audio_error_t (*start)(duer_audio_input_context_t* context);
    duer_audio_error_t (*read)(duer_audio_input_context_t* context,
                               duer_audio_data_buff_t *buffer);
    duer_audio_error_t (*seek)(duer_audio_input_context_t* context, size_t offset);
    duer_audio_error_t (*write)(duer_audio_input_context_t* context, const char *data, int size);
    duer_audio_error_t (*stop)(duer_audio_input_context_t* context);
    duer_audio_error_t (*close)(duer_audio_input_context_t* context);
    struct duer_audio_input_s *next;
} duer_audio_input_t;

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_INPUT_H
