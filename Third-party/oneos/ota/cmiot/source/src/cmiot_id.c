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
 * @file        cmiot_id.c
 *
 * @brief       Implement id functions
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#include "cmiot_id.h"
#include "cmiot_cfg.h"
#include "os_util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include "mn_type.h"
#include "mn_api.h"
/**
 ***********************************************************************************************************************
 * @brief                This function will get device unique id, max len is CMIOT_MID_MAXLEN
 * @param[out]      uid        The memory to saved unique id
 *
 * @retval              void
 ***********************************************************************************************************************
 */
void cmiot_get_uniqueid(cmiot_char *uid)
{
    cmiot_uint8 imei_str[(MN_MAX_IMEI_LENGTH<<1) + 1] = {0};
    MN_IMEISVN_T imeisvn = {0};/*lint !e64*/
    cmiot_uint16 imei_len = 0;
    cmiot_int32 ret = MNNV_GetIMEISVNEx(MN_DUAL_SYS_1, &imeisvn);
    if(!ret)
    {
        os_kprintf("cmiot_get_uniqueid: get imei failed");
        return;
    }
    MMIAPICOM_BcdToStr(0, (cmiot_uint8 *)imeisvn.imei, MN_MAX_IMEI_LENGTH<<1, (char*)imei_str);

    imei_len = (cmiot_uint16)strlen((char *)imei_str) - 1;
    memcpy(uid, imei_str+1, imei_len);
    os_kprintf("cmiot_get_uniqueid: uid = %s", uid);
    // memcpy(uid, "112233445566778899AABBCCDDEEFF", sizeof("112233445566778899AABBCCDDEEFF") - 1);
}
