// Copyright (2022) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_pcm_file.h
 * Auth: Lu song Wang (wanglusong01@baidu.com)
 * Desc: audio pcm save file.
 */

#ifndef DUERAPP_PCM_FILE_H
#define DUERAPP_PCM_FILE_H

#include "lightduer_types.h"
#include "lightduer_audio_player.h"

BOOLEAN PUB_DUERAPP_PcmWriteBuffer(unsigned char *buf , int size);

void PUB_DUERAPP_PcmParamCfg(int channel, int sample_rate, int bits_per_sample);

int PUB_DUERAPP_PcmPlayOpen(void);

void duer_audio_dac_tts_mode(int mode);

#endif
