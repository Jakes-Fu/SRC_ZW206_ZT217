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
 * @file        cmiot_cfg.c
 *
 * @brief       Implement config functions
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#include "cmiot_cfg.h"

/**
 ***********************************************************************************************************************
 * @brief           This function will return extra block number
 * @param           void
 *
 * @retval          extra block number
 ***********************************************************************************************************************
 */
cmiot_uint8 cmiot_get_num_of_extra_blocks(void)
{
#ifdef CMIOT_ALGORITHM_FULL_UPGRADE
    return CMIOT_ONE_EXTRA_BLOCKS;
#else
    return CMIOT_TWO_EXTRA_BLOCKS;
#endif
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return retry count when flow failed
 * @param           void
 *
 * @retval          retry count
 ***********************************************************************************************************************
 */
cmiot_uint8 cmiot_get_try_count(void)
{
    return CMIOT_DEFAULT_TRY_COUNT;
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return segment size for download
 * @param           void
 *
 * @retval          segment size
 ***********************************************************************************************************************
 */
cmiot_uint8 cmiot_get_default_segment_size(void)
{
    return CMIOT_DEFAULT_SEGMENT_SIZE_INDEX;
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return network type
 * @param           void
 *
 * @retval          network type
 ***********************************************************************************************************************
 */
// cmiot_char *cmiot_get_network_type(void)
// {
//     return "-";
// }

/**
 ***********************************************************************************************************************
 * @brief           This function will return download part name
 * @param           void
 *
 * @retval          download part name
 ***********************************************************************************************************************
 */
cmiot_char *cmiot_download_name()
{
    return CMIOT_ONEOS_UPDATE;
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return product id
 * @param           void
 *
 * @retval          product id
 ***********************************************************************************************************************
 */
// cmiot_char *cmiot_get_product_id(void)
// {
//     return CMIOT_FOTA_SERVICE_PRODUCT_ID;
// }

/**
 ***********************************************************************************************************************
 * @brief           This function will return product sec
 * @param           void
 *
 * @retval          product sec
 ***********************************************************************************************************************
 */
// cmiot_char *cmiot_get_product_sec(void)
// {
//     return CMIOT_FOTA_SERVICE_PRODUCT_SEC;
// }

/**
 ***********************************************************************************************************************
 * @brief           This function will return apk version
 * @param           void
 *
 * @retval          apk version
 ***********************************************************************************************************************
 */
// cmiot_char *cmiot_get_apk_version(void)
// {
//     return CMIOT_FOTA_OS_VERSION;
// }

/**
 ***********************************************************************************************************************
 * @brief           This function will return firmware version
 * @param           void
 *
 * @retval          firmware version
 ***********************************************************************************************************************
 */
cmiot_char *cmiot_get_firmware_version(void)
{
    return CMIOT_FIRMWARE_VERSION;
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return recv buf len
 * @param           void
 *
 * @retval          recv buf len
 ***********************************************************************************************************************
 */
cmiot_uint16 cmiot_get_recieve_buf_len()
{
    return CMIOT_RECIEVE_BUF_MAXLEN;
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return send buf len
 * @param           void
 *
 * @retval          send buf len
 ***********************************************************************************************************************
 */
cmiot_uint16 cmiot_get_send_buf_len(void)
{
    return CMIOT_SEND_BUF_MAXLEN;
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return timeout
 * @param           void
 *
 * @retval          timeout
 ***********************************************************************************************************************
 */
cmiot_uint32 cmiot_get_try_time(void)
{
    return CMIOT_DEFAULT_TRY_TIME;
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return utc time
 * @param           void
 *
 * @retval          utc time
 ***********************************************************************************************************************
 */
// cmiot_uint32 cmiot_get_utc_time(void)
// {
//     return CMIOT_DEFAULT_UTC_TIME;
// }

#ifdef CMIOT_ALGORITHM_LUCKPAND
/**
 ***********************************************************************************************************************
 * @brief           This function will return diff block size
 * @param           void
 *
 * @retval          block size
 ***********************************************************************************************************************
 */
cmiot_uint32 cmiot_get_diff_block_size(void)
{
    return CMIOT_FOTA_CONTRAST_BLOCK_SIZE * 1024;
}
#endif

cmiot_char *cmiot_get_manufacturer(void)
{
    return CMIOT_FOTA_SERVICE_OEM;
}

cmiot_char *cmiot_get_model_number(void)
{
    return CMIOT_FOTA_SERVICE_MODEL;
}

cmiot_char *cmiot_get_language(void)
{
    return CMIOT_FOTA_SERVICE_LANG;
}

cmiot_char *cmiot_get_ota_server_host(void)
{
    return CMIOT_FOTA_SERVER_HOST;
}