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
 * @file        cmiot_main.h
 *
 * @brief       The main header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __CMIOT_MAIN_H__
#define __CMIOT_MAIN_H__

#include "cmiot_type.h"

#ifdef __cplusplus
extern "C" {
#endif

cmiot_int8 cmiot_upgrade(void);
cmiot_int8 cmiot_report_upgrade(void);
cmiot_int8 cmiot_check_version(char version[CMIOT_VERSION_NAME_MAX]);
cmiot_int8 cmiot_only_download(void);

extern cmiot_int8 cmiot_upgrade_http(void);
extern cmiot_int8 cmiot_report_upgrade_http(void);
extern cmiot_int8 cmiot_check_version_http(char *version);
extern cmiot_int8 cmiot_only_upgrade_http(void);

#ifdef __cplusplus
}
#endif

#endif
