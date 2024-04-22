// Copyright (2021) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_dict.h
 * Auth: Tangquan (tangquan@baidu.com)
 * Desc: xiaodu dictionary.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 11/2021         tangquan         Create                                    *
******************************************************************************/
#ifndef DUERAPP_DICT_CLOUD_H
#define DUERAPP_DICT_CLOUD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if 1

#include "baidu_json.h"
#include "duerapp_login.h"

/*********************
 *      DEFINES
 *********************/
#define DICT_MALLOC     DUER_MALLOC
#define DICT_FREE       DUER_FREE

/**********************
 *      TYPEDEFS
 **********************/
typedef struct duerapp_dict_entry
{
    struct duerapp_dict_entry *next;
    char title[32];
    char detail[2048];
} duerapp_dict_entry_t;

typedef struct 
{
    int n_words;
    int n_pinyins;
    char words[128];
    char info[128];
    char pinyin[6][64];
    duerapp_dict_entry_t *head;
} duerapp_dict_card_t;

typedef struct 
{
    int num;
    char candidates[32][64];
} duerapp_dict_candidate_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
duerapp_dict_card_t * duerapp_dict_create_card(void);
int duerapp_dict_delete(duerapp_dict_card_t *card);
int duerapp_dict_add_entry(duerapp_dict_card_t *card, duerapp_dict_entry_t *entry);
int duerapp_dict_print_card(duerapp_dict_card_t *card);
duerapp_dict_entry_t * duerapp_create_entry(char *title, char *detail);
char* duerapp_get_utf8_char(char *instr, char *outstr);
char* duerapp_get_n_utf8_char(char *instr, char *outstr, int n);
int duerapp_strlen_utf8(char *instr);
int duerapp_parse_card(duerapp_dict_card_t *card, baidu_json *json);

duerapp_dict_candidate_t * duerapp_dict_candidate_create(void);
int duerapp_dict_candidate_free(duerapp_dict_candidate_t *candy);
int duerapp_dict_candidate_add(duerapp_dict_candidate_t *candy, char *str);
int duerapp_dict_candidate_print(duerapp_dict_candidate_t *candy);
int duerapp_parse_candy(duerapp_dict_candidate_t *candy, baidu_json *json);

void duerapp_dict_init(void);
int duerapp_dict_query(char *words);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*CALCULATOR_H*/
