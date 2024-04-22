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
// Description: buffer for http download

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_DOWNLOAD_BUFFER_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_DOWNLOAD_BUFFER_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * FIFO circle buffer for download
 */
typedef struct duer_audio_download_buffer_t {
    uint8_t *buffer;
    size_t  buffer_size;
    int     start;      // valid data of buffer is [start, end)
    int     end;
    void    *mutex;
} duer_audio_download_buffer_t;

/*
 * Create download buffer
 *
 * @Param size, the size of the buffer to create
 */
duer_audio_download_buffer_t *duer_audio_download_buffer_create(size_t size);

/*
 * Destory download buffer
 *
 * @Param buffer, the point of the buffer
 */
void duer_audio_download_buffer_destory(duer_audio_download_buffer_t *buffer);

/*
 * Write data to buffer
 *
 * @Param buffer, the point of the buffer
 * @Param data, the data to write
 * @Param size, the size of the data
 * @Return, actual write size
 */
int duer_audio_download_buffer_write(duer_audio_download_buffer_t *buffer,
        const uint8_t *data, size_t size);

/*
 * Write data to the front of buffer, only use to recovery data
 *
 * @Param buffer, the point of the buffer
 * @Param data, the data to write
 * @Param size, the size of the data
 * @Return, actual write size
 */
int duer_audio_download_buffer_write_front(duer_audio_download_buffer_t *buffer,
        const uint8_t *data, size_t size);

/*
 * Read data from buffer
 *
 * @Param buffer, the point of the buffer
 * @Param data, store read data
 * @Param size, the size to read
 * @Return, actual read size
 */
int duer_audio_download_buffer_read(duer_audio_download_buffer_t *buffer,
        uint8_t *data, size_t size);

/*
 * Skip data from buffer
 *
 * @Param buffer, the point of the buffer
 * @Param size, the size to read
 * @Return, actual read size
 */
int duer_audio_download_buffer_skip(duer_audio_download_buffer_t *buffer, size_t size);

/*
 * Get free size of buffer
 *
 * @Param buffer, the point of the buffer
 * @Return, the size of free buffer
 */
size_t duer_audio_download_buffer_get_free_size(duer_audio_download_buffer_t *buffer);

/*
 * Get data size in buffer
 *
 * @Param buffer, the point of the buffer
 * @Return, the size of data in buffer
 */
size_t duer_audio_download_buffer_get_data_size(duer_audio_download_buffer_t *buffer);

/*
 * Clear data of the buffer
 *
 * @Param buffer, the point of the buffer
 */
void duer_audio_download_buffer_clear(duer_audio_download_buffer_t *buffer);

/*
 * Reduce the buffer size
 *
 * @Param buffer, the point of the buffer
 * @Param size, the targe size to reduce
 * @Return, 0: successed nonzero: failed
 */
int duer_audio_download_buffer_reduce_size(duer_audio_download_buffer_t *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_DOWNLOAD_BUFFER_H
