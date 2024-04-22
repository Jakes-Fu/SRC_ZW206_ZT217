// Copyright (2023) Baidu Inc. All rights reserveed.
/**
 * File: lightduer_watch_openapi.h
 * Desc: call watch openapi and wechat service.
 */
#ifndef BAIDU_DUER_LIGHTDUER_MODULES_WATCH_API_LIGHTDUER_WATCH_API_H
#define BAIDU_DUER_LIGHTDUER_MODULES_WATCH_API_LIGHTDUER_WATCH_API_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lightduer_http_client.h"
#include "lightduer_thread.h"
#include "baidu_json.h"

typedef duer_http_result_t (*duer_watch_api_prepare_handler)(duer_http_client_t *p_client, void *user_ctx);

typedef enum {
    WATCH_SERVICE_OPENAPI,
    WATCH_SERVICE_WECHAT,
    WATCH_SERVICE_BOTWATCH,
    WATCH_SERVICE_MAX,
} duer_watch_service;

/**
 * send HTTP GET request to openapi service
 * @param http_response, in, response handler
 * @param user_ctx, in/out, p_user_ctx passed to response handler or other callback
 * @param service, in, service to call
 * @param request_url, in, url without service prefix
 * @param http_prepare, in, callback used to set up request before it is sent
 * @return DUER_OK if response is started, otherwise error code is returned.
 */
duer_http_result_t duer_watch_http_get(
    duer_http_data_handler http_response,
    void *user_ctx,
    duer_watch_service service,
    const char *request_path,
    duer_watch_api_prepare_handler http_prepare
);

/**
 * send HTTP POST request to bot-watch service
 * @param http_response, in, response handler
 * @param user_ctx, in/out, p_user_ctx passed to response handler or other callback
 * @param service, in, service to call
 * @param request_path, in, url without service prefix
 * @param json_body, in, data sent as body
 * @param http_prepare, in, callback used to set up request before it is sent
 * @return DUER_OK if response is started, otherwise error code is returned.
 */
duer_http_result_t duer_watch_http_post(
    duer_http_data_handler http_response,
    void *user_ctx,
    duer_watch_service service,
    const char *request_path,
    const baidu_json *json_body,
    duer_watch_api_prepare_handler http_prepare
);

/**
 * send HTTP GET request to openapi service
 * @param http_response, in, response handler
 * @param user_ctx, in/out, p_user_ctx passed to response handler or other callback
 * @param max_body_size, in, limit of response body, 0 for unlimited
 * @param service, in, service to call
 * @param request_path, in, url without service prefix
 * @param http_prepare, in, callback used to set up request before it is sent
 * @return DUER_OK if response is started, otherwise error code is returned.
 */
duer_http_result_t duer_watch_http_get_body(
    duer_http_body_handler http_response,
    void *user_ctx,
    int max_body_size,
    duer_watch_service service,
    const char *request_path,
    duer_watch_api_prepare_handler http_prepare
);

/**
 * send HTTP POST request to bot-watch service
 * @param http_response, in, response handler
 * @param user_ctx, in/out, p_user_ctx passed to response handler or other callback
 * @param max_body_size, in, limit of response body, 0 for unlimited
 * @param service, in, service to call
 * @param request_path, in, url without service prefix
 * @param json_body, in, data sent as body
 * @param http_prepare, in, callback used to set up request before it is sent
 * @return DUER_OK if response is started, otherwise error code is returned.
 */
duer_http_result_t duer_watch_http_post_body(
    duer_http_body_handler http_response,
    void *user_ctx,
    int max_body_size,
    duer_watch_service service,
    const char *request_path,
    const baidu_json *json_body,
    duer_watch_api_prepare_handler http_prepare
);

/**
 * execute request in openapi_http task
 * @param task_entry, in, code to execute in thread
 * @param param, in/out, user context to pass into task_entry
 * @result DUER_OK if thread is started, otherwise error code is returned
 */
int duer_watch_api_task_execute(duer_thread_entry_f_t task_entry, void *param);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_MODULES_WATCH_API_LIGHTDUER_WATCH_API_H
