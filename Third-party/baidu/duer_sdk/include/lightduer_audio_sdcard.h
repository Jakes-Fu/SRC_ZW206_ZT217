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
// Author: Chen Xihao(chenxihao@baidu.com)
//
// Description: sdcard APIs.

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_SDCARD_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_SDCARD_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Get the path of sdcard
 *
 * @Return the path of sdcard(e.g. "/sdcard")
 */
const char * duer_get_sdcard_path(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_SDCARD_H
