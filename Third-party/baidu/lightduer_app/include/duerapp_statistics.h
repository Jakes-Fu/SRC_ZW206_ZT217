// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_statistics.c
 * Auth: shichenyu (shichenyu01@baidu.com)
 * Desc: duerapp data statistics.
 */
/***************************************************************************/
#ifndef _DUERAPP_STATISTICS_H_
#define _DUERAPP_STATISTICS_H_

// #include "baidu_json.h"
// #include "lightduer_thread.h"
// #include "lightduer_types.h"
// #include "lightduer_http_client.h"
// #include "lightduer_http_client_ops.h"

/**--------------------------------------------------------------------------*
 **                         DEFINE                                         *
 **--------------------------------------------------------------------------*/
#define STATISTICS_BANNER_NUM_MAX           10  //banner最大数量
#define STATISTICS_APP_ICON_NUM_MAX         40  //运营位最大数量
#define STATISTICS_APP_ICON_TAG_LEN         40  //运营位tag最大长度

/**--------------------------------------------------------------------------*
 **                         TYPEDEFS                                        *
 **--------------------------------------------------------------------------*/
typedef enum {
    STATISTICS_CLICK_NONE = 0,
    STATISTICS_CLICK_HOME_PAGE,             //首页计数
    STATISTICS_CLICK_BANNER,                //banner点击计数
    STATISTICS_CLICK_APP_ICON,              //APP ICON点击计数
    STATISTICS_CLICK_START_OPERATE,         //弹窗点击计数
    STATISTICS_CLICK_ACTIVITY_LONG_PIC,     //长图点击计数
    STATISTICS_CLICK_DUER_HEAD,             //度头点击计数

    STATISTICS_CLICK_TIME_XIAODU_USE,       //小度助手整体使用时间
    STATISTICS_CLICK_TIME_WRITING_USE,      //作文使用时间
    STATISTICS_CLICK_TIME_DICT_USE,         //字典使用时间

    STATISTICS_CLICK_INPUT_ASR,             //输入法点击识别次数
    STATISTICS_CLICK_INPUT_OTHER,           //输入法选择其他输入次数

    STATISTICS_CLICK_TYPE_MAX
} statistics_click_type_e;

typedef enum {
    STATISTICS_PAGE_NONE = 0,
    STATISTICS_PAGE_FIRST,
    STATISTICS_PAGE_SECOND,
    STATISTICS_PAGE_THIRD,

    STATISTICS_PAGE_MAX
} statistics_page_type_e;

typedef enum {
    STATISTICS_TIME_TYPE_NONE = 0,
    STATISTICS_TIME_TYPE_XIAODU,
    STATISTICS_TIME_TYPE_WRITING,
    STATISTICS_TIME_TYPE_DICT,

    STATISTICS_TIME_TYPE_MAX
} statistics_time_type_e;

typedef struct {
    int                     click_type;             //statistics_click_type_e
    int                     page_type;              //statistics_page_type_e
    int                     banner_id;
    char                    icon_tag[STATISTICS_APP_ICON_TAG_LEN];
} statistics_data_t;

/**--------------------------------------------------------------------------*
 **                         FUNCTION                                        *
 **--------------------------------------------------------------------------*/
int duer_add_click_statistics(unsigned char click_type, unsigned int param, const char *icon_tag);
int duer_send_click_statistics_quick(void);
void duer_set_statistics_time_minutes(int minutes);

int duer_send_version_statistics_quick(void);

void duer_statistics_time_count_start(unsigned char time_type); //statistics_time_type_e
void duer_statistics_time_count_stop(unsigned char time_type);  //statistics_time_type_e

/**--------------------------------------------------------------------------*/
#endif
