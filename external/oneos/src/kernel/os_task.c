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
 * @file        os_task.c
 *
 * @brief       This file implements the task functions.
 *
 * @revision
 * Date         Author          Notes
 * 2020-04-06   OneOS team      First Version
 * 2020-11-10   OneOS team      Refactor task implementation.
 ***********************************************************************************************************************
 */
#include "os_types.h"
#include "os_errno.h"
#include "os_task.h"
#include "os_api.h"

/**
 ***********************************************************************************************************************
 * @brief           Force the current task to sleep.
 *
 * @details         If the time is 0, giveup the cpu.
 *
 * @attention       This function actually calls os_task_tsleep, so the minimum precision is tick.
 *                  This interface is not allowed in the following cases:
 *                      1. In interrupt context.
 *                      2. Interrupt is disabled.
 *                      3. Scheduler is locked.
 *
 * @param[in]       ms              The value of task sleep in millisecond.
 *
 * @return          The result of forcing current task to sleep.
 * @retval          OS_EOK          Force current task to sleep successfully.
 * @retval          else            Force current task to sleep failed.
 ***********************************************************************************************************************
 */
os_err_t os_task_msleep(os_uint32_t ms)
{
    SCI_Sleep(ms);
    return OS_EOK;
}
