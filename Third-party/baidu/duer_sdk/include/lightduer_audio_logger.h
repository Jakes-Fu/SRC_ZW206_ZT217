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
// Author: Chen Xihao (chenxihao@baidu.com)
//
// Description: Definition of baidu audio utility functions

#ifndef BAIDU_TINYDU_IOT_OS_SRC_AUDIO_DECODER_BAIDU_AUDIO_LOGGER_H
#define BAIDU_TINYDU_IOT_OS_SRC_AUDIO_DECODER_BAIDU_AUDIO_LOGGER_H

#include "lightduer_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOGD(...)   DUER_LOGD(__VA_ARGS__)
#define LOGI(...)   DUER_LOGI(__VA_ARGS__)
#define LOGW(...)   DUER_LOGW(__VA_ARGS__)
#define LOGE(...)   DUER_LOGE(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif//BAIDU_TINYDU_IOT_OS_SRC_AUDIO_DECODER_BAIDU_AUDIO_LOGGER_H
