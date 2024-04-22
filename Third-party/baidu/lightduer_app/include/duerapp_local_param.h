// Copyright (2022) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_local_param.h
 * Auth: shichenyu (shichenyu01@baidu.com)
 * Desc: duerapp_local_param.
 */
/**--------------------------------------------------------------------------*/
#ifndef _DUERAPP_LOCAL_PARAMS_H_
#define _DUERAPP_LOCAL_PARAMS_H_

/**--------------------------------------------------------------------------*
 **                         FUNCTION                                        *
 **--------------------------------------------------------------------------*/
/* 设置机型名称，当前可用名称："XTC-Q1A"、"XTC-Z6A"
    example 1:
        duer_set_device_name("XTC-Q1A");
    example 2:
        duer_set_device_name("XTC-Z6A");
*/
int duer_set_device_name(const char *device_name);
char *duer_get_device_name(void);

int duer_client_id_init(void);

int duer_sand_domain_change(char is_to_sandbox);

/**--------------------------------------------------------------------------*/
#endif
