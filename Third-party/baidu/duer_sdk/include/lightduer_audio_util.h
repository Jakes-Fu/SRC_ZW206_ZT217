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
// Description: Definition of baidu audio utility functions

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_UTIL_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_UTIL_H

#include <stdio.h>
#include "include/lightduer_audio_logger.h"
#include "include/lightduer_audio_error.h"
#include "include/lightduer_audio_datatype.h"
#include "include/lightduer_audio_demuxer.h"
#include "include/lightduer_audio_decoder.h"
#include "include/lightduer_audio_input.h"
#include "include/lightduer_audio_output.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct duer_audio_pcm_info_s {
    int bit_rate;
    int sample_rate;
    short channel;
    short bits_per_sample;
    int byte_rate;
} duer_audio_pcm_info_t;

typedef struct duer_audio_context_s {
    duer_audio_input_t              *input;
    duer_audio_demuxer_t            *demuxer;
    duer_audio_decoder_t            *decoder;
    duer_audio_output_t             *output;

    duer_audio_input_context_t      *input_context;
    duer_audio_demuxer_context_t    *demuxer_context;
    duer_audio_decoder_context_t    *decoder_context;
    duer_audio_output_context_t     *output_context;

    duer_audio_type_t               audio_type;
    duer_audio_type_t               adjust_type;
    duer_audio_demuxer_type_t       demuxer_type;
    duer_audio_decoder_type_t       decoder_type;

    duer_audio_data_buff_t          *input_buf;
    duer_audio_data_buff_t          *demuxer_buf;
    duer_audio_data_buff_t          *decode_buf;

    duer_audio_pcm_info_t           pcm_info;

    int                             demuxer_accel;
    int                             decode_accel;

    int                             seek_time;
    int                             seek_position;
} duer_audio_context_t;

/**
 * DESC:
 * This function is used to get the audio type of input url
 * PARAM:
 * path: url path
 *
 * @RETURN: duer_audio_type_t
 */
duer_audio_type_t duer_audio_get_audio_type(const char *path);

/**
 * DESC:
 * This function is used to registe all the plugins, include inputs, demuxers, decoders and output
 *
 * PARAM: NULL
 *
 * @RETURN: NULL
 */
void duer_audio_register_plugin(void);

/**
 * DESC:
 * This function is used to init the demuxer according to context infos
 *
 * PARAM:
 * context: the pointer which points to the audio context
 *
 * @RETURN: duer_audio_error_t
 */
duer_audio_error_t duer_audio_init_demuxer(duer_audio_context_t *context);

/**
 * DESC:
 * This function is used to init the decoder according to context infos
 *
 * PARAM:
 * context: the pointer which points to the audio context
 *
 * @RETURN: duer_audio_error_t
 */
duer_audio_error_t duer_audio_init_decoder(duer_audio_context_t *context);

/**
 * DESC:
 * This function is used to init the audio cnotext
 *
 * PARAM:
 * url, the path of input audio source file
 * resumable, whether resumable type
 * has_pre_download, whether has pre-download
 * spec_type, we know the audio type of the url, pass it by this param
 * context, output the audio context
 *
 * @RETURN: duer_audio_error_t
 */
duer_audio_error_t duer_audio_init_context(const char *url, duer_bool resumable,
        duer_bool has_pre_download, duer_audio_type_t spec_type, duer_audio_context_t **context);

/**
 * DESC:
 * This function is used to uninit the audio cnotext
 *
 * PARAM:
 * context, the pointer which points to audio context
 *
 * @RETURN: NULL
 */
void duer_audio_uninit_context(duer_audio_context_t *context);

/**
 * DESC:
 * This function is used to init the pre-download input cnotext
 *
 * PARAM:
 * url, the path of input audio source file
 * input, out input
 *
 * @RETURN: the pointer which points to input context
 */
duer_audio_input_context_t *duer_audio_init_pre_download_input_context(
        const char *url, duer_audio_input_t **input);

/**
 * DESC:
 * This function is used to adjust the audio type
 *
 * PARAM:
 * context, the pointer which points to audio context
 *
 * @RETURN: duer_audio_error_t
 */
duer_audio_error_t duer_audio_adjust_audio_type(duer_audio_context_t *context);

/**
 * DESC:
 * This function is used to check channel
 *
 * PARAM:
 * channel, the channel of audio
 *
 * @RETURN: DUER_TURE: valid, DUER_FALSE: unvalid
 */
duer_bool duer_audio_check_channel(int channel);

/**
 * DESC:
 * This function is used to check sample_rate
 *
 * PARAM:
 * sample_rate, the sample_rate of audio
 *
 * @RETURN: DUER_TURE: valid, DUER_FALSE: unvalid
 */
duer_bool duer_audio_check_sample_rate(int sample_rate);

/**
 * DESC:
 * This function is used to check bits_per_sample
 *
 * PARAM:
 * bits_per_sample, the bits_per_sample of audio
 *
 * @RETURN: DUER_TURE: valid, DUER_FALSE: unvalid
 */
duer_bool duer_audio_check_bits_per_sample(int bits_per_sample);

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_UTIL_H
