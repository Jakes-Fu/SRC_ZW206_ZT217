/**
 ***********************************************************************************************************************
 * Copyright (c) 2021, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        cmiot_fal.h
 *
 * @brief       Interface related to OS.
 *
 * @revision
 * Date         Author          Notes
 * 2021-04-09   OneOS Team      First version.
 ***********************************************************************************************************************/
#ifndef __CMIOT_FAL_H__
#define __CMIOT_FAL_H__

#include <stdint.h>
#include <stddef.h>
#include "cmiot_type.h"

#if defined(__cplusplus)
extern "C" {
#endif

void        *cmiot_ota_fal_part_find(const cmiot_char *name);
void        *cmiot_hal_get_device(cmiot_uint8 type);
cmiot_int32  cmiot_ota_fal_part_read(void *part, cmiot_uint32 addr, cmiot_char *buf, cmiot_uint32 size);
cmiot_int32  cmiot_ota_fal_part_write(void *part, cmiot_uint32 addr, const cmiot_char *buf, cmiot_uint32 size);
cmiot_int32  cmiot_ota_fal_part_erase(void *part, cmiot_uint32 addr, size_t size);
cmiot_uint32 cmiot_hal_get_true_blocksize(cmiot_uint8 type);
cmiot_uint32 cmiot_hal_get_true_pagesize(cmiot_uint8 type);
cmiot_uint32 cmiot_hal_get_download_size(void);
cmiot_uint32 cmiot_hal_get_delta_addr(void);

#if defined(__cplusplus)
}
#endif

#endif /* __CMIOT_FAL_H__ */
