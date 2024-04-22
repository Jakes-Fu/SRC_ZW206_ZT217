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
 * @file        cmiot_main.c
 *
 * @brief       Implement main functions
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */
#include "cmiot_main.h"
#include "cmiot_cfg.h"
#include "cmiot_sys.h"

/**
 ***********************************************************************************************************************
 * @brief           This function will return download result
 * @param           void
 *
 * @retval          result
 ***********************************************************************************************************************
 */
cmiot_int8 cmiot_upgrade_inner(void)
{
    return cmiot_upgrade_http();
}

/**
 ***********************************************************************************************************************
 * @brief           一键升级接口,检测到新版本后直接升级
 *
 * @param
 *
 * @return          E_CMIOT_SUCCESS：检测到新版本并下载成功，重启终端后会进入还原流程
 *                  E_CMIOT_FAILURE：无网络等异常原因
 *                  E_CMIOT_NOMEMORY：内存不足，初始化失败
 *                  E_CMIOT_LAST_VERSION：当前版本已经是最新
 *                  E_CMIOT_PAKAGE_TOO_LARGE：检测到新版本，但是版本数据过大，无法升级
 ***********************************************************************************************************************
 */
cmiot_int8 cmiot_upgrade(void)
{
    return cmiot_upgrade_inner();
}

/**
 ***********************************************************************************************************************
 * @brief           This function will return report upgrade result
 * @param           void
 *
 * @retval          result
 ***********************************************************************************************************************
 */
cmiot_int8 cmiot_report_upgrade_inner(void)
{
    return cmiot_report_upgrade_http();
}

/**
 ***********************************************************************************************************************
 * @brief           升级结果上报接口
 *
 * @param
 *
 * @return          E_CMIOT_SUCCESS：升级结果上报成功
 *                  E_CMIOT_FAILURE：无网络等异常原因上报失败
 *                  E_CMIOT_NO_UPGRADE：未检测到升级，无需上报
 ***********************************************************************************************************************
 */
cmiot_int8 cmiot_report_upgrade(void)
{
    return cmiot_report_upgrade_inner();
}

/**
 ***********************************************************************************************************************
 * @brief           检测是否有新版本升级，网络正常才能使用
 *
 * @param[in]       version: 如果新版本检测成功，存放新版本号，字符长度最大CMIOT_VERSION_NAME_MAX
 *
 * @return          E_CMIOT_SUCCESS：检测成功
 *                  E_CMIOT_FAILURE：无网络等异常原因
 *                  E_CMIOT_NOMEMORY：内存不足，初始化失败
 *                  E_CMIOT_LAST_VERSION：当前版本已经是最新
 *                  E_CMIOT_PAKAGE_TOO_LARGE：检测到新版本，但是版本数据超限，无法升级
 *                  E_CMIOT_NEW_VERSION_EXIST：新版本已下载，请重启升级
 ***********************************************************************************************************************
 */
cmiot_int8 cmiot_check_version(char version[CMIOT_VERSION_NAME_MAX])
{
    cmiot_uint8 ret;
    ret = cmiot_check_version_http(version);
    return ret;
}

/**
 ***********************************************************************************************************************
 * @brief           检测到新版本后，可调用此接口下载新版本
 *
 * @param
 *
 * @return          E_CMIOT_SUCCESS：下载成功，重启终端后会进入还原流程
 *                  E_CMIOT_FAILURE：无网络等异常原因
 *                  E_CMIOT_NOMEMORY：内存不足，初始化失败
 ***********************************************************************************************************************
 */
cmiot_int8 cmiot_only_download(void)
{
    cmiot_uint8 ret;
    ret = cmiot_only_upgrade_http();
    return ret;
}
