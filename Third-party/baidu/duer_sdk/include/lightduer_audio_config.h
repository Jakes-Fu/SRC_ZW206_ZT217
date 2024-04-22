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
// Author: Huang Jianfeng (huangjianfeng02@baidu.com)
//
// Description: Configuration of audio decoder features

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CONFIG_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*INPUT*/
#ifndef DUER_MINI_LIB
#define CONFIG_FILE_INPUT
#define CONFIG_HTTP_INPUT
#define CONFIG_HLS_INPUT
#define CONFIG_FLASH_INPUT
#define CONFIG_STREAM_INPUT
#endif
#define CONFIG_RAM_INPUT
/*DEMUXER*/
#ifndef DUER_MINI_LIB
#define CONFIG_M4A_DEMUXER
#define CONFIG_TS_DEMUXER
#define CONFIG_WAV_DEMUXER
#endif
/*DECODER*/
#define CONFIG_MP3_DECODER
#ifndef DUER_MINI_LIB
#define CONFIG_AAC_DECODER
#endif
/*OUTPUT*/
#define CONFIG_CODEC_OUTPUT
//#define CONFIG_FILE_OUTPUT

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_CONFIG_H
