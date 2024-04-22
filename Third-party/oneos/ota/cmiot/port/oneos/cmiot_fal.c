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
 * @file        ota_fal.c
 *
 * @brief       Interface related to OS.
 *
 * @revision
 * Date         Author          Notes
 * 2020-10-13   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#include "cmiot_fal.h"
#include "cmiot_cfg.h"
#include <os_fal_part.h>

/**
 ***********************************************************************************************************************
 * @brief           This function for find part by name
 * @param[in]       name       the part name
 *
 * @retval          part info
 ***********************************************************************************************************************
 */
void *cmiot_ota_fal_part_find(const cmiot_char *name)
{
    return (void *)os_fal_part_find(name);
}

/**
 ***********************************************************************************************************************
 * @brief           This function for find part by type
 * @param[in]       type       the part type
 *
 * @retval          part info
 ***********************************************************************************************************************
 */
void *cmiot_hal_get_device(cmiot_uint8 type)
{
    cmiot_char *device = NULL;
    if ((type == CMIOT_FILETYPE_PATCH) || (type == CMIOT_FILETYPE_PATCH_INFO))
    {
        device = cmiot_download_name();
    }

    if (device != NULL)
    {
        return cmiot_ota_fal_part_find(device);
    }
    else
    {
        return NULL;
    }
}

/**
 ***********************************************************************************************************************
 * @brief           This function for read flash
 * @param[in]       part       the part info
 * @param[in]       addr       offset
 * @param[out]      buf        read date
 * @param[in]       addr       read size
 *
 * @retval          read size
 ***********************************************************************************************************************
 */
cmiot_int32 cmiot_ota_fal_part_read(void *part, cmiot_uint32 addr, cmiot_char *buf, cmiot_uint32 size)
{
    return os_fal_part_read((os_fal_part_t *)part, (uint32_t)addr, (uint8_t *)buf, size);
}

/**
 ***********************************************************************************************************************
 * @brief           This function for write flash
 * @param[in]       part       the part info
 * @param[in]       addr       offset
 * @param[in]       buf        write date
 * @param[in]       size       write size
 *
 * @retval          write size
 ***********************************************************************************************************************
 */
cmiot_int32 cmiot_ota_fal_part_write(void *part, cmiot_uint32 addr, const cmiot_char *buf, cmiot_uint32 size)
{
    return os_fal_part_write((os_fal_part_t *)part, (uint32_t)addr, (const uint8_t *)buf, size);
}

/**
 ***********************************************************************************************************************
 * @brief           This function for erase flash
 * @param[in]       part       the part info
 * @param[in]       addr       offset
 * @param[in]       size       erase size
 *
 * @retval          erase size
 ***********************************************************************************************************************
 */
cmiot_int32 cmiot_ota_fal_part_erase(void *part, cmiot_uint32 addr, size_t size)
{
    return os_fal_part_erase((os_fal_part_t *)part, (uint32_t)addr, size);
}

/**
 ***********************************************************************************************************************
 * @brief           This function for get part block size
 * @param[in]       type       part type
 *
 * @retval          block size
 ***********************************************************************************************************************
 */
cmiot_uint32 cmiot_hal_get_true_blocksize(cmiot_uint8 type)
{
    os_fal_part_t *part = (os_fal_part_t *)cmiot_hal_get_device(type);

    if (part && part->flash)
    {
        return (part->flash->block_size < 1024 ? 1024 : part->flash->block_size);
    }

    return 0;
}

/**
 ***********************************************************************************************************************
 * @brief           This function for get part page size
 * @param[in]       type       part type
 *
 * @retval          page size
 ***********************************************************************************************************************
 */
cmiot_uint32 cmiot_hal_get_true_pagesize(cmiot_uint8 type)
{
    os_fal_part_t *part = (os_fal_part_t *)cmiot_hal_get_device(type);

    if (part && part->flash)
    {
        return part->flash->page_size;
    }

    return 0;
}

/**
 ***********************************************************************************************************************
 * @brief           This function for get download part addr
 * @param           void
 *
 * @retval          part addr
 ***********************************************************************************************************************
 */
cmiot_uint32 cmiot_hal_get_delta_addr(void)
{
    os_fal_part_t *part = (os_fal_part_t *)cmiot_hal_get_device(CMIOT_FILETYPE_PATCH);

    if (part && part->info)
    {
        return part->info->offset;
    }

    return 0;
}

/**
 ***********************************************************************************************************************
 * @brief           This function for get download part size
 * @param           void
 *
 * @retval          part size
 ***********************************************************************************************************************
 */
cmiot_uint32 cmiot_hal_get_download_size()
{
    os_fal_part_t *part = (os_fal_part_t *)cmiot_hal_get_device(CMIOT_FILETYPE_PATCH);

    if (part && part->info)
    {
        return part->info->size;
    }

    return 0;
}
