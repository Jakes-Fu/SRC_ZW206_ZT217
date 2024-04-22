// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_dcs.h
 * Auth: Li Chang (changli@baidu.com)
 * Desc: DCS handler header file.
 */

#ifndef BAIDU_DUER_DUERAPP_DCS_H
#define BAIDU_DUER_DUERAPP_DCS_H

#include "lightduer_dcs.h"
#include "lightduer_connagent.h"
#include "lightduer_audio_player.h"

/*
 * DCS init function
 *
 * @param void:
 *
 * @return void:
 */
void duer_dcs_init(void);

/*
 * DCS audio on_started callback function
 *
 * @param flag: duer_audio_play_type_t
 *
 * @return success:0
 *             fail: others
 */
int duer_dcs_audio_on_started_cb(duer_audio_play_type_t flag);

/*
 * DCS audio on_stop callback function
 *
 * @param flag: duer_audio_play_type_t
 *
 * @return success:0
 *             fail: others
 */
int duer_dcs_audio_on_stopped_cb(duer_audio_play_type_t flag);

/*
 * DCS audio on_finished callback function
 *
 * @param flag: duer_audio_play_type_t
 *
 * @return success:0
 *             fail: others
 */
int duer_dcs_audio_on_finished_cb(duer_audio_play_type_t flag);

/*
 * DCS audio on_error callback function
 *
 * @param flag: duer_audio_play_type_t
 *
 * @return success:0
 *             fail: others
 */
int duer_dcs_audio_on_error_cb(duer_audio_play_type_t flag);

void duer_ext_stop_speak();

void duer_event_hook(duer_event_t *event);

int duer_auth_hook(int status);

#endif // BAIDU_DUER_DUERAPP_DCS_H
