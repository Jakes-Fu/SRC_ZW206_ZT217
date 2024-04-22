/**
 * Copyright (2019) Baidu Inc. All rights reserved.
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
// Description: define the priority of audio thread

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_THREAD_PRIORITY_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_THREAD_PRIORITY_H

enum duer_audio_thread_id_e {
    THREAD_AUDIO_PLAYER,
    THREAD_HTTP_DOWNLOAD,
    THREAD_HLS_DOWNLOAD,
    THREAD_HLS_RELOAD,
    THREAD_M4A_HEAD_DOWNLOAD,
    THREAD_TOTAL
};

int duer_get_thread_priority(int thread_id);

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_THREAD_PRIORITY_H
