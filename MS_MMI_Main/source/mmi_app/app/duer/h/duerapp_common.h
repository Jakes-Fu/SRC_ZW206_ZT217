// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_login
 * Auth: liuwenshuai (liuwenshuai@baidu.com)
 * Desc: DCS handler header file.
 */
/**--------------------------------------------------------------------------*/
#ifndef _DUERAPP_COMMON_H_
#define _DUERAPP_COMMON_H_

#include "mmk_type.h"
#include "mmi_font.h"
#include "os_api.h"

/*----------------------------------------------------------------------------*/
/*                          STATIC DEFINITION                                 */
/*----------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------*/
/*                          ENUM DEFINITION                                   */
/*----------------------------------------------------------------------------*/
typedef enum _duer_asr_error{
    DUER_ERR_ASR_TIMEOUT             = -0x01,
    DUER_ERR_ASR_NO_TEXT             = -0x02,

    DUER_ERR_ASR_UNKOWN              = -0x10,
}duer_asr_error_t;

typedef enum _duer_card_type{
    DUER_CARD_TYPE_NOKNOWN           = 0x00,
    DUER_CARD_TYPE_TEXT              = 0x01,
    DUER_CARD_TYPE_STANDARD          = 0x02,
    DUER_CARD_TYPE_LIST              = 0x03,
}duer_card_type_t;

typedef enum _duer_standardcard_type{
    DUER_STANDARDCARD_TYPE_NOKNOWN   = 0x00,
    DUER_STANDARDCARD_TYPE_SRC       = 0x01,//图片资源URL
    DUER_STANDARDCARD_TYPE_LINK      = 0x02,//图片点击URL
    DUER_STANDARDCARD_TYPE_ALL       = 0x03,
}duer_standardcard_type_t;

typedef enum _duer_listcard_type{
    DUER_LISTCARD_TYPE_NOKNOWN   = 0x00,
    DUER_LISTCARD_TYPE_SRC       = 0x01,//图片资源URL
    DUER_LISTCARD_TYPE_TITLE     = 0x02,//标题
    DUER_LISTCARD_TYPE_CONTENT   = 0x03,//内容
    DUER_LISTCARD_TYPE_LINK      = 0x04,//图片点击URL
    DUER_LISTCARD_TYPE_ALL       = 0x05,
}duer_listcard_type_t;

typedef struct {
    _SIGNAL_VARS
    void (*func)(void *userData);
    void *userData;
} MMK_duer_call_func_struct;

#define CTRLANIM_SetCallBackFunc(...)

/*----------------------------------------------------------------------------*/
/*                          MACRO DEFINITION                                  */
/*----------------------------------------------------------------------------*/
#define MSG_DUERAPP_AUTH_STATUS       (MSG_DUERAPP_MUSIC_UPDATE + 1)
#define MSG_DUERAPP_INPUT_SCREEN      (MSG_DUERAPP_MUSIC_UPDATE + 2)
#define MSG_DUERAPP_DIRECTIVE_CARD    (MSG_DUERAPP_MUSIC_UPDATE + 3)
#define MSG_DUERAPP_PLAYER_INFO_CARD  (MSG_DUERAPP_MUSIC_UPDATE + 4)
#define MSG_DUERAPP_DUER_UPDATE       (MSG_DUERAPP_MUSIC_UPDATE + 5)
#define MSG_DUERAPP_MEDIA_BUTTON      (MSG_DUERAPP_MUSIC_UPDATE + 6)
#define MSG_DUERAPP_LOGIN_SUCCESS     (MSG_DUERAPP_MUSIC_UPDATE + 7)
#define MSG_DUERAPP_QR_LOGIN_SHOW     (MSG_DUERAPP_MUSIC_UPDATE + 8)

#define MSG_DUERAPP_LOGIN_USER_INFO   (MSG_DUERAPP_MUSIC_UPDATE + 9)
#define MSG_DUERAPP_QR_CALL_BACK      (MSG_DUERAPP_MUSIC_UPDATE + 10)
#define MSG_DUERAPP_FEEDBACK_CALL_BACK (MSG_DUERAPP_MUSIC_UPDATE + 11)
#define MSG_DUERAPP_OPERATE_CALL_BACK (MSG_DUERAPP_MUSIC_UPDATE + 12)
#define MSG_DUERAPP_LESSON_CALL_BACK  (MSG_DUERAPP_MUSIC_UPDATE + 13)
#define MSG_DUERAPP_DISCOVERY_CALL_BACK (MSG_DUERAPP_MUSIC_UPDATE + 14)
#define MSG_DUERAPP_SKILL_CALL_BACK   (MSG_DUERAPP_MUSIC_UPDATE + 15)
#define MSG_DUERAPP_ANIM_PIC_UPDATE   (MSG_DUERAPP_MUSIC_UPDATE + 16)
#define MSG_DUERAPP_ACTIVITY_CALL_BACK (MSG_DUERAPP_MUSIC_UPDATE + 17)

// Xiaodu Writing
#define MSG_DUERAPP_GRADE_SELECTED      (MSG_DUERAPP_MUSIC_UPDATE + 18)
#define MSG_DUERAPP_GOT_COMMENT         (MSG_DUERAPP_MUSIC_UPDATE + 19)
#define MSG_DUERAPP_VOICE_QUERY         (MSG_DUERAPP_MUSIC_UPDATE + 20)

#define MSG_DUERAPP_PAY_SUCESS     (MSG_DUERAPP_MUSIC_UPDATE + 21)
#define MSG_DUERAPP_ACTIVITY_SHOW     (MSG_DUERAPP_MUSIC_UPDATE + 22)
#define MSG_DUERAPP_GOT_COMPLIST        (MSG_DUERAPP_MUSIC_UPDATE + 23)

//DUER INPUT
#define MSG_DUERINPUT_MID_RESULT_SCREEN      (MSG_DUERAPP_MUSIC_UPDATE + 24)
#define MSG_DUERINPUT_FINAL_RESULT_SCREEN      (MSG_DUERAPP_MUSIC_UPDATE + 25)
#define MSG_DUERINPUT_TIMEOUT_RESULT_SCREEN      (MSG_DUERAPP_MUSIC_UPDATE + 26)

//DUER OTHER
#define MSG_DUERAPP_LOGIN_OUT          (MSG_DUERAPP_MUSIC_UPDATE + 27)
#define MSG_DUERAPP_MUSIC_PLAY_ERR     (MSG_DUERAPP_MUSIC_UPDATE + 28)
#define MSG_DUERAPP_QRPAY_SUCCESS      (MSG_DUERAPP_MUSIC_UPDATE + 29)
#define MSG_DUERAPP_SHOW_MIGU          (MSG_DUERAPP_MUSIC_UPDATE + 30)
#define MSG_DUERAPP_CLOSE_PLAY_INFO    (MSG_DUERAPP_MUSIC_UPDATE + 31)
#define MSG_DUERAPP_CLOSE_CENTER_INFO  (MSG_DUERAPP_MUSIC_UPDATE + 32)
#define MSG_DUERINPUT_CLOUD_ACK_RESULT_SCREEN      (MSG_DUERAPP_MUSIC_UPDATE + 33)
#define MSG_DUERAPP_TOAST              (MSG_DUERAPP_MUSIC_UPDATE + 34)
#define MSG_DUERAPP_QRCODE_TEMPLATE    (MSG_DUERAPP_MUSIC_UPDATE + 35)

//DUER DICT
#define MSG_DUERAPP_DICT_RENDER_EMPTY   (MSG_DUERAPP_MUSIC_UPDATE + 41)
#define MSG_DUERAPP_DICT_RENDER_DICTSEL (MSG_DUERAPP_MUSIC_UPDATE + 42)
#define MSG_DUERAPP_DICT_RENDER_NORMAL  (MSG_DUERAPP_MUSIC_UPDATE + 43)

//DUER ASSISTS
#define MSG_DUERAPP_ASSISTS_WORDS_UPDATE   (MSG_DUERAPP_MUSIC_UPDATE + 50)
#define MSG_DUERAPP_ASSISTS_RESPONSE_RESULT    (MSG_DUERAPP_MUSIC_UPDATE + 51)
#define MSG_DUERAPP_ASSISTS_RESPONSE_TRANS    (MSG_DUERAPP_MUSIC_UPDATE + 52)

#define TEN_YEARS_1970_1980_SECONDS 315504000

//DUER Permission Print
PUBLIC char *duerapp_TimeStampGet(void);//duerapp_TimeStampGet
#define DuerPermissionLog(fmt, ...) do { \
                                    SCI_TraceLow("CNAL [%s]<%s>[%s][%s][%s,%d]:" fmt, duerapp_TimeStampGet(), "xiaodu", "xiaodu", "com.baidu.voicesearch", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                   } while (0)

/*----------------------------------------------------------------------------*/
int duerapp_timestamp();
char *duerapp_get_nonce();

void duerapp_show_toast(wchar *toast_ptr);
void duerapp_close_waiting();
void duerapp_show_waiting(wchar *wstr_ptr);

unsigned int duerapp_parse_color(char *color_string);

PUBLIC uint16 duerapp_DisplayLineTextInRect(
                            GUI_LCD_DEV_INFO *lcd_dev_ptr,
                            GUI_RECT_T      *rect_ptr,
                            GUI_FONT_T       font,
                            GUI_COLOR_T      font_color,
                            wchar            *wstr_ptr,
                            uint16           wstr_len
                            );

PUBLIC void duerapp_show_waiting_text();
PUBLIC void duerapp_send_query(char *query_text);
PUBLIC void duer_print_long_str(char* str);

PUBLIC void MMK_duer_call_func(void *param);
PUBLIC void MMK_duer_other_task_to_MMI( 
                                     MMI_HANDLE_T     handle,
                                     MMI_MESSAGE_ID_E msg_id,
                                     DPARAM           param_ptr,
                                     uint32           size_of_param
                                     );

#endif