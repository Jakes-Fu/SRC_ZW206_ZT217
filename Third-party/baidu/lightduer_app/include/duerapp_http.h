// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_main.c
 * Auth: Liuwenshuai (liuwenshuai@baidu.com)
 * Desc: duerapp main.
 */
/***************************************************************************/
#ifndef _DUERAPP_HTTP_REQUEST_H_
#define _DUERAPP_HTTP_REQUEST_H_

#include "baidu_json.h"
#include "lightduer_thread.h"
#include "lightduer_types.h"
#include "lightduer_http_client.h"
#include "lightduer_http_client_ops.h"
/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
int duer_send_link_click_url(const char *url);
int duer_send_link_click_url_with_params(const char *url, baidu_json *params);
int duerapp_request_task(duer_thread_entry_f_t task_entry);
int duerapp_request_task_with_param(duer_thread_entry_f_t task_entry, void *param);
baidu_json *duerapp_device_report_body(void);
duer_http_result_t duerapp_create_short_url(duer_http_data_handler http_response,const char *url,const char *request_url);
duer_http_result_t duerapp_wechat_request(duer_http_data_handler http_response,const char *request_url);
duer_http_result_t duerapp_wechat_request_with_param(duer_http_data_handler http_response, void *user_ctx, const char *request_url);
duer_http_result_t duerapp_openapi_request(duer_http_data_handler http_response,const char *request_url);
duer_http_result_t duerapp_device_sync(duer_http_data_handler http_response,const char *request_url);
duer_http_result_t duerapp_openapi_post(duer_http_data_handler http_response,baidu_json *json_body,const char *request_url);
duer_http_result_t duerapp_openapi_post_user_data(duer_http_data_handler http_response,baidu_json *json_body,const char *request_url, void *p_usr_ctx);
duer_http_result_t duerapp_wechat_post(duer_http_data_handler http_response,baidu_json *json_body,const char *request_url);
duer_http_result_t duerapp_device_assists_request(duer_http_data_handler http_response,const char *request_url, char *pOpenIds, char *info);
void duerinput_params_init(void);
/**--------------------------------------------------------------------------*/
#endif
