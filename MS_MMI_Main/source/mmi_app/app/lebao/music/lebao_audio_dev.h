#ifndef __LEBAO_AUDIO_DEV_H
#define __LEBAO_AUDIO_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "audio_api.h"

int lebao_get_audio_srv_route_type(void);
int lebao_get_audio_dev_mode(void);
int lebao_get_record_dev_mode(void);
HAUDIODEV lebao_get_audio_device(void);
HAUDIODEV lebao_get_audio_record_device(void);
void lebao_open_bt_device(void);

void lebao_set_headset_default_volume(void);

void lebao_set_max_volume(const int vol);
int lebao_get_max_volume(void);
int lebao_set_volume(const int vol);
void lebao_set_default_volume(void);
int lebao_get_volume(void);
int lebao_get_volume_percent(void);
int lebao_add_volume(void);
int lebao_sub_volume(void);
int lebao_get_volume_step(void);

#ifdef __cplusplus
}
#endif

#endif
