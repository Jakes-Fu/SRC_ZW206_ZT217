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
// Description: Wrapper for audio codec ops

#ifndef BBAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_OPS_H
#define BBAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_OPS_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    AUDIO_DATA,
    AUDIO_HEADER
};

/*
 * API to init the codec
 *
 * @Param NULL
 * @Return 0: if success, other: failure
 */
DUER_INT_IMPL int duer_audio_codec_init(void);

/*
 * API to uninit the codec
 *
 * @Param NULL
 * @Return 0: if success, other: failure
 */
DUER_INT_IMPL int duer_audio_codec_uninit(void);

/*
 * API to send start play command to codec
 *
 * @Param type, type of the to-play audio
 * @Param channel, channel of the codec setting
 * @Param sample_rate, sample rate of the codec setting
 * @Param bits_per_sample, bits of the per sample
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_start_play(int type, int channel, int sample_rate,
        int bits_per_sample);

/*
 * API to write data to codec
 *
 * @Param buf: pointer to the data
 * @Param size: length of the data
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_write(int type, unsigned char *buf, size_t size);

/*
 * API to pause the play of audio data
 *
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_pause_play(void);

/*
 * API to resume the play of audio data
 *
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_resume_play(void);

/*
 * API to stop the play of audio data
 *
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_stop_play(void);

/*
 * API to wait the play end
 *
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_wait_play_end(void);

/*
 * API to get mute status of codec
 *
 * @Param mute, value of mute status
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_get_mute(int *mute);

/*
 * API to set mute status of codec
 *
 * @Param mute, value of mute status
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_set_mute(int mute);

/*
 * API to get volume of codec
 *
 * @Param vol, value of volume
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_get_volume(int *vol);

/*
 * API to set volume of codec
 *
 * @Param vol, value of volume
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_set_volume(int vol);

/*
 * API to adjust volume of codec
 *
 * @Param vol, value of volume to adjust
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_adj_volume(int vol);

/*
 * API to set power of codec
 *
 * @Param 0:disable, 1:enable
 * @Return 0: if success, other: failure
 */
DUER_INT int duer_audio_codec_set_adc(int enable);

/*
 * The codec operation callbacks
 */

typedef int (*duer_audio_codec_init_f)(void);
typedef int (*duer_audio_codec_uninit_f)(void);
typedef int (*duer_audio_codec_start_play_f)(int type, int channel, int sample_rate,
        int bits_per_sample);
typedef int (*duer_audio_codec_write_f)(int type, unsigned char *buf, size_t size);
typedef int (*duer_audio_codec_pause_f)(void);
typedef int (*duer_audio_codec_release_f)(void);
typedef int (*duer_audio_codec_stop_f)(void);
typedef int (*duer_audio_codec_wait_play_end_f)(void);
typedef int (*duer_audio_codec_get_mute_f)(int *mute);
typedef int (*duer_audio_codec_set_mute_f)(int mute);
typedef int (*duer_audio_codec_get_volume_f)(int *vol);
typedef int (*duer_audio_codec_set_volume_f)(int vol);
typedef int (*duer_audio_codec_adj_volume_f)(int adj_vol);
typedef int (*duer_audio_codec_set_adc_f)(int enable);

/*
 * Initial the codec operation callbacks
 *
 * @Param f_get_mute, in, the function to get mute status
 * @Param f_set_mute, in, the function to set mute status
 * @Param f_get_volume, in, the function to get volume
 * @Param f_set_volume, in, the function to set volume
 * @Param f_adj_volume, in, the function to adjust volume
 * @Param f_set_adc, in, the function to set adc power
 */
DUER_EXT void duer_audio_codec_ops_init(duer_audio_codec_init_f f_init,
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

#endif // BBAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_OPS_H

