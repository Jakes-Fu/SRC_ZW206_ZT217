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
// Description: Declearation of audio report module

#ifndef BAIDU_LIGHTDUER_AUDIO_PLAYER_LIGHTDUER_AUDIO_REPORT_H
#define BAIDU_LIGHTDUER_AUDIO_PLAYER_LIGHTDUER_AUDIO_REPORT_H

#include <stdint.h>
#include "lightduer_audio_error.h"
#include "lightduer_audio_datatype.h"
#ifdef CONFIG_AUDIO_REPORT_ENABLE
#include "lightduer_ds_log_audio.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DO_NOTHING()

#ifdef CONFIG_AUDIO_REPORT_ENABLE
#define DUER_AUDIO_REPORT_MALLOC_FAILURE    DUER_DS_LOG_AUDIO_MEMORY_OVERFLOW
#define DUER_AUDIO_REPORT_NULL_POINTER      DUER_DS_LOG_AUDIO_NULL_POINTER
#define DUER_AUDIO_REPORT_INVALID_CONTEXT   DUER_DS_LOG_AUDIO_INVALID_CONTEXT
#else
#define DUER_AUDIO_REPORT_MALLOC_FAILURE    DO_NOTHING
#define DUER_AUDIO_REPORT_NULL_POINTER      DO_NOTHING
#define DUER_AUDIO_REPORT_INVALID_CONTEXT   DO_NOTHING
#endif


/**
 * Audio Player report framework error code function
 *
 * @PARAM: duer_audio_error_t
 *
 * @RETURN: 0:success, other:failure
 */
int duer_audio_report_framework_error(duer_audio_error_t retcode);

/**
 * Audio Player report player error code function
 *
 * @PARAM: duer_audio_error_t
 *
 * @RETURN: 0:success, other:failure
 */
int duer_audio_report_player_error(uint32_t retcode);

/**
 * Audio Player report hls error code function
 *
 * @PARAM: duer_audio_error_t
 *
 * @RETURN: 0:success, other:failure
 */
int duer_audio_report_hls_error(duer_audio_error_t retcode, const uint32_t line);

/**
 * Audio Player report audio info function
 *
 * @PARAM: duer_audio_error_t
 *
 * @RETURN: 0:success, other:failure
 */
int duer_audio_report_audio_info(int bit_rate, int sample_rate,
        int bits_per_sample, int channel);

/**
 * Audio Player report play start action function
 *
 * @PARAM: duer_audio_error_t
 *
 * @RETURN: 0:success, other:failure
 */

int duer_audio_report_play_start(const char* url, int type);

/**
 * Audio Player report play pause action function
 *
 * @PARAM:
 *
 * @RETURN: 0:success, other:failure
 */
int duer_audio_report_play_pause(void);

/**
 * Audio Player report play resume action function
 *
 * @PARAM:
 *
 * @RETURN: 0:success, other:failure
 */
int duer_audio_report_play_resume(void);

/**
 * Audio Player report play stop action function
 *
 * @PARAM: duer_audio_error_t
 *
 * @RETURN: 0:success, other:failure
 */
int duer_audio_report_play_stop(uint32_t duration, uint32_t block_count,
        uint32_t max_bitrate, uint32_t min_bitrate, uint32_t avg_bitrate);

/**
 * Audio Player report play finish action function
 *
 * @PARAM: duer_audio_error_t
 *
 * @RETURN: 0:success, other:failure
 */
int duer_audio_report_play_finish(uint32_t duration, uint32_t block_count,
        uint32_t max_bitrate, uint32_t min_bitrate, uint32_t avg_bitrate);

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_PLAYER_LIGHTDUER_AUDIO_REPORT_H
