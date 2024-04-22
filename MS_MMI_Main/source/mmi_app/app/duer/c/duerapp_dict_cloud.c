// Copyright (2021) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_dict_cloud.c
 * Auth: Tangquan (tangquan@baidu.com)
 * Desc: xiaodu dictionary.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 3/2021         tangquan         Create                                    *
******************************************************************************/

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "duerapp.h"
#include "duerapp_id.h"
#include "duerapp_dcs.h"
#include "duerapp_recorder.h"
#include "duerapp_main.h"
#include "duerapp_auth.h"
#include "duerapp_http.h"
#include "duerapp_image.h"
#include "duerapp_audio_play_utils.h"

#include "lightduer_dcs_local.h"
#include "lightduer_log.h"
#include "lightduer_asr.h"
#include "lightduer_sleep.h"
#include "lightduer_thread.h"
#include "lightduer_types.h"
#include "lightduer_profile.h"
#include "lightduer_memory.h"
#include "lightduer_random.h"
#include "lightduer_semaphore.h"
#include "lightduer_http_client.h"
#include "lightduer_http_client_ops.h"
#include "lightduer_dcs.h"
#include "lightduer_timestamp.h"
#include "lightduer_events.h"
#include "lightduer_dlp_dcs_adapter.h"
#include "lightduer_uuid.h"

#include "duerapp_payload.h"
#include "duerapp_common.h"
#include "duerapp_dict_activity.h"
#include "duerapp_dict_cloud.h"

/*********************
 *      DEFINES
 *********************/
#define DUERAPP_NAMESPACE_THIRDPARTY_WATCH      "ai.dueros.device_interface.thirdparty.watch"

#define DUERAPP_DIRECTIVE_DICTIONARY            "DuerWatchDictionary"

#define LINKCLICK_QUERY                         "dueros://server.dueros.ai/query?q=%s"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  FUNCTION PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void duerapp_hex_print(unsigned char *input, int length);
static void duerapp_hex_print2(unsigned char *name, unsigned char *input, int length);
static duer_status_t duer_dcs_dictionary_handler(baidu_json *directive);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
***********************/
duerapp_dict_card_t * duerapp_dict_create_card(void)
{
    duerapp_dict_card_t *card = NULL;
    int i = 0;
    do {
        card = DICT_MALLOC(sizeof(duerapp_dict_card_t));
        if (!card) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:duerapp_dict_card_t malloc failed", __FUNCTION__, __LINE__);
            break;
        }

        card->n_words = 0;
        strcpy(card->words, "");
        for (i = 0; i < 6; i++) {
            strcpy(card->pinyin[i], "");
        }
        card->head = NULL;
    } while(0);
    
    return card;
}

int duerapp_dict_delete(duerapp_dict_card_t *card)
{
    duerapp_dict_entry_t *entry = NULL;
    duerapp_dict_entry_t *tmp = NULL;

    if (!card) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
    } else {
        entry = card->head;
        while (entry) {
            tmp = entry->next;
            DICT_FREE(entry);
            entry = tmp;
        }

        DICT_FREE(card);
    }
    return 0;
}

int duerapp_dict_add_entry(duerapp_dict_card_t *card, duerapp_dict_entry_t *entry)
{
    int retcode = -1;
    duerapp_dict_entry_t *tmp = NULL;

    do {
        if (!card || !entry) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
            break;
        }

        entry->next = NULL;

        if (card->head == NULL) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Create head node", __FUNCTION__, __LINE__);
            card->head = entry;
            card->head->next = NULL;
        } else {
            // SCI_TraceLow("(%s)(%d)[duer_watch]:Insert node");
            // duerapp_dict_entry_t *tmp = card->head;
            // card->head->next = entry;
            // entry->next = tmp;

            SCI_TraceLow("(%s)(%d)[duer_watch]:Append node", __FUNCTION__, __LINE__);
            tmp = card->head;
            while (tmp->next) {
                tmp = tmp->next;
            }
            tmp->next = entry;
            entry->next = NULL;
        }

        retcode = 0;
    } while(0);

    return retcode;
}

int duerapp_dict_print_card(duerapp_dict_card_t *card)
{
    int retcode = -1;
    duerapp_dict_entry_t *tmp = NULL;
    int id = 0;

    do {
        if (!card) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
            break;
        }

        SCI_TraceLow("(%s)(%d)[duer_watch]:Card words count: %d", __FUNCTION__, __LINE__, card->n_words);
        SCI_TraceLow("(%s)(%d)[duer_watch]:Card title: %s", __FUNCTION__, __LINE__, card->words);

        tmp = card->head;
        while (tmp) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Entry %d", __FUNCTION__, __LINE__, id++);
            SCI_TraceLow("(%s)(%d)[duer_watch]:%s", __FUNCTION__, __LINE__, tmp->title);
            SCI_TraceLow("(%s)(%d)[duer_watch]:%s", __FUNCTION__, __LINE__, tmp->detail);
            tmp = tmp->next;
        }

        retcode = 0;
    } while(0);

    return retcode;
}

static void duerapp_hex_print(unsigned char *input, int length)
{
    unsigned char pbuf[128];
    int count = length;
    unsigned char *p = NULL;
    int i = 0;
    if (!input || !length) {
        return;
    }

    while (count) {
        if (count > 16) {
            p = pbuf;
            for (i = 0; i < 16; i++) {
                snprintf(p, 4, "%.2X ", *input++);
                p += 3;
            }
            SCI_TraceLow("[duer_watch]:%s", pbuf);
            SCI_TraceLow("(%s)(%d)[duer_watch]:", __FUNCTION__, __LINE__);
            count -= 16;
        } else {
            p = pbuf;
            for (i = 0; i < count; i++) {
                snprintf(p, 4, "%.2X ", *input++);
                p += 3;
            }
            SCI_TraceLow("[duer_watch]:%s", pbuf);
            SCI_TraceLow("(%s)(%d)[duer_watch]:", __FUNCTION__, __LINE__);
            break;
        }
    }

}

static void duerapp_hex_print2(unsigned char *name, unsigned char *input, int length)
{
#if DUERAPP_DICTIONARY_JSON_TEST_SUPPORT == 0
    unsigned char pbuf[128];
    int count = 0;
    unsigned char *p = NULL;
    int i = 0;
    if (!input || !length) {
        return;
    }

    
    count = length < 31 ? length : 31;
    p = pbuf;
    p += snprintf(p, sizeof(pbuf), "%s:", name);
    for (i = 0; i < count; i++) {
        if (sizeof(pbuf) - ((int)p - (int)pbuf) > 4) {
            p += snprintf(p, 5, "\\x%.2X", *input++);
        } else {
            break;
        }
    }
    SCI_TraceLow("[duer_watch]:%s", pbuf);
#endif
}

duerapp_dict_entry_t * duerapp_create_entry(char *title, char *detail)
{
    duerapp_dict_entry_t *entry = NULL;
    do {
        if (!title || !detail) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
            break;
        }

        entry = DICT_MALLOC(sizeof(duerapp_dict_entry_t));
        if (!entry) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:duerapp_dict_entry_t malloc failed", __FUNCTION__, __LINE__);
            break;
        }

        strncpy(entry->title, title, sizeof(entry->title) - 1);
        strncpy(entry->detail, detail, sizeof(entry->detail) - 1);
    } while(0);
    
    return entry;
}

char* duerapp_get_utf8_char(char *instr, char *outstr)
{
    if (!instr || !outstr) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return NULL;
    }

    if (!(*instr & 0x80)) {
        // 00 - 7F
        *outstr++ = *instr++;
    } else if ((*instr & 0xE0) == 0xC0) {
        // 80 - 7FF
        *outstr++ = *instr++;
        *outstr++ = *instr++;
    } else if ((*instr & 0xF0) == 0xE0) {
        // 800 - FFFF
        *outstr++ = *instr++;
        *outstr++ = *instr++;
        *outstr++ = *instr++;
    } else if ((*instr & 0xF8) == 0xF0) {
        // 1000 - 1FFFFF
        *outstr++ = *instr++;
        *outstr++ = *instr++;
        *outstr++ = *instr++;
        *outstr++ = *instr++;
    } else {

    }

    *outstr = '\0';
    return instr;
}

char* duerapp_get_n_utf8_char(char *instr, char *outstr, int n)
{
    int i = 0;

    if (!instr || !outstr || !n) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return NULL;
    }

    for (i = 0; i < n; i++) {
        if (!(*instr & 0x80)) {
            // 00 - 7F
            *outstr++ = *instr++;
        } else if ((*instr & 0xE0) == 0xC0) {
            // 80 - 7FF
            *outstr++ = *instr++;
            *outstr++ = *instr++;
        } else if ((*instr & 0xF0) == 0xE0) {
            // 800 - FFFF
            *outstr++ = *instr++;
            *outstr++ = *instr++;
            *outstr++ = *instr++;
        } else if ((*instr & 0xF8) == 0xF0) {
            // 1000 - 1FFFFF
            *outstr++ = *instr++;
            *outstr++ = *instr++;
            *outstr++ = *instr++;
            *outstr++ = *instr++;
        } else {

        }
    }
    
    *outstr = '\0';
    return instr;
}

int duerapp_strlen_utf8(char *instr)
{
    int len = 0;

    if (!instr) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return 0;
    }

    while (*instr) {
        if (!(*instr & 0x80)) {
            // 00 - 7F
            instr += 1;
            len += 1;
        } else if ((*instr & 0xC0) == 0x80) {
            // suffix
            instr += 1;
        } else if ((*instr & 0xE0) == 0xC0) {
            // 80 - 7FF
            instr += 2;
            len += 1;
        } else if ((*instr & 0xF0) == 0xE0) {
            // 800 - FFFF
            instr += 3;
            len += 1;
        } else if ((*instr & 0xF8) == 0xF0) {
            // 1000 - 1FFFFF
            instr += 4;
            len += 1;
        } else {

        }
    }
    return len;
}

baidu_json* duerapp_get_item(baidu_json *parent, int n, ...)
{
    int i = 0;
    baidu_json *obj = NULL;
    char *param = NULL;
    va_list argptr;
    va_start(argptr, n);
    for (i = 0; i < n; i++) {
        param = va_arg( argptr, char *);
        if (param) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:%s", __FUNCTION__, __LINE__, param);
            obj = baidu_json_GetObjectItem(parent, param);
            if (obj) {
                parent = obj;
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:NULL object", __FUNCTION__, __LINE__);
                break;
            }
        } else {
            SCI_TraceLow("(%s)(%d)[duer_watch]:NULL param", __FUNCTION__, __LINE__);
            obj = NULL;
            break;
        }
    }
    va_end(argptr);
    return obj;
}

static baidu_json* duerapp_get_first_array_item(baidu_json *parent, char *array_name)
{
    baidu_json *obj = baidu_json_GetObjectItem(parent, array_name);
    baidu_json *obj_item = NULL;

    if (obj) {
        obj_item = baidu_json_GetArrayItem(obj, 0);
        if (obj_item) {
            return obj_item;
        }
    }

    return NULL;
}


int duerapp_parse_card(duerapp_dict_card_t *card, baidu_json *json)
{
    int retcode = -1;
    int word_count = 0;
    char *p_str = NULL;
    duerapp_dict_entry_t *entry1 = NULL;
    baidu_json *ret_array = NULL;
    baidu_json *ret_array_item = NULL;
    int ret_array_item_count = 0;
    int i = 0;
    baidu_json *name = NULL;
    baidu_json *radicals = NULL;
    baidu_json *stroke_count = NULL;
    baidu_json *struct_type = NULL;
    baidu_json *pinyin = NULL;
    int pinyin_item_count = 0;
    baidu_json *pinyin_item = NULL;
    baidu_json *add_mean = NULL;
    int add_mean_item_count = 0;
    baidu_json *add_mean_item = NULL;
    baidu_json *add_mean_item_pinyin = NULL;
    baidu_json *definition = NULL;
    int definition_item_count = 0;
    baidu_json *definition_item = NULL;
    baidu_json *mean_list_array = NULL;
    int mean_list_array_item_count = 0;
    baidu_json *mean_list_array_item = NULL;
    // baidu_json *pinyin = NULL;
    // baidu_json *definition = NULL;
    // baidu_json *definition = NULL;
    // baidu_json *definition_item = NULL;
    baidu_json *antonym = NULL;
    int antonym_item_count = 0;
    baidu_json *antonym_item = NULL;
    baidu_json *synonym = NULL;
    baidu_json *zuci_array = NULL;
    int zuci_array_item_count = 0;
    baidu_json *zuci_array_item = NULL;
    baidu_json *liju = NULL;
    int liju_item_count = 0;
    baidu_json *liju_item = NULL;
    baidu_json *zaoju = NULL;
    int zaoju_item_count = 0;
    baidu_json *zaoju_item = NULL;
    baidu_json *zaoju_item_name = NULL;

    do {
        if (!card || !json) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
            break;
        }

#if DUERAPP_DICTIONARY_JSON_TEST_SUPPORT == 0
        ret_array = duerapp_get_item(json, 3, "payload", "data", "ret_array");
#else
        ret_array = duerapp_get_item(json, 4, "directive", "payload", "data", "ret_array");
#endif
        if (!ret_array) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:'ret_array' not found", __FUNCTION__, __LINE__);
            break;
        }

        ret_array_item_count = baidu_json_GetArraySize(ret_array);
        for (i = 0; i < ret_array_item_count; i++) {
            ret_array_item = baidu_json_GetArrayItem(ret_array, i);
            if (!ret_array_item) {
                SCI_TraceLow("(%s)(%d)[duer_watch]:No 'ret_array_item' found", __FUNCTION__, __LINE__);
                continue;
            }
#if 1
            p_str = card->info;
            // word structures
            name = duerapp_get_first_array_item(ret_array_item, "name");
            if (name) {
                duerapp_hex_print2("name", name->valuestring, strlen(name->valuestring));
                word_count = duerapp_strlen_utf8(name->valuestring);
                card->n_words = word_count;
                strcpy(card->words, name->valuestring);
            }

            radicals = duerapp_get_first_array_item(ret_array_item, "radicals");
            if (radicals) {
                duerapp_hex_print2("radicals", radicals->valuestring, strlen(radicals->valuestring));
                p_str += snprintf(p_str, sizeof(card->info) - ((int)p_str - (int)card->info), 
                                        "部首：%s\n", radicals->valuestring);
            }

            stroke_count = duerapp_get_first_array_item(ret_array_item, "stroke_count");
            if (stroke_count) {
                SCI_TraceLow("(%s)(%d)[duer_watch]:stroke_count(%d)", __FUNCTION__, __LINE__, stroke_count->valueint);
                p_str += snprintf(p_str, sizeof(card->info) - ((int)p_str - (int)card->info), 
                                        "笔画：%d\n", stroke_count->valueint);
            }

            struct_type = duerapp_get_first_array_item(ret_array_item, "struct_type");
            if (struct_type) {
                duerapp_hex_print2("struct_type", struct_type->valuestring, strlen(struct_type->valuestring));
                p_str += snprintf(p_str, sizeof(card->info) - ((int)p_str - (int)card->info), 
                                        "结构：%s\n", struct_type->valuestring);
            }

            duerapp_hex_print2("card->info", card->info, strlen(card->info));

            pinyin = baidu_json_GetObjectItem(ret_array_item, "pinyin");
            if (pinyin) {
                pinyin_item_count = baidu_json_GetArraySize(pinyin);
                pinyin_item_count = (pinyin_item_count < 6) ? pinyin_item_count : 6;
                card->n_pinyins = pinyin_item_count;
                for (i = 0; i < pinyin_item_count; i++) {
                    pinyin_item = baidu_json_GetArrayItem(pinyin, i);
                    if (pinyin_item) {
                        duerapp_hex_print2("pinyin_item", pinyin_item->valuestring, strlen(pinyin_item->valuestring));
                        snprintf(card->pinyin[i], sizeof(card->pinyin[0]), "[%s]", pinyin_item->valuestring);
                    } else {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'pinyin_item' is NULL", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
#endif

#if 1
            // add_mean
            add_mean = baidu_json_GetObjectItem(ret_array_item, "add_mean");
            if (add_mean) {
                add_mean_item_count = baidu_json_GetArraySize(add_mean);
                if (add_mean_item_count) {
                    entry1 = duerapp_create_entry("释义", "");
                    if (!entry1) {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'entry1' create failed", __FUNCTION__, __LINE__);
                        break;
                    }
                    duerapp_dict_add_entry(card, entry1);
                    p_str = entry1->detail;
                }

                for (i = 0; i < add_mean_item_count; i++) {
                    if (i) {
                        p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail), "\n");
                    }
                    add_mean_item = baidu_json_GetArrayItem(add_mean, i);
                    if (add_mean_item) {
                        add_mean_item_pinyin = baidu_json_GetObjectItem(add_mean_item, "pinyin");
                        if (!add_mean_item_pinyin) {
                            continue;
                        }
                        duerapp_hex_print2("add_mean_item_pinyin", add_mean_item_pinyin->valuestring, strlen(add_mean_item_pinyin->valuestring));
                        p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                            "[%s]\n", add_mean_item_pinyin->valuestring);
                        
                        definition = baidu_json_GetObjectItem(add_mean_item, "definition");
                        if (!definition) {
                            continue;
                        }
                        
                        if (definition) {
                            definition_item_count = baidu_json_GetArraySize(definition);
                            for (i = 0; i < definition_item_count; i++) {
                                definition_item = baidu_json_GetArrayItem(definition, i);
                                duerapp_hex_print2("definition_item", definition_item->valuestring, strlen(definition_item->valuestring));
                                p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                                    "%s\n", definition_item->valuestring);
                            }
                        }
                    } else {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'add_mean_item' is NULL", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
#endif

#if 1
            // mean_list
            mean_list_array = duerapp_get_item(ret_array_item, 1, "mean_list");
            if (mean_list_array) {
                mean_list_array_item_count = baidu_json_GetArraySize(mean_list_array);
                if (mean_list_array_item_count) {
                    entry1 = duerapp_create_entry("释义", "");
                    if (!entry1) {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'entry1' create failed", __FUNCTION__, __LINE__);
                        break;
                    }
                    duerapp_dict_add_entry(card, entry1);
                    p_str = entry1->detail;
                }

                for (i = 0; i < mean_list_array_item_count; i++) {
                    mean_list_array_item = baidu_json_GetArrayItem(mean_list_array, i);
                    if (mean_list_array_item) {
                        if (i == 0) {
                            pinyin = duerapp_get_first_array_item(mean_list_array_item, "pinyin");
                            if (!pinyin) {
                                SCI_TraceLow("(%s)(%d)[duer_watch]:No 'pinyin' found", __FUNCTION__, __LINE__);
                                continue;
                            }
                            duerapp_hex_print2("pinyin", pinyin->valuestring, strlen(pinyin->valuestring));
                            snprintf(card->pinyin[0], sizeof(card->pinyin[0]), "[%s]", pinyin->valuestring);
                        }

                        definition = duerapp_get_first_array_item(mean_list_array_item, "definition");
                        if (!definition) {
                            SCI_TraceLow("(%s)(%d)[duer_watch]:No 'definition' found", __FUNCTION__, __LINE__);
                            continue;
                        }
                        duerapp_hex_print2("definition", definition->valuestring, strlen(definition->valuestring));
                        if (i) {
                            p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail), "\n");
                        }
                        p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                            "%s", definition->valuestring);
                    } else {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'mean_list_array_item' is NULL", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
#endif

#if 1
            // definition
            definition = baidu_json_GetObjectItem(ret_array_item, "definition");
            if (definition) {
                int definition_item_count = baidu_json_GetArraySize(definition);
                if (definition_item_count) {
                    entry1 = duerapp_create_entry("释义", "");
                    if (!entry1) {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'entry1' create failed", __FUNCTION__, __LINE__);
                        break;
                    }
                    duerapp_dict_add_entry(card, entry1);
                    p_str = entry1->detail;
                }

                for (i = 0; i < definition_item_count; i++) {
                    definition_item = baidu_json_GetArrayItem(definition, i);
                    if (definition_item) {
                        duerapp_hex_print2("definition_item", definition_item->valuestring, strlen(definition_item->valuestring));
                        if (i) {
                            p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail), "\n");
                        }
                        p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                            "%d. %s", i + 1, definition_item->valuestring);
                    } else {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'definition_item' is NULL", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
#endif

#if 1
            // antonym
            antonym = baidu_json_GetObjectItem(ret_array_item, "antonym");
            if (antonym) {
                antonym_item_count = baidu_json_GetArraySize(antonym);
                if (antonym_item_count) {
                    entry1 = duerapp_create_entry("反义词", "");
                    if (!entry1) {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'entry1' create failed", __FUNCTION__, __LINE__);
                        break;
                    }
                    duerapp_dict_add_entry(card, entry1);
                    p_str = entry1->detail;
                }

                for (i = 0; i < antonym_item_count; i++) {
                    antonym_item = baidu_json_GetArrayItem(antonym, i);
                    if (antonym_item) {
                        duerapp_hex_print2("antonym_item", antonym_item->valuestring, strlen(antonym_item->valuestring));
                        if (i) {
                            p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail), "\n");
                        }
                        p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                            "%d. %s", i + 1, antonym_item->valuestring);
                    } else {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'antonym_item' is NULL", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
#endif
            
#if 1
            // synonym
            synonym = baidu_json_GetObjectItem(ret_array_item, "synonym");
            if (synonym) {
                antonym_item_count = baidu_json_GetArraySize(synonym);
                if (antonym_item_count) {
                    entry1 = duerapp_create_entry("近义词", "");
                    if (!entry1) {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'entry1' create failed", __FUNCTION__, __LINE__);
                        break;
                    }
                    duerapp_dict_add_entry(card, entry1);
                    p_str = entry1->detail;
                }

                for (i = 0; i < antonym_item_count; i++) {
                    antonym_item = baidu_json_GetArrayItem(synonym, i);
                    if (antonym_item) {
                        duerapp_hex_print2("antonym_item", antonym_item->valuestring, strlen(antonym_item->valuestring));
                        if (i) {
                            p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail), "\n");
                        }
                        p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                            "%d. %s", i + 1, antonym_item->valuestring);
                    } else {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'antonym_item' is NULL", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
#endif
            
#if 1
            // zuci
            zuci_array = duerapp_get_item(ret_array_item, 2, "zuci_array", "ret_array");
            if (zuci_array) {
                zuci_array_item_count = baidu_json_GetArraySize(zuci_array);
                if (zuci_array_item_count) {
                    entry1 = duerapp_create_entry("组词", "");
                    if (!entry1) {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'entry1' create failed", __FUNCTION__, __LINE__);
                        break;
                    }
                    duerapp_dict_add_entry(card, entry1);
                    p_str = entry1->detail;
                }

                for (i = 0; i < zuci_array_item_count; i++) {
                    zuci_array_item = baidu_json_GetArrayItem(zuci_array, i);
                    if (zuci_array_item) {
                        name = duerapp_get_first_array_item(zuci_array_item, "name");
                        if (!name) {
                            SCI_TraceLow("(%s)(%d)[duer_watch]:No 'name' found", __FUNCTION__, __LINE__);
                            continue;
                        }
                        duerapp_hex_print2("name", name->valuestring, strlen(name->valuestring));
                        p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                            "%s, ", name->valuestring);
                    } else {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'zuci_array_item' is NULL", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
#endif

#if 1
            // liju & zaoju
            liju = baidu_json_GetObjectItem(ret_array_item, "liju");
            if (liju) {
                liju_item_count = baidu_json_GetArraySize(liju);
                if (liju_item_count) {
                    entry1 = duerapp_create_entry("造句", "");
                    if (!entry1) {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:'entry1' create failed", __FUNCTION__, __LINE__);
                        break;
                    }
                    duerapp_dict_add_entry(card, entry1);
                    p_str = entry1->detail;

                    for (i = 0; i < liju_item_count; i++) {
                        liju_item = baidu_json_GetArrayItem(liju, i);
                        if (liju_item) {
                            duerapp_hex_print2("liju_item", liju_item->valuestring, strlen(liju_item->valuestring));
                            if (i) {
                                p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail), "\n");
                            }
                            p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                                "%d. %s", i + 1, liju_item->valuestring);
                        } else {
                            SCI_TraceLow("(%s)(%d)[duer_watch]:'liju_item' is NULL", __FUNCTION__, __LINE__);
                            break;
                        }
                    }
                } else {
                    zaoju = baidu_json_GetObjectItem(ret_array_item, "zaoju");
                    if (zaoju) {
                        zaoju_item_count = baidu_json_GetArraySize(zaoju);
                        if (zaoju_item_count) {
                            entry1 = duerapp_create_entry("造句", "");
                            if (!entry1) {
                                SCI_TraceLow("(%s)(%d)[duer_watch]:'entry1' create failed", __FUNCTION__, __LINE__);
                                break;
                            }
                            duerapp_dict_add_entry(card, entry1);
                            p_str = entry1->detail;
                        }

                        for (i = 0; i < zaoju_item_count; i++) {
                            zaoju_item = baidu_json_GetArrayItem(zaoju, i);
                            if (zaoju_item) {
                                zaoju_item_name = duerapp_get_first_array_item(zaoju_item, "name");
                                if (!zaoju_item_name) {
                                    SCI_TraceLow("(%s)(%d)[duer_watch]:'name' not found", __FUNCTION__, __LINE__);
                                    continue;
                                }
                                if (i) {
                                    p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail), "\n");
                                }
                                p_str += snprintf(p_str, sizeof(entry1->detail) - ((int)p_str - (int)entry1->detail),
                                                    "%d. %s", i + 1, zaoju_item_name->valuestring);
                            } else {
                                SCI_TraceLow("(%s)(%d)[duer_watch]:'zaoju_item' is NULL", __FUNCTION__, __LINE__);
                                break;
                            }
                        }
                    }
                }
            }
#endif

        }

        retcode = 0;
    } while (0);

    return retcode;
}

duerapp_dict_candidate_t * duerapp_dict_candidate_create(void)
{
    duerapp_dict_candidate_t *candy = NULL;
    int i = 0;
    do {
        candy = DICT_MALLOC(sizeof(duerapp_dict_candidate_t));
        if (!candy) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:duerapp_dict_candidate_t malloc failed", __FUNCTION__, __LINE__);
            break;
        }

        candy->num = 0;
        for (i = 0; i < sizeof(candy->candidates) / sizeof(candy->candidates[0]); i++) {
            strcpy(candy->candidates[i], "");
        }
    } while(0);
    
    return candy;
}

int duerapp_dict_candidate_free(duerapp_dict_candidate_t *candy)
{
    if (!candy) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return -1;
    }

    DICT_FREE(candy);
    
    return 0;
}

int duerapp_dict_candidate_add(duerapp_dict_candidate_t *candy, char *str)
{
    if (!candy || !str) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return -1;
    }

    if (candy->num < sizeof(candy->candidates) / sizeof(candy->candidates[0])) {
        strncpy(candy->candidates[candy->num], str, sizeof(candy->candidates[0]) - 1);
        candy->num++;
    } else {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Candidates full", __FUNCTION__, __LINE__);
    }

    return 0;
}

int duerapp_dict_candidate_print(duerapp_dict_candidate_t *candy)
{
    int i = 0;
    if (!candy) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return -1;
    }

    if (candy->num) {
        for (i = 0; i < candy->num; i++) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Candidates %d:%s", __FUNCTION__, __LINE__, i, candy->candidates[i]);
            duerapp_hex_print2("candy", candy->candidates[i], strlen(candy->candidates[i]));
        }
    } else {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Candidates empty", __FUNCTION__, __LINE__);
    }

    return 0;
}

int duerapp_parse_candy(duerapp_dict_candidate_t *candy, baidu_json *json)
{
    int retcode = -1;
    baidu_json *ret_array = NULL;
    int ret_array_item_count = 0;
    int i = 0;
    baidu_json *ret_array_item = NULL;
    baidu_json *name = NULL;

    do {
        if (!candy || !json) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
            break;
        }

#if DUERAPP_DICTIONARY_JSON_TEST_SUPPORT == 0
        ret_array = duerapp_get_item(json, 3, "payload", "data", "ret_array");
#else
        ret_array = duerapp_get_item(json, 4, "directive", "payload", "data", "ret_array");
#endif
        if (!ret_array) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:'ret_array' not found", __FUNCTION__, __LINE__);
            break;
        }

        ret_array_item_count = baidu_json_GetArraySize(ret_array);
        if (ret_array_item_count <= 1) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:No candidates", __FUNCTION__, __LINE__);
            break;
        }
        for (i = 0; i < ret_array_item_count; i++) {
            ret_array_item = baidu_json_GetArrayItem(ret_array, i);
            if (!ret_array_item) {
                SCI_TraceLow("(%s)(%d)[duer_watch]:No 'ret_array_item' found", __FUNCTION__, __LINE__);
                continue;
            }

            name = duerapp_get_first_array_item(ret_array_item, "name");
            if (name) {
                duerapp_hex_print2("name", name->valuestring, strlen(name->valuestring));
                duerapp_dict_candidate_add(candy, name->valuestring);
            }
        }

        retcode = 0;
    } while (0);

    return retcode;
}

static void duerapp_print_long_msg(char *msg)
{
    int p = 0, len = strlen(msg);
    char tmp[81];
    SCI_TraceLow("(%s)(%d)[duer_watch]:", __FUNCTION__, __LINE__);
    while(len)
    {
        if(len > 80)
        {
            len -= 80;
            strncpy(tmp, msg + p, 80);
            tmp[80] = '\0';
            p += 80;
            SCI_TraceLow("(%s)(%d)[duer_watch]:%s", __FUNCTION__, __LINE__, tmp);
        }
        else
        {
            strncpy(tmp, msg + p, len);
            tmp[len] = '\0';
            len = 0;
            SCI_TraceLow("(%s)(%d)[duer_watch]:%s", __FUNCTION__, __LINE__, tmp);
        }
    }
    SCI_TraceLow("(%s)(%d)[duer_watch]:", __FUNCTION__, __LINE__);
}

void duerapp_dict_init(void)
{
#if DUERAPP_DICTIONARY_JSON_TEST_SUPPORT == 0
    static bool is_initialized = FALSE;
    duer_directive_list res[] = {
        {DUERAPP_DIRECTIVE_DICTIONARY,   duer_dcs_dictionary_handler},
    };
    if (!is_initialized) {
        is_initialized = TRUE;
        SCI_TraceLow("(%s)(%d)[duer_watch]:Initialize Xiaodu Dict...", __FUNCTION__, __LINE__);
        SCI_TraceLow("(%s)(%d)[duer_watch]:Registering directives", __FUNCTION__, __LINE__);
        
        duer_add_dcs_directive_internal(res, sizeof(res) / sizeof(res[0]), DUERAPP_NAMESPACE_THIRDPARTY_WATCH);
    }
#endif
}

static duer_status_t duer_dcs_dictionary_handler(baidu_json *directive)
{
#if DUERAPP_DICTIONARY_JSON_TEST_SUPPORT == 0
    int ret = DUER_ERR_FAILED;
    dictionary_mmk_msg_info_t mmk_info = {0};
    // int ret = 0;
    duerapp_dict_card_t *card = NULL;
    //duerapp_set_interact_mode(DUERAPP_INTERACT_MODE_NORMAL);//回到正常模式

    duerapp_dict_candidate_t *candy = duerapp_dict_candidate_create();//候选词

    SCI_TraceLow("(%s)(%d)[duer_watch]:", __FUNCTION__, __LINE__);
    if (candy) {
        ret = duerapp_parse_candy(candy, directive);
        if (ret == 0) {
            mmk_info.dict_candidate_free_cb = duerapp_dict_candidate_free;//回调处释放
            mmk_info.userdata = (void *)candy;
            MMK_duer_other_task_to_MMI(MMI_DUERAPP_MAIN_WIN_ID, MSG_DUERAPP_DICT_RENDER_DICTSEL, &mmk_info, sizeof(dictionary_mmk_msg_info_t));
            return DUER_OK;
        } else {
            duerapp_dict_candidate_free(candy);
        }
    }

    card = duerapp_dict_create_card();//字典词
    if (card) {
        if (directive) {
            if (duerapp_parse_card(card, directive) == 0) {
                if (card->n_words > 0) {
                        SCI_TraceLow("(%s)(%d)[duer_watch]:Normal card", __FUNCTION__, __LINE__);
                        mmk_info.dict_delete_cb = duerapp_dict_delete;//回调处释放
                        mmk_info.userdata = (void *)card;
                        MMK_duer_other_task_to_MMI(MMI_DUERAPP_MAIN_WIN_ID, MSG_DUERAPP_DICT_RENDER_NORMAL, &mmk_info, sizeof(dictionary_mmk_msg_info_t));
                } else {
                    SCI_TraceLow("(%s)(%d)[duer_watch]:Empty card", __FUNCTION__, __LINE__);
                    duerapp_dict_delete(card);
                    MMK_duer_other_task_to_MMI(MMI_DUERAPP_MAIN_WIN_ID, MSG_DUERAPP_DICT_RENDER_EMPTY, NULL, 0);
                }
            }
        } else {
            duerapp_dict_delete(card);
        }
    }

#endif
    return DUER_OK;
}

int duerapp_dict_query(char *words)
{
    char *link_url = NULL;
    if (!words) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return -1;
    }
    link_url = DUER_MALLOC(512);
    if (link_url) {
        snprintf(link_url, 512, LINKCLICK_QUERY, words);
        SCI_TraceLow("(%s)(%d)[duer_watch]:%s", __FUNCTION__, __LINE__, link_url);
        duerapp_set_interact_mode(DUERAPP_INTERACT_MODE_DICTIONARY);
        duer_dcs_on_link_clicked(link_url);
        DUER_FREE(link_url);
    } else {
    }
}
