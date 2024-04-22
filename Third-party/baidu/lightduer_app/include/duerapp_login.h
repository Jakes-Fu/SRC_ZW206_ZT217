// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_login
 * Auth: liuwenshuai (liuwenshuai@baidu.com)
 * Desc: DCS handler header file.
 */
/**--------------------------------------------------------------------------*/
#include "baidu_json.h"
#include "duerapp_payload.h"

#ifndef _DUERAPP_LOGIN_H_
#define _DUERAPP_LOGIN_H_


/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
typedef struct
{
	char uname[30];
	char nick[30];
	// 是否vip
	int is_vip;
	// 是否续费会员 ,1是续费会员
	int is_renew;
	// 会员起始时间
	int begin_timestamp;
	// 会员到期时间
	int end_timestamp;
} DUER_USER_INFO,*Pt_DUER_USER_INFO;

typedef void (*duerapp_http_upload_cb_t)(char *, void *user_data);
typedef void (*duerapp_http_download_cb_t)(char *, int len, void *);

PUBLIC void  duerapp_query_login_status();
PUBLIC void duerapp_sync_bind_user_info();
PUBLIC void  duerapp_user_login();
PUBLIC void duerapp_sync_bind_user_info_and_login_out(void);
PUBLIC void  duerapp_user_login();
PUBLIC BOOLEAN duerapp_is_login();
PUBLIC void duerapp_device_report();
PUBLIC void duerapp_user_login_out();
PUBLIC void duer_s_watch_userinfo_inprogress_reset(void);
PUBLIC void duerapp_user_force_login_out(void);
PUBLIC DUER_USER_INFO* duerapp_get_user_info();
PUBLIC void duerapp_create_user_info();
PUBLIC Pt_QR_LOGIN_URL_INFO *get_qr_url_info(void);
PUBLIC void duerapp_WatchAuthLogin(baidu_json *payload);
PUBLIC void duerapp_WatchPayLogin(baidu_json *payload);
PUBLIC void duerapp_WatchPromotedPay(baidu_json *directive);
PUBLIC void duerapp_WatchQrcodeTemplate(baidu_json *directive);
PUBLIC void duerapp_get_watch_config();

PUBLIC void duerapp_get_feedback();
PUBLIC void duerapp_user_info_release();
PUBLIC void duerapp_free_all_list();

PUBLIC P_DUEROS_OPERATE_NOTIFY *duerapp_get_operate_notify(void);

PUBLIC void duerapp_http_upload(unsigned char *data, unsigned int len, const char *format, duerapp_http_upload_cb_t fun, void *user_data);
PUBLIC void duerapp_reg_http_upload_cb(duerapp_http_upload_cb_t fun);
PUBLIC void duerapp_http_download(const char *url, duerapp_http_download_cb_t cb, void *param);
PUBLIC char *duer_base64_encode(unsigned char *in_data, unsigned int len);



#endif
