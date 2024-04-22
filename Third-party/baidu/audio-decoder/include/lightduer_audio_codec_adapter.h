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
// Description: Adapter for codec

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_ADAPTER_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DESC:
 * Codec adapter init function, used to register codec callbacks
 *
 * PARAM: NULL
 *
 * @RETURN: NULL
 */
extern void duer_audio_codec_adapter_init(void);

/**
 * DESC:
 * Codec adapter init function with user functions, used to register codec callbacks
 *
 * PARAM: user functions
 *
 * @RETURN: NULL
 */
#include "lightduer_audio_codec_ops.h"
extern void duer_audio_codec_adapter_init_with_param(duer_audio_codec_init_f f_init,
                              duer_audio_codec_uninit_f f_uninit,
                              duer_audio_codec_start_play_f f_start_play,
                              duer_audio_codec_write_f f_write,
                              duer_audio_codec_pause_f f_pause_play,
                              duer_audio_codec_release_f f_release_play,
                              duer_audio_codec_stop_f f_stop_play,
                              duer_audio_codec_wait_play_end_f f_wait_play_end,
                              duer_audio_codec_get_mute_f f_get_mute,
                              duer_audio_codec_set_mute_f f_set_mute,
                              duer_audio_codec_get_volume_f f_get_volume,
                              duer_audio_codec_set_volume_f f_set_volume,
                              duer_audio_codec_adj_volume_f f_adj_volume,
                              duer_audio_codec_set_adc_f f_set_adc);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_ADAPTER_H
