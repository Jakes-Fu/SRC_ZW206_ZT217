// Copyright (2022) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_res_guard.h
 * Auth: Liuwenshuai (liuwenshuai@baidu.com)
 * Desc: 小度资源管控.
 */
/***************************************************************************/

#ifndef _DUERAPP_RES_GUARD_H_
#define _DUERAPP_RES_GUARD_H_

#include "baidu_json.h"
#include "sci_types.h"


#define USER_MAP_MAX_LENGTH      2050
#define FUZZY_WORDS_MAX_LENGTH   300
#define ALBUM_FUZZY_WORDS_MAX_LENGTH 300

typedef struct
{
    char *userMap;
    char *sysMap;
    int userBitLength;
    int userHashCnt;
    int sysBitLength;
    int sysHashcnt;
    char *fuzzyWords[FUZZY_WORDS_MAX_LENGTH];
    char *albumFuzzyWords[ALBUM_FUZZY_WORDS_MAX_LENGTH];
} DUER_USER_RES_GUARD;

typedef enum {
    BASIC_CHAT,
    INFORMATION,
    UNICAST,
    MUSIC,
    RES_CATEGORY_MAX
} RES_CATEGORY_E;

typedef void (*ResPassedCallback)(void *ctx, RES_CATEGORY_E category, baidu_json *payload_json);
typedef void (*ResFilteredCallback)(void *ctx, RES_CATEGORY_E category);

PUBLIC BOOLEAN duerapp_get_user_mapinfo();
PUBLIC BOOLEAN duerapp_res_is_filter(baidu_json *payload_json,RES_CATEGORY_E category);
PUBLIC BOOLEAN duerapp_res_is_filter_async(
    baidu_json *payload_json,
    RES_CATEGORY_E category,
    ResPassedCallback pass_callback,
    ResFilteredCallback fail_callback,
    void *user_ctx
);
PUBLIC void duerapp_free_res_map();
PUBLIC BOOLEAN duerapp_get_sensitive_words_config();

PUBLIC int duerapp_res_guard_init();
PUBLIC int duerapp_res_guard_deinit();

#endif