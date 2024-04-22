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
// Description: Definition of baidu audio output structs

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_OUTPUT_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_OUTPUT_H

#include <stdbool.h>
#include "include/lightduer_audio_error.h"
#include "include/lightduer_audio_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BIT(x) (1 << x)

typedef enum {
    AUDIO_OUTPUT_UNKNOWN,
    AUDIO_OUTPUT_FILE,
    AUDIO_OUTPUT_CODEC,
} duer_audio_output_type_t;

typedef void (*duer_output_on_stuttered_cb)(int type, bool is_stuttered);

typedef struct duer_audio_output_context_s {
    void *audio_context;
    int output_channel;
    duer_audio_output_type_t type;
    duer_audio_type_t audio_type;
    unsigned int play_time;
    int inited;
    void *priv_data;
    int source_type;
    duer_output_on_stuttered_cb stuttered_cb;
} duer_audio_output_context_t;

typedef struct duer_audio_output_s {
    duer_audio_output_type_t type;
    int demuxer_setting;
    int decode_setting;
    duer_audio_error_t (*init)(duer_audio_output_context_t *context);
    duer_audio_error_t (*parse)(duer_audio_output_context_t *context,
                                duer_audio_data_buff_t *buffer);
    duer_audio_error_t (*decode)(duer_audio_output_context_t *context,
                                 duer_audio_data_buff_t *packet);
    duer_audio_error_t (*write)(duer_audio_output_context_t *context,
                                duer_audio_data_buff_t *frame);
    duer_audio_error_t (*pause)(duer_audio_output_context_t *context);
    duer_audio_error_t (*resume)(duer_audio_output_context_t *context);
    duer_audio_error_t (*stuck_begin)(duer_audio_output_context_t *context);
    duer_audio_error_t (*stuck_end)(duer_audio_output_context_t *context);
    duer_audio_error_t (*stop)(duer_audio_output_context_t *context);
    duer_audio_error_t (*wait_end)(duer_audio_output_context_t *context);
    duer_audio_error_t (*uninit)(duer_audio_output_context_t *context);
} duer_audio_output_t;

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_OUTPUT_H
