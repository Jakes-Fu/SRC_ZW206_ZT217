// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_auth.h
 * Auth: Li Chang (changli@baidu.com)
 * Desc: DCS authorization.
 */

#ifndef LIGHTDUER_DUERAPP_AUTH_H
#define LIGHTDUER_DUERAPP_AUTH_H

typedef int (*auth_cb_f_t)(int status);

void duer_auth_login_out();

void duer_auth_set_callback(auth_cb_f_t cb);

int duer_auth_get_status();

void duer_auth_set_status(int status);

int duer_auth_anonymous();

#endif
