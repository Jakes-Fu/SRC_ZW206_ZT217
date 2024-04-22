// Copyright 2017 Baidu Inc. All Rights Reserved.
//
// Auth: Chang Li (changli@baidu.com)
// Desc: Recorder controller function

#ifndef BAIDU_LIGHTDUER_DUERAPP_RECORDER_NEW_H
#define BAIDU_LIGHTDUER_DUERAPP_RECORDER_NEW_H
#include <stdint.h>
#include <stdbool.h>
#include "lightduer_types.h"

void duer_record_start(bool is_positive);

void duer_on_record_finish(int code);

#endif  // BAIDU_LIGHTDUER_DUERAPP_RECORDER_NEW_H
