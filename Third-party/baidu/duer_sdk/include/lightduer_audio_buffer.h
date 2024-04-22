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
// Description: baidu buffer for c

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_BUFFER_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_BUFFER_H

#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

enum duer_audio_buffer_type_t {
    AUDIO_BUFFER_TYPE_COMMON,
    AUDIO_BUFFER_TYPE_FILE,
    AUDIO_BUFFER_TYPE_PSRAM,
    AUDIO_BUFFER_TYPE_HIMEM,
    AUDIO_BUFFER_TYPE_MAX
};

typedef struct duer_audio_buffer_s {
    void *specific_buffer;
    int buffer_type;

    /**
     * @brief write data to buffer
     *
     * @param[in] duer_buffer point of buffer
     * @param[in] pos position of buffer to write
     * @param[in] buffer source data
     * @param[in] length length of write data
     *
     * @return 0:success -1:fail
     */
    int (*write)(struct duer_audio_buffer_s *duer_buffer, uint32_t pos,
            const void *buffer, uint32_t length);

    /**
     * @brief read data from buffer
     *
     * @param[in] duer_buffer point of buffer
     * @param[in] pos position of buffer to read
     * @param[in] buffer destination data
     * @param[in] length length of read data
     *
     * @return 0:success -1:fail
     */
    int (*read)(struct duer_audio_buffer_s *duer_buffer, uint32_t pos,
            void *buffer, uint32_t length);

    /**
     * @brief get size of buffer
     */
    uint32_t (*size)(struct duer_audio_buffer_s *duer_buffer);
} duer_audio_buffer_t;

/**
 * create heap or data segment buffer
 *
 * @param[in] heap or data segment buffer
 * @param[in] size of buffer
 * @param[in] indicate whether free buffer when destory duer_audio_buffer_t
 *
 * @return point of duer_audio_buffer_t
 */
duer_audio_buffer_t *duer_audio_create_common_buffer(void *buffer, uint32_t size, bool need_free);

/**
 * destory duer_audio_buffer_t created by duer_audio_create_common_buffer
 */
void duer_audio_destroy_common_buffer(duer_audio_buffer_t *duer_buffer);

/**
 * create file buffer
 *
 * @param[in] fd of file which must be readable and writable
 * @param[in] size of file
 *
 * @return point of duer_audio_buffer_t
 */
duer_audio_buffer_t *duer_audio_create_file_buffer(void *file, uint32_t size);

/**
 * destory duer_audio_buffer_t created by duer_audio_create_file_buffer
 */
void duer_audio_destroy_file_buffer(duer_audio_buffer_t *duer_buffer);

#ifdef PSRAM_ENABLED

/**
 * create psram buffer
 * @param[in] start address of buffer in psram
 * @param[in] size of buffer
 *
 * @return point of duer_audio_buffer_t
 */
duer_audio_buffer_t *duer_audio_create_psram_buffer(uint32_t address, uint32_t size);

/**
 * destory duer_audio_buffer_t created by duer_audio_create_psram_buffer
 */
void duer_audio_destroy_psram_buffer(duer_audio_buffer_t *duer_buffer);

#endif // PSRAM_ENABLED

#ifdef ESP_HIMEM_ENABLED
/**
 * create himem buffer, just used on esp32, non thread safe
 * @param[in] size of buffer
 *
 * @return point of duer_audio_buffer_t
 */
duer_audio_buffer_t *duer_audio_create_himem_buffer(uint32_t size);

/**
 * destory duer_audio_buffer_t created by duer_audio_create_himem_buffer
 */
void duer_audio_destroy_himem_buffer(duer_audio_buffer_t *duer_buffer);

#endif // ESP_HIMEM_ENABLED

void duer_audio_destroy_buffer(duer_audio_buffer_t *duer_buffer);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_BUFFER_H
