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
// Description: Adapter for codec

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_ADAPTER_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DESC:
 * Codec adapter init function, used to register codec callbacks
 *
 * PARAM: NULL
 *
 * @RETURN: NULL
 */
extern void duer_audio_codec_adapter_init(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CODEC_ADAPTER_H
