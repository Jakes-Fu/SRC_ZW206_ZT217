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
// Description: declaration of the rda58xx flash API

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_FLASH_IMP_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_FLASH_IMP_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Read data from the flash
 *
 * @param offset Address of the data to be read, relative to the
 *                   beginning of the flash.
 * @param dst Pointer to the buffer where data should be stored.
 *            Pointer must be non-NULL and buffer must be at least 'size' bytes long.
 * @param size Size of data to be read, in bytes.
 *
 * @Return int, the operation result
 */
int duer_audio_flash_read_impl(duer_u32_t offset, void *dst, duer_u32_t size);

#ifdef __cplusplus
}
#endif

#endif //BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_FLASH_IMP_H
