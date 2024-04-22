// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_writing.c
 * Auth: tangquan (tangquan@baidu.com)
 * Desc: duerapp writing.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 11/2021        tangquan         Create                                    *
******************************************************************************/

/**--------------------------------------------------------------------------*/
/**                         Include Files                                    */
/**--------------------------------------------------------------------------*/
#include "std_header.h"
#include "window_parse.h"
#include "guilcd.h"
#include "mmidisplay_data.h"
#include "guifont.h"
#include "mmi_textfun.h"
#include "mmipub.h"
#include "guilistbox.h"
#include "guibutton.h"
#include "guitext.h"
#include "guilabel.h"
#include "duerapp_id.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "duerapp_text.h"
#include "duerapp_image.h"
#include "duerapp_anim.h"
#include "duerapp_nv.h"
#include "duerapp_main.h"
#include "watch_common_btn.h"
#include "watch_common_list.h"
#include "watch_commonwin_export.h"
#include "mmi_event_api.h"
#include "mmk_app.h"
#include "mmi_applet_table.h"
#include "guirichtext.h"
// #include "mmiwifi_export.h"
#include "mmiphone_export.h"
#include "ctrlsetlist_export.h"
#include "guisetlist.h"

#include "dal_time.h"
#include "duerapp.h"
#include "mmicom_time.h"
#include "duerapp_common.h"
#include "duerapp_login.h"
#include "duerapp_qrwin.h"
#include "duerapp_http.h"
#include "duerapp_operate.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_audio_play_utils.h"
#include "duerapp_center.h"
#include "duerapp_statistics.h"

#include "baidu_json.h"
#include "lightduer_log.h"
#include "lightduer_random.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_dcs.h"
#include "lightduer_types.h"
#include "lightduer_ap_info.h"
#include "lightduer_lib.h"
#include "lightduer_audio_codec_ops.h"
#include "lightduer_audio_adapter.h"
#include "lightduer_audio_player.h"
#include "lightduer_audio_codec_adapter.h"
#include "lightduer_dcs_local.h"


#include "duerapp_writing.h"

/*********************
 *      DEFINES
 *********************/
#define DUERAPP_RECT(x, y, w, h)        {x,y,x+w-1,y+h-1}
#define DUERAPP_COLOR_MAKE(r, g, b)     RGB8882RGB565((r << 16) | (g << 8) | b)
#define DUERAPP_FULL_SCREEN_RECT        {0,0,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT}

#define DUERAPP_NAMESPACE_THIRDPARTY_WATCH      "ai.dueros.device_interface.thirdparty.watch"
#define DUERAPP_DIRECTIVE_GET_CONF_INFO         "DuerWatchCompositionGetConfInfo"
#define DUERAPP_DIRECTIVE_GET_GRADE_INFO        "DuerWatchCompositionGetGradeInfo"
#define DUERAPP_DIRECTIVE_UPD_GRADE_INFO        "DuerWatchCompositionUpdGradeInfo"
#define DUERAPP_DIRECTIVE_QUERY                 "DuerWatchCompositionQuery"
#define DUERAPP_DIRECTIVE_GETCOMMENTINFO        "DuerWatchCompositionGetCommentInfo"
#define DUERAPP_DIRECTIVE_COMPOSITION           "DuerWatchComposition"

#define LINKCLICK_GET_CONF_INFO                 "dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=getConfInfo"
#define LINKCLICK_GET_GRADE_INFO                "dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=getGradeInfo"
#define LINKCLICK_UPD_GRADE_INFO                "dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=updGradeInfo&grade=%s"
// #define LINKCLICK_QUERY                         "dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=query&grade=%s&literary_style=%s&hot_sort=%s&limit_word_cnt=%d"
#define LINKCLICK_QUERY                         "dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=query&grade=%s"
#define LINKCLICK_GETCOMMENTINFO                "dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=getCommentInfo&id=%s"

// extern BOOL OEM_UTF8ToWStr(const byte * pSrc,int nLen, uint16 * pDst, int nSize);
extern uint32 GUI_UTF8ToWstr(//ucs2b len
                             wchar *wstr_ptr,//out
                             uint32 wstr_len,//in
                             const uint8 *utf8_ptr,//in
                             uint32 utf8_len//in
                             );
extern void EnterAppBaiduMainWin(CAF_HANDLE_T app_handle);
extern void duerapp_show_toast(wchar *toast_ptr);

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    DUERAPP_COLOR0  = 0xFFFFFF,
    DUERAPP_COLOR1  = 0xFA5858,
    DUERAPP_COLOR2  = 0xFA8258,
    DUERAPP_COLOR3  = 0xFAAC58,
    DUERAPP_COLOR4  = 0xF7D358,
    DUERAPP_COLOR5  = 0xF4FA58,
    DUERAPP_COLOR6  = 0xD0FA58,
    DUERAPP_COLOR7  = 0xACFA58,
    DUERAPP_COLOR8  = 0x58FAF4,
    DUERAPP_COLOR9  = 0x58D3F7,
    DUERAPP_COLOR10 = 0x5858FA,
    DUERAPP_COLOR11 = 0x8258FA,
    DUERAPP_COLOR12 = 0xD358F7,
    DUERAPP_COLOR13 = 0xA4A4A4,
    DUERAPP_COLOR14 = 0x000000,
} DUERAPP_COLORS;

typedef enum
{
    DUERAPP_GRADE1 = 0,
    DUERAPP_GRADE2,
    DUERAPP_GRADE3,
    DUERAPP_GRADE4,
    DUERAPP_GRADE5,
    DUERAPP_GRADE6,
    DUERAPP_GRADE7,
    DUERAPP_GRADE8,
    DUERAPP_GRADE9,
    DUERAPP_GRADE_MAX,
} duerapp_grade_t;

typedef enum
{
    DUERAPP_LITERARY_STYLE_NARRATIVE,
    DUERAPP_LITERARY_STYLE_ARGUMENTATION,
    DUERAPP_LITERARY_STYLE_THOUGHTS,
    DUERAPP_LITERARY_STYLE_LETTER,
    DUERAPP_LITERARY_STYLE_BLANK,
} duerapp_literary_style_t;

typedef enum
{
    DUERAPP_HOTWORD_SCENERY,
    DUERAPP_HOTWORD_FIGURE,
    DUERAPP_HOTWORD_GROWTH,
    DUERAPP_HOTWORD_ANIMALS,
    DUERAPP_HOTWORD_NATURE,
    DUERAPP_HOTWORD_EVENTS,
    DUERAPP_HOTWORD_BLANK,
} duerapp_hotword_t;

typedef enum
{
    DUERAPP_WORDS_200 = 0x01,
    DUERAPP_WORDS_400 = 0x02,
    DUERAPP_WORDS_600 = 0x04,
    DUERAPP_WORDS_800 = 0x08,
    DUERAPP_WORDS_1000 = 0x10,
    DUERAPP_WORDS_ALL = 0x80,
} duerapp_words_t;

typedef struct duerapp_entity
{
    struct duerapp_entity *next;
    char *content;
    char *name;
    char *id;
    int length;
} duerapp_entity_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
LOCAL MMI_RESULT_E grade_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
LOCAL MMI_RESULT_E main_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
LOCAL MMI_RESULT_E list_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
LOCAL MMI_RESULT_E vip_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
LOCAL MMI_RESULT_E filter_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
LOCAL MMI_RESULT_E content_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
LOCAL MMI_RESULT_E trial_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

LOCAL void grade_win_full_paint(void);
LOCAL void main_win_full_paint(void);
LOCAL void list_win_full_paint(void);
LOCAL void vip_win_full_paint(void);
LOCAL void filter_win_full_paint(void);
LOCAL void content_win_full_paint(void);
LOCAL void trial_win_full_paint(void);

LOCAL void grade_win_bg_paint(void);
LOCAL void OpenVipCallBack(void);
LOCAL void grade_btn_init(uint32 ctrl_id, GUI_RECT_T *rect, wchar *text);
LOCAL GUI_RECT_T * duerapp_rect_create(GUI_RECT_T *rect, uint32 x, uint32 y, uint32 w, uint32 h);

LOCAL MMI_RESULT_E grade_win_button_press_handler(MMI_HANDLE_T ctrl_handle);
LOCAL MMI_RESULT_E main_win_button_press_handler(MMI_HANDLE_T ctrl_handle);
LOCAL MMI_RESULT_E filter_win_button_press_handler(MMI_HANDLE_T ctrl_handle);

static duer_status_t duer_dcs_get_conf_info_handler(baidu_json *directive);
static duer_status_t duer_dcs_get_grade_info_handler(baidu_json *directive);
static duer_status_t duer_dcs_upd_grade_info_handler(baidu_json *directive);
static duer_status_t duer_dcs_query_handler(baidu_json *directive);
static duer_status_t duer_dcs_get_comment_info_handler(baidu_json *directive);
static duer_status_t duer_dcs_composition(baidu_json *directive);

static int duerapp_writing_main_win_open(void);
static int duerapp_writing_grade_win_open(void);
static int duerapp_writing_list_win_open(void);
static int duerapp_writing_content_win_open(void);
static int duerapp_filter_win_open(void);

void duer_hex_print(unsigned char *input, int length);
static duer_status_t duer_free_entities(void);
static duerapp_entity_t* duer_get_entity(int index);
static duer_status_t duer_free_comments(void);

static int duer_set_grade_info(duerapp_grade_t grade);
static int duer_get_grade_info(void);
static int duer_get_conf_info(void);
static int duer_query(duerapp_grade_t grade, duerapp_literary_style_t style, duerapp_hotword_t hotword, int words_limit);
static int duer_get_comment(char *id);


/**********************
 *  STATIC VARIABLES
 **********************/
static const char tag[] = "composition";
static const wchar s_grades_unicode[][6] =
{
    L"一年级",
    L"二年级",
    L"三年级",
    L"四年级",
    L"五年级",
    L"六年级",
    L"初一",
    L"初二",
    L"初三",
};
static const char s_grades_utf8[][16] =
{
    "\xe4\xb8\x80\xe5\xb9\xb4\xe7\xba\xa7", // 一年级
    "\xe4\xba\x8c\xe5\xb9\xb4\xe7\xba\xa7", // 二年级
    "\xe4\xb8\x89\xe5\xb9\xb4\xe7\xba\xa7", // 三年级
    "\xe5\x9b\x9b\xe5\xb9\xb4\xe7\xba\xa7", // 四年级
    "\xe4\xba\x94\xe5\xb9\xb4\xe7\xba\xa7", // 五年级
    "\xe5\x85\xad\xe5\xb9\xb4\xe7\xba\xa7", // 六年级
    "\xe5\x88\x9d\xe4\xb8\x80", // 初一
    "\xe5\x88\x9d\xe4\xba\x8c", // 初二
    "\xe5\x88\x9d\xe4\xb8\x89", // 初三
};
static const wchar s_literary_styles_unicode[][12] =
{
    L"记叙文",
    L"议论文",
    L"读后感",
    L"书信",
    L"",
};
static const char s_literary_style_utf8[][16] =
{
    "\xe8\xae\xb0\xe5\x8f\x99\xe6\x96\x87", // 记叙文
    "\xe8\xae\xae\xe8\xae\xba\xe6\x96\x87", // 议论文
    "\xe8\xaf\xbb\xe5\x90\x8e\xe6\x84\x9f", // 读后感
    "\xe4\xb9\xa6\xe4\xbf\xa1", // 书信
    "",
};
static const wchar s_hotwords_unicode[][6] =
{
    L"春天",
    L"感恩",
    L"朋友",
    L"家乡",
    L"梦想",
    L"祖国",
    L"",
};
static const char s_hotword_utf8[][16] =
{
    "\xe6\x98\xa5\xe5\xa4\xa9", // 春天
    "\xe6\x84\x9f\xe6\x81\xa9", // 感恩
    "\xe6\x9c\x8b\xe5\x8f\x8b", // 朋友
    "\xe5\xae\xb6\xe4\xb9\xa1", // 家乡
    "\xe6\xa2\xa6\xe6\x83\xb3", // 梦想
    "\xe7\xa5\x96\xe5\x9b\xbd", // 祖国
    "",
};
static duerapp_grade_t s_grade_selected = DUERAPP_GRADE_MAX;
// 默认作文字数不限制
static int s_words_limit = 0;
static duerapp_entity_t *s_entity_list = NULL;
static duerapp_entity_t *s_entity_selected = NULL;
static char *s_comment = NULL;
static char *s_content = NULL;
static unsigned char s_words_flag = DUERAPP_WORDS_200;      //0000 0000
                                                            //|  | ||||
                                                            //|  | |||\
                                                            //|  | ||\ 200
                                                            //|  | |\ 400
                                                            //|  | \ 600
                                                            //|  \  800
                                                            //\   1000
                                                            // ALL

/**********************
 *  GLOBAL VARIABLES
 **********************/
WINDOW_TABLE( MMI_DUERAPP_WRITING_GRADE_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_WRITING_GRADE_WIN_ID ),
    WIN_FUNC((uint32) grade_win_msg_handler ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),

    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_LABEL_CTRL_ID),
    // CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE1_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE1_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE2_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE3_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE4_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE5_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE6_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE7_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE8_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE9_CTRL_ID),

    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_WRITING_MAIN_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_WRITING_MAIN_WIN_ID ),
    WIN_FUNC((uint32) main_win_msg_handler ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),

    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID),
        CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID),
            CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN11_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, TRUE, MMI_DUERAPP_WRITING_MAIN_LABEL1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID),
            CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN12_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID),

        CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN13_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID),
        // CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, TRUE, MMI_DUERAPP_WRITING_MAIN_LABEL7_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID),

        CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_WRITING_MAIN_FORM1_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID),
            CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_WRITING_MAIN_FORM1_2_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_1_CTRL_ID),
                CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_WRITING_MAIN_LABEL2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_1_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_1_CTRL_ID),
                CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_WRITING_MAIN_LABEL3_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_1_CTRL_ID),
            CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_WRITING_MAIN_FORM1_2_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN3_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_2_CTRL_ID),
                CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_WRITING_MAIN_LABEL4_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_2_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN4_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_2_CTRL_ID),
                CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_WRITING_MAIN_LABEL5_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_2_CTRL_ID),

        CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_WRITING_MAIN_LABEL6_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID),
        
        CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_WRITING_MAIN_FORM1_3_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID),
            CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_3_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN5_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN6_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN7_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID),
            CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_3_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN8_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN9_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID),
                CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_WRITING_MAIN_BTN10_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID),

    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_WRITING_LIST_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_WRITING_LIST_WIN_ID ),
    WIN_FUNC((uint32) list_win_msg_handler ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),

    // CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_WRITING_LIST_FORM1_CTRL_ID),
    //     CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_WRITING_LIST_LABEL1_CTRL_ID, MMI_DUERAPP_WRITING_LIST_FORM1_CTRL_ID),
    //     CHILD_LIST_CTRL(FALSE, GUILIST_TEXTLIST_E, MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID, MMI_DUERAPP_WRITING_LIST_FORM1_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_LIST_LABEL1_CTRL_ID),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID),

    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_WRITING_VIP_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_WRITING_VIP_WIN_ID ),
    WIN_FUNC((uint32) vip_win_msg_handler ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),

    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_VIP_LABEL1_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_VIP_LABEL2_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_VIP_LABEL3_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_VIP_LABEL4_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID),

    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_WRITING_TRIAL_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_WRITING_TRIAL_WIN_ID ),
    WIN_FUNC((uint32) trial_win_msg_handler ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),

    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_TRIAL_LABEL1_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_TRIAL_LABEL2_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_TRIAL_LABEL3_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_TRIAL_BTN1_CTRL_ID),

    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_WRITING_FILTER_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_WRITING_FILTER_WIN_ID ),
    WIN_FUNC((uint32) filter_win_msg_handler ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),

    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_FILTER_LABEL_CTRL_ID),
    // CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_GRADE1_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_FILTER_WALL_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_FILTER_W200_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_FILTER_W400_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_FILTER_W600_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_FILTER_W800_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_FILTER_W1000_CTRL_ID),

    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_WRITING_CONTENT_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_WRITING_CONTENT_WIN_ID ),
    WIN_FUNC((uint32) content_win_msg_handler ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),

    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_WRITING_CONTENT_LABEL1_CTRL_ID),
    CREATE_TEXT_CTRL(MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID),
    // CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_WRITING_CONTENT_BTN1_CTRL_ID),

    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};


/**********************
 *   GLOBAL FUNCTIONS
***********************/
static void log(char level, char *tag, const char *fun, int line, const char *fmt, ...)
{
    char buf[512];
    int index = 0;
    va_list arg;
    va_start(arg, fmt);
    index = snprintf(buf, sizeof(buf), "%c [%s,%d] %s:", level, fun, line, tag);
    vsnprintf(buf + index, sizeof(buf) - index, fmt, arg);
    va_end(arg);
    // SCI_TRACE_HIGH("%s", buf);
    DUER_LOGE("%s", buf);
}

LOCAL GUI_RECT_T * duerapp_rect_create(GUI_RECT_T *rect, uint32 x, uint32 y, uint32 w, uint32 h)
{
    if (rect) {
        rect->left = x;
        rect->top = y;
        rect->right = x+w-1;
        rect->bottom = y+h-1;
    }
    return rect;
}

void duerapp_writing_init(void)
{
    duer_directive_list res[] = {
        {DUERAPP_DIRECTIVE_GET_CONF_INFO,   duer_dcs_get_conf_info_handler},
        {DUERAPP_DIRECTIVE_GET_GRADE_INFO,  duer_dcs_get_grade_info_handler},
        {DUERAPP_DIRECTIVE_UPD_GRADE_INFO,  duer_dcs_upd_grade_info_handler},
        {DUERAPP_DIRECTIVE_QUERY,           duer_dcs_query_handler},
        {DUERAPP_DIRECTIVE_GETCOMMENTINFO,  duer_dcs_get_comment_info_handler},
        {DUERAPP_DIRECTIVE_COMPOSITION,     duer_dcs_composition},
    };
    static bool is_initialized = FALSE;
    if (!is_initialized) {
        is_initialized = TRUE;
        LOGI(tag, "Initialize Xiaodu Writing...");
        LOGI(tag, "Registering directives");
        
        duer_add_dcs_directive_internal(res, sizeof(res) / sizeof(res[0]), DUERAPP_NAMESPACE_THIRDPARTY_WATCH);
    }
}

LOCAL void OpenVipCallBack(void)
{
    // 保险期间关闭当前页面，避免刷新不及时导致用户困扰
    MMK_CloseWin(MMI_DUERAPP_WRITING_VIP_WIN_ID);
    if (MMK_IsOpenWin(MMI_DUERAPP_WRITING_LIST_WIN_ID))
    {
        MMK_CloseWin(MMI_DUERAPP_WRITING_LIST_WIN_ID);
    }
    duer_send_link_click_url(LINKCLICK_BUY_VIP);
}


static int duerapp_writing_main_win_open(void)
{
    return MMK_CreateWin((uint32*)MMI_DUERAPP_WRITING_MAIN_WIN_TAB, PNULL);
}

static int duerapp_writing_openvip_win(void)
{
    return MMK_CreateWin((uint32*)MMI_DUERAPP_WRITING_VIP_WIN_TAB, PNULL);
}


void duerapp_writing_start(void)
{
    // 获取保存的年级信息
    int i = 0;
    char *grade = MMIDUERAPP_GetCompositionGrade();
    if (grade && strlen(grade) > 0) {
        for (i = 0;i < 8;i++) {
            if (strcmp(grade,s_grades_utf8[i]) == 0) {
                s_grade_selected = i;
                break;
            }
        }
        duerapp_writing_main_win_open();
    } else {
        duerapp_writing_grade_win_open();
    }

    duer_free_entities();
    s_entity_list = NULL;
    s_entity_selected = NULL;

    duer_free_comments();
}

void duerapp_writing_exit(void)
{
    duer_free_entities();
}

// free after using
wchar* duerapp_utf8_to_unicode(char *str_utf8)
{
    wchar *str_unicode = NULL;
    int utf8_len = strlen(str_utf8);
    str_unicode = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
    if (str_unicode) {
        GUI_UTF8ToWstr(str_unicode, utf8_len, str_utf8, utf8_len);
        // MMIAPICOM_Wstrcpy(s_last_user_name, str_unicode);
    } else {
        LOGE(tag, "Malloc failed");
    }
    return str_unicode;
}

#if 1   // grade selection window
static int duerapp_writing_grade_win_open(void)
{
    LOGI(tag, "Opening grade selection page");
    return MMK_CreateWin((uint32*)MMI_DUERAPP_WRITING_GRADE_WIN_TAB, PNULL);
}

LOCAL MMI_RESULT_E grade_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_POINT_T start_point = {0};

    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        LOGI(tag, "Open window %d", win_id);
        grade_win_full_paint();
        break;

    case MSG_GET_FOCUS:
        grade_win_full_paint();
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        LOGI(tag, "Cancle window %d", win_id);
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        LOGI(tag, "Close window %d", win_id);
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;
}

LOCAL void grade_btn_init(uint32 ctrl_id, GUI_RECT_T *rect, wchar *text)
{
    MMI_STRING_T str = {0};
    GUI_BG_T btn_bg_release = {0};
    GUI_BG_T btn_bg_press = {0};
    GUI_FONT_ALL_T font;
    // GUI_FONT_ALL_T font = 
    // {
    //     .font = WATCH_DEFAULT_NORMAL_FONT,
    //     .color = MMI_WHITE_COLOR,
    // };
    memset(&font, 0x00, sizeof(font));
    font.font = WATCH_DEFAULT_NORMAL_FONT;
    font.color = MMI_WHITE_COLOR;

    str.wstr_ptr = text;
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);

    btn_bg_release.bg_type = GUI_BG_COLOR;
    btn_bg_release.color = DUERAPP_COLOR_MAKE(42, 42, 49);

    btn_bg_press.bg_type = GUI_BG_COLOR;
    btn_bg_press.color = DUERAPP_COLOR_MAKE(100, 128, 250);

    GUIBUTTON_SetRect(ctrl_id, rect);
    GUIBUTTON_SetRunSheen(ctrl_id,FALSE);
    //GUIAPICTRL_SetState(ctrl_id, GUICTRL_STATE_TOPMOST, TRUE);
    GUIBUTTON_SetHandleLong(ctrl_id, TRUE);

    GUIBUTTON_SetBg(ctrl_id, &btn_bg_release);
    GUIBUTTON_SetPressedFg(ctrl_id, &btn_bg_press);
    
    GUIBUTTON_SetFont(ctrl_id, &font);
    GUIBUTTON_SetText(ctrl_id, str.wstr_ptr, str.wstr_len);

    // GUIBUTTON_SetCallBackFunc(ctrl_id, button_grade_press_handler);
    GUIBUTTON_SetCallBackFuncExt(ctrl_id, grade_win_button_press_handler);
}

LOCAL void grade_win_bg_paint(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
}

LOCAL void grade_win_full_paint(void)
{
    uint32 ctrl_id;
    GUI_RECT_T rect;
    GUI_RECT_T rect1 = {0,0,240,30};
    MMI_STRING_T querySting = {0};
    GUI_BG_T btn_bg_press = {0};
    // uint32 ctrl_id = 0;
    // SCI_TRACE_HIGH("grade_win_full_paint");
    DUER_LOGE("grade_win_full_paint");

    grade_win_bg_paint();

    {
        ctrl_id = MMI_DUERAPP_WRITING_LABEL_CTRL_ID;

        GUILABEL_SetRect(ctrl_id, &rect1, FALSE);
        GUILABEL_SetFont(ctrl_id, WATCH_DEFAULT_NORMAL_FONT, DUERAPP_COLOR_MAKE(255, 255, 255));

        querySting.wstr_ptr = L"请选择年级";
        querySting.wstr_len = MMIAPICOM_Wstrlen(querySting.wstr_ptr);
        GUILABEL_SetText(ctrl_id ,&querySting, FALSE);
    }

    grade_btn_init(MMI_DUERAPP_WRITING_GRADE1_CTRL_ID, duerapp_rect_create(&rect, 5, 34, 110, 35),      s_grades_unicode[DUERAPP_GRADE1]);
    grade_btn_init(MMI_DUERAPP_WRITING_GRADE2_CTRL_ID, duerapp_rect_create(&rect, 126, 34, 110, 35),    s_grades_unicode[DUERAPP_GRADE2]);
    grade_btn_init(MMI_DUERAPP_WRITING_GRADE3_CTRL_ID, duerapp_rect_create(&rect, 5, 74, 110, 35),      s_grades_unicode[DUERAPP_GRADE3]);
    grade_btn_init(MMI_DUERAPP_WRITING_GRADE4_CTRL_ID, duerapp_rect_create(&rect, 126, 74, 110, 35),    s_grades_unicode[DUERAPP_GRADE4]);
    grade_btn_init(MMI_DUERAPP_WRITING_GRADE5_CTRL_ID, duerapp_rect_create(&rect, 5, 114, 110, 35),     s_grades_unicode[DUERAPP_GRADE5]);
    grade_btn_init(MMI_DUERAPP_WRITING_GRADE6_CTRL_ID, duerapp_rect_create(&rect, 126, 114, 110, 35),   s_grades_unicode[DUERAPP_GRADE6]);
    grade_btn_init(MMI_DUERAPP_WRITING_GRADE7_CTRL_ID, duerapp_rect_create(&rect, 5, 154, 110, 35),     s_grades_unicode[DUERAPP_GRADE7]);
    grade_btn_init(MMI_DUERAPP_WRITING_GRADE8_CTRL_ID, duerapp_rect_create(&rect, 126, 154, 110, 35),   s_grades_unicode[DUERAPP_GRADE8]);
    grade_btn_init(MMI_DUERAPP_WRITING_GRADE9_CTRL_ID, duerapp_rect_create(&rect, 5, 194, 110, 35),     s_grades_unicode[DUERAPP_GRADE9]);

    if (s_grade_selected != DUERAPP_GRADE_MAX) {
        ctrl_id = MMI_DUERAPP_WRITING_GRADE1_CTRL_ID + (s_grade_selected - DUERAPP_GRADE1);
        btn_bg_press.bg_type = GUI_BG_COLOR;
        btn_bg_press.color = DUERAPP_COLOR_MAKE(100, 128, 250);
        GUIBUTTON_SetBg(ctrl_id, &btn_bg_press);
        GUIBUTTON_SetPressedFg(ctrl_id, &btn_bg_press);
    }
}

LOCAL MMI_RESULT_E grade_win_button_press_handler(MMI_HANDLE_T ctrl_handle)
{
    duerapp_grade_t grade = DUERAPP_GRADE_MAX;
    uint32 ctrl_id;

    GUI_BG_T btn_bg_release = {0};
    GUI_BG_T btn_bg_press = {0};

    LOGI(tag, "0x%.8X", (uint32)ctrl_handle);
    btn_bg_release.bg_type = GUI_BG_COLOR;
    btn_bg_release.color = DUERAPP_COLOR_MAKE(42, 42, 49);

    btn_bg_press.bg_type = GUI_BG_COLOR;
    btn_bg_press.color = DUERAPP_COLOR_MAKE(100, 128, 250);

    for (ctrl_id = MMI_DUERAPP_WRITING_GRADE1_CTRL_ID; ctrl_id <= MMI_DUERAPP_WRITING_GRADE9_CTRL_ID; ctrl_id++) {
        if (ctrl_handle == MMK_ConvertIdToHandle(ctrl_id)) {
            grade = DUERAPP_GRADE1 + (ctrl_id - MMI_DUERAPP_WRITING_GRADE1_CTRL_ID);
            break;
        }
    }

    if (s_grade_selected != grade) {
        if (s_grade_selected != DUERAPP_GRADE_MAX) {
            ctrl_id = MMI_DUERAPP_WRITING_GRADE1_CTRL_ID + (s_grade_selected - DUERAPP_GRADE1);
            GUIBUTTON_SetBg(ctrl_id, &btn_bg_release);
            GUIBUTTON_SetPressedFg(ctrl_id, &btn_bg_press);
            GUIBUTTON_Update(ctrl_id);
        }

        s_grade_selected = grade;
        if (s_grade_selected != DUERAPP_GRADE_MAX) {
            LOGI(tag, "Selected grade is:%d", s_grade_selected);
            ctrl_id = MMI_DUERAPP_WRITING_GRADE1_CTRL_ID + (s_grade_selected - DUERAPP_GRADE1);
            GUIBUTTON_SetBg(ctrl_id, &btn_bg_press);
            GUIBUTTON_SetPressedFg(ctrl_id, &btn_bg_press);
            GUIBUTTON_Update(ctrl_id);
            duer_set_grade_info(s_grade_selected);
            MMK_CloseWin(MMI_DUERAPP_WRITING_GRADE_WIN_ID);
        } else {
            LOGI(tag, "Selected grade is INVALID");
        }
    }

    return MMI_RESULT_TRUE;
}

#endif

#if 1   // main window
LOCAL MMI_RESULT_E main_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_POINT_T start_point = {0};
    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        LOGI(tag, "%s[duer]:Open window %d", __func__, win_id);
        main_win_full_paint();
        // 进入作文后需要暂停音乐播放，不然影响dcs交互
        duer_audio_play_pause();
        // 配置asr语音，防止开机直接进入小度作文，进语音查询点击说话后提示失败
        duer_refresh_bds_asr_config();
        // bdsc_session_asr_set_fun_value(1);
        duer_statistics_time_count_start(STATISTICS_TIME_TYPE_WRITING);
        break;

    case MSG_GET_FOCUS:
        main_win_full_paint();
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        LOGI(tag, "Close window %d", win_id);
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        duer_statistics_time_count_stop(STATISTICS_TIME_TYPE_WRITING);
        break;
    case MSG_DUERAPP_GOT_COMPLIST: {
        duerapp_writing_list_win_open();
        break;
    }
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;

}

LOCAL void main_win_label_init(uint32 ctrl_id, GUI_RECT_T *rect, wchar *text, uint32 font)
{
    MMI_STRING_T str = {0};
    // GUILABEL_SetRect(ctrl_id, rect, FALSE);
    GUILABEL_SetFont(ctrl_id, font, DUERAPP_COLOR_MAKE(255, 255, 255));

    str.wstr_ptr = text;
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUILABEL_SetText(ctrl_id ,&str, FALSE);
}

LOCAL void main_win_form_init(uint32 ctrl_id, uint16 color)
{
    GUI_BG_T form_bg = {0};
    form_bg.bg_type = GUI_BG_COLOR;
    form_bg.color = color;
    GUIFORM_SetBg(ctrl_id, &form_bg);
}

LOCAL void main_win_icon_init(uint32 ctrl_id, uint32 img)
{
    // MMI_STRING_T str = {0};
    // str.wstr_ptr = text;
    // str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);

    GUI_BG_T btn_bg_rel = {0};
    GUI_BG_T btn_bg_press = {0};

    btn_bg_rel.bg_type = GUI_BG_IMG;
    btn_bg_rel.img_id = img;
    GUIBUTTON_SetBg(ctrl_id, &btn_bg_rel);

    btn_bg_press.bg_type = GUI_BG_IMG;
    btn_bg_press.img_id = img;
    GUIBUTTON_SetPressedFg(ctrl_id, &btn_bg_press);

    // GUIBUTTON_SetRect(ctrl_id, rect);
    GUIBUTTON_SetRunSheen(ctrl_id,FALSE);
    //GUIAPICTRL_SetState(ctrl_id, GUICTRL_STATE_TOPMOST, TRUE);
    GUIBUTTON_SetHandleLong(ctrl_id, TRUE);

    // GUIBUTTON_SetText(ctrl_id, str.wstr_ptr, str.wstr_len);

    // GUIBUTTON_SetCallBackFunc(ctrl_id, button_grade_press_handler);
    GUIBUTTON_SetCallBackFuncExt(ctrl_id, main_win_button_press_handler);
}

LOCAL void main_win_btn_init(uint32 ctrl_id, wchar *text)
{
    MMI_STRING_T str = {0};
    GUI_BG_T btn_bg_release = {0};
    GUI_BG_T btn_bg_press = {0};
    // GUI_FONT_ALL_T font = 
    // {
    //     .font = WATCH_DEFAULT_BIG_FONT,
    //     .color = DUERAPP_COLOR_MAKE(255, 255, 255),
    // };
    GUI_FONT_ALL_T font;
    memset(&font, 0x00, sizeof(font));
    font.font = WATCH_DEFAULT_BIG_FONT;
    font.color = DUERAPP_COLOR_MAKE(255, 255, 255);

    str.wstr_ptr = text;
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);

    btn_bg_release.bg_type = GUI_BG_COLOR;
    btn_bg_release.color = DUERAPP_COLOR_MAKE(100, 128, 250);

    btn_bg_press.bg_type = GUI_BG_COLOR;
    btn_bg_press.color = DUERAPP_COLOR_MAKE(42, 42, 49);

    // GUIBUTTON_SetRect(ctrl_id, rect);
    GUIBUTTON_SetRunSheen(ctrl_id,FALSE);
    //GUIAPICTRL_SetState(ctrl_id, GUICTRL_STATE_TOPMOST, TRUE);
    GUIBUTTON_SetHandleLong(ctrl_id, TRUE);

    GUIBUTTON_SetBg(ctrl_id, &btn_bg_release);
    GUIBUTTON_SetPressedFg(ctrl_id, &btn_bg_press);

    GUIBUTTON_SetText(ctrl_id, str.wstr_ptr, str.wstr_len);
    
    GUIBUTTON_SetFont(ctrl_id, &font);
    // GUIBUTTON_SetCallBackFunc(ctrl_id, button_grade_press_handler);
    GUIBUTTON_SetCallBackFuncExt(ctrl_id, main_win_button_press_handler);
}

LOCAL void main_win_full_paint(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    uint32 ctrl_id;
    GUI_RECT_T rect1;
    GUI_BG_T form_bg = {0};
    GUIFORM_CHILD_WIDTH_T width;
    GUIFORM_CHILD_WIDTH_T height;

    uint16 hor_space = 0;
    uint16 ver_space = 10;
    // GUIFORM_CHILD_WIDTH_T width = 
    // {
    //     .add_data = 240 - 90,
    //     .type = GUIFORM_CHILD_WIDTH_FIXED,
    // };

    memset(&width, 0x00, sizeof(width));
    width.add_data = 240 - 90;
    width.type = GUIFORM_CHILD_WIDTH_FIXED;

    // GUIFORM_CHILD_WIDTH_T height = 
    // {
    //     .add_data = 43,
    //     .type = GUIFORM_CHILD_WIDTH_FIXED,
    // };
    memset(&height, 0x00, sizeof(height));
    height.add_data = 43;
    height.type = GUIFORM_CHILD_WIDTH_FIXED;

    LOGI(tag, "");

    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);

    form_bg.bg_type = GUI_BG_COLOR;

    GUIFORM_PermitChildBg(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, FALSE);     // 支持子控件背景
    GUIFORM_PermitChildFont(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, FALSE);   // 支持子控件字体
    GUIFORM_PermitChildBorder(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, FALSE);

    main_win_form_init(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID,      RGB8882RGB565(DUERAPP_COLOR14));
    main_win_form_init(MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID,    RGB8882RGB565(DUERAPP_COLOR14));
    main_win_form_init(MMI_DUERAPP_WRITING_MAIN_FORM1_2_CTRL_ID,    RGB8882RGB565(DUERAPP_COLOR14));
    main_win_form_init(MMI_DUERAPP_WRITING_MAIN_FORM1_2_1_CTRL_ID,  RGB8882RGB565(DUERAPP_COLOR14));
    main_win_form_init(MMI_DUERAPP_WRITING_MAIN_FORM1_2_2_CTRL_ID,  RGB8882RGB565(DUERAPP_COLOR14));
    main_win_form_init(MMI_DUERAPP_WRITING_MAIN_FORM1_3_CTRL_ID,    RGB8882RGB565(DUERAPP_COLOR14));
    main_win_form_init(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID,  RGB8882RGB565(DUERAPP_COLOR14));
    main_win_form_init(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID,  RGB8882RGB565(DUERAPP_COLOR14));

    // main title
    {
        

        main_win_label_init(MMI_DUERAPP_WRITING_MAIN_LABEL1_CTRL_ID, duerapp_rect_create(&rect, 5, 154, 110, 35), L"小度作文", WATCH_DEFAULT_BIG_FONT);
        main_win_icon_init(MMI_DUERAPP_WRITING_MAIN_BTN11_CTRL_ID, IMAGE_DUERAPP_WRITING_SETTINGS);
        main_win_icon_init(MMI_DUERAPP_WRITING_MAIN_BTN12_CTRL_ID, IMAGE_DUERAPP_WRITING_FILTER);
        GUIFORM_SetAlign(MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);
        GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_LABEL1_CTRL_ID, &width);
        width.add_data = 40;
        GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN11_CTRL_ID, &width);
        width.add_data = 40;
        GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN12_CTRL_ID, &width);
    }

    main_win_label_init(MMI_DUERAPP_WRITING_MAIN_LABEL2_CTRL_ID, duerapp_rect_create(&rect, 40, 59, 110, 35), s_literary_styles_unicode[DUERAPP_LITERARY_STYLE_NARRATIVE], WATCH_DEFAULT_NORMAL_FONT);
    main_win_label_init(MMI_DUERAPP_WRITING_MAIN_LABEL3_CTRL_ID, duerapp_rect_create(&rect, 131, 59, 110, 35), s_literary_styles_unicode[DUERAPP_LITERARY_STYLE_THOUGHTS], WATCH_DEFAULT_NORMAL_FONT);
    main_win_label_init(MMI_DUERAPP_WRITING_MAIN_LABEL4_CTRL_ID, duerapp_rect_create(&rect, 40, 117, 110, 35), s_literary_styles_unicode[DUERAPP_LITERARY_STYLE_ARGUMENTATION], WATCH_DEFAULT_NORMAL_FONT);
    main_win_label_init(MMI_DUERAPP_WRITING_MAIN_LABEL5_CTRL_ID, duerapp_rect_create(&rect, 131, 117, 110, 35), s_literary_styles_unicode[DUERAPP_LITERARY_STYLE_LETTER], WATCH_DEFAULT_NORMAL_FONT);

    main_win_label_init(MMI_DUERAPP_WRITING_MAIN_LABEL6_CTRL_ID, duerapp_rect_create(&rect, 0, 154, 240, 35), L"热门分类", WATCH_DEFAULT_BIG_FONT);

    main_win_icon_init(MMI_DUERAPP_WRITING_MAIN_BTN1_CTRL_ID, IMAGE_DUERAPP_WRITING_NARRATIVE);
    main_win_icon_init(MMI_DUERAPP_WRITING_MAIN_BTN2_CTRL_ID, IMAGE_DUERAPP_WRITING_THOUGHTS);
    main_win_icon_init(MMI_DUERAPP_WRITING_MAIN_BTN3_CTRL_ID, IMAGE_DUERAPP_WRITING_ARGUMENTATION);
    main_win_icon_init(MMI_DUERAPP_WRITING_MAIN_BTN4_CTRL_ID, IMAGE_DUERAPP_WRITING_LETTER);

    main_win_btn_init(MMI_DUERAPP_WRITING_MAIN_BTN5_CTRL_ID,    s_hotwords_unicode[DUERAPP_HOTWORD_SCENERY]);
    main_win_btn_init(MMI_DUERAPP_WRITING_MAIN_BTN6_CTRL_ID,    s_hotwords_unicode[DUERAPP_HOTWORD_FIGURE]);
    main_win_btn_init(MMI_DUERAPP_WRITING_MAIN_BTN7_CTRL_ID,    s_hotwords_unicode[DUERAPP_HOTWORD_GROWTH]);
    main_win_btn_init(MMI_DUERAPP_WRITING_MAIN_BTN8_CTRL_ID,    s_hotwords_unicode[DUERAPP_HOTWORD_ANIMALS]);
    main_win_btn_init(MMI_DUERAPP_WRITING_MAIN_BTN9_CTRL_ID,    s_hotwords_unicode[DUERAPP_HOTWORD_NATURE]);
    main_win_btn_init(MMI_DUERAPP_WRITING_MAIN_BTN10_CTRL_ID,   s_hotwords_unicode[DUERAPP_HOTWORD_EVENTS]);

    GUIFORM_SetChildAlign(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN5_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);
    GUIFORM_SetChildAlign(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN6_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);
    GUIFORM_SetChildAlign(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN7_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);

    GUIFORM_SetChildAlign(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN8_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);
    GUIFORM_SetChildAlign(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN9_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);
    GUIFORM_SetChildAlign(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN10_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);

    // GUIFORM_CHILD_WIDTH_T width = 
    // {
    //     .add_data = 104,
    //     .type = GUIFORM_CHILD_WIDTH_FIXED,
    // };
    memset(&width, 0x00, sizeof(width));
    width.add_data = 104;
    width.type = GUIFORM_CHILD_WIDTH_FIXED;

    GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN5_CTRL_ID, &width);
    GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN6_CTRL_ID, &width);
    GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN7_CTRL_ID, &width);

    GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN8_CTRL_ID, &width);
    GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN9_CTRL_ID, &width);
    GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN10_CTRL_ID, &width);


    GUIFORM_SetChildHeight(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN5_CTRL_ID, &height);
    GUIFORM_SetChildHeight(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN6_CTRL_ID, &height);
    GUIFORM_SetChildHeight(MMI_DUERAPP_WRITING_MAIN_FORM1_3_1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN7_CTRL_ID, &height);

    GUIFORM_SetChildHeight(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN8_CTRL_ID, &height);
    GUIFORM_SetChildHeight(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN9_CTRL_ID, &height);
    GUIFORM_SetChildHeight(MMI_DUERAPP_WRITING_MAIN_FORM1_3_2_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN10_CTRL_ID, &height);

    main_win_btn_init(MMI_DUERAPP_WRITING_MAIN_BTN13_CTRL_ID, L"语音查询");
    // main_win_icon_init(MMI_DUERAPP_WRITING_MAIN_BTN13_CTRL_ID, IMAGE_DUERAPP_TITLE);
    GUIFORM_SetChildSpace(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN13_CTRL_ID, &hor_space, &ver_space);
    width.add_data = 220;
    GUIFORM_SetChildWidth(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN13_CTRL_ID, &width);
    GUIFORM_SetChildHeight(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN13_CTRL_ID, &height);
    GUIFORM_SetChildAlign(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_BTN13_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);

    GUIFORM_SetChildSpace(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_FORM1_2_CTRL_ID, &hor_space, &ver_space);

    // hor_space = 0;
    // ver_space = 10;
    // main_win_label_init(MMI_DUERAPP_WRITING_MAIN_LABEL7_CTRL_ID, duerapp_rect_create(&rect, 40, 59, 110, 35), L"锟斤拷锟斤拷锟斤拷询", WATCH_DEFAULT_NORMAL_FONT);
    // GUIFORM_SetChildSpace(MMI_DUERAPP_WRITING_MAIN_FORM1_CTRL_ID, MMI_DUERAPP_WRITING_MAIN_LABEL7_CTRL_ID, &hor_space, &ver_space);

    MMK_SetAtvCtrl(MMI_DUERAPP_WRITING_MAIN_WIN_ID, MMI_DUERAPP_WRITING_MAIN_LABEL1_CTRL_ID);
}

LOCAL MMI_RESULT_E main_win_button_press_handler(MMI_HANDLE_T ctrl_handle)
{
    LOGI(tag, "0x%.8X", (uint32)ctrl_handle);

    duer_free_entities();

    if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN1_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_NARRATIVE, DUERAPP_HOTWORD_BLANK, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN2_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_THOUGHTS, DUERAPP_HOTWORD_BLANK, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN3_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_ARGUMENTATION, DUERAPP_HOTWORD_BLANK, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN4_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_LETTER, DUERAPP_HOTWORD_BLANK, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN5_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_BLANK, DUERAPP_HOTWORD_SCENERY, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN6_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_BLANK, DUERAPP_HOTWORD_FIGURE, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN7_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_BLANK, DUERAPP_HOTWORD_GROWTH, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN8_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_BLANK, DUERAPP_HOTWORD_ANIMALS, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN9_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_BLANK, DUERAPP_HOTWORD_NATURE, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN10_CTRL_ID)) {
        duer_query(s_grade_selected, DUERAPP_LITERARY_STYLE_BLANK, DUERAPP_HOTWORD_EVENTS, s_words_limit);
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN11_CTRL_ID)) {
        duerapp_writing_grade_win_open();
        return MMI_RESULT_TRUE;
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN12_CTRL_ID)) {
        duerapp_filter_win_open();
        return MMI_RESULT_TRUE;
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_MAIN_BTN13_CTRL_ID)) {
        LOGI(tag, "(%s)[duer]:Creating writing'win", __func__);
        MMIDUERAPP_StartNormalAsrSet(); //先设置回normal
        duerapp_set_interact_mode(DUERAPP_INTERACT_MODE_WRITING);
        MMIDUERAPP_CreateDuerMainWin(FALSE);
        return MMI_RESULT_TRUE;
    } else {
    }

    duerapp_show_toast(L"请稍等");
    return MMI_RESULT_TRUE;
}

#endif

#if 1   // list window
LOCAL MMI_RESULT_E list_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_POINT_T start_point = {0};

    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID;
    int idx = 0;
    duerapp_entity_t *entity = NULL;
    wchar *wstr_ptr = L"跳转中...";
    DUER_USER_INFO *info = NULL;

    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        LOGI(tag, "Open window %d", win_id);
        list_win_full_paint();
        break;

    case MSG_GET_FOCUS:
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_DUERAPP_GOT_COMMENT:
    {
        duerapp_writing_content_win_open();
        break;
    }

    case MSG_CTL_PENOK:
    {
        idx = GUILIST_GetCurItemIndex(ctrl_id);
        LOGI(tag, "List item id:%d", idx);
        entity = duer_get_entity(idx);
        if (entity) {
            LOGI(tag, "Got entity");
            if (!duerapp_is_login()) {
                LOGI(tag, "Please login or vip");
                #if 0
                duerapp_user_login();
                #else
                //支锟斤拷锟斤拷锟铰?
                duerapp_show_toast(wstr_ptr);
                duer_send_link_click_url(LINKCLICK_BUY_VIP);
                #endif
            } else {
                info = duerapp_get_user_info();
                if (info && info->is_vip) {
                    LOGI(tag, "VIP");
                    s_entity_selected = entity;
                    duerapp_show_toast(L"请稍等");
                    duer_get_comment(s_entity_selected->id);
                } else {
                    LOGI(tag, "Not VIP");
                    duerapp_writing_openvip_win();
                    // duer_send_link_click_url(LINKCLICK_BUY_VIP);
                }
            }

        } else {
            LOGE(tag, "Failed to get entity");
        }
        break;
    }

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        LOGI(tag, "Cancle window %d", win_id);
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        LOGI(tag, "Close window %d", win_id);
        duer_free_entities();
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;
}

LOCAL void list_win_list_add_item(wchar *title, wchar *count, wchar *content)
{
    GUILIST_ITEM_T item_info = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T string = {0};

    item_info.item_style    = GUIITEM_STYLE_3STR;
    item_info.item_data_ptr = &item_data;
    item_info.user_data     = NULL;

    string.wstr_ptr = title;
    string.wstr_len = MMIAPICOM_Wstrlen(string.wstr_ptr);
    item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[0].item_data.text_buffer = string;

    string.wstr_ptr = count;
    string.wstr_len = MMIAPICOM_Wstrlen(string.wstr_ptr);
    item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[1].item_data.text_buffer = string;
    // item_data.item_content[2].bg_color_id = MMITHEME_COLOR_3;

    string.wstr_ptr = content;
    string.wstr_len = MMIAPICOM_Wstrlen(string.wstr_ptr);
    item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[2].item_data.text_buffer = string;
    // item_data.item_content[2].bg_color_id = MMITHEME_COLOR_BLUE;

    GUILIST_AppendItem(MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID, &item_info);
}

LOCAL void list_win_full_paint(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    uint32 ctrl_id = 0;
    GUI_BG_T form_bg = {0};
    GUI_RECT_T rect1 = DUERAPP_RECT(0, 0, 240, 30);
    MMI_STRING_T str = {0};
    GUI_RECT_T rect2 = DUERAPP_RECT(0, 30, 240, 210);
    duerapp_entity_t *p = NULL;
    char length_utf8[32];
    char content_utf8[64];

    wchar *name_uni = NULL;
    wchar *length_uni = NULL;
    wchar *content_uni = NULL;


    LCD_FillRect(&lcd_dev_info, rect, DUERAPP_COLOR_MAKE(0, 0, 0));

    form_bg.bg_type = GUI_BG_COLOR;
    form_bg.color = DUERAPP_COLOR_MAKE(100, 255, 255);

    GUILABEL_SetRect(MMI_DUERAPP_WRITING_LIST_LABEL1_CTRL_ID, &rect1, FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_WRITING_LIST_LABEL1_CTRL_ID, WATCH_DEFAULT_BIG_FONT, DUERAPP_COLOR_MAKE(255, 255, 255));
    str.wstr_ptr = L"小度作文";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_WRITING_LIST_LABEL1_CTRL_ID ,&str, FALSE);

    GUILIST_SetBgColor(MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID, DUERAPP_COLOR_MAKE(0, 0, 0));
    GUILIST_SetRect(MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID, &rect2);
    GUILIST_SetMaxItem(MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID, 100, FALSE); 
    GUILIST_SetTextFont(MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID, WATCH_DEFAULT_NORMAL_FONT, DUERAPP_COLOR_MAKE(255, 255, 255));
    GUILIST_SetListState(MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID, GUILIST_STATE_SPLIT_LINE, FALSE);
    GUILIST_SetNeedHiLightBar(MMI_DUERAPP_WRITING_LIST_LIST_CTRL_ID, FALSE);


    p = s_entity_list;
    while (p) {
        memset(length_utf8, 0x00, sizeof(length_utf8));
        snprintf(length_utf8, sizeof(length_utf8), "%d字", p->length);
        LOGI(tag, "length:%s", length_utf8);

        memset(content_utf8, 0x00, sizeof(content_utf8));

        snprintf(content_utf8, sizeof(content_utf8), "%s", p->content);

        name_uni = duerapp_utf8_to_unicode(p->name);
        length_uni = duerapp_utf8_to_unicode(length_utf8);
        content_uni = duerapp_utf8_to_unicode(content_utf8);

        if (name_uni && length_uni && content_uni) {
            list_win_list_add_item(name_uni, length_uni, content_uni);
            p = p->next;
        }
        if (name_uni) {
            DUER_FREE(name_uni);
        }
        if (length_uni) {
            DUER_FREE(length_uni);
        }
        if (content_uni) {
            DUER_FREE(content_uni);
        }
    }

}

static int duerapp_writing_list_win_open(void)
{
    LOGI(tag, "Opening list page");
    return MMK_CreateWin((uint32*)MMI_DUERAPP_WRITING_LIST_WIN_TAB, PNULL);
}

#endif 

#if 1   // filter window
LOCAL MMI_RESULT_E filter_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_POINT_T start_point = {0};

    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        LOGI(tag, "Open window %d", win_id);
        filter_win_full_paint();
        break;

    case MSG_GET_FOCUS:
        filter_win_full_paint();
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        LOGI(tag, "Close window %d", win_id);
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        LOGI(tag, "Close window %d", win_id);
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;

}

LOCAL void filter_win_btn_init(uint32 ctrl_id, GUI_RECT_T *rect, wchar *text)
{
    MMI_STRING_T str = {0};
    GUI_BG_T btn_bg_release = {0};
    GUI_BG_T btn_bg_press = {0};
    // GUI_FONT_ALL_T font = 
    // {
    //     .font = WATCH_DEFAULT_NORMAL_FONT,
    //     .color = MMI_WHITE_COLOR,
    // };
    GUI_FONT_ALL_T font;
    memset(&font, 0x00, sizeof(font));
    font.font = WATCH_DEFAULT_NORMAL_FONT;
    font.color = MMI_WHITE_COLOR;

    str.wstr_ptr = text;
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);

    btn_bg_release.bg_type = GUI_BG_COLOR;
    btn_bg_release.color = DUERAPP_COLOR_MAKE(42, 42, 49);

    btn_bg_press.bg_type = GUI_BG_COLOR;
    btn_bg_press.color = DUERAPP_COLOR_MAKE(100, 128, 250);

    GUIBUTTON_SetRect(ctrl_id, rect);
    GUIBUTTON_SetRunSheen(ctrl_id,FALSE);
    //GUIAPICTRL_SetState(ctrl_id, GUICTRL_STATE_TOPMOST, TRUE);
    GUIBUTTON_SetHandleLong(ctrl_id, TRUE);

    GUIBUTTON_SetBg(ctrl_id, &btn_bg_release);
    GUIBUTTON_SetPressedFg(ctrl_id, &btn_bg_press);
    
    GUIBUTTON_SetFont(ctrl_id, &font);
    GUIBUTTON_SetText(ctrl_id, str.wstr_ptr, str.wstr_len);

    // GUIBUTTON_SetCallBackFunc(ctrl_id, button_grade_press_handler);
    GUIBUTTON_SetCallBackFuncExt(ctrl_id, filter_win_button_press_handler);
}



static void filter_win_hilight_selected(void)
{
    GUI_BG_T btn_bg_selected = {0};
    GUI_BG_T btn_bg_diselected = {0};

    btn_bg_selected.bg_type = GUI_BG_COLOR;
    btn_bg_selected.color = DUERAPP_COLOR_MAKE(100, 128, 250);

    btn_bg_diselected.bg_type = GUI_BG_COLOR;
    btn_bg_diselected.color = DUERAPP_COLOR_MAKE(42, 42, 49);

    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_WALL_CTRL_ID, &btn_bg_diselected);
    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W200_CTRL_ID, &btn_bg_diselected);
    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W400_CTRL_ID, &btn_bg_diselected);
    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W200_CTRL_ID, &btn_bg_diselected);
    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W600_CTRL_ID, &btn_bg_diselected);
    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W800_CTRL_ID, &btn_bg_diselected);
    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W1000_CTRL_ID, &btn_bg_diselected);

    if (s_words_limit == 0) {
        GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_WALL_CTRL_ID, &btn_bg_selected);
    } else if (s_words_limit == 200) {
        GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W200_CTRL_ID, &btn_bg_selected);
    } else if (s_words_limit == 400) {
        GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W400_CTRL_ID, &btn_bg_selected);
    } else if (s_words_limit == 600) {
        GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W600_CTRL_ID, &btn_bg_selected);
    } else if (s_words_limit == 800) {
        GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W800_CTRL_ID, &btn_bg_selected);
    } else if (s_words_limit == 1000) {
        GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_FILTER_W1000_CTRL_ID, &btn_bg_selected);
    }

    GUIBUTTON_Update(MMI_DUERAPP_WRITING_FILTER_WALL_CTRL_ID);
    GUIBUTTON_Update(MMI_DUERAPP_WRITING_FILTER_W200_CTRL_ID);
    GUIBUTTON_Update(MMI_DUERAPP_WRITING_FILTER_W400_CTRL_ID);
    GUIBUTTON_Update(MMI_DUERAPP_WRITING_FILTER_W200_CTRL_ID);
    GUIBUTTON_Update(MMI_DUERAPP_WRITING_FILTER_W600_CTRL_ID);
    GUIBUTTON_Update(MMI_DUERAPP_WRITING_FILTER_W800_CTRL_ID);
    GUIBUTTON_Update(MMI_DUERAPP_WRITING_FILTER_W1000_CTRL_ID);

}

LOCAL void filter_win_full_paint(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    uint32 ctrl_id = 0;
    GUI_RECT_T rect1 = {0,0,240,30};
    MMI_STRING_T querySting = {0};

    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);

    {
        ctrl_id = MMI_DUERAPP_WRITING_FILTER_LABEL_CTRL_ID;

        GUILABEL_SetRect(ctrl_id, &rect1, FALSE);
        GUILABEL_SetFont(ctrl_id, WATCH_DEFAULT_NORMAL_FONT, DUERAPP_COLOR_MAKE(255, 255, 255));

        querySting.wstr_ptr = L"请选择字数";
        querySting.wstr_len = MMIAPICOM_Wstrlen(querySting.wstr_ptr);
        GUILABEL_SetText(ctrl_id ,&querySting, FALSE);
    }

    filter_win_btn_init(MMI_DUERAPP_WRITING_FILTER_WALL_CTRL_ID,    duerapp_rect_create(&rect, 5, 34, 110, 35),     L"全锟斤拷");
    filter_win_btn_init(MMI_DUERAPP_WRITING_FILTER_W200_CTRL_ID,    duerapp_rect_create(&rect, 126, 34, 110, 35),   L"200锟斤拷");
    filter_win_btn_init(MMI_DUERAPP_WRITING_FILTER_W400_CTRL_ID,    duerapp_rect_create(&rect, 5, 74, 110, 35),     L"400锟斤拷");
    filter_win_btn_init(MMI_DUERAPP_WRITING_FILTER_W600_CTRL_ID,    duerapp_rect_create(&rect, 126, 74, 110, 35),   L"600锟斤拷");
    filter_win_btn_init(MMI_DUERAPP_WRITING_FILTER_W800_CTRL_ID,    duerapp_rect_create(&rect, 5, 114, 110, 35),    L"800锟斤拷");
    filter_win_btn_init(MMI_DUERAPP_WRITING_FILTER_W1000_CTRL_ID,   duerapp_rect_create(&rect, 126, 114, 110, 35),  L"1000锟斤拷");

    filter_win_hilight_selected();
}

LOCAL MMI_RESULT_E filter_win_button_press_handler(MMI_HANDLE_T ctrl_handle)
{
    if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_FILTER_WALL_CTRL_ID)) {
        s_words_limit = 0;
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_FILTER_W200_CTRL_ID)) {
        s_words_limit = 200;
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_FILTER_W400_CTRL_ID)) {
        s_words_limit = 400;
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_FILTER_W600_CTRL_ID)) {
        s_words_limit = 600;
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_FILTER_W800_CTRL_ID)) {
        s_words_limit = 800;
    } else if (ctrl_handle == MMK_ConvertIdToHandle(MMI_DUERAPP_WRITING_FILTER_W1000_CTRL_ID)) {
        s_words_limit = 1000;
    }

    LOGI(tag, "s_words_limit:%d", s_words_limit);
    filter_win_hilight_selected();

    MMK_CloseWin(MMI_DUERAPP_WRITING_FILTER_WIN_ID);

    return MMI_RESULT_TRUE;
}

static int duerapp_filter_win_open(void)
{
    LOGI(tag, "Opening filter page");
    return MMK_CreateWin((uint32*)MMI_DUERAPP_WRITING_FILTER_WIN_TAB, PNULL);
}
#endif

#if 1   // vip window
LOCAL MMI_RESULT_E vip_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_POINT_T start_point = {0};

    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        LOGI(tag, "Open window %d", win_id);
        vip_win_full_paint();
        break;

    case MSG_GET_FOCUS:
        vip_win_full_paint();
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        LOGI(tag, "Close window %d", win_id);
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;

}

LOCAL void vip_win_full_paint(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    MMI_STRING_T str = {0};

    GUI_BG_T btn_bg_release = {0};
    GUI_BG_T btn_bg_press = {0};
    // GUI_FONT_ALL_T font = 
    // {
    //     .font = WATCH_DEFAULT_BIG_FONT,
    //     .color = DUERAPP_COLOR_MAKE(255, 255, 255),
    // };
    GUI_FONT_ALL_T font;
    memset(&font, 0x00, sizeof(font));
    font.font = WATCH_DEFAULT_BIG_FONT;
    font.color = DUERAPP_COLOR_MAKE(255, 255, 255);

    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);


    GUILABEL_SetRect(MMI_DUERAPP_WRITING_VIP_LABEL1_CTRL_ID, duerapp_rect_create(&rect, 11, 15, 229, 33), FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_WRITING_VIP_LABEL1_CTRL_ID, WATCH_DEFAULT_BIG_FONT, RGB8882RGB565(0xFFB436));
    
    GUILABEL_SetRect(MMI_DUERAPP_WRITING_VIP_LABEL2_CTRL_ID, duerapp_rect_create(&rect, 13, 52, 216, 33), FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_WRITING_VIP_LABEL2_CTRL_ID, WATCH_DEFAULT_BIG_FONT, RGB8882RGB565(0xFFFFFF));
    
    GUILABEL_SetRect(MMI_DUERAPP_WRITING_VIP_LABEL3_CTRL_ID, duerapp_rect_create(&rect, 13, 88, 216, 33), FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_WRITING_VIP_LABEL3_CTRL_ID, WATCH_DEFAULT_BIG_FONT, RGB8882RGB565(0xFFFFFF));
    
    // GUILABEL_SetRect(MMI_DUERAPP_WRITING_VIP_LABEL4_CTRL_ID, duerapp_rect_create(&rect, 55, 191, 144, 33), FALSE);
    // GUILABEL_SetFont(MMI_DUERAPP_WRITING_VIP_LABEL4_CTRL_ID, WATCH_DEFAULT_BIG_FONT, RGB8882RGB565(0x4386FE));

    str.wstr_ptr = L"开会员 看作文";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_WRITING_VIP_LABEL1_CTRL_ID, &str, FALSE);

    str.wstr_ptr = L"请找爸爸妈妈开通";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_WRITING_VIP_LABEL2_CTRL_ID, &str, FALSE);

    str.wstr_ptr = L"会员后使用哦！";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_WRITING_VIP_LABEL3_CTRL_ID, &str, FALSE);

    // str.wstr_ptr = L"会员是什么？";
    // str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    // GUILABEL_SetText(MMI_DUERAPP_WRITING_VIP_LABEL4_CTRL_ID ,&str, FALSE);

    GUIBUTTON_SetRect(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID, duerapp_rect_create(&rect, 20, 150, 200, 50));

    btn_bg_release.bg_type = GUI_BG_COLOR;
    btn_bg_release.color = DUERAPP_COLOR_MAKE(100, 128, 250);

    btn_bg_press.bg_type = GUI_BG_COLOR;
    btn_bg_press.color = DUERAPP_COLOR_MAKE(42, 42, 49);

    GUIBUTTON_SetRunSheen(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID,FALSE);
    //GUIAPICTRL_SetState(ctrl_id, GUICTRL_STATE_TOPMOST, TRUE);
    GUIBUTTON_SetHandleLong(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID, TRUE);
    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID, &btn_bg_release);
    GUIBUTTON_SetPressedFg(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID, &btn_bg_press);

    GUIBUTTON_SetFont(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID, &font);

    str.wstr_ptr = L"开通会员";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUIBUTTON_SetText(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID, str.wstr_ptr, str.wstr_len);
    GUIAPICTRL_SetState(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID, GUICTRL_STATE_TOPMOST, TRUE);
    GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_WRITING_VIP_BTN1_CTRL_ID, OpenVipCallBack);
}


#endif

#if 1   // trial window
LOCAL MMI_RESULT_E trial_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_POINT_T start_point = {0};

    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        LOGI(tag, "Open window %d", win_id);
        trial_win_full_paint();
        break;

    case MSG_GET_FOCUS:
        trial_win_full_paint();
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        LOGI(tag, "Close window %d", win_id);
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;

}

LOCAL void trial_win_full_paint(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    MMI_STRING_T str = {0};

    GUI_BG_T btn_bg_release = {0};
    GUI_BG_T btn_bg_press = {0};
    // GUI_FONT_ALL_T font = 
    // {
    //     .font = WATCH_DEFAULT_BIG_FONT,
    //     .color = DUERAPP_COLOR_MAKE(255, 255, 255),
    // };
    GUI_FONT_ALL_T font;
    memset(&font, 0x00, sizeof(font));
    font.font = WATCH_DEFAULT_BIG_FONT;
    font.color = DUERAPP_COLOR_MAKE(255, 255, 255);

    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);


    GUILABEL_SetRect(MMI_DUERAPP_WRITING_TRIAL_LABEL1_CTRL_ID, duerapp_rect_create(&rect, 72, 6, 96, 33), FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_WRITING_TRIAL_LABEL1_CTRL_ID, WATCH_DEFAULT_BIG_FONT, RGB8882RGB565(0xFFFFFF));
    
    GUILABEL_SetRect(MMI_DUERAPP_WRITING_TRIAL_LABEL2_CTRL_ID, duerapp_rect_create(&rect, 40, 55, 160, 33), FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_WRITING_TRIAL_LABEL2_CTRL_ID, WATCH_DEFAULT_BIG_FONT, RGB8882RGB565(0xFFB436));
    
    GUILABEL_SetRect(MMI_DUERAPP_WRITING_TRIAL_LABEL3_CTRL_ID, duerapp_rect_create(&rect, 40, 88, 160, 33), FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_WRITING_TRIAL_LABEL3_CTRL_ID, WATCH_DEFAULT_BIG_FONT, RGB8882RGB565(0xFFB436));
    
    str.wstr_ptr = L"小度作文";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_WRITING_TRIAL_LABEL1_CTRL_ID, &str, FALSE);

    str.wstr_ptr = L"限时领取7天";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_WRITING_TRIAL_LABEL2_CTRL_ID, &str, FALSE);

    str.wstr_ptr = L"免费使用特权";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_WRITING_TRIAL_LABEL3_CTRL_ID, &str, FALSE);

    GUIBUTTON_SetRect(MMI_DUERAPP_WRITING_TRIAL_BTN1_CTRL_ID, duerapp_rect_create(&rect, 17, 162, 206, 59));

    btn_bg_release.bg_type = GUI_BG_COLOR;
    btn_bg_release.color = DUERAPP_COLOR_MAKE(100, 128, 250);

    btn_bg_press.bg_type = GUI_BG_COLOR;
    btn_bg_press.color = DUERAPP_COLOR_MAKE(42, 42, 49);

    GUIBUTTON_SetRunSheen(MMI_DUERAPP_WRITING_TRIAL_BTN1_CTRL_ID,FALSE);
    //GUIAPICTRL_SetState(ctrl_id, GUICTRL_STATE_TOPMOST, TRUE);
    GUIBUTTON_SetHandleLong(MMI_DUERAPP_WRITING_TRIAL_BTN1_CTRL_ID, TRUE);
    GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_TRIAL_BTN1_CTRL_ID, &btn_bg_release);
    GUIBUTTON_SetPressedFg(MMI_DUERAPP_WRITING_TRIAL_BTN1_CTRL_ID, &btn_bg_press);

    GUIBUTTON_SetFont(MMI_DUERAPP_WRITING_TRIAL_BTN1_CTRL_ID, &font);

    str.wstr_ptr = L"立即领取";
    str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    GUIBUTTON_SetText(MMI_DUERAPP_WRITING_TRIAL_BTN1_CTRL_ID, str.wstr_ptr, str.wstr_len);
}


#endif

#if 1   // content window
LOCAL MMI_RESULT_E content_win_msg_handler(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_POINT_T start_point = {0};
    int x = 0, y = 0;
    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;
    // MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        LOGI(tag, "Open window %d", win_id);
        content_win_full_paint();
        break;

    case MSG_GET_FOCUS:
        // content_win_full_paint();
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_TP_PRESS_DOWN:
    {
        x = MMK_GET_TP_X(param);
        y = MMK_GET_TP_Y(param);

        LOGI(tag, "%d, %d", x, y);
        if ((x > 185 && x < 230) && (y > 185 && y < 230)) {
            LOGI(tag, "Icon tts pressed");
        }
        break;
    }

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        LOGI(tag, "Close window %d", win_id);
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;

}

LOCAL GUI_LCD_DEV_INFO s_layer_test;

LOCAL void content_win_full_paint(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    MMI_STRING_T str = {0};
    wchar *name_uni = NULL;
    GUI_BG_T bg_text = {0};
    // GUI_BORDER_T border = 
    // {
    //     .type = GUI_BORDER_NONE,
    // };
    GUI_BORDER_T border;

    GUI_FONT_T font = WATCH_DEFAULT_BIG_FONT;
    GUI_COLOR_T color = RGB8882RGB565(0x4A4A4A);
    wchar *comment_uni = NULL;
    wchar *content_uni = NULL;
    MMI_STRING_T dst_ptr = {0};
    MMI_STRING_T src1_ptr = {0};
    MMI_STRING_T src2_ptr = {0};
    MMI_STRING_T src3_ptr = {0};

    MMI_STRING_T src4_ptr = {0};

    memset(&border, 0x00, sizeof(border));
    border.type = GUI_BORDER_NONE;

    LCD_FillRect(&lcd_dev_info, rect, DUERAPP_COLOR_MAKE(255, 255, 255));

    GUILABEL_SetRect(MMI_DUERAPP_WRITING_CONTENT_LABEL1_CTRL_ID, duerapp_rect_create(&rect, 0, 2, 240, 33), FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_WRITING_CONTENT_LABEL1_CTRL_ID, WATCH_DEFAULT_BIG_FONT, RGB8882RGB565(0x4386FE));    
    if (s_entity_selected) {
        name_uni = duerapp_utf8_to_unicode(s_entity_selected->name);
        if (name_uni) {
            str.wstr_ptr = name_uni;
            str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
            GUILABEL_SetText(MMI_DUERAPP_WRITING_CONTENT_LABEL1_CTRL_ID, &str, FALSE);
            DUER_FREE(name_uni);
        }
    }
    // str.wstr_ptr = L"·春天来了·";
    // str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    // GUILABEL_SetText(MMI_DUERAPP_WRITING_CONTENT_LABEL1_CTRL_ID, &str, FALSE);

    GUITEXT_SetRect(MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID, duerapp_rect_create(&rect, 0, 40, 240, 200));

    bg_text.bg_type = GUI_BG_COLOR;
    bg_text.color = DUERAPP_COLOR_MAKE(255, 255, 255);

    GUITEXT_SetBg(MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID, &bg_text);
    GUITEXT_SetBorder(&border, MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID);
    GUITEXT_SetFont(MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID, &font, &color);
    
    if (s_entity_selected) {
        if (s_comment) {
            comment_uni = duerapp_utf8_to_unicode(s_comment);
        }

        if (s_content) {
            content_uni = duerapp_utf8_to_unicode(s_content);
        }

        if (content_uni) {
            src1_ptr.wstr_ptr = content_uni;
            src1_ptr.wstr_len = MMIAPICOM_Wstrlen(src1_ptr.wstr_ptr);
            
            src2_ptr.wstr_ptr = comment_uni;
            src2_ptr.wstr_len = MMIAPICOM_Wstrlen(src2_ptr.wstr_ptr);
            
            if (comment_uni) {
                src3_ptr.wstr_ptr = L"\n\n●作文点评：\n";
                src3_ptr.wstr_len = MMIAPICOM_Wstrlen(src3_ptr.wstr_ptr);

                MMIAPICOM_CatTwoString(&src4_ptr, &src1_ptr, &src3_ptr);

                MMIAPICOM_CatTwoString(&dst_ptr, &src4_ptr, &src2_ptr);

                GUITEXT_SetString(MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID, dst_ptr.wstr_ptr, dst_ptr.wstr_len, FALSE);

                MMIAPICOM_DestroyString(src4_ptr);
                MMIAPICOM_DestroyString(dst_ptr);
            } else {
                GUITEXT_SetString(MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID, src1_ptr.wstr_ptr, src1_ptr.wstr_len, FALSE);
            }
        }

        if (content_uni) {
            DUER_FREE(content_uni);
        }

        if (comment_uni) {
            DUER_FREE(comment_uni);
        }
    }
    // str.wstr_ptr = L"盼望着，\n盼望着，东风来了，春天的脚步近了。\n一切都像刚睡醒的样子，欣欣然张开了眼。山朗润起来了，水长起来了，太阳的脸红起来了。小草偷偷地从土里钻出来，嫩嫩的，绿绿的。园子里，田野里，瞧去，一大片一大片满是的。坐着，躺着，打两个滚，踢几脚球，赛几趟跑，捉几回迷藏。风轻悄悄的，草绵软软的。";
    // str.wstr_len = MMIAPICOM_Wstrlen(str.wstr_ptr);
    // GUITEXT_SetString(MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID, str.wstr_ptr, str.wstr_len, FALSE);
    GUIAPICTRL_SetState(MMI_DUERAPP_WRITING_CONTENT_TEXT1_CTRL_ID, GUICTRL_STATE_TOPMOST, FALSE);

    // GUI_BG_T btn_bg_rel = {0};
    // btn_bg_rel.bg_type = GUI_BG_IMG;
    // btn_bg_rel.img_id = IMAGE_DUERAPP_WRITING_ARGUMENTATION;
    // GUIBUTTON_SetBg(MMI_DUERAPP_WRITING_CONTENT_BTN1_CTRL_ID, &btn_bg_rel);

    // GUI_BG_T btn_bg_press = {0};
    // btn_bg_press.bg_type = GUI_BG_IMG;
    // btn_bg_press.img_id = IMAGE_DUERAPP_WRITING_ARGUMENTATION;
    // GUIBUTTON_SetPressedFg(MMI_DUERAPP_WRITING_CONTENT_BTN1_CTRL_ID, &btn_bg_press);

    // GUIAPICTRL_SetState(MMI_DUERAPP_WRITING_CONTENT_BTN1_CTRL_ID, GUICTRL_STATE_TOPMOST, TRUE);
    // GUIBUTTON_SetRect(MMI_DUERAPP_WRITING_CONTENT_BTN1_CTRL_ID, duerapp_rect_create(&rect, 0, 0, 90, 90));


#if 0
    if (UILAYER_IsMultiLayerEnable())
    {
        UILAYER_CREATE_T    create_info = {0};        
        
        //s_layer_test[0]
        create_info.lcd_id = MAIN_LCD_ID;
        create_info.owner_handle = MMI_DUERAPP_WRITING_CONTENT_WIN_ID;
        create_info.offset_x = 185;
        create_info.offset_y = 185;
        create_info.width = 40;
        create_info.height = 40;
        create_info.is_bg_layer = FALSE;
        create_info.is_static_layer = FALSE;   
        UILAYER_CreateLayer(&create_info, &s_layer_test);           
    }

    GUI_LCD_DEV_INFO main_dev_info = {0, 0};
    GUI_RECT_T rect0 = {0};
    UILAYER_APPEND_BLT_T append_layer = {0};
    uint16 lcd_width = 0;
    uint16 lcd_height = 0;

    append_layer.lcd_dev_info = s_layer_test;
    append_layer.layer_level = UILAYER_LEVEL_HIGH;
    UILAYER_AppendBltLayer(&append_layer);

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);
    rect0.left = 0;
    rect0.top = 0;
    rect0.right = (lcd_width-1);
    rect0.bottom = (lcd_height-1);
    // LCD_FillRect(&main_dev_info, rect0, MMI_RED_COLOR);
    // LCD_FillRect(&s_layer_test, UILAYER_GetLayerRect(&s_layer_test), MMI_RED_COLOR);

    GUI_POINT_T point = 
    {
        .x = 185,
        .y = 185,
    };
    GUIRES_DisplayImg(&point,
		PNULL,
		PNULL,
		MMI_DUERAPP_WRITING_CONTENT_WIN_ID,
		IMAGE_DUERAPP_WRITING_PLAY,
		&s_layer_test);
#endif
}

static int duerapp_writing_content_win_open(void)
{
    LOGI(tag, "Opening content page");
    return MMK_CreateWin((uint32*)MMI_DUERAPP_WRITING_CONTENT_WIN_TAB, PNULL);
}

#endif

static int duer_set_grade_info(duerapp_grade_t grade)
{
    if (grade == DUERAPP_GRADE_MAX) {
        LOGE(tag, "INVALID grade");
        return DUER_CANCEL;
    }
    // 本地更新保存
    MMIDUERAPP_SetCompositionGrade(s_grades_utf8[grade]);

    duerapp_writing_main_win_open();
    return DUER_OK;
}

static int duer_get_grade_info(void)
{
    duer_dcs_on_link_clicked(LINKCLICK_GET_GRADE_INFO);
}

static int duer_get_conf_info(void)
{
    duer_dcs_on_link_clicked(LINKCLICK_GET_CONF_INFO);
}

static int duer_query(duerapp_grade_t grade, duerapp_literary_style_t style, duerapp_hotword_t hotword, int words_limit)
{
    char *link_url = DUER_MALLOC(512);
    char tmp[8];

    if (grade == DUERAPP_GRADE_MAX) {
        LOGE(tag, "INVALID grade");
        duerapp_writing_grade_win_open();
        return DUER_CANCEL;
    }
    // if (style == DUERAPP_LITERARY_STYLE_BLANK) {
    //     LOGE(tag, "INVALID style");
    //     return DUER_CANCEL;
    // }
    // if (hotword == DUERAPP_HOTWORD_BLANK) {
    //     LOGE(tag, "INVALID hotword");
    //     return DUER_CANCEL;
    // }

    LOGI(tag, "grade:%d, style:%d, hotword:%d, words:%d" , grade, style, hotword, words_limit);

    if (link_url) {
        // "dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=query&grade=%s&literary_style=%s&hot_sort=%s&limit_word_cnt=%d"
        //dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=query&grade=一年级&literary_style=议论文&hot_sort=春天&limit_word_cnt=200
        snprintf(link_url, 512, LINKCLICK_QUERY, s_grades_utf8[grade]);
        //s_literary_style_utf8[style], s_hotword_utf8[hotword], words_limit
        if (style < DUERAPP_LITERARY_STYLE_BLANK) {
            strcat(link_url, "&literary_style=");
            strcat(link_url, s_literary_style_utf8[style]);
        }
        if (hotword < DUERAPP_HOTWORD_BLANK) {
            strcat(link_url, "&hot_sort=");
            strcat(link_url, s_hotword_utf8[hotword]);
        }
        if (words_limit > 0) {
            strcat(link_url, "&limit_word_cnt=");
            snprintf(tmp, sizeof(tmp), "%d", words_limit);
            strcat(link_url, tmp);
        }
        duer_hex_print(link_url, strlen(link_url));
        LOGI(tag, "%s", link_url);
        duer_dcs_on_link_clicked(link_url);
        DUER_FREE(link_url);
    } else {
    }
}

static int duer_get_comment(char *id)
{
    char *link_url = DUER_MALLOC(512);
    if (link_url) {
        // 获取获取点评信息(DuerWatchCompositionGetCommentInfo)：dueros://a10d5bed-a5c2-304b-fde9-6ffd6f6b86bd/scene?action=getCommentInfo&id=612cb609e213287285be240d4f739795
        snprintf(link_url, 512, LINKCLICK_GETCOMMENTINFO, id);
        // duer_hex_print(link_url, strlen(link_url));
        LOGI(tag, "%s", link_url);
        duer_dcs_on_link_clicked(link_url);
        DUER_FREE(link_url);
    } else {
    }
}

void duer_hex_print(unsigned char *input, int length)
{
    unsigned char pbuf[128];
    int count = length;
    unsigned char *p = NULL;
    int i = 0;

    if (!input || !length) {
        return;
    }

    LOGI(tag, "");

    while (count) {
        if (count > 16) {
            p = pbuf;
            for (i = 0; i < 16; i++) {
                snprintf(p, 4, "%.2X ", *input++);
                p += 3;
            }
            // SCI_TRACE_HIGH(pbuf);
            DUER_LOGE(pbuf);
            count -= 16;
        } else {
            p = pbuf;
            for (i = 0; i < count; i++) {
                snprintf(p, 4, "%.2X ", *input++);
                p += 3;
            }
            // SCI_TRACE_HIGH(pbuf);
            DUER_LOGE(pbuf);
            break;
        }
    }


    // unsigned char *p = pbuf;
    // for (int i = 0; i < length; i++) {
    //     snprintf(p, 4, "%.2X ", input[i]);
    //     p += 3;
    // }

    // // LOGI(tag, pbuf);
    // SCI_TRACE_HIGH(pbuf);

}

static void duer_print_long_msg(char *msg)
{
    // UINT32 cpsr = disableInterrupts();//disable interrupt
    int p = 0, len = strlen(msg);
    char tmp[81];
    LOGI(tag, "");
    while(len)
    {
        if(len > 80)
        {
            len -= 80;
            strncpy(tmp, msg + p, 80);
            tmp[80] = '\0';
            p += 80;
            // SCI_TRACE_HIGH("%s",tmp);
            DUER_LOGE("%s",tmp);
        }
        else
        {
            strncpy(tmp, msg + p, len);
            tmp[len] = '\0';
            len = 0;
            // SCI_TRACE_HIGH("%s",tmp);
            DUER_LOGE("%s",tmp);
        }
    }
    // SCI_TRACE_HIGH("\r\n");
    // restoreInterrupts(cpsr);//restore interrupt
}

static void duer_print_json(baidu_json *directive)
{
    char *p = baidu_json_Print(directive);
    if (p) {
        // char *s;
        // for (s = p; *s != '\0'; s++) {
        //     if (*s == '\r' || *s == '\n' || *s == '\t') {
        //         *s = ' ';
        //     } else {

        //     }
        // }

        duer_print_long_msg(p);

        duer_hex_print(p, strlen(p));

        baidu_json_release(p);
    }
}

static duer_status_t duer_dcs_get_conf_info_handler(baidu_json *directive)
{
    int ret = DUER_ERR_FAILED;
    LOGI(tag, "");
#if 1
    duer_print_json(directive);
#endif

}

static duer_status_t duer_dcs_get_grade_info_handler(baidu_json *directive)
{
    int ret = DUER_ERR_FAILED;
    LOGI(tag, "");
#if 1
    duer_print_json(directive);
#endif

}

static duer_status_t duer_dcs_upd_grade_info_handler(baidu_json *directive)
{
    int ret = DUER_ERR_FAILED;
    LOGI(tag, "");
#if 1
    duer_print_json(directive);
#endif
    MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_WRITING_GRADE_WIN_ID, MSG_CTL_CANCEL, NULL, 0);
    // MMK_SendMsg(MMI_DUERAPP_WRITING_GRADE_WIN_ID, MSG_CTL_CANCEL, NULL);
}

static duer_status_t duer_free_entities(void)
{
    duerapp_entity_t *p = s_entity_list;
    duerapp_entity_t *tmp = NULL;
    LOGI(tag, "");
    while (p) {
        // LOGI(tag, "%.8X", (uint32)p);
        tmp = p->next;
        if (p->content) {
            DUER_FREE(p->content);
        }
        if (p->name) {
            DUER_FREE(p->name);
        }
        if (p->id) {
            DUER_FREE(p->id);
        }
        DUER_FREE(p);
        p = tmp;
    }
    s_entity_list = NULL;
}

// free s_commnet and s_content
static duer_status_t duer_free_comments(void)
{
    if (s_content) {
        DUER_FREE(s_content);
    }
    s_content = NULL;
    
    if (s_comment) {
        DUER_FREE(s_comment);
    }
    s_comment = NULL;
}

// 'index' starts from 0
static duerapp_entity_t* duer_get_entity(int index)
{
    duerapp_entity_t *p = s_entity_list;
    duerapp_entity_t *entity = NULL;
    LOGI(tag, "");
    while (p) {
        if (!index) {
            break;
        } else {
            index--;
        }
        p = p->next;
    }
    if (p && !index) {
        entity = p;
    }
    return entity;
}

static duer_status_t duer_dcs_query_handler(baidu_json *directive)
{
    int retcode = DUER_ERR_FAILED;
    baidu_json *content = NULL;
    baidu_json *payload = NULL;
    baidu_json *ret = NULL;
    baidu_json *result = NULL;
    baidu_json *result_item = NULL;
    int result_item_count = 0;
    baidu_json *response = NULL;
    baidu_json *entity = NULL;
    int entity_item_count = 0;
    int i = 0;
    baidu_json *entity_item = NULL;
    baidu_json *name = NULL;
    baidu_json *attrs = NULL;
    baidu_json *value = NULL;
    baidu_json *id = NULL;
    baidu_json *length = NULL;
    int attrs_item_count = 0;
    baidu_json *attrs_item = NULL;
    baidu_json *label = NULL;
    baidu_json *objects = NULL;
    int objects_item_count = 0;
    baidu_json *objects_item = NULL;
    // baidu_json *objects = NULL;
    // int objects_item_count = NULL;
    // baidu_json *objects_item = NULL;
    // baidu_json *objects = NULL;
    // int objects_item_count = NULL;
    // baidu_json *objects_item = NULL;
    duerapp_entity_t *p_entity = NULL;
    char *str_body  = NULL;
    char *str_name  = NULL;
    char *str_id    = NULL;
    duerapp_entity_t *tmp = NULL;

    LOGI(tag, "");
#if 0
    duer_print_json(directive);
#endif


    do {
        payload = baidu_json_GetObjectItem(directive, "payload");
        if (!payload) {
            LOGE(tag, "No 'payload' found");
            break;
        }
        ret = baidu_json_GetObjectItem(payload, "payload");
        if (!payload) {
            LOGE(tag, "No 'ret' found");
            break;
        }
        LOGE(tag, "ret type:%.8X", ret->type);
        if (1) {
            if (ret->valueint == 0) {
                result = baidu_json_GetObjectItem(payload, "result");
                if (!result) {
                    LOGE(tag, "No 'result' found");
                    break;
                }

                result_item_count = baidu_json_GetArraySize(result);
                if (result_item_count) {
                    result_item = baidu_json_GetArrayItem(result, 0);
                    if (!result_item) {
                        LOGE(tag, "No 'result_item' found");
                        break;
                    }
                } else {
                    LOGE(tag, "Result is empty");
                    break;
                }

                // LOGI(tag, "result_item:");
                // duer_hex_print(result_item->valuestring, strlen(result_item->valuestring));

                // main content
                content = baidu_json_Parse(result_item->valuestring);
                if (!content) {
                    LOGE(tag, "'content' parse error");
                    break;
                }
                response = baidu_json_GetObjectItem(content, "response");
                if (!response) {
                    LOGE(tag, "No 'response' found");
                    break;
                }
                entity = baidu_json_GetObjectItem(response, "entity");
                if (!entity) {
                    LOGE(tag, "No 'entity' found");
                    break;
                }
                entity_item_count = baidu_json_GetArraySize(entity);
                for (i = 0; i < entity_item_count; i++) {
                    // composition item
                    entity_item = baidu_json_GetArrayItem(entity, i);
                    if (!entity_item) {
                        LOGE(tag, "No 'entity_item' found");
                        continue;
                    }

                    name = baidu_json_GetObjectItem(entity_item, "name");
                    if (!name) {
                        LOGE(tag, "No 'name' found");
                        continue;
                    }

                    attrs = baidu_json_GetObjectItem(entity_item, "attrs");
                    if (!attrs) {
                        LOGE(tag, "No 'attrs' found");
                        continue;
                    }

                    attrs_item_count = baidu_json_GetArraySize(attrs);
                    for (i = 0; i < attrs_item_count; i++) {
                        attrs_item = baidu_json_GetArrayItem(attrs, i);
                        if (!attrs_item) {
                            LOGE(tag, "No 'attrs_item' found");
                            continue;
                        }

                        label = baidu_json_GetObjectItem(attrs_item, "label");
                        if (!label) {
                            LOGE(tag, "No 'label' found");
                            continue;
                        }

                        if (strcmp(label->valuestring, "\xe4\xbd\x9c\xe6\x96\x87\xe6\x91\x98\xe8\xa6\x81") == 0) { // 作文摘要
                            objects = baidu_json_GetObjectItem(attrs_item, "objects");
                            if (!objects) {
                                LOGE(tag, "No 'objects' found");
                                break;
                            }
                            objects_item_count = baidu_json_GetArraySize(objects);
                            if (objects_item_count) {
                                objects_item = baidu_json_GetArrayItem(objects, 0);
                                if (!objects_item) {
                                    LOGE(tag, "No 'objects_item' found");
                                    break;
                                }
                                value = baidu_json_GetObjectItem(objects_item, "@value");
                                if (!value) {
                                    LOGE(tag, "No 'value' found");
                                    break;
                                }
                            } else {
                                LOGE(tag, "objects is null");
                            }
                        } else if (strcmp(label->valuestring, "\xe4\xbd\x9c\xe6\x96\x87ID") == 0) { // 作文ID
                            objects = baidu_json_GetObjectItem(attrs_item, "objects");
                            if (!objects) {
                                LOGE(tag, "No 'objects' found");
                                break;
                            }
                            objects_item_count = baidu_json_GetArraySize(objects);
                            if (objects_item_count) {
                                objects_item = baidu_json_GetArrayItem(objects, 0);
                                if (!objects_item) {
                                    LOGE(tag, "No 'objects_item' found");
                                    break;
                                }
                                id = baidu_json_GetObjectItem(objects_item, "@value");
                                if (!id) {
                                    LOGE(tag, "No 'value' found");
                                    break;
                                }
                            } else {
                                LOGE(tag, "objects is null");
                            }
                        } else if (strcmp(label->valuestring, "\xe4\xbd\x9c\xe6\x96\x87\xe5\xad\x97\xe6\x95\xb0") == 0) { // 作文字数
                            objects = baidu_json_GetObjectItem(attrs_item, "objects");
                            if (!objects) {
                                LOGE(tag, "No 'objects' found");
                                break;
                            }
                            objects_item_count = baidu_json_GetArraySize(objects);
                            if (objects_item_count) {
                                objects_item = baidu_json_GetArrayItem(objects, 0);
                                if (!objects_item) {
                                    LOGE(tag, "No 'objects_item' found");
                                    break;
                                }
                                length = baidu_json_GetObjectItem(objects_item, "@value");
                                if (!length) {
                                    LOGE(tag, "No 'value' found");
                                    break;
                                }
                            } else {
                                LOGE(tag, "objects is null");
                            }
                        } else {}
                    }

                    if (value && name && id && length) {
                        LOGI(tag, "name:");
                        duer_hex_print(name->valuestring, strlen(name->valuestring));
                        
                        LOGI(tag, "value:");
                        duer_hex_print(value->valuestring, strlen(value->valuestring));
                        
                        LOGI(tag, "id:%s", id->valuestring);

                        LOGI(tag, "length:%s", length->valuestring);

                        p_entity = DUER_MALLOC(sizeof(duerapp_entity_t));
                        str_body  = DUER_MALLOC(strlen(value->valuestring) + 1);
                        str_name  = DUER_MALLOC(strlen(name->valuestring) + 1);
                        str_id    = DUER_MALLOC(strlen(id->valuestring) + 1);
                        if (p_entity && str_body && str_name && str_id) {
                            // LOGI(tag, "%.8X", (uint32)p_entity);
                            
                            strcpy(str_body, value->valuestring);
                            strcpy(str_name, name->valuestring);
                            strcpy(str_id, id->valuestring);

                            p_entity->next      = NULL;
                            p_entity->content   = str_body;
                            p_entity->name      = str_name;
                            p_entity->id        = str_id;
                            p_entity->length    = 0;
                            sscanf(length->valuestring, "%d", &p_entity->length);
                            if (s_entity_list == NULL) {
                                LOGI(tag, "Create head node");
                                s_entity_list = p_entity;
                                s_entity_list->next = NULL;
                            } else {
                                LOGI(tag, "Insert node");
                                tmp = s_entity_list->next;
                                s_entity_list->next = p_entity;
                                p_entity->next = tmp;
                            }
                        } else {
                            LOGE(tag, "DUER_MALLOC failed");
                            if (p_entity) {
                                DUER_FREE(p_entity);
                            }
                            if (str_body) {
                                DUER_FREE(str_body);
                            }
                            if (str_name) {
                                DUER_FREE(str_name);
                            }
                            if (str_id) {
                                DUER_FREE(str_id);
                            }
                        }
                    }
                }
            } else {
                LOGE(tag, "Wrong ret type");
                break;
            }
        } else {
            LOGE(tag, "ret type is not NUMBER:%.8X", ret->type);
            break;
        }

        retcode = DUER_OK;
    } while (0);

    if (content) {
        baidu_json_Delete(content);
    }

    if (retcode == DUER_OK) {
        LOGE(tag, "Query result parse successfully!");
        MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_WRITING_MAIN_WIN_ID, MSG_DUERAPP_GOT_COMPLIST, NULL, 0);
        // duerapp_writing_list_win_open();     do not do UI operations in NONE-UI task
    } else {
        LOGE(tag, "Query result parse failed!:%d", retcode);
        // duerapp_show_toast(L"未搜到相关内容"); //避免非UI线程操作UI
    }
    return retcode;
}

static duer_status_t duer_dcs_get_comment_info_handler(baidu_json *directive)
{
    baidu_json *content = NULL;
    baidu_json *payload = NULL;
    baidu_json *ret = NULL;
    baidu_json *result = NULL;
    baidu_json *result_item = NULL;
    int result_item_count = 0;
    baidu_json *response = NULL;
    baidu_json *entity = NULL;
    int entity_item_count = 0;
    int i = 0;
    baidu_json *entity_item = NULL;
    baidu_json *attrs = NULL;
    baidu_json *comment = NULL;
    // baidu_json *content = NULL;
    int attrs_item_count = 0;
    baidu_json *attrs_item = NULL;
    baidu_json *label = NULL;
    baidu_json *objects = NULL;
    int objects_item_count = 0;
    baidu_json *objects_item = NULL;
    // baidu_json *objects = NULL;
    // int objects_item_count = 0;
    // baidu_json *objects_item = NULL;
    int len = 0;

    int retcode = DUER_ERR_FAILED;
    // int entity_item_count = 0;
    
    LOGI(tag, "");
#if 1
    // duer_print_json(directive);
#endif


    do {
        payload = baidu_json_GetObjectItem(directive, "payload");
        if (!payload) {
            LOGE(tag, "No 'payload' found");
            break;
        }
        ret = baidu_json_GetObjectItem(payload, "payload");
        if (!payload) {
            LOGE(tag, "No 'ret' found");
            break;
        }
        LOGE(tag, "ret type:%.8X", ret->type);
        if (1) {
            if (ret->valueint == 0) {
                result = baidu_json_GetObjectItem(payload, "result");
                if (!result) {
                    LOGE(tag, "No 'result' found");
                    break;
                }

                result_item_count = baidu_json_GetArraySize(result);
                if (result_item_count) {
                    result_item = baidu_json_GetArrayItem(result, 0);
                    if (!result_item) {
                        LOGE(tag, "No 'result_item' found");
                        break;
                    }
                } else {
                    LOGE(tag, "Result is empty");
                    break;
                }

                // LOGI(tag, "result_item:");
                // duer_hex_print(result_item->valuestring, strlen(result_item->valuestring));

                // main content
                content = baidu_json_Parse(result_item->valuestring);
                if (!content) {
                    LOGE(tag, "'content' parse error");
                    break;
                }
                response = baidu_json_GetObjectItem(content, "response");
                if (!response) {
                    LOGE(tag, "No 'response' found");
                    break;
                }
                entity = baidu_json_GetObjectItem(response, "entity");
                if (!entity) {
                    LOGE(tag, "No 'entity' found");
                    break;
                }
                entity_item_count = baidu_json_GetArraySize(entity);
                for (i = 0; i < entity_item_count; i++) {
                    // composition item
                    entity_item = baidu_json_GetArrayItem(entity, i);
                    if (!entity_item) {
                        LOGE(tag, "No 'entity_item' found");
                        continue;
                    }

                    attrs = baidu_json_GetObjectItem(entity_item, "attrs");
                    if (!attrs) {
                        LOGE(tag, "No 'attrs' found");
                        continue;
                    }

                    attrs_item_count = baidu_json_GetArraySize(attrs);
                    for (i = 0; i < attrs_item_count; i++) {
                        attrs_item = baidu_json_GetArrayItem(attrs, i);
                        if (!attrs_item) {
                            LOGE(tag, "No 'attrs_item' found");
                            continue;
                        }

                        label = baidu_json_GetObjectItem(attrs_item, "label");
                        if (!label) {
                            LOGE(tag, "No 'label' found");
                            continue;
                        }

                        if (strcmp(label->valuestring, "\xe4\xbd\x9c\xe6\x96\x87\xe7\x82\xb9\xe8\xaf\x84") == 0) { // 作文点评
                            objects = baidu_json_GetObjectItem(attrs_item, "objects");
                            if (!objects) {
                                LOGE(tag, "No 'objects' found");
                                break;
                            }
                            objects_item_count = baidu_json_GetArraySize(objects);
                            if (objects_item_count) {
                                objects_item = baidu_json_GetArrayItem(objects, 0);
                                if (!objects_item) {
                                    LOGE(tag, "No 'objects_item' found");
                                    break;
                                }
                                comment = baidu_json_GetObjectItem(objects_item, "@value");
                                if (!comment) {
                                    LOGE(tag, "No 'value' found");
                                    break;
                                }
                            } else {
                                LOGE(tag, "objects is null");
                            }
                        } else if (strcmp(label->valuestring, "\xe4\xbd\x9c\xe6\x96\x87\xe5\x86\x85\xe5\xae\xb9") == 0) { // 作文内容
                            objects = baidu_json_GetObjectItem(attrs_item, "objects");
                            if (!objects) {
                                LOGE(tag, "No 'objects' found");
                                break;
                            }
                            objects_item_count = baidu_json_GetArraySize(objects);
                            if (objects_item_count) {
                                objects_item = baidu_json_GetArrayItem(objects, 0);
                                if (!objects_item) {
                                    LOGE(tag, "No 'objects_item' found");
                                    break;
                                }
                                content = baidu_json_GetObjectItem(objects_item, "@value");
                                if (!content) {
                                    LOGE(tag, "No 'value' found");
                                    break;
                                }
                            } else {
                                LOGE(tag, "objects is null");
                            }
                        } else {}
                    }

                    duer_free_comments();

                    if (comment && comment->valuestring) {
                        LOGI(tag, "value:");
                        len = strlen(comment->valuestring);
                        duer_hex_print(comment->valuestring, len);
                        if (len) {
                            s_comment = DUER_MALLOC(len + 1);
                            if (s_comment) {
                                strcpy(s_comment, comment->valuestring);
                            } else {
                                LOGE(tag, "Comment malloc failed");
                            }
                        } else {
                            LOGW(tag, "Comment is empty");
                        }
                    } else {
                        LOGE(tag, "value is null");
                    }

                    if (content && content->valuestring) {
                        LOGI(tag, "value:");
                        len = strlen(content->valuestring);
                        duer_hex_print(content->valuestring, len);
                        if (len) {
                            s_content = DUER_MALLOC(len + 1);
                            if (s_content) {
                                strcpy(s_content, content->valuestring);
                            } else {
                                LOGE(tag, "Content malloc failed");
                            }
                        } else {
                            LOGW(tag, "Content is empty");
                        }
                    } else {
                        LOGE(tag, "value is null");
                    }
                }
            } else {
                LOGE(tag, "Wrong ret type");
                break;
            }
        } else {
            LOGE(tag, "ret type is not NUMBER:%.8X", ret->type);
            break;
        }

        retcode = DUER_OK;
    } while (0);

    MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_WRITING_LIST_WIN_ID, MSG_DUERAPP_GOT_COMMENT, NULL, 0);
    // MMK_SendMsg(MMI_DUERAPP_WRITING_LIST_WIN_ID, MSG_DUERAPP_GOT_COMMENT, NULL);

    if (content) {
        baidu_json_Delete(content);
    }

    if (retcode == DUER_OK) {
        LOGE(tag, "Comment result parse successfully!");

    } else {
        LOGE(tag, "Comment result parse failed!:%d", retcode);
    }
    return retcode;
}

static duer_status_t duer_dcs_composition(baidu_json *directive)
{
    LOGI(tag, "");
    duer_dcs_query_handler(directive);
}