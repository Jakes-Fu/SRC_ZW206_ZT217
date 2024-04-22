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
// Author: Huang Jianfeng(huangjianfeng02@baidu.com)
//
// Description: Declaration of dcs reprot functions

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DCS_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DCS_H

#include "lightduer_audio_error.h"

int duer_audio_dcs_report_play_error(duer_audio_error_t retcode, const char *url);

void duer_audio_dcs_on_nearly_finished(void);

void duer_audio_dcs_init_err_msg(void);

void duer_audio_dcs_update_http_ip_info(const char *host, const char *ip, const char *ip_info);

char *duer_audio_dcs_get_http_ip_infos(char **host, char **ip);

void duer_audio_dcs_set_http_code(int http_code);

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_DCS_H
