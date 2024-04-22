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
// Description: Wrapper for aes128 decrypt

#ifndef BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_AES_DECRYPT_H
#define BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_AES_DECRYPT_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AES_BLOCK_SIZE      16 // Only support AES-128 */

typedef void* duer_audio_aes_context_t;

/**
 * Create AES context
 *
 * @Return duer_audio_aes_context_t, the created AES context
 */
DUER_INT duer_audio_aes_context_t duer_audio_aes_context_create(void);

/**
 * AES key schedule (decryption)
 *
 * @Param ctx, AES context to be initialized
 * @Param key, decryption key, must be 128 bits
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_audio_aes_setkey_dec(
        duer_audio_aes_context_t ctx, const duer_u8_t *key);

/**
 * AES-CBC buffer decryption
 * Length should be a multiple of the block size (16 bytes)
 *
 * @Param ctx, AES context
 * @Param length, length of the input data
 * @Param iv, initialization vector (updated after use)
 * @Param input, buffer holding the input data
 * @Param output, buffer holding the output data
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_audio_aes_decrypt_cbc(duer_audio_aes_context_t ctx, size_t length,
        duer_u8_t iv[AES_BLOCK_SIZE], const duer_u8_t *input, duer_u8_t *output);

/**
 * Destory AES context
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_audio_aes_context_destroy(duer_audio_aes_context_t ctx);

/*
 * The aes decrypt callbacks
 */
typedef duer_audio_aes_context_t (*duer_audio_aes_context_create_f)(void);
typedef duer_status_t (*duer_audio_aes_setkey_dec_f)(duer_audio_aes_context_t ctx, const duer_u8_t *key);
typedef duer_status_t (*duer_audio_aes_decrypt_cbc_f)(duer_audio_aes_context_t ctx, size_t length,
        duer_u8_t iv[AES_BLOCK_SIZE], const duer_u8_t *input, duer_u8_t *output);
typedef duer_status_t (*duer_audio_aes_context_destroy_f)(duer_audio_aes_context_t ctx);

/*
 * Initial the aes_context callbacks
 *
 * @Param f_create, in, the function create aes context
 * @Param f_setkey, in, the function for AES key schedule
 * @Param f_decrypt, in, the function for AES-CBC buffer decryption
 * @Param f_destroy, in, the function destroy aes context
 */
DUER_EXT void duer_audio_aes_decrypt_init(duer_audio_aes_context_create_f f_create,
                            duer_audio_aes_setkey_dec_f f_setkey,
                            duer_audio_aes_decrypt_cbc_f f_decrypt,
                            duer_audio_aes_context_destroy_f f_destroy);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_LIGHTDUER_AUDIO_DECODER_LIGHTDUER_AUDIO_AES_DECRYPT_H
