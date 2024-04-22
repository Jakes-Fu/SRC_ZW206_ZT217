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
// Description:

#ifndef BAIDU_LIGHTDUER_AUDIO_PLAYER_LIGHTDUER_AUDIO_PLAY_H
#define BAIDU_LIGHTDUER_AUDIO_PLAYER_LIGHTDUER_AUDIO_PLAY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_PLAY_STATUS_IDLE,
    AUDIO_PLAY_STATUS_PLAY,
    AUDIO_PLAY_STATUS_PAUSE,
    AUDIO_PLAY_STATUS_STOP,
} duer_audio_play_status_t;

typedef enum {
    AUDIO_TYPE_NULL         = -1,
    AUDIO_TYPE_DCS_AUDIO    = 0,
    AUDIO_TYPE_DCS_SPEECH,
    AUDIO_TYPE_DLNA_AUDIO,
    AUDIO_TYPE_BLE_AUDIO,
    AUDIO_TYPE_SDCARD_FILE,
    AUDIO_TYPE_FLASH_DATA,
    AUDIO_TYPE_PROMPT_TONE,
    AUDIO_TYPE_ALERT_SPEECH,
    AUDIO_TYPE_ALERT_RINGING,
    AUDIO_TYPE_ALERT_MUSIC,
    AUDIO_TYPE_WECHAT_NOTIFY,
    AUDIO_TYPE_WECHAT_MESSAGE,
    AUDIO_TYPE_HOURLY_CHIME,
    AUDIO_TYPE_DCS_OTHER,
} duer_audio_play_type_t;

typedef enum {
    AUDIO_PLAYER_RETCODE_SUCCESS,
    AUDIO_PLAYER_RETCODE_FAILED,
    AUDIO_PLAYER_RETCODE_NOT_INITED,
    AUDIO_PLAYER_RETCODE_INVALID_PARAM,
    AUDIO_PLAYER_RETCODE_MEMORY_ERROR,
    AUDIO_PLAYER_RETCODE_MUTEX_ERROR,
    AUDIO_PLAYER_RETCODE_SEMAPHORE_ERROR,
    AUDIO_PLAYER_RETCODE_THREAD_ERROR,
    AUDIO_PLAYER_RETCODE_INVALID_URL,
    AUDIO_PLAYER_RETCODE_NOT_IN_PLAY,
    AUDIO_PLAYER_RETCODE_NOT_SEEK_TYPE,
    AUDIO_PLAYER_RETCODE_NO_PAUSED_INFO,
    AUDIO_PLAYER_RETCODE_NO_PAUSED_FLAG,
    AUDIO_PLAYER_RETCODE_INVALID_SEEK_POSITION,
    AUDIO_PLAYER_RETCODE_DUPLICATE_LISTENER,
    AUDIO_PLAYER_RETCODE_CODEC_ERROR,
    AUDIO_PLAYER_RETCODE_STREAM_ERROR,
} duer_audio_player_retcode_t;

typedef struct duer_audio_player_listener_s {
    int (*on_start)(duer_audio_play_type_t flags);
    int (*on_stop)(duer_audio_play_type_t flags);
    int (*on_finish)(duer_audio_play_type_t flags);
    int (*on_error)(duer_audio_play_type_t flags);
} duer_audio_player_listener_t;

typedef struct duer_audio_playinfo_s {
    duer_audio_play_status_t status;
    duer_audio_play_type_t type;
    char *url;
    uint32_t offset;
} duer_audio_playinfo_t;

typedef void (*duer_audio_first_data_cb)(duer_audio_play_type_t type);

typedef void (*duer_audio_write_codec_cb)(duer_audio_play_type_t type);

typedef void (*duer_audio_on_stuttered_cb)(duer_audio_play_type_t type, bool is_stuttered,
        const char *err_msg, const char *host, const char *ip);

/**
 * Register get first audio data from url callback function
 *
 * @PARAM: duer_audio_first_data_cb function type
 *
 * @RETURN: void
*/
void duer_audio_register_first_data_cb(duer_audio_first_data_cb cb);

/**
 * Register send audio to codec callback function
 *
 * @PARAM: duer_send_codec_cb function type
 *
 * @RETURN: void
*/
void duer_audio_register_write_codec_cb(duer_audio_write_codec_cb);

/**
 * Register audio play on stuttered callback function
 *
 * @PARAM: duer_audio_on_stuttered function type
 *
 * @RETURN: void
*/
void duer_audio_register_on_stuttered_cb(duer_audio_on_stuttered_cb cb);

/**
 * Audio Player init function
 *
 * @PARAM: NULL
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_init(void);

/**
 * Audio Player play function, used to play audio source
 *
 * @PARAM:
 * url: path of audio source, can be local filename or http url
 * audio_type: audio type of the url
 * offset: the position start to play
 *
 * @RETURN: duer_audio_player_retcode_t
 */
 duer_audio_player_retcode_t duer_audio_player_play(const char *url,
        duer_audio_play_type_t audio_type, uint32_t offset);

/**
 * Audio Player pause function, used to pasue play from dcs
 *
 * @PARAM:
 * *offset: pointer used to return offset value
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_dcs_pause(uint32_t *offset);

/**
 * Audio Player pause function, used to pause the current playing
 *
 * @PARAM: NULL
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_pause(void);

/**
 * Audio Player resume function, used to resume the current playing for dcs
 *
 * @PARAM: NULL
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_dcs_resume(void);

/**
 * Audio Player resume function, used to resume the current playing
 *
 * @PARAM: NULL
 *
  * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_resume(void);

/**
 * Audio Player seek function, used to change the play position
 *
 * @PARAM:
 * size, time offset of the play positioin
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_seek(uint32_t playtime);

/**
 * Audio Player stop function, used to stop the current audio play
 *
 * @PARAM:
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_stop(void);

/**
 * Audio Player uninit function
 *
 * @PARAM: NULL
 *
 * @RETURN: NULL
 */
void duer_audio_player_uninit(void);

/**
 * Audio Player get mute function, used to get the mute status
 *
 * @PARAM:
 * enable: pointer to the enable value
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_get_mute(int *enable);

/**
 * Audio Player set mute function, used to set the mute status
 *
 * @PARAM:
 * enable: mute status
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_set_mute(int enable);

/**
 * Audio Player get volume function, used to get volume
 *
 * @PARAM:
 * vol: pointer to the volume value
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_get_volume(int *vol);

/**
 * Audio Player set volume function, used to get volume
 *
 * @PARAM:
 * vol: the volume value
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_set_volume(int vol);

/**
 * Audio Player adjust volume function, used to adjust volume
 *
 * @PARAM:
 * vol: the volume value need to adjust
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_adjust_volume(int adj_vol);

/**
 * Audio Player set adc power function, used to enable/disable adc
 *
 * @PARAM:
 * enable: 1-enable, 0-disable
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_set_adc(int enable);

/**
 * Audio Player register listener function
 *
 * @PARAM:
 * listener: audio player listener
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_register_listener(
        duer_audio_player_listener_t *listener);

/**
 * Audio Player unregister listener function
 *
 * @PARAM:
 * listener: audio player listener
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_unregister_listener(
        duer_audio_player_listener_t *listener);

/**
 * Audio Player get status function
 *
 * @PARAM:
 * status: pointer used to return status value
 *
 * @RETURN: duer_audio_player_retcode_t
 * audio player status
 */
duer_audio_player_retcode_t duer_audio_player_get_status(duer_audio_play_status_t *status);

/**
 * Audio Player get playtime function
 *
 * @PARAM:
 * playtime: the played time in millisecond
 * resumable_type: whether want to get progress of resumable types
 *      resumable types: AUDIO_TYPE_DCS_AUDIO, AUDIO_TYPE_DLNA_AUDIO, AUDIO_TYPE_BLE_AUDIO
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_get_play_progress(
        uint32_t *playtime, bool resumable_type);

/**
 * Audio Player get duration function
 *
 * @PARAM:
 * duration: the duration time in millisecond
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_get_duration(uint32_t *duration);

/**
 * Audio Player get current playinfo function
 *
 * @PARAM:
 * info: point to the playinfo struct
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_get_current_playinfo(
        duer_audio_playinfo_t *info);

/**
 * Audio Player get interrupted playinfo function
 *
 * @PARAM:
 * info: point to the playinfo struct
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_get_interrupt_playinfo(
        duer_audio_playinfo_t *info);

/**
 * Audio Player config pcm setting function
 *
 * @PARAM:
 * sample_rate: sample_rate of the pcm
 * channel: channel of the pcm
 * num_of_bits: num_of_bits of the pcm
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_set_pcm_config(
        int sample_rate, int channel, int num_of_bits);

/**
 * Audio Player config stream write auido data
 *
 * @PARAM:
 * buf: pointer of the data
 * len: len of the data
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_stream_write_data(
        char *buf, int len);

/**
 * To download audio data in advance, which will be played soon
 *
 * @PARAM:
 * url: url of audio source, must be http url
 * offset: the position start to play
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_pre_download(
        const char *url, uint32_t offset);


/**
 * Set http download retry time, only valid for the next play
 *
 * @PARAM:
 * time: the retry time, 0 is use default setting
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_set_download_retry_time(int time);

/**
 * Set http download connect timeout, only valid for the next play
 *
 * @PARAM:
 * time: the retry time, 0 is use default setting
 *
 * @RETURN: duer_audio_player_retcode_t
 */
duer_audio_player_retcode_t duer_audio_player_set_connect_timeout(int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_PLAYER_LIGHTDUER_AUDIO_PLAY_H
