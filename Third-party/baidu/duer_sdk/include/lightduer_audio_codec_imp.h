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
// Description: Declaration of codec APIs

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_IMP_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_IMP_H

/**
 * This function is used to send start play command to codec
 *
 * @PARAM:
 * channel, channel of the codec setting
 * sample_rate, sample rate of the codec setting
 * bits_per_sample, bits of the per sample
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_start_play(int type, int channel, int sample_rate, int bits_per_sample);

/**
 * This function is used to write data to codec
 *
 * @PARAM:
 * buf: pointer to the data
 * size: length of the data
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_write(int type, unsigned char *buf, unsigned int size);

/**
 * DESC:
 * This function is used to pause the play of audio data
 *
 * PARAM: NULL
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_pause_play(void);

/**
 * DESC:
 * This function is used to release the play of audio data
 *
 * PARAM: NULL
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
 int duer_audio_dac_release_play(void);
/**
 * DESC:
 * This function is used to stop the play of audio data
 *
 * PARAM: NULL
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_stop_play(void);

/**
 * DESC:
 * This function is used to wait the play end
 *
 * PARAM: NULL
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_wait_play_end(void);

/**
 * This function is used to load the es8388 codec driver
 *
 * @PARAM: NULL
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_load_driver(void);

/**
 * This function is used to unload the es8388 codec driver
 *
 * @PARAM: NULL
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_unload_driver(void);

/**
 * This function is used to get the mute status of dac
 *
 * @PARAM:
 * enable: pointer to the data
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_get_mute(int *enable);

/**
 * This function is used to set the mute status of dac
 *
 * @PARAM:
 * enable: 1-enable, 0-disable
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_set_mute(int enable);

/**
 * This function is used to get the volume of dac
 *
 * @PARAM:
 * enable: pointer to the volume
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_get_volume(int *vol);

/**
 * This function is used to set the volume of dac
 *
 * @PARAM:
 * vol: the input volume
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_set_volume(int vol);

/**
 * This function is used to adjust the volume of dac
 *
 * @PARAM:
 * vol: the adjust volume
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_dac_adj_volume(int adj_vol);

/**
 * This function is used to set the power of adc
 *
 * @PARAM:
 * enable: 1-enable, 0-disable
 *
 * @RETURN:
 * 0, if success
 * others, if failed
 */
int duer_audio_adc_set_pwr(int enable);

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_IMP_H
