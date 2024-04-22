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
 * @file        cmiot_sys.c
 *
 * @brief       Interface related to OS.
 *
 * @revision
 * Date         Author          Notes
 * 2020-10-13   OneOS Team      First Version
 ***********************************************************************************************************************
 */
#include "cmiot_sys.h"
#include <os_util.h>
#include <os_task.h>
#include "os_api.h"
/**
 ***********************************************************************************************************************
 * @brief           This function for sleep
 * @param[in]       time       The sleep time, ms
 *
 * @retval          void
 ***********************************************************************************************************************
 */
void cmiot_msleep(cmiot_uint32 time)
{
    os_task_msleep(time);
}

/**
 ***********************************************************************************************************************
 * @brief           This function for output data
 * @param[in]       data       The data
 * @param[in]       len        The data len
 *
 * @retval          void
 ***********************************************************************************************************************
 */
void cmiot_printf(cmiot_char *data, cmiot_uint32 len)
{
    // SCI_TRACE_LOW("cmiot test:%s\n", data);
    os_kprintf("%s", data);
}
