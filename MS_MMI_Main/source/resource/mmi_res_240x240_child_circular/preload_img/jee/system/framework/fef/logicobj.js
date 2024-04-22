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
 * @file     logicobj.js
 * @brief    -
 * @author   qingjun.yu@unisoc.com
 * @version  V1.0.0
 * @date     2022-4-12
 * @history
 *****************************************************************************
 **/

export class LogicObject {
    constructor() {
        this.tag = "LogicObject";

        this.data = new Object();
    }


    onInit() {
        console.log("%s: onInit", this.tag);
    }

    onReady() {
        console.log("%s: onReady", this.tag);
    }

    onShow() {
        console.log("%s: onShow", this.tag);
    }

    onHide() {
        console.log("%s: onHide", this.tag);
    }

    onDestroy() {
        console.log("%s: onDestroy", this.tag);
    }
}

console.log("logicobj.js ok");