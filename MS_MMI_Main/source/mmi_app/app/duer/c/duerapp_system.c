// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_system.c
 * Auth: Li Chang (changli@baidu.com)
 * Desc: System interface porting.
 *       This file is exposed to external use.
 */

#include "duerapp_system.h"

#include "lightduer_audio_codec_imp.h"
#include "lightduer_audio_codec_ops.h"

#include "lightduer_log.h"
#include "lightduer_memory.h"
#include "lightduer_types.h"

#include "mmi_app_eng_trc.h"
#include "audio_api.h"
#include "asm.h"
#include "wav_adp.h"

/* volume: [0,9] */
static int s_volume = 8;
static int s_is_mute = 0;

static gps_info_t s_gps_info;
// static gps_info_t s_gps_info = {
//     .longitude = 116.41667,
//     .latitude = 39.91667,
//     .coordinate_sys = "WGS84"
// };

int duer_audio_dac_get_mute(int *enable)
{
    if (enable) {
        *enable = s_is_mute;
    }

    return 0;
}

int duer_audio_dac_set_mute(int enable)
{
    s_is_mute = enable;
    AUDIO_SetDownLinkMute(enable);

    return 0;
}

int duer_audio_dac_get_volume(int *vol)
{
    AUDIO_RESULT_E result = AUDIO_GetVolume(vol);
    if (result == AUDIO_NO_ERROR) {
        *vol *= 10;
    } else {
        *vol = s_volume * 10;
    }
    DUER_LOGI("vol=%d.", *vol);

    return 0;
}

int duer_audio_dac_set_volume(int vol)
{
    AUDIO_RESULT_E result;

    if (vol <= 0) {
        vol = 0;
    } else if (vol >= 100) {
        vol = 99;
    }

    vol /= 10;

    if (vol == 0) {
        vol = 1;
    }
    result = AUDIO_SetVolume(vol);
    if (result == AUDIO_NO_ERROR) {
        s_volume = vol;
    }

    return 0;
}

int duer_audio_dac_adj_volume(int adj_vol)
{
    int vol = 0;
    
    DUER_LOGI("duer_audio_dac_adj_volume:%d", adj_vol);
    duer_audio_dac_get_volume(&vol);
    vol += adj_vol;
    duer_audio_dac_set_volume(vol);

    return 0;
}

int duer_get_gps_info(gps_info_t *info)
{
    info->longitude = s_gps_info.longitude;
    info->latitude = s_gps_info.latitude;
    snprintf(info->coordinate_sys, 16, "%s", s_gps_info.coordinate_sys);

    return 0;
}

int duer_set_gps_info(float longitude, float latitude, char *coord)
{
    s_gps_info.longitude = longitude;
    s_gps_info.latitude = latitude;
    snprintf(s_gps_info.coordinate_sys, 16, "%s", coord);

    return 0;
}

int duer_get_net_id()
{
    int net_id = 0;
    DUER_LOGI("net_id=%d.", net_id);
    return net_id;
}

