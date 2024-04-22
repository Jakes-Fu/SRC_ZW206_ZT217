/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
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
 * @file        os_fal_part.h
 *
 * @brief       fal_part
 *
 * @revision
 * Date         Author          Notes
 * 2020-02-20   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __OS_FAL_PART_H__
#define __OS_FAL_PART_H__

#include "os_types.h"
#include "os_fal.h"

#ifndef OS_FAL_DEV_NAME_MAX
#define OS_FAL_DEV_NAME_MAX 16
#endif

#define FAL_PART_INFO_FLAGS_LOCKED    (1 << 0)
#define FAL_PART_INFO_FLAGS_UNLOCKED  (0 << 0)

typedef struct os_fal_part_info
{
    char name[OS_FAL_DEV_NAME_MAX];  /* the name of the part */
    char flash_name[OS_FAL_DEV_NAME_MAX];  /* the flash name contian the part */

    os_uint32_t offset;  /* the part address in the flash */
    os_uint32_t size;  /* the size of the part */

    os_uint32_t flags;  /* unused */
}os_fal_part_info_t;

typedef struct os_fal_part
{
    os_fal_part_info_t *info;  /* fal part info */
    os_fal_flash_t     *flash;  /* the flash info which contain the fal part */

    os_uint32_t flags;  /* unused */
}os_fal_part_t;

/**
 ***********************************************************************************************************************
 * @brief           find the fal part info according the name of flash part
 *
 * @param[in]       name: the name of the flash part
 *
 * @return          os_fal_part_t *: see the structure description
 ***********************************************************************************************************************
 */
os_fal_part_t *os_fal_part_find(const char *name);

/**
 ***********************************************************************************************************************
 * @brief           read the data of part by offset address
 *
 * @param[in]       part: the name of the flash part
 *                  addr: offset address of the part
 *                  buf: save the data
 *                  size: read size
 *
 * @return          int
 ***********************************************************************************************************************
 */
int os_fal_part_read(os_fal_part_t *part, os_uint32_t addr, os_uint8_t *buf, os_size_t size);

/**
 ***********************************************************************************************************************
 * @brief           write the data in part by offset address
 *
 * @param[in]       part: the name of the flash part
 *                  addr: offset address of the part
 *                  buf: the data to be written
 *                  size: write data size
 *
 * @return          int
 ***********************************************************************************************************************
 */
int os_fal_part_write(os_fal_part_t *part, os_uint32_t addr, const os_uint8_t *buf, os_size_t size);

/**
 ***********************************************************************************************************************
 * @brief           erase the part by offset address and len
 *
 * @param[in]       part: the name of the flash part
 *                  addr: offset address of the part
 *                  size: erase size
 *
 * @return          int
 ***********************************************************************************************************************
 */
int os_fal_part_erase(os_fal_part_t *part, os_uint32_t addr, os_size_t size);


#define os_fal_part_size(part)         (part->info->size)
#define os_fal_part_block_size(part)   (part->flash->block_size)
#define os_fal_part_page_size(part)    (part->flash->page_size)

#endif /* __OS_FAL_PART_H__ */
