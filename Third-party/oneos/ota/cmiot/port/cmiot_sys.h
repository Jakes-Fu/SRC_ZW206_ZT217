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
 * @file        cmiot_sys.h
 *
 * @brief       Interface related to OS.
 *
 * @revision
 * Date         Author          Notes
 * 2021-04-09   OneOS Team      First version.
 ***********************************************************************************************************************/
#ifndef __CMIOT_SYS_H__
#define __CMIOT_SYS_H__

#include "cmiot_type.h"

#if defined(__cplusplus)
extern "C" {
#endif

void cmiot_msleep(cmiot_uint32 time);
void cmiot_printf(cmiot_char *data, cmiot_uint32 len);

#if defined(__cplusplus)
}
#endif

#endif /* __CMIOT_SYS_H__ */
