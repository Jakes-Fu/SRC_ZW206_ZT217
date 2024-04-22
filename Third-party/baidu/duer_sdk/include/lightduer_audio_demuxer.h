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
// Description: Definition of baidu audio demuxer structs

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DEMUXER_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DEMUXER_H

#include <stddef.h>
#include "include/lightduer_audio_error.h"
#include "include/lightduer_audio_datatype.h"
#include "include/lightduer_audio_util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_DEMUXER_UNKNOWN,
    AUDIO_DEMUXER_UNSPECIFIED,
    AUDIO_DEMUXER_M4A,
    AUDIO_DEMUXER_TS,
    AUDIO_DEMUXER_WAV,
    AUDIO_DEMUXER_PCM
} duer_audio_demuxer_type_t;

typedef struct duer_audio_demuxer_context_s {
    void *audio_context;
    duer_audio_demuxer_type_t type;
    uint32_t position;
    void *priv_data;
} duer_audio_demuxer_context_t;

typedef struct duer_audio_demuxer_s {
    duer_audio_demuxer_type_t type;
    duer_audio_error_t (*init)(duer_audio_demuxer_context_t *context);
    duer_audio_error_t (*parse)(duer_audio_demuxer_context_t *context,
                                duer_audio_data_buff_t *buffer,
                                duer_audio_data_buff_t *packet);
    duer_audio_error_t (*uninit)(duer_audio_demuxer_context_t *context);
    struct duer_audio_demuxer_s *next;
} duer_audio_demuxer_t;

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DEMUXER_H
