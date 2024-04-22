/**
 * Copyright (2019) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * File: lightduer_dcs_settings.h
 * Auth: lingfeng meng (menglingfeng@baidu.com)
 * Desc: apply APIs to support DCS Settings Interface.
 */
#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DCS_SETTINGS_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DCS_SETTINGS_H
#include "lightduer_dcs.h"
#include "lightduer_dcs_local.h"
#include "lightduer_connagent.h"
#include "lightduer_log.h"
#include "lightduer_ds_log_dcs.h"
#include "lightduer_night_mode.h"
#include "lightduer_nm_time_utils.h"
/**
 * DESC:
 * Initialize settings interface.
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_settings_init(void);

/**
 * DESC:
 * Get SettingsState client context.
 */
baidu_json *duer_get_settings_state_internal();

#endif/*BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DCS_SETTINGS_H*/
