// Copyright (2022) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_operate_comqr.c
 * Auth: shichenyu (shichenyu01@baidu.com)
 * Desc: duerapp operate common qr.
 */
/**--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         INCLUDE                                         *
 **--------------------------------------------------------------------------*/

#include "mmidisplay_data.h"
#include "std_header.h"
#include "window_parse.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "mmk_timer.h"
#include "guiimg.h"
#include "guistring.h"
#include "guires.h"
#include "guilcd.h"
#include "guitext.h"
#include "guilabel.h"

#include "lightduer_log.h"
#include "lightduer_memory.h"
#include "lightduer_lib.h"
#include "lightduer_log.h" 
#include "baidu_json.h"

#include "qrencode.h"

#include "duerapp_id.h"
#include "duerapp_image.h"
#include "duerapp_common.h"
#include "duerapp_payload.h"
#include "duerapp_operate_comqr.h"

/**--------------------------------------------------------------------------*
 **                         DEFINE                                         *
 **--------------------------------------------------------------------------*/
/*
* 二维码版本 矩阵  纠错等级L的8容量
*      3     29         53
*     10     57         271
*     15     77         约470+
*/
#define DEFAULT_QR_VERSION  15       // 默认使用的二维码版本，version=15对应矩阵大小77，容量
#define DEFAULT_QR_SCALE    2        // 默认扩放比例

#define DEFAULT_TIP_WSTR    L"请扫描二维码"
/**--------------------------------------------------------------------------*
 **                         DECLARE                                        *
 **--------------------------------------------------------------------------*/
LOCAL MMI_RESULT_E HandleOpComQrWinMsg(
                                       MMI_WIN_ID_T       win_id,     // 窗口的ID
                                       MMI_MESSAGE_ID_E   msg_id,     // 窗口的内部消息ID
                                       DPARAM             param       // 相应消息的参数
                                       );
LOCAL void MMIDUERAPP_DisplayComQrImage(QRcode *code);
LOCAL void display_qrencoder(const char *url_data);

/**--------------------------------------------------------------------------*
 **                         STATIC VARIABLES                                 *
 **--------------------------------------------------------------------------*/
// 通用二维码弹框
WINDOW_TABLE( MMI_DUERAPP_OP_COMQR_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_OP_COMQR_WIN_ID ),
    WIN_FUNC((uint32) HandleOpComQrWinMsg ),
    CREATE_TEXT_CTRL(MMI_DUERAPP_OP_COM_QR_CONTENT_CTRL_ID),
    END_WIN
};

static wchar *gsp_wstr          = NULL;
static char  *gsp_qr_url        = NULL;

/**--------------------------------------------------------------------------*
 **                         FUNCTION                                        *
 **--------------------------------------------------------------------------*/
static void duer_operate_comqr_msg_clean(void)
{
    if (gsp_qr_url) {
        DUER_LOGI("%s free gsp_qr_url clean", __FUNCTION__);
        DUER_FREE(gsp_qr_url);
        gsp_qr_url = NULL;
    }
    if (gsp_wstr) {
        DUER_LOGI("%s free gsp_wstr clean", __FUNCTION__);
        DUER_FREE(gsp_wstr);
        gsp_wstr = NULL;
    }
}

PUBLIC MMI_HANDLE_T MMIDUERAPP_CreateOperateCommonQrWin(char *url, char *utf8_str)
{
    DUER_LOGI("%s", __FUNCTION__);
    if (!url) {
        DUER_LOGI("%s input url error", __FUNCTION__);
        return 0;
    }

    duer_operate_comqr_msg_clean();
    gsp_qr_url = DUER_CALLOC(1, strlen(url) + 1);
    DUER_MEMCPY(gsp_qr_url, url, strlen(url) + 1);

    //如果不指定显示文字，则显示成默认文字
    if (utf8_str) {
        unsigned int utf8_len = strlen(utf8_str);
        gsp_wstr = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
        if (gsp_wstr) {
            GUI_UTF8ToWstr(gsp_wstr, utf8_len, utf8_str, utf8_len);
        }
    }

    return MMK_CreateWin((uint32*)MMI_DUERAPP_OP_COMQR_WIN_TAB, PNULL);
}

LOCAL void WatchRec_MainwinDrawBG(void)
{
    GUI_POINT_T start_point = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
    GUIRES_DisplayImg(&start_point, PNULL, PNULL,
            MMI_DUERAPP_OP_COMQR_WIN_ID,
            IMAGE_DUERAPP_QR_FRAME,
            &lcd_dev_info);
}

LOCAL void MMIDUERAPP_CreateLoginText(wchar *content)
{
    GUI_RECT_T loginRect = {10,184,230,240};
    GUI_BG_T bg = {0};
    GUI_FONT_T font_size = SONG_FONT_24;
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    MMI_STRING_T querySting = {0};

    bg.bg_type  =  GUI_BG_COLOR;
    bg.color    = MMI_BLACK_COLOR;
    GUITEXT_SetBg(MMI_DUERAPP_OP_COM_QR_CONTENT_CTRL_ID,&bg);
    GUITEXT_IsDisplayPrg(FALSE, MMI_DUERAPP_OP_COM_QR_CONTENT_CTRL_ID );
    GUITEXT_SetAlign(MMI_DUERAPP_OP_COM_QR_CONTENT_CTRL_ID, ALIGN_HMIDDLE );
    GUITEXT_SetRect(MMI_DUERAPP_OP_COM_QR_CONTENT_CTRL_ID, &loginRect);
    GUITEXT_SetClipboardEnabled(MMI_DUERAPP_OP_COM_QR_CONTENT_CTRL_ID,FALSE);
    
    GUITEXT_SetFont(MMI_DUERAPP_OP_COM_QR_CONTENT_CTRL_ID, &font_size, &font_color);
    querySting.wstr_ptr = content;
    querySting.wstr_len = MMIAPICOM_Wstrlen(querySting.wstr_ptr);
    GUITEXT_SetString(MMI_DUERAPP_OP_COM_QR_CONTENT_CTRL_ID ,querySting.wstr_ptr,querySting.wstr_len,FALSE);
}

LOCAL MMI_RESULT_E HandleOpComQrWinMsg(
                                        MMI_WIN_ID_T        win_id,    //IN:
                                        MMI_MESSAGE_ID_E    msg_id,    //IN:
                                        DPARAM              param      //IN:
                                        )
{

    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    int ret = -1;
    wchar *def_wstr = NULL;
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW: {
            WatchRec_MainwinDrawBG();
        } break;
        case MSG_GET_FOCUS: {
        } break;
        case MSG_BACKLIGHT_TURN_ON: {
        } break;
        case MSG_BACKLIGHT_TURN_OFF: {
        } break;
        case MSG_LOSE_FOCUS: {
        } break;

        case MSG_FULL_PAINT: {
            WatchRec_MainwinDrawBG();
            if (gsp_wstr) {
                MMIDUERAPP_CreateLoginText(gsp_wstr);
            } else {
                def_wstr = DEFAULT_TIP_WSTR;
                MMIDUERAPP_CreateLoginText(def_wstr);
            }
            display_qrencoder(gsp_qr_url);
        } break;

        case MSG_END_FULL_PAINT: {
        } break;

        case MSG_CTL_OK:
        case MSG_CTL_CANCEL: {
            MMK_CloseWin(win_id);
        } break;
        case MSG_CLOSE_WINDOW: {
            duer_operate_comqr_msg_clean();
        } break;
        default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return (recode);
}

LOCAL void display_qrencoder(const char *url_data)
{
    wchar *error = L"二维码生成失败了";
    QRcode *code = NULL;

    if (!url_data) {
        duerapp_show_toast(error);
        return;
    }
    code = QRcode_encodeString(url_data, DEFAULT_QR_VERSION, QR_ECLEVEL_L, QR_MODE_8, 1); 
    if (code == NULL) {
        DUER_LOGW("QRcode_encodeString error!");
        return;
    }
    MMIDUERAPP_DisplayComQrImage(code);
    QRcode_free(code);
}


LOCAL void MMIDUERAPP_DisplayComQrImage(QRcode *code)
{
#if 1
    // 绘制二维码（SCALE）
    GUI_LCD_DEV_INFO lcd_dev = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T box = {0};
    int offset_x   = 43; //48;
    int offset_y   = 24; //29;
    unsigned char *row;
    int i = 0, j = 0;

    for (i = 0; i < code->width; i++) {
        row = code->data + (i * code->width);
        for (j = 0; j < code->width; j++) {
            if (row[j] & 0x1) {
                box.top = offset_y + i * DEFAULT_QR_SCALE;
                box.bottom = offset_y + (i + 1) * DEFAULT_QR_SCALE;
                box.left = offset_x + j * DEFAULT_QR_SCALE;
                box.right = offset_x + (j + 1) * DEFAULT_QR_SCALE;
                LCD_FillRect(&lcd_dev, box, MMI_BLACK_COLOR);
            }
        }
    }
#else
    // 绘制二维码（单像素）
    GUI_LCD_DEV_INFO    lcd_dev={GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    int offset_x   = 48;
    int offset_y   = 29;

    GUI_RECT_T box = {0};
    unsigned char *row;
    for (int i = 0; i < code->width; i++) {
        row = code->data + (i * code->width);
        for (int j=0; j < code->width; j++) {
            if (row[j] & 0x1) {
                box.top = offset_y + i;
                box.left = offset_x + j;
                LCD_DrawLine(&lcd_dev, 
                        box.left, 
                        box.top,
                        box.left, 
                        box.top,
                        MMI_BLACK_COLOR);
            }
        }
    }
#endif
}

