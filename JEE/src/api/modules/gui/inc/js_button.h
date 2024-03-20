/**
 * SPDX-FileCopyrightText: 2022-present Unisoc (Shanghai) Technologies Co., Ltd
 * SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
 *
 * Copyright 2022-present Unisoc (Shanghai) Technologies Co., Ltd.
 * Licensed under the Unisoc General Software License V1;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * Software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 * See the Unisoc General Software License v1 for more details.
 *
 **/

 /**
  *****************************************************************************
  * @file     js_button.h
  * @brief    -
  * @author   qingjun.yu@unisoc.com
  * @version  V1.0.0
  * @date
  * @history
  * @note
  * detailed description:
  *****************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2022 unisoc </center></h2>
  **/

#ifndef __JS_BUTTON_H__
#define __JS_BUTTON_H__

#include "jerry_bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

    DECLARE_PUBLIC_HANDLER(js_button_create);
    DECLARE_PUBLIC_HANDLER(js_button_delete);
    DECLARE_PUBLIC_HANDLER(js_button_set_text);
    DECLARE_PUBLIC_HANDLER(js_button_set_text_color);
    DECLARE_PUBLIC_HANDLER(js_button_set_font_size);
    DECLARE_PUBLIC_HANDLER(js_button_set_text_align);

#ifdef __cplusplus
}
#endif

#endif // __JS_BUTTON_H__
