#ifndef _DUERAPP_DICT_H_
#define _DUERAPP_DICT_H_ 

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "os_api.h"
#include "mmk_type.h"
#include "mmi_module.h"
#include "duerapp_dict_cloud.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#define DUERAPP_DICTIONARY_JSON_TEST_SUPPORT 0//测试json

#define DUERAPP_DICTIONARY_VOICE_SUPPORT//字典主线程
#define DUERAPP_DICTIONARY_GROUPWORDS_SUPPORT//组词界面
#define DUERAPP_DICTIONARY_SINGLEWORD_SUPPORT//单字界面
#define DUERAPP_DICTIONARY_MULTIWORDS_SUPPORT//多字界面

#define DUERAPP_DICTIONARY_SINGLEWORD_PINYIN_IMG_WIDTH 40 //单字界面的拼音图片的宽度
#define DUERAPP_DICTIONARY_SINGLEWORD_PINYIN_IMG_HEIGHT 30 //单字界面的拼音图片的高度
#define DUERAPP_DICTIONARY_MULTIWORDS_PINYIN_IMG_WIDTH 60 //多字界面的拼音图片的宽度
#define DUERAPP_DICTIONARY_MULTIWORDS_PINYIN_IMG_HEIGHT 50 //多字界面的拼音图片的高度

#define DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN 2 //标题上下间隙
#define DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN 8 //标题左右间隙

#define DUERAPP_DICTIONARY_BIGBIG_FONT  SONG_FONT_60 //一至二列的汉字字体
#define DUERAPP_DICTIONARY_BIG_FONT     SONG_FONT_45 //三列的汉字字体
#define DUERAPP_DICTIONARY_TITLE_FONT   SONG_FONT_30 //标题字体
#define DUERAPP_DICTIONARY_CONTENT_FONT SONG_FONT_24 //内容字体

#define DUERAPP_DICTIONARY_FONT_BIG 6
#define DUERAPP_DICTIONARY_FONT_NORMAL 6
#define DUERAPP_DICTIONARY_STRUCT_MAX 6//字典属性
#define DUERAPP_DICTIONARY_GROUPWORDS_MAX 15//最多15个组词
#define DUERAPP_DICTIONARY_PINYIN_MAX 6//最多6个多音
#define DUERAPP_DICTIONARY_IMGTXTBUTTONX_MAX 15//最多15个字

#define DUERAPP_DICTIONARY_TRY_USE_CNT  1   //字典试用次数

/**************************************************************************************************************************
 *                                                          TYPEDEF TYPE                                                   
 **************************************************************************************************************************/
typedef int(*msg_dict_candidate_free_cb)(duerapp_dict_candidate_t *ctx);
typedef int(*msg_dict_delete_cb)(duerapp_dict_card_t *ctx);

/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/
typedef enum _dictionary_card_type_t
{
    DUERAPP_DICT_RENDER_EMPTY = 1,
    DUERAPP_DICT_RENDER_DICTSEL,//候选词
    DUERAPP_DICT_RENDER_NORMAL,
}dictionary_card_type_t;

typedef struct dictionary_thread_msg_info_t
{
    SIGNAL_VARS
    msg_dict_candidate_free_cb dict_candidate_free_cb;
    msg_dict_delete_cb dict_delete_cb;
    void *userdata;
}dictionary_thread_msg_info_t;

typedef struct dictionary_mmk_msg_info_t
{
    msg_dict_candidate_free_cb dict_candidate_free_cb;
    msg_dict_delete_cb dict_delete_cb;
    void *userdata;
}dictionary_mmk_msg_info_t;

typedef struct _dictionary_tmp_info_t
{
    char out_str[64];
    char outdetail_str[2048];
    wchar out_wstr[64];
    wchar outdetail_wstr[2048];
}dictionary_tmp_info_t;

typedef struct _dictionary_group_words_info_t
{
    char words[DUERAPP_DICTIONARY_GROUPWORDS_MAX][64];//最多15个组词
    void *userdata;
}dictionary_group_words_info_t;

typedef struct _dictionary_single_word_info_t
{
    char pinyin[DUERAPP_DICTIONARY_PINYIN_MAX][64];//最多6个音
    void *userdata;
}dictionary_single_word_info_t;

typedef struct _dictionary_multi_word_info_t
{
    char pinyin[64];//多字的拼音
    char words[DUERAPP_DICTIONARY_IMGTXTBUTTONX_MAX][64];//最多12个字
    void *userdata;
}dictionary_multi_word_info_t;

typedef struct _dictionary_info_t
{
    dictionary_group_words_info_t group_word_info;
    dictionary_single_word_info_t single_word_info;
    dictionary_multi_word_info_t multi_word_info;
}dictionary_info_t;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

PUBLIC void PUB_DUERAPP_DictionaryMainWinOpen(void);

PUBLIC uint32 PUB_DUERAPP_Dictionary_Thread_Id_Get(void);

PUBLIC void PUB_DUERAPP_Dictionary_Thread_SendMsg(dictionary_thread_msg_info_t *msg);

PUBLIC void PUB_DUERAPP_DictionaryCardWinOpen(dictionary_card_type_t type, dictionary_mmk_msg_info_t *ctx);

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif
