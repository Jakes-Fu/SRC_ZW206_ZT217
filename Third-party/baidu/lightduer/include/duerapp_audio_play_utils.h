// Copyright (2019) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_audio_play_utils.h
 * Auth: Chen xihao (chenxihao@baidu.com)
 * Desc: utility functions of audio play
 */

#ifndef BAIDU_DUER_DUERAPP_AUDIO_PLAY_UTILS_H
#define BAIDU_DUER_DUERAPP_AUDIO_PLAY_UTILS_H

#include "lightduer_types.h"
#include "lightduer_audio_player.h"

typedef enum {
    TRIGGER_QUERY,
    TRIGGER_TOUCH,
    TRIGGER_APP
} duer_trigger_t;

void duer_audio_play_pause();

void duer_audio_play_resume();

void duer_audio_play_toggle();

void duer_audio_play_previous();

void duer_audio_play_next();

// only AUDIO_TYPE_DCS_AUDIO, AUDIO_TYPE_DLNA_AUDIO,
// AUDIO_TYPE_BLE_AUDIO are main audio type
duer_audio_play_type_t duer_get_current_main_audio_type(void);

bool duer_is_main_audio_type(duer_audio_play_type_t type);

void duer_audio_play_dcs_other(const char *url, int16_t loop_count);

void duer_audio_on_play_started(duer_audio_play_type_t type);

void duer_audio_on_play_stopped(duer_audio_play_type_t type);

void duer_audio_on_play_error(duer_audio_play_type_t type);

void duer_audio_on_play_finished(duer_audio_play_type_t type);

void duer_audio_on_dcs_paused(duer_bool paused);

#endif // BAIDU_DUER_DUERAPP_AUDIO_PLAY_UTILS_H
