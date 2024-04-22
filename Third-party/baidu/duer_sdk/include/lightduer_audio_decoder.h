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
// Author: Chen Xihao (chenxihao@baidu.com)
//
// Description: Audio decoder

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DECODER_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DECODER_H

#include "include/lightduer_audio_error.h"
#include "include/lightduer_audio_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DECODE_BREAK_ERROR_COUNTS 10

typedef enum {
    AUDIO_DECODER_UNKNOWN,
    AUDIO_DECODER_UNSPECIFIED,
    AUDIO_DECODER_MP3,
    AUDIO_DECODER_AAC,
    AUDIO_DECODER_PCM
} duer_audio_decoder_type_t;

typedef struct duer_audio_decoder_context_s {
    void *audio_context;
    duer_audio_decoder_type_t type;
    int header_length;
    int decoded_frame_size;
    int decoded_avg_size;
    int decoded_frame_count;
    int decoded_frame_bytes;
    void *priv_data;
} duer_audio_decoder_context_t;

typedef struct duer_audio_decoder_s {
    duer_audio_decoder_type_t type;
    duer_audio_error_t (*init)(duer_audio_decoder_context_t *context);
    duer_audio_error_t (*decode)(duer_audio_decoder_context_t *context,
                                 duer_audio_data_buff_t *in_packet,
                                 duer_audio_data_buff_t *out_frame);
    duer_audio_error_t (*uninit)(duer_audio_decoder_context_t *context);
    struct duer_audio_decoder_s *next;
} duer_audio_decoder_t;

#ifdef __cplusplus
}
#endif

#endif // BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DECODER_H
