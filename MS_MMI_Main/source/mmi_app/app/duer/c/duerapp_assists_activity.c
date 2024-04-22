// Copyright (2022) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_assists_activity.c
 * Auth: Wanglusong (wanglusong01@baidu.com)
 * Desc: duerapp xiaodu assista
 */

#include "std_header.h"
#include "window_parse.h"
#include "mmk_app.h"
#include "ctrlsetlist_export.h"
#include "ctrlownerdraw_export.h"
#include "dal_time.h"
#include "guilcd.h"
#include "guifont.h"
#include "guilistbox.h"
#include "guibutton.h"
#include "guitext.h"
#include "guilabel.h"
#include "guirichtext.h"
#include "guisetlist.h"
#include "gui_ucs2b_converter.h"
#include "mmipub.h"
#include "mmi_textfun.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "mmi_event_api.h"
#include "mmi_applet_table.h"
#include "mmidisplay_data.h"
// #include "mmiwifi_export.h"
#include "mmiphone_export.h"
#include "mmicom_time.h"
#include "watch_common_btn.h"
#include "watch_common_list.h"
#include "watch_commonwin_export.h"

#include "qrencode.h"

#include "lightduer_log.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_types.h"
#include "lightduer_ap_info.h"

#include "duerapp.h"
#include "duerapp_id.h"
#include "duerapp_text.h"
#include "duerapp_image.h"
#include "duerapp_anim.h"
#include "duerapp_nv.h"
#include "duerapp_main.h"
#include "duerapp_openapi.h"
#include "duerapp_common.h"
#include "duerapp_http.h"
#include "duerapp_statistics.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_res_down.h"
#include "duerapp_homepage.h"
#include "duerapp_main.h"
#include "duerapp_assists_activity.h"

/**************************************************************************************************************************
 *                                                        STATIC VARIABLES                                                
 **************************************************************************************************************************/

/**************************************************************************************************************************
 *                                                        STATIC FUNCTIONS                                                
 *************************************************************************************************************************/
#if defined(DUERAPP_ASSISTS_VIP_TRANSMIT_MESSAGE)
LOCAL MMI_RESULT_E  PRV_DUERAPP_TransmitMessageHandleMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         );
LOCAL void PRV_DUERAPP_TransmitMessageUiDraw(void);

LOCAL MMI_RESULT_E PRV_DUERAPP_MessageSelectImgButtonCb(MMI_HANDLE_T ctrl_handle);

#endif

#if defined(DUERAPP_ASSISTS_VIP_MESSAGE_SELECT)
LOCAL MMI_RESULT_E  PRV_DUERAPP_MessageSelectHandleMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         );
LOCAL void PRV_DUERAPP_MessageSelectUiDraw(void);
#endif

LOCAL void PRV_DUERAPP_ScreenInputTexUpdate(char *in_text);

#if defined(DUERAPP_ASSISTS_VIP_TRANSMIT_MESSAGE)
WINDOW_TABLE(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_TAB) =
{
    WIN_HIDE_STATUS,
    WIN_ID(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_ID),
    WIN_FUNC((uint32)PRV_DUERAPP_TransmitMessageHandleMsg),
    
    END_WIN
};
#endif

#if defined(DUERAPP_ASSISTS_VIP_MESSAGE_SELECT)
WINDOW_TABLE(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_TAB) =
{
    WIN_HIDE_STATUS,
    WIN_ID(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID),
    WIN_FUNC((uint32)PRV_DUERAPP_MessageSelectHandleMsg),
    
    END_WIN
};

LOCAL wchar *message_select_text_array[DUERAPP_ASSISTS_WORDS_NUMBER] = {
    L"°Ö£¬ÎÒÕý¸úÍ¬Ñ§PKµ¥´Ê£¬ÏÂÒ»¹ØÐèÒª»áÔ±£¬Çó¿ªÍ¨£¡",
    L"Âè£¬Ð¡¶ÈÀïÃæÓÐºÜ¶à×÷ÎÄ·¶ÎÄ£¬ÎÒÐèÒª²Î¿¼£¬°ï¿ª¸ö»áÔ±",
    L"ÀÏ°Ö£¬°ïÎÒ¿ª¸ö»áÔ±£¬ÎÒÐèÒªÓÃÕâ¸öÌý¿ÎÎÄ",
    L"Âè£¬ÎÒÏëÌýÕâ¸ö¹ÊÊÂ£¬ÐèÒª»áÔ±£¬°ïÎÒ¿ªÍ¨ÏÂ°É",
    L"ÂèÂè£¬°ïÎÒ¿ªÍ¨ÏÂ»áÔ±°É£¬½ñÌìÍí·¹ÎÒÏ´Íë£¡",
    L"Âè£¬ÎÒÕâÖÜ±£Ö¤ºÃºÃÐ´×÷Òµ£¬°´Ê±Íê³É£¬°ïÎÒ¿ª¸ö»áÔ±",
    L"°Ö£¬ÎÒÍ¬Ñ§¶¼ÔÚÐ¡¶ÈÉÏ¿ª»áÔ±Ñ§Ï°£¬ÎÒÒ²Òª",
    L"Âè£¬ÎÒÕâ´Î¿¼ÁË°àÀïµÚÒ»£¬¸ø¸ö½±Àø£¬°ïÃ¦¿ª¸ö»áÔ±°É",
};
#endif

// LOCAL assists_info_t assists_attr = {
//     .last_button_id = -1,
// };
LOCAL assists_info_t assists_attr;

LOCAL uint8 assists_input_click_cnt = 0;

#if defined(DUERAPP_ASSISTS_COMMON_FUNCTION)

LOCAL void PRV_DUERAPP_Toast(wchar *wtxt)
{
    WATCH_SOFTKEY_TEXT_ID_T softkey={COMMON_TXT_NULL, COMMON_TXT_NULL, COMMON_TXT_NULL};
    MMI_STRING_T tipSting = {0};
    tipSting.wstr_ptr = wtxt;
    tipSting.wstr_len = MMIAPICOM_Wstrlen(wtxt);
    WatchCOM_NoteWin_2Line_Enter(MMI_DUERAPP_QUERY_WIN_ID, &tipSting, 0, softkey, PNULL);
}

LOCAL void PRV_DUERAPP_WinDrawBG(MMI_WIN_ID_T win_id, GUI_COLOR_T color)
{
    GUI_RECT_T win_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUI_RECT_T clientRect = DUERAPP_FULL_SCREEN_RECT;

    MMK_GetWinRect(win_id, &win_rect);
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    win_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, clientRect);
    GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);
}

LOCAL int PRV_DUERAPP_UserListResponse(void *p_user_ctx, duer_http_data_pos_t pos,
                              const char *body, size_t len, const char *type)
{
    int ret = DUER_ERR_FAILED;
    int body_len = 0;
    char *pTmpStr = NULL;
//    duer_print_long_str(body);//test printf
    baidu_json *info = NULL;
    baidu_json *list_item = NULL;
    baidu_json *list_array = baidu_json_GetObjectItem(info, "data");
    // int len = 0;
    baidu_json *openId_item = NULL;
    baidu_json *nickname_item = NULL;
    char *openId_value = NULL;
    char *nickname_value = NULL;
    int i = 0;
    MMI_STRING_T wstr = {0};
    
    if (body && pos == DUER_HTTP_DATA_FIRST) {
        if (assists_attr.http_response_body) {
            DUER_FREE(assists_attr.http_response_body);
            assists_attr.http_response_body = NULL;
        }
        assists_attr.http_response_body = DUER_CALLOC(1, strlen(body) + 1);
        strcpy(assists_attr.http_response_body, body);
        return DUER_OK;
    } else if (body && pos == DUER_HTTP_DATA_MID) {
        body_len = strlen(body);
        pTmpStr = DUER_REALLOC(assists_attr.http_response_body, strlen(assists_attr.http_response_body) + body_len + 1);
        if (pTmpStr) {
            assists_attr.http_response_body = pTmpStr;
        }
        strncat(assists_attr.http_response_body, body, body_len);
        return DUER_OK;
    }

    if (!body && pos == DUER_HTTP_DATA_LAST) {
        DUER_LOGI("(%s)(%d)[duer_watch]:receiver end", __FUNCTION__, __LINE__);
    }
    
    info = baidu_json_Parse(assists_attr.http_response_body);
    if (!info) {
        if (assists_attr.http_response_body) {
            DUER_FREE(assists_attr.http_response_body);
            assists_attr.http_response_body = NULL;
        }
        DUER_LOGI("(%s)(%d)[duer_watch]:error, NULL json", __FUNCTION__, __LINE__);
        return ret;
    }
    
    do 
    {
        list_array = baidu_json_GetObjectItem(info, "data");
        len = baidu_json_GetArraySize(list_array);
        if (len == 0) {
            if (assists_attr.http_response_body) {
                DUER_FREE(assists_attr.http_response_body);
                assists_attr.http_response_body = NULL;
            }
            DUER_LOGI("(%s)(%d)[duer_watch]:len 0", __FUNCTION__, __LINE__);
            break;
        }
        assists_attr.binding_number = len;
        
        for (i = 0; i < len; i++) {
            list_item = baidu_json_GetArrayItem(list_array,i);
            if (list_item) {
                    openId_item = baidu_json_GetObjectItem(list_item, "openId");
                    if (openId_item) {
                        openId_value = openId_item->valuestring;
                        strcpy(assists_attr.bind_info[i].openId, openId_value);
                        DUER_LOGI("(%s)(%d)[duer_watch]:openId(%s)", __FUNCTION__, __LINE__, openId_value);
                    }
                    
                    nickname_item = baidu_json_GetObjectItem(list_item, "nickname");
                    if (openId_item) {
                        nickname_value = nickname_item->valuestring;
                        strcpy(assists_attr.bind_info[i].nickname, nickname_value);
                        DUER_LOGI("(%s)(%d)[duer_watch]:nickname(%s)", __FUNCTION__, __LINE__, nickname_value);
                    }
            }
        }
        
        if (assists_attr.binding_number > 0) {
            MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_ASSISTS_WIN_ID, MSG_DUERAPP_ASSISTS_RESPONSE_TRANS, PNULL, 0);
        } else {
            wstr.wstr_ptr = L"ÎÞ°ó¶¨ÓÃ»§";
            MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_ASSISTS_WIN_ID, MSG_DUERAPP_ASSISTS_RESPONSE_RESULT, &wstr, sizeof(MMI_STRING_T));
        }
    } while(0);
    
    if (assists_attr.http_response_body) {
        DUER_FREE(assists_attr.http_response_body);
        assists_attr.http_response_body = NULL;
    }

    if (info) {
        baidu_json_Delete(info);
    }
    return DUER_OK;
}

LOCAL int PRV_DUERAPP_ParentsAssistsResponse(void *p_user_ctx, duer_http_data_pos_t pos,
                              const char *body, size_t len, const char *type)
{
    int ret = DUER_ERR_FAILED;
    int body_len = 0;
    char *pTmpStr = NULL;
//    duer_print_long_str(body);//test printf
    baidu_json *info = NULL;
    baidu_json *response_item = NULL;
    baidu_json *status_item = NULL;
    baidu_json *msg_item = NULL;
    char *value_str = NULL;
    int value_int = -1;
    char *nickname_value = NULL;
    MMI_STRING_T wstr = {0};
    
    if (body && pos == DUER_HTTP_DATA_FIRST) {
        if (assists_attr.http_response_body) {
            DUER_FREE(assists_attr.http_response_body);
            assists_attr.http_response_body = NULL;
        }
        assists_attr.http_response_body = DUER_CALLOC(1, strlen(body) + 1);
        strcpy(assists_attr.http_response_body, body);
        return DUER_OK;
    } else if (body && pos == DUER_HTTP_DATA_MID) {
        
        
        body_len = strlen(body);
        pTmpStr = DUER_REALLOC(assists_attr.http_response_body, strlen(assists_attr.http_response_body) + body_len + 1);
        if (pTmpStr) {
            assists_attr.http_response_body = pTmpStr;
        }
        strncat(assists_attr.http_response_body, body, body_len);
        return DUER_OK;
    }

    if (!body && pos == DUER_HTTP_DATA_LAST) {
        DUER_LOGI("(%s)(%d)[duer_watch]:receiver end", __FUNCTION__, __LINE__);
    }
    
    info = baidu_json_Parse(assists_attr.http_response_body);
    if (!info) {
        if (assists_attr.http_response_body) {
            DUER_FREE(assists_attr.http_response_body);
            assists_attr.http_response_body = NULL;
        }
        DUER_LOGI("(%s)(%d)[duer_watch]:error, NULL json", __FUNCTION__, __LINE__);
        return ret;
    }
    
    do 
    {   
        response_item = baidu_json_GetObjectItem(info, "data");
        if (response_item) {
            value_str = response_item->valuestring;
            DUER_LOGI("(%s)(%d)[duer_watch]:data(%s)", __FUNCTION__, __LINE__, value_str);
        }
        
        status_item = baidu_json_GetObjectItem(info, "status");
        if (status_item) {
            value_int = status_item->valueint;
            DUER_LOGI("(%s)(%d)[duer_watch]:status(%d)", __FUNCTION__, __LINE__, value_int);
            switch (value_int) {
                case 0: 
                {
                    wstr.wstr_ptr = L"ÖúÁ¦·¢ËÍ³É¹¦";
                    break;
                }
                case 3008: 
                {
                    wstr.wstr_ptr = L"ÖúÁ¦·¢ËÍÊ§°Ü£¬Ã»ÓÐÕÒµ½°ó¶¨µÄÎ¢ÐÅÓÃ»§";
                    break;
                }
                case 3009: 
                {
                    wstr.wstr_ptr = L"½ñÌìÒÑ¾­·¢ËÍ¹ýÀ²£¬³¢ÊÔµç»°ÁªÏµÏÂ°ÖÂè¿´¿´Å¶";
                    break;
                }
            }
            wstr.wstr_len = MMIAPICOM_Wstrlen(wstr.wstr_ptr);
            MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_ASSISTS_WIN_ID, MSG_DUERAPP_ASSISTS_RESPONSE_RESULT, &wstr, sizeof(MMI_STRING_T));
        }
        
        msg_item = baidu_json_GetObjectItem(info, "msg");
        if (msg_item) {
            value_str = msg_item->valuestring;
            DUER_LOGI("(%s)(%d)[duer_watch]:msg(%s)", __FUNCTION__, __LINE__, value_str);
        }
    } while(0);
    
    if (assists_attr.http_response_body) {
        DUER_FREE(assists_attr.http_response_body);
        assists_attr.http_response_body = NULL;
    }

    if (info) {
        baidu_json_Delete(info);
    }
    return DUER_OK;
}

LOCAL  void PRV_DUERAPP_ScreenInputTexUpdate(char *in_text)
{
    MMI_STRING_T tmp_str = {0};
    wchar *wch_txt = NULL;
    char *tmp_text = in_text;
    int utf8_len = 0;

    tmp_text = in_text;
    utf8_len = strlen(in_text);
    wch_txt = DUER_CALLOC(1, (utf8_len + 1)*sizeof(wchar));
    
    if (wch_txt)
    {
        GUI_UTF8ToWstr(wch_txt, utf8_len, (const uint8 *)tmp_text, utf8_len);
        CTRLBASEFLEX_SetString(MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_EDIT_CTRL_ID, wch_txt, MMIAPICOM_Wstrlen(wch_txt));
        GUIEDIT_SetFontColor(MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_EDIT_CTRL_ID, MMI_BLACK_COLOR);
        DUER_FREE(wch_txt);
        wch_txt = NULL;
        CTRLBASEFLEX_GetString(MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_EDIT_CTRL_ID, &tmp_str);
        if (tmp_str.wstr_len >= 24) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:max len", __func__, __LINE__);
        }
        MMK_SendMsg(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

#endif

#if defined(DUERAPP_ASSISTS_VIP_SCAN_QR_CODE)

LOCAL QRcode *s_assists_ownerdraw_qrcode = NULL;
LOCAL Pt_QR_LOGIN_URL_INFO * s_assists_qr_login_info = NULL;
LOCAL char s_assists_login_sta_url[256] = {0};

LOCAL MMI_RESULT_E PRV_DUERAPP_AssistsHandleMsg(
                                       MMI_WIN_ID_T       win_id,     // ´°¿ÚµÄID
                                       MMI_MESSAGE_ID_E   msg_id,     // ´°¿ÚµÄÄÚ²¿ÏûÏ¢ID
                                       DPARAM             param       // ÏàÓ¦ÏûÏ¢µÄ²ÎÊý
                                       );

LOCAL void PRV_DUERAPP_AssistsOwnerDrawCb(GUIOWNDRAW_INFO_T *owner_draw_ptr);

WINDOW_TABLE( MMI_DUERAPP_VIP_ASSISTS_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_VIP_ASSISTS_WIN_ID ),
    WIN_FUNC((uint32) PRV_DUERAPP_AssistsHandleMsg ),
    END_WIN
};

LOCAL void PRV_DUERAPP_AssistsOwnerDrawCb(GUIOWNDRAW_INFO_T *owner_draw_ptr)
{
    GUI_LCD_DEV_INFO lcd_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    int offset_x   = 48;
    int offset_y   = 5;
    int tmp_top   = 0;
    GUI_RECT_T rect = {0};
    uint8 *row = NULL;
    int i = 0, j = 0;
    
    if (owner_draw_ptr == NULL) {
        return;
    }
    
    //·Ç³£·Ç³£ÖØÒª£¬´ËÉèÖÃ¶¯Ì¬»æÖÆ¶þÎ¬Âë£¬»¬¶¯»á²»¶ÏµÄcb
    if (owner_draw_ptr->ctrl_rect.top < 0) {
        offset_y += owner_draw_ptr->ctrl_rect.top;//»»Ëã×ø±êµã, Ëã·¨¿´log±ãÖª£¬top³öÆÁ±ä¸ºÊý
    } else {
        offset_y += owner_draw_ptr->display_rect.top;
    }
    offset_x += owner_draw_ptr->display_rect.left;
    
//    SCI_TraceLow("(%d)[duer_watch]:left(%d), dis_t(%d), dis_b(%d), ctrl_t(%d), ctrl_b(%d), show_t(%d)", __LINE__, owner_draw_ptr->display_rect.left, owner_draw_ptr->display_rect.top, \
//    owner_draw_ptr->display_rect.bottom, owner_draw_ptr->ctrl_rect.top, owner_draw_ptr->ctrl_rect.bottom, offset_y);
    
    if (s_assists_ownerdraw_qrcode == NULL) {
        DUER_LOGI("(%s)(%d)[duer_watch]:error", __FUNCTION__, __LINE__);
        return;
    }
    
    if (s_assists_ownerdraw_qrcode->data && s_assists_ownerdraw_qrcode->width > 0) {
        for (i = 0; i < s_assists_ownerdraw_qrcode->width; i++) {//29
            row = s_assists_ownerdraw_qrcode->data + (i * s_assists_ownerdraw_qrcode->width);
            for (j = 0; j < s_assists_ownerdraw_qrcode->width; j++) {
                if (row[j] & 0x1) {
                    rect.top = offset_y + i * 5;//5-ï¿½Å´ï¿½5ï¿½ï¿½
                    rect.bottom = offset_y + (i+1) * 5;
                    rect.left = offset_x + j * 5;
                    rect.right = offset_x + (j+1) * 5;
                    GUI_FillRect(&lcd_info, rect, MMI_BLACK_COLOR);
                }
            }
        }
    } else {
        SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
    }
}

LOCAL void PRV_DUERAPP_AssistsDisplay(MMI_WIN_ID_T win_id, const char *url_data)
{
    QRcode *code = NULL;
    wchar *error = L"¶þÎ¬ÂëÉú³ÉÊ§°ÜÁË";

    if (!url_data) {
        duerapp_show_toast(error);
        return;
    }
    
    if (s_assists_ownerdraw_qrcode) {
        QRcode_free(s_assists_ownerdraw_qrcode);
        s_assists_ownerdraw_qrcode = NULL;
    }
    code = QRcode_encodeString(url_data, 3, QR_ECLEVEL_L, QR_MODE_8, 1);//Ê¹ÓÃ¶þÎ¬Âë°æ±¾-3
    
    if (code == NULL) {
        DUER_LOGI("(%s)(%d)[duer_watch]:error", __FUNCTION__, __LINE__);
        return;
    }
    
    s_assists_ownerdraw_qrcode = code;
    CTRLOWNERDRAW_Update(MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID);
}

LOCAL MMI_RESULT_E PRV_DUERAPP_AssistsEditButtonCb(MMI_HANDLE_T ctrl_handle)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MMI_STRING_T show_str = {0};
    MMI_HANDLE_T ctrl_id = MMK_GetCtrlId(ctrl_handle);
    int ret = 0;

    switch (ctrl_id)
    {
        case MMI_DUERAPP_ASSISTS_FORM1_FORM2_BUTTON_CTRL_ID :
        {
            ret = PUB_DUERAPP_UserListRequest();
            if (ret) {
                MMI_STRING_T wshowtxt = {0};
                wshowtxt.wstr_ptr = L"ÇëÇó°ó¶¨Î¢ÐÅºÅÊ§°Ü!";
                wshowtxt.wstr_len = MMIAPICOM_Wstrlen(wshowtxt.wstr_ptr);
                MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_ASSISTS_WIN_ID, MSG_DUERAPP_ASSISTS_RESPONSE_RESULT, &wshowtxt, sizeof(MMI_STRING_T));
            }
            break;
        }
        
        default:
        {
            break;
        }
    }
    SCI_TraceLow("(%s)(%d)[duer_watch]:button cb", __func__, __LINE__);
    
    return result;
}

LOCAL void PRV_DUERAPP_AssistsCtrlDestory(BOOLEAN is_create)
{
    static BOOLEAN is_have_create = FALSE;
    BOOLEAN state = FALSE;
    int i = 0;
    int number = 0;
    
    if (is_create) {
        if (is_have_create) {
            state = TRUE;
        }
        is_have_create = is_create;
        if (state) {
            goto DESTORY;
        }
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            goto DESTORY;
        }
    }
    return;
DESTORY:
    number = MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID - MMI_DUERAPP_ASSISTS_FORM_CTRL_ID;
    for (i = number; i >= 0; i--) {
        MMK_DestroyControl(MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID - (number - i));
    }
}

LOCAL MMI_HANDLE_T PRV_DUERAPP_AssistsCtrlCreate(MMI_HANDLE_T win_handle)
{
    MMI_HANDLE_T ctrl_handle = 0;
    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_ASSISTS_FORM_CTRL_ID;
    BOOLEAN is_login = duerapp_is_login();
    GUIFORM_INIT_DATA_T childform_init_data = {0};
    GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
    MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_CTRL_ID;
    // GUIFORM_INIT_DATA_T childform_init_data = {0};
    // GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
    // MMI_HANDLE_T child_form_ctrl_id = 0;
        
    GUIBUTTON_INIT_DATA_T buttom_init_data = {0};
    GUIFORM_DYNA_CHILD_T button_form_child_ctrl = {0};
    
    GUILABEL_INIT_DATA_T label_init_data = {0};
    GUIFORM_DYNA_CHILD_T label_form_child_ctrl = {0};

    // MMI_HANDLE_T child_form_ctrl_id = 0;
        
    // GUIBUTTON_INIT_DATA_T buttom_init_data = {0};
    // GUIFORM_DYNA_CHILD_T button_form_child_ctrl = {0};
    
    // GUILABEL_INIT_DATA_T label_init_data = {0};
    // GUIFORM_DYNA_CHILD_T label_form_child_ctrl = {0};
    
    GUIOWNDRAW_INIT_DATA_T owndraw_init_data = {0};
    GUIFORM_DYNA_CHILD_T owndraw_form_child_ctrl = {0};
    
    //0ÊÍ·Å
    PRV_DUERAPP_AssistsCtrlDestory(TRUE);

    //1¶¯Ì¬´´½¨form
    {
        GUIFORM_CreatDynaCtrl(win_handle, ctrl_id, GUIFORM_LAYOUT_ORDER);
    }

    //2¶¯Ì¬´´½¨×Óform
    {
        
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        //µÚ1ÐÐ
        if (is_login) {
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM1_CTRL_ID;
            GUIFORM_CreatDynaChildCtrl(win_handle, ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
        
        //µÚ2ÐÐ
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM2_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
    }

    //3¶¯Ì¬´´½¨×Óform×Óform1ºÍ×Óform2
    {
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_CTRL_ID;
        // childform_init_data = {0};
        // childform_form_child_ctrl = {0};
        memset(&childform_init_data, 0x00, sizeof(childform_init_data));
        memset(&childform_form_child_ctrl, 0x00, sizeof(childform_form_child_ctrl));
        
        //ID
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        
        //À®°ÈºÍ1.º°°ÖÂèÀ´ÖúÁ¦
        if (is_login) {
            childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM1_FORM1_CTRL_ID;
            childform_form_child_ctrl.init_data_ptr = &childform_init_data;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
            //±êÌâºÍ±à¼­°´Å¥
            childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM1_FORM2_CTRL_ID;
            childform_form_child_ctrl.init_data_ptr = &childform_init_data;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
        
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_CTRL_ID;
        //À®°ÈºÍ1.º°°ÖÂèÀ´É¨Âë
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM2_FORM1_CTRL_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        //±êÌâºÍ¶þÎ¬Âë
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM2_FORM2_CTRL_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
    }

    //4¶¯Ì¬´´½¨-º°°ÖÂèÀ´ÖúÁ¦
    if (is_login) {
        child_form_ctrl_id = 0;
        
        // buttom_init_data = {0};
        // button_form_child_ctrl = {0};
        
        // label_init_data = {0};
        // label_form_child_ctrl = {0};
        memset(&buttom_init_data, 0x00, sizeof(buttom_init_data));
        memset(&button_form_child_ctrl, 0x00, sizeof(button_form_child_ctrl));

        memset(&label_init_data, 0x00, sizeof(label_init_data));
        memset(&label_form_child_ctrl, 0x00, sizeof(label_form_child_ctrl));

        
        //ID
        button_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        label_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        //¶¯Ì¬´´½¨form1×Óform1µÄÀ®°ÈºÍ1.º°°ÖÂèÀ´ÖúÁ¦
        button_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM1_FORM1_IMAGE_CTRL_ID;
        button_form_child_ctrl.init_data_ptr = &buttom_init_data;
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &button_form_child_ctrl);
        
        label_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM1_FORM1_LABEL_CTRL_ID;
        label_form_child_ctrl.init_data_ptr = &label_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
        
        //¶¯Ì¬´´½¨form1×Óform2µÄ±êÌâºÍ¶þÎ¬Âë
        label_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM1_FORM2_LABEL_CTRL_ID;
        label_form_child_ctrl.init_data_ptr = &label_init_data;
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM2_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
        
        button_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM1_FORM2_BUTTON_CTRL_ID;
        button_form_child_ctrl.init_data_ptr = &buttom_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &button_form_child_ctrl);
    }

    //5¶¯Ì¬´´½¨-º°°ÖÂèÀ´É¨Âë
    {
        child_form_ctrl_id = 0;
        
        // buttom_init_data = {0};
        // button_form_child_ctrl = {0};
        
        // label_init_data = {0};
        // label_form_child_ctrl = {0};
        
        // owndraw_init_data = {0};
        // owndraw_form_child_ctrl = {0};
        memset(&buttom_init_data, 0x00, sizeof(buttom_init_data));
        memset(&button_form_child_ctrl, 0x00, sizeof(button_form_child_ctrl));

        memset(&label_init_data, 0x00, sizeof(label_init_data));
        memset(&label_form_child_ctrl, 0x00, sizeof(label_form_child_ctrl));

        memset(&owndraw_init_data, 0x00, sizeof(owndraw_init_data));
        memset(&owndraw_form_child_ctrl, 0x00, sizeof(owndraw_form_child_ctrl));
        
        //ID
        button_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        label_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        owndraw_form_child_ctrl.guid = SPRD_GUI_OWNDRAW_ID;
        
        //¶¯Ì¬´´½¨form2×Óform1µÄÀ®°ÈºÍ1.º°°ÖÂèÀ´É¨Âë
        button_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM2_FORM1_IMAGE_CTRL_ID;
        button_form_child_ctrl.init_data_ptr = &buttom_init_data;
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &button_form_child_ctrl);
        
        label_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM2_FORM1_LABEL_CTRL_ID;
        label_form_child_ctrl.init_data_ptr = &label_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
        
        //¶¯Ì¬´´½¨form1×Óform2µÄ±êÌâºÍ¶þÎ¬Âë
        label_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM2_FORM2_LABEL_CTRL_ID;
        label_form_child_ctrl.init_data_ptr = &label_init_data;
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_FORM2_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
        
        owndraw_init_data.OwnDrawFunc = PRV_DUERAPP_AssistsOwnerDrawCb;
        owndraw_form_child_ctrl.child_handle = MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID;
        owndraw_form_child_ctrl.init_data_ptr = &owndraw_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &owndraw_form_child_ctrl);
    }

    ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, ctrl_id);
    
    return ctrl_handle;
}

LOCAL void PRV_DUERAPP_AssistsUiDraw(void)
{
    MMI_HANDLE_T form_ctrl_handle = PRV_DUERAPP_AssistsCtrlCreate(MMI_DUERAPP_VIP_ASSISTS_WIN_ID);
    int i = 0;
    BOOLEAN is_login = duerapp_is_login();
    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
    GUI_RECT_T form_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
    IGUICTRL_T *IGuiCtrl = MMK_GetCtrlPtr(form_ctrl_handle);
    MMI_HANDLE_T child_form_ctrl_id = 0;
    GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
    GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    // GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
    
    uint16 hor_space = 1;//Ë®Æ½ï¿½ï¿½ï¿?
    uint16 ver_space = 1;//ï¿½ï¿½Ö±ï¿½ï¿½ï¿?
    // IGUICTRL_T *IGuiCtrl = NULL;

    // MMI_HANDLE_T child_form_ctrl_id = 0;
    // GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
    // GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    // GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
    
    // uint16 hor_space = 1;//Ë®Æ½ï¿½ï¿½ï¿?
    // uint16 ver_space = 1;//ï¿½ï¿½Ö±ï¿½ï¿½ï¿?

    // MMI_HANDLE_T child_form_ctrl_id = 0;
    // GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
    // GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    GUI_PADDING_T padding = {0};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    MMI_HANDLE_T label_ctrl_id = 0;
    GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
    GUI_FONT_T label_font_size = 0;
    MMI_STRING_T label_wshowtxt = {0};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    MMI_HANDLE_T button_ctrl_id = 0;
    GUI_BG_T voice_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_TRUMPET_BUTTON, MMI_WHITE_COLOR, TRUE};
    GUI_BG_T edit_button_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_SEND_BUTTON, MMI_WHITE_COLOR, TRUE};
    GUI_FONT_ALL_T font_info = {DUERAPP_ASSISTS_EDIT_BUTTON_FONT, MMI_WHITE_COLOR};
    MMI_STRING_T button_text = {0};

    // MMI_HANDLE_T child_form_ctrl_id = 0;
    // GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
    // GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    // GUI_PADDING_T padding = {0};
    // GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    // MMI_HANDLE_T label_ctrl_id = 0;
    // GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
    // GUI_FONT_T label_font_size = 0;
    // MMI_STRING_T label_wshowtxt = {0};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    // MMI_HANDLE_T button_ctrl_id = 0;
    // GUI_BG_T voice_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_TRUMPET_BUTTON, MMI_WHITE_COLOR, TRUE};
    // GUI_BG_T edit_button_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_SEND_BUTTON, MMI_WHITE_COLOR, TRUE};
    // GUI_FONT_ALL_T font_info = {DUERAPP_ASSISTS_EDIT_BUTTON_FONT, MMI_WHITE_COLOR};
    // MMI_STRING_T button_text = {0};

    //1ÉèÖÃ-form²ÎÊý
    {
        // form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
        memset(&form_bg, 0x00, sizeof(form_bg));
        form_bg.bg_type = GUI_BG_COLOR;
        form_bg.shape = GUI_SHAPE_ROUNDED_RECT;
        form_bg.img_id = 0;
        form_bg.color = MMI_BLACK_COLOR;
        form_bg.is_screen_img = FALSE;

        // form_rect = {0, 0, 240, 240};
        memset(&form_rect, 0x00, sizeof(form_rect));
        form_rect.left = 0;
        form_rect.top = 0;
        form_rect.right = 240;

        IGuiCtrl = MMK_GetCtrlPtr(form_ctrl_handle);
        
        CTRLFORM_SetRect(form_ctrl_handle, &form_rect);
        CTRLFORM_SetBg(form_ctrl_handle, &form_bg);
        CTRLFORM_PermitChildBg(form_ctrl_handle, FALSE);
        CTRLFORM_PermitChildFont(form_ctrl_handle, FALSE);
        CTRLFORM_PermitChildBorder(form_ctrl_handle, FALSE);
        // GUICTRL_SetProgress(IGuiCtrl, FALSE);
    }

    //2ÉèÖÃ-form×Óform1ºÍ×Óform2²ÎÊý
    {
        child_form_ctrl_id = 0;
        // width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        memset(&width, 0x00, sizeof(width));
        width.add_data = 0;
        width.type = GUIFORM_CHILD_WIDTH_FIXED;
        // height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        memset(&height, 0x00, sizeof(height));
        height.add_data = 0;
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;
        // form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
        memset(&form_bg, 0x00, sizeof(form_bg));
        form_bg.bg_type = GUI_BG_COLOR;
        form_bg.shape = GUI_SHAPE_ROUNDED_RECT;
        form_bg.img_id = 0;
        form_bg.color = MMI_BLACK_COLOR;
        form_bg.is_screen_img = FALSE;
        
        hor_space = 1;//Ë®Æ½¼ä¾à
        ver_space = 1;//´¹Ö±¼ä¾à
        
        width.add_data = 240;
        form_bg.color = MMI_WHITE_COLOR;
        form_ctrl_handle = MMI_DUERAPP_ASSISTS_FORM_CTRL_ID;
        
        if (is_login) {
            height.add_data = 160;
            child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_CTRL_ID;
            IGuiCtrl = MMK_GetCtrlPtr(MMK_GetCtrlHandleByWin(MMI_DUERAPP_VIP_ASSISTS_WIN_ID, child_form_ctrl_id));
            CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
            CTRLFORM_SetChildWidth(form_ctrl_handle, child_form_ctrl_id, &width);
            CTRLFORM_SetChildHeight(form_ctrl_handle, child_form_ctrl_id, &height);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
            CTRLFORM_SetSpace(child_form_ctrl_id, &hor_space, &ver_space);
            // GUICTRL_SetProgress(IGuiCtrl, FALSE);
        }
        
        height.add_data = 250;//¶¨Òå¶þÎ¬ÂëFORM¸ß¶È
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_CTRL_ID;
        CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
        CTRLFORM_SetChildWidth(form_ctrl_handle, child_form_ctrl_id, &width);
        CTRLFORM_SetChildHeight(form_ctrl_handle, child_form_ctrl_id, &height);
        CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
        CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
        CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
        CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
        CTRLFORM_SetSpace(child_form_ctrl_id, &hor_space, &ver_space);
    }
    
    //3ÉèÖÃ-formµÄ×Óform1ºÍform2µÄ×Óform1¡¢form2
    {
        child_form_ctrl_id = 0;
        // width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        // height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        // form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
        memset(&width, 0x00, sizeof(width));
        width.add_data = 0;
        width.type = GUIFORM_CHILD_WIDTH_FIXED;

        memset(&height, 0x00, sizeof(height));
        height.add_data = 0;
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;

        memset(&form_bg, 0x00, sizeof(form_bg));
        form_bg.bg_type = GUI_BG_COLOR;
        form_bg.shape = GUI_SHAPE_ROUNDED_RECT;
        form_bg.img_id = 0;
        form_bg.color = MMI_BLACK_COLOR;
        form_bg.is_screen_img = FALSE;
        
        hor_space = 1;//Ë®Æ½¼ä¾à
        ver_space = 1;//´¹Ö±¼ä¾à
        
        //´óformµÄ×Óform1¼°form2µÄ×Óform1ºÍform2
        width.add_data = 240;
        form_bg.color = MMI_WHITE_COLOR;
        
        form_ctrl_handle = MMI_DUERAPP_ASSISTS_FORM1_CTRL_ID;
        if (is_login) {
            for (i = 0; i < 2; i++) {
                child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM1_CTRL_ID + i;
                CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
                CTRLFORM_SetChildWidth(form_ctrl_handle, child_form_ctrl_id, &width);
                CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
                CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
                CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
                CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
                CTRLFORM_SetSpace(child_form_ctrl_id, &hor_space, &ver_space);
            }
        }
        
        form_ctrl_handle = MMI_DUERAPP_ASSISTS_FORM2_CTRL_ID;
        for (i = 0; i < 2; i++) {
            child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
            CTRLFORM_SetChildWidth(form_ctrl_handle, child_form_ctrl_id, &width);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
            CTRLFORM_SetSpace(child_form_ctrl_id, &hor_space, &ver_space);
        }
    }

    //4ÉèÖÃ-º°°ÖÂèÀ´ÖúÁ¦
    if (is_login) {
        child_form_ctrl_id = 0;
        // width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        // height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        // padding = {0};
        memset(&width, 0x00, sizeof(width));
        width.add_data = 0;
        width.type = GUIFORM_CHILD_WIDTH_FIXED;

        memset(&height, 0x00, sizeof(height));
        height.add_data = 0;
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;

        memset(&padding, 0x00, sizeof(padding));
        
        //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        label_ctrl_id = 0;
        label_font_color = MMI_WHITE_COLOR;
        label_font_size = 0;
        // label_wshowtxt = {0};
        memset(&label_wshowtxt, 0x00, sizeof(label_wshowtxt));
        
        //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        button_ctrl_id = 0;
        // voice_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_TRUMPET_BUTTON, MMI_WHITE_COLOR, TRUE};
        // edit_button_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_SEND_BUTTON, MMI_WHITE_COLOR, TRUE};
        // font_info = {DUERAPP_ASSISTS_EDIT_BUTTON_FONT, MMI_WHITE_COLOR};
        // button_text = {0};
        memset(&voice_attr, 0x00, sizeof(voice_attr));
        voice_attr.bg_type = GUI_BG_IMG;
        voice_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        voice_attr.img_id = IMAGE_DUERAPP_TRUMPET_BUTTON;
        voice_attr.color = MMI_WHITE_COLOR;
        voice_attr.is_screen_img = TRUE;  //is all screen image,only for img

        memset(&edit_button_attr, 0x00, sizeof(edit_button_attr));
        edit_button_attr.bg_type = DUERAPP_ASSISTS_EDIT_BUTTON_FONT;
        edit_button_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        edit_button_attr.img_id = IMAGE_DUERAPP_ASSISTS_SEND_BUTTON;
        edit_button_attr.color = MMI_WHITE_COLOR;
        edit_button_attr.is_screen_img = TRUE;  //is all screen image,only for img

        memset(&font_info, 0x00, sizeof(font_info));
        font_info.font = DUERAPP_ASSISTS_EDIT_BUTTON_FONT;
        font_info.color = MMI_WHITE_COLOR;

        memset(&button_text, 0x00, sizeof(button_text));

        
        //ï¿½ï¿½formï¿½ï¿½form1ï¿½ï¿½form1ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        width.add_data = 70;//92
        height.add_data = 50;
        padding.top = 5;
        padding.left = 5;
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM1_CTRL_ID;
        button_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM1_IMAGE_CTRL_ID;
        CTRLFORM_SetChildWidth(child_form_ctrl_id, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(child_form_ctrl_id, button_ctrl_id, &height);
        CTRLFORM_SetPadding(child_form_ctrl_id, &padding);
        CTRLBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
        CTRLBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        CTRLBUTTON_SetFg(button_ctrl_id, &voice_attr);
        CTRLBUTTON_SetPressedFg(button_ctrl_id, &voice_attr);
        
        //´óformµÄform1×Óform1µÄlabel
        label_font_size = DUERAPP_ASSISTS_TITLE1_FONT;
        label_font_color = RGB8882RGB565(0xFA6400);
        label_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM1_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        label_wshowtxt.wstr_ptr = L"1.º°°ÖÂèÀ´ÖúÁ¦", label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, 0, 0);//ÉèÖÃÉÏÏÂ×óÓÒµÄ¿ÕÏ¶
        
        //´óformµÄform1×Óform2µÄlabel
        label_font_size = DUERAPP_ASSISTS_CONTENT1_FONT;
        label_font_color = MMI_BLACK_COLOR;
        label_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM2_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_MIDDLE);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        label_wshowtxt.wstr_ptr = L"±à¼­ÖúÁ¦ÄÚÈÝ·¢¸ø¼Ò³¤", label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, 0, 0);//ÉèÖÃÉÏÏÂ×óÓÒµÄ¿ÕÏ¶
        
        //´óformµÄform1×Óform2µÄ±à¼­
        width.add_data = 240;//162
        height.add_data = 78;
        padding.left = 10;
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM2_CTRL_ID;
        button_ctrl_id = MMI_DUERAPP_ASSISTS_FORM1_FORM2_BUTTON_CTRL_ID;
        CTRLFORM_SetChildWidth(child_form_ctrl_id, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(child_form_ctrl_id, button_ctrl_id, &height);
        CTRLFORM_SetPadding(child_form_ctrl_id, &padding);
        CTRLBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
        CTRLBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        CTRLBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_AssistsEditButtonCb);
        CTRLBUTTON_SetFg(button_ctrl_id, &edit_button_attr);
        CTRLBUTTON_SetPressedFg(button_ctrl_id, &edit_button_attr);
        CTRLBUTTON_SetFont(button_ctrl_id, &font_info);
        button_text.wstr_ptr = L"±à ¼­",  button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
        CTRLBUTTON_SetText(button_ctrl_id, button_text.wstr_ptr, button_text.wstr_len);
    }
    
    //5ÉèÖÃ-º°°ÖÂèÀ´É¨Âë
    {
        child_form_ctrl_id = 0;
        // width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        // height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        // padding = {0};
        // form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
        memset(&width, 0x00, sizeof(width));
        width.add_data = 0;
        width.type = GUIFORM_CHILD_WIDTH_FIXED;

        memset(&height, 0x00, sizeof(height));
        height.add_data = 0;
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;

        memset(&padding, 0x00, sizeof(padding));

        memset(&form_bg, 0x00, sizeof(form_bg));
        form_bg.bg_type = GUI_BG_COLOR;
        form_bg.shape = GUI_SHAPE_ROUNDED_RECT;
        form_bg.img_id = 0;
        form_bg.color = MMI_BLACK_COLOR;
        form_bg.is_screen_img = FALSE;
        
        //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        label_ctrl_id = 0;
        label_font_color = MMI_WHITE_COLOR;
        label_font_size = 0;
        // label_wshowtxt = {0};
        memset(&label_wshowtxt, 0x00, sizeof(label_wshowtxt));
        
        //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        button_ctrl_id = 0;
        // voice_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_TRUMPET_BUTTON, MMI_WHITE_COLOR, TRUE};
        // edit_button_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_SEND_BUTTON, MMI_WHITE_COLOR, TRUE};
        // font_info = {DUERAPP_ASSISTS_EDIT_BUTTON_FONT, MMI_WHITE_COLOR};
        // button_text = {0};
        memset(&voice_attr, 0x00, sizeof(voice_attr));
        voice_attr.bg_type = GUI_BG_IMG;
        voice_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        voice_attr.img_id = IMAGE_DUERAPP_TRUMPET_BUTTON;
        voice_attr.color = MMI_WHITE_COLOR;
        voice_attr.is_screen_img = TRUE;  //is all screen image,only for img

        memset(&edit_button_attr, 0x00, sizeof(edit_button_attr));
        edit_button_attr.bg_type = GUI_BG_IMG;
        edit_button_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        edit_button_attr.img_id = IMAGE_DUERAPP_ASSISTS_SEND_BUTTON;
        edit_button_attr.color = MMI_WHITE_COLOR;
        edit_button_attr.is_screen_img = TRUE;  //is all screen image,only for img

        memset(&font_info, 0x00, sizeof(font_info));
        font_info.font = DUERAPP_ASSISTS_EDIT_BUTTON_FONT;
        font_info.color = MMI_WHITE_COLOR;

        memset(&button_text, 0x00, sizeof(button_text));

        //ï¿½ï¿½formï¿½ï¿½form2ï¿½ï¿½form1ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        width.add_data = 70;//92
        height.add_data = 50;
        padding.top = 5;
        padding.left = 5;
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_FORM1_CTRL_ID;
        button_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_FORM1_IMAGE_CTRL_ID;
        CTRLFORM_SetChildWidth(child_form_ctrl_id, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(child_form_ctrl_id, button_ctrl_id, &height);
        CTRLFORM_SetPadding(child_form_ctrl_id, &padding);
        CTRLBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
        CTRLBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        CTRLBUTTON_SetFg(button_ctrl_id, &voice_attr);
        CTRLBUTTON_SetPressedFg(button_ctrl_id, &voice_attr);
        
        //ï¿½ï¿½formï¿½ï¿½form2ï¿½ï¿½form1ï¿½ï¿½label
        label_font_size = DUERAPP_ASSISTS_TITLE1_FONT;
        label_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_FORM1_LABEL_CTRL_ID;
        label_font_color = RGB8882RGB565(0xFA6400);
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        
        if (is_login) {
            label_wshowtxt.wstr_ptr = L"2.º°°ÖÂèÀ´É¨Âë";
        } else {
            label_wshowtxt.wstr_ptr = L"º°°ÖÂèÀ´É¨Âë";
        }
        label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, 0, 0);//ÉèÖÃÉÏÏÂ×óÓÒµÄ¿ÕÏ¶
        
        //´óformµÄform2×Óform2µÄlabel
        label_font_size = DUERAPP_ASSISTS_CONTENT1_FONT;
        label_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_FORM2_LABEL_CTRL_ID;
        label_font_color = MMI_BLACK_COLOR;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_MIDDLE);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        label_wshowtxt.wstr_ptr = L"Ê¹ÓÃÎ¢ÐÅÉ¨Âë¿ª»áÔ±", label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, 0, 0);//ÉèÖÃÉÏÏÂ×óÓÒµÄ¿ÕÏ¶
        
        //´óformµÄform2×Óform2µÄ±à¼­
        width.add_data = 220;
        height.add_data = 150;
        child_form_ctrl_id = MMI_DUERAPP_ASSISTS_FORM2_FORM2_CTRL_ID;
        CTRLFORM_SetChildWidth(form_ctrl_handle, MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID, &width);
        CTRLFORM_SetChildHeight(form_ctrl_handle, MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID, &height);
        
        form_bg.color = MMI_WHITE_COLOR;
        CTRLOWNERDRAW_SetHeight(MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID, height.add_data);
        CTRLOWNERDRAW_SetBg(MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID, &form_bg);
        CTRLOWNERDRAW_SetVisible(MMI_DUERAPP_ASSISTS_FORM2_FORM2_OWNER_DRAW_CTRL_ID, TRUE, TRUE);
    }
    
    MMK_SetAtvCtrl(MMI_DUERAPP_VIP_ASSISTS_WIN_ID, MMI_DUERAPP_ASSISTS_FORM_CTRL_ID);
}

LOCAL MMI_RESULT_E PRV_DUERAPP_AssistsHandleMsg(
                                        MMI_WIN_ID_T        win_id,    //IN:
                                        MMI_MESSAGE_ID_E    msg_id,    //IN:
                                        DPARAM              param      //IN:
                                        )
{

    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    int ret = -1;
    MMI_STRING_T *toast_str = NULL;

    s_assists_qr_login_info = (Pt_QR_LOGIN_URL_INFO *)get_qr_url_info();
    
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            PRV_DUERAPP_WinDrawBG(win_id, MMI_BLACK_COLOR);
            if (s_assists_qr_login_info && *s_assists_qr_login_info) {
                strcpy(s_assists_login_sta_url, (*s_assists_qr_login_info)->url_status);
                PRV_DUERAPP_AssistsUiDraw();
                PRV_DUERAPP_AssistsDisplay(win_id, (*s_assists_qr_login_info)->url);
            } else {
                MMK_CloseWin(win_id);
            }
            break;
        }
        
        case MSG_GET_FOCUS:
        break;
        
        case MSG_FULL_PAINT:
        {
            break;
        }

        case MSG_DUERAPP_ASSISTS_RESPONSE_TRANS: {
            DUER_LOGI("%s ASSISTS_RESPONSE_TRANS", __FUNCTION__);
#if defined(DUERAPP_ASSISTS_VIP_TRANSMIT_MESSAGE)
            PUB_DUERAPP_TransmitMessageWinOpen();
#endif
        } break;
        
        case MSG_DUERAPP_ASSISTS_RESPONSE_RESULT:
        {
            toast_str = (MMI_STRING_T *)param;
            if(toast_str) {
                PRV_DUERAPP_Toast(toast_str->wstr_ptr);
            }
            break;
        }
        
        case MSG_CTL_OK:
        case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
        case MSG_CLOSE_WINDOW:
        if (s_assists_qr_login_info && *s_assists_qr_login_info) {
            if ((*s_assists_qr_login_info)->url != PNULL) {
                DUER_FREE((*s_assists_qr_login_info)->url);
                (*s_assists_qr_login_info)->url = NULL;
            }
            
            if ((*s_assists_qr_login_info)->url_status != PNULL) {
                DUER_FREE((*s_assists_qr_login_info)->url_status);
                (*s_assists_qr_login_info)->url_status = NULL;
            }
            DUER_FREE(*s_assists_qr_login_info);
            *s_assists_qr_login_info = NULL;
            s_assists_qr_login_info = NULL;
        }
        
        if (s_assists_ownerdraw_qrcode) {
            QRcode_free(s_assists_ownerdraw_qrcode);
            s_assists_ownerdraw_qrcode = NULL;
        }
        PRV_DUERAPP_AssistsCtrlDestory(FALSE);
        break;
        default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return (recode);
}

#endif

#if defined(DUERAPP_ASSISTS_VIP_TRANSMIT_MESSAGE)

LOCAL void PRV_DUERAPP_TransmitMessageCtrlDestory(BOOLEAN is_create);

LOCAL void PRV_DUERAPP_TransmitMessageAnimUpdate(MMI_WIN_ID_T win_id)
{
    
    MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM_CTRL_ID;
    MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_CTRL_ID;
    MMI_HANDLE_T anim_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_ANIM_CTRL_ID;
    
    GUIANIM_DATA_INFO_T img_info = {0};
    img_dsc_t *resource = NULL;
    // GUIANIM_CTRL_INFO_T ctrl_info = {.is_ctrl_id = TRUE, .ctrl_id = anim_ctrl_id};
    GUIANIM_CTRL_INFO_T ctrl_info;

    // GUIANIM_DISPLAY_INFO_T  display_info = {
    //     .align_style = GUIANIM_ALIGN_HVMIDDLE,
    //     .is_auto_zoom_in = TRUE,
    //     .is_update = TRUE,
    //     .is_disp_one_frame = TRUE,
    //     .is_zoom = TRUE,
    //     .is_handle_transparent = TRUE,
    //     .is_syn_decode = TRUE,
    // };
    GUIANIM_DISPLAY_INFO_T  display_info;
    memset(&display_info, 0x00, sizeof(display_info));

    memset(&ctrl_info, 0x00, sizeof(ctrl_info));
    ctrl_info.is_ctrl_id = TRUE;
    ctrl_info.ctrl_id = anim_ctrl_id;

    display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
    display_info.is_auto_zoom_in = TRUE;
    display_info.is_update = TRUE;
    display_info.is_disp_one_frame = TRUE;
    display_info.is_zoom = TRUE;
    display_info.is_handle_transparent = TRUE;
    display_info.is_syn_decode = TRUE;

    DUER_LOGI("(%s)(%d)[duer_watch]:update pic anim", __FUNCTION__, __LINE__);
    
    resource = imageGetResource(IMG_DOWN_PARENTS_ASSISTS, 0);
    if (resource) {
        if (resource->data && resource->data_size > 0) {
            img_info.data_ptr = (resource)->data;
            img_info.data_size = (resource)->data_size;
        } else {
            DUER_LOGE("(%s)[duer_watch]:resource get err", __FUNCTION__);
            img_info.img_id = IMAGE_DUERAPP_TITLE;
        }
    } else {
        DUER_LOGE("(%s)[duer_watch]:resource get err", __FUNCTION__);
        img_info.img_id = IMAGE_DUERAPP_TITLE;
    }
    
    GUIANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);
    GUIANIM_SetVisible(anim_ctrl_id, TRUE, TRUE);
    MMK_SetAtvCtrl(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_CTRL_ID, anim_ctrl_id);
}

LOCAL MMI_RESULT_E  PRV_DUERAPP_TransmitMessageHandleMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
    MMI_STRING_T *wtxt = NULL;

    SCI_TraceLow("(%s)(%d)[duer_watch]:win_id(0x%x), msg_id(0x%x)", __func__, __LINE__, win_id, msg_id);
    
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            PRV_DUERAPP_WinDrawBG(win_id, MMI_BLACK_COLOR);
            PRV_DUERAPP_TransmitMessageUiDraw();
            // imageStartDownResource(IMG_DOWN_PARENTS_ASSISTS);
            break;
        }
        case MSG_FULL_PAINT:
        {
            break;
        }
        case MSG_CTL_OK:
        case MSG_CTL_MIDSK:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_DUERAPP_ASSISTS_WORDS_UPDATE:
        {
            wtxt = (MMI_STRING_T *)param;
            memset(assists_attr.assists_wtxt_words, 0, sizeof(wchar)*256);
            MMIAPICOM_Wstrcpy(assists_attr.assists_wtxt_words, wtxt->wstr_ptr);
            GUITEXT_SetString(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM2_TEXTBOX_CTRL_ID, wtxt->wstr_ptr, wtxt->wstr_len, TRUE);
            break;
        }
        case MSG_DUERAPP_ANIM_PIC_UPDATE:
        {
            PRV_DUERAPP_TransmitMessageAnimUpdate(win_id);
            break;
        }
        case MSG_CLOSE_WINDOW:
        {
            memset(&assists_attr, 0, sizeof(assists_info_t));
            PRV_DUERAPP_TransmitMessageCtrlDestory(FALSE);
            break;
        }
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }

    return recode;
}

LOCAL MMI_RESULT_E PRV_DUERAPP_TransmitMessageImgButtonCb(MMI_HANDLE_T ctrl_handle)
{
    int index = -1;
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MMI_STRING_T show_str = {0};
    BOOLEAN checkbox_sta = FALSE;
    MMI_HANDLE_T ctrl_id = MMK_GetCtrlId(ctrl_handle);
    
    switch (ctrl_id)
    {
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 0;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM2_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 1;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM3_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 2;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM4_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 3;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM5_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 4;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM6_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 5;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM7_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 6;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM8_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 7;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM9_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 8;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM10_FORM2_CHECKBOX_CTRL_ID :
        {
            index = 9;
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_BUTTON_CTRL_ID ://L"±à¼­"
        {
            MMK_CreateWin((uint32 *)MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_TAB, PNULL);
            break;
        }
        
        case MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM3_BUTTON_CTRL_ID ://L"Ò»¼ü·¢ËÍ"
        {
            int ret = PUB_DUERAPP_ParentsAssistsRequest();
            if (ret) {
                PRV_DUERAPP_Toast(L"ÖúÁ¦·¢ËÍÊ§°Ü!");
            }
            MMK_CloseWin(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_ID);
            break;
        }
        
        default:
        {
            break;
        }
    }

    if (index >= 0) {
        checkbox_sta = CTRLCHECKBOX_GetSelect(ctrl_id);
        DUER_LOGI("(%d)[duer_watch]:sta(%d), cnt(%d)", __LINE__, checkbox_sta, assists_attr.checkbox_number);
        if (!checkbox_sta) {
            if (assists_attr.checkbox_number == 1) {//±ØÐë±£ÁôÒ»¸ö
                CTRLCHECKBOX_SetSelect(ctrl_id, TRUE);
                //REFRESH
                MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
            } else {
                assists_attr.bind_info[index].is_select = FALSE;
                assists_attr.checkbox_number--;
            }
        } else {
            assists_attr.bind_info[index].is_select = TRUE;
            assists_attr.checkbox_number++;
        }
    }
    return result;
}

LOCAL void PRV_DUERAPP_TransmitMessageCtrlDestory(BOOLEAN is_create)
{
    static BOOLEAN is_have_create = FALSE;
    BOOLEAN state = FALSE;
    int i = 0;
    int number = 0;
    
    if (is_create) {
        if (is_have_create) {
            state = TRUE;
        }
        is_have_create = is_create;
        if (state) {
            goto DESTORY;
        }
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            goto DESTORY;
        }
    }
    return;
DESTORY:
    number = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_ANIM_CTRL_ID - MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM_CTRL_ID;
    for (i = number; i >= 0; i--) {
        MMK_DestroyControl(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_ANIM_CTRL_ID - (number - i));
    }
}

LOCAL MMI_HANDLE_T PRV_DUERAPP_TransmitMessageCtrlCreate(MMI_HANDLE_T win_handle)
{
    MMI_HANDLE_T ctrl_handle = 0;
    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM_CTRL_ID;
    int i = 0, j = 0, k = 0;
    uint32_t handle_id = 0;

    GUIFORM_INIT_DATA_T childform_init_data = {0};
    GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};

    // GUIFORM_INIT_DATA_T childform_init_data = {0};
    // GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
    
    GUIBUTTON_INIT_DATA_T buttom_init_data = {0};
    GUIFORM_DYNA_CHILD_T button_form_child_ctrl = {0};
    
    GUILABEL_INIT_DATA_T label_init_data = {0};
    GUIFORM_DYNA_CHILD_T label_form_child_ctrl = {0};
    
    GUITEXT_INIT_DATA_T textbox_init_data = {0};
    GUIFORM_DYNA_CHILD_T textbox_form_child_ctrl = {0};
    
    MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_CTRL_ID;
    MMI_HANDLE_T child_form_ctrl_id = 0;

    // MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_CTRL_ID;
        
    GUIANIM_INIT_DATA_T anim_init_data = {0};
    // GUIFORM_DYNA_CHILD_T anim_form_child_ctrl = {
    //     .child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_ANIM_CTRL_ID,
    //     .init_data_ptr = &anim_init_data,
    //     .guid = SPRD_GUI_ANIM_ID,
    // };
    GUIFORM_DYNA_CHILD_T anim_form_child_ctrl;
    memset(&anim_form_child_ctrl, 0x00, sizeof(anim_form_child_ctrl));
    anim_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_ANIM_CTRL_ID;
    anim_form_child_ctrl.init_data_ptr = &anim_init_data;
    anim_form_child_ctrl.guid = SPRD_GUI_ANIM_ID;
    
    if (assists_attr.binding_number == 0) {
        assists_attr.binding_number = 1;
    } else if (assists_attr.binding_number > 10) {
        assists_attr.binding_number = 10;
    }
    
    //0ÊÍ·Å
    PRV_DUERAPP_TransmitMessageCtrlDestory(TRUE);

    //1¶¯Ì¬´´½¨form
    {
        GUIFORM_CreatDynaCtrl(win_handle, ctrl_id, GUIFORM_LAYOUT_ORDER);
    }

    //2¶¯Ì¬´´½¨×Óform
    {   
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_CTRL_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
#if DUERAPP_ASSISTS_SHOW_BIND_SUPPORT
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
#endif
        //µÚ2ÐÐ
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        //µÚ3ÐÐ
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_CTRL_ID;
        childform_form_child_ctrl.is_get_active = TRUE;
        GUIFORM_CreatDynaChildCtrl(win_handle, ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
    }

#if DUERAPP_ASSISTS_SHOW_BIND_SUPPORT
    //3¶¯Ì¬´´½¨-·¢¸ø
    {
        GUIFORM_INIT_DATA_T childform_init_data = {0};
        GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
        
        GUICHECKBOX_INIT_DATA_T checkbox_init_data = {0};
        GUIFORM_DYNA_CHILD_T checkbox_form_child_ctrl = {0};
        
        GUILABEL_INIT_DATA_T label_init_data = {0};
        GUIFORM_DYNA_CHILD_T label_form_child_ctrl = {0};
        
        MMI_HANDLE_T form_ctrl_id = 0;
        MMI_HANDLE_T child_form_ctrl_id = 0;
        MMI_HANDLE_T child_form_ctrl_id2 = 0;
        
        checkbox_form_child_ctrl.guid = SPRD_GUI_CHECKBOX_ID;
        label_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        //¶¯Ì¬´´½¨form1×Óform¿Ø¼þ
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_CTRL_ID;
        for (i = 0; i < assists_attr.binding_number; i++) {
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
        
        //¶¯Ì¬´´½¨form1×Óform1×Óform¿Ø¼þ
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        for (i = 0; i < assists_attr.binding_number; i++) {
            form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_CTRL_ID + i;
            for (j = 0; j < 2; j++) {
                childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM1_CTRL_ID + k;
                GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
                GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
                k++;
            }
        }
        
        //¶¯Ì¬´´½¨form1×Óform1×Óform1(·¢¸ø)¿Ø¼þLABEL
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        handle_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM1_LABEL_CTRL_ID;
        for (i = 0; i < assists_attr.binding_number; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM1_CTRL_ID + i*2;
            child_form_ctrl_id2 = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM2_CTRL_ID + i*2;
            //label send
            label_form_child_ctrl.child_handle = handle_id;
            label_form_child_ctrl.init_data_ptr = &label_init_data;
            child_form_ctrl_id = child_form_ctrl_id;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
            handle_id++;
            //checkbox
            checkbox_form_child_ctrl.child_handle = handle_id;
            checkbox_form_child_ctrl.init_data_ptr = &checkbox_init_data;
            child_form_ctrl_id = child_form_ctrl_id2;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &checkbox_form_child_ctrl);
            handle_id++;
            //label name
            label_form_child_ctrl.child_handle = handle_id;
            label_form_child_ctrl.init_data_ptr = &label_init_data;
            child_form_ctrl_id = child_form_ctrl_id2;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
            handle_id++;
        }
    }
#endif

    //4¶¯Ì¬´´½¨-ÖúÁ¦ÁôÑÔ
    {
        // childform_init_data = {0};
        // childform_form_child_ctrl = {0};
        
        // buttom_init_data = {0};
        // button_form_child_ctrl = {0};
        
        // label_init_data = {0};
        // label_form_child_ctrl = {0};
        
        // textbox_init_data = {0};
        // textbox_form_child_ctrl = {0};

        memset(&childform_init_data, 0x00, sizeof(childform_init_data));
        memset(&childform_form_child_ctrl, 0x00, sizeof(childform_form_child_ctrl));

        memset(&buttom_init_data, 0x00, sizeof(buttom_init_data));
        memset(&button_form_child_ctrl, 0x00, sizeof(button_form_child_ctrl));

        memset(&label_init_data, 0x00, sizeof(label_init_data));
        memset(&label_form_child_ctrl, 0x00, sizeof(label_form_child_ctrl));

        memset(&textbox_init_data, 0x00, sizeof(textbox_init_data));
        memset(&textbox_form_child_ctrl, 0x00, sizeof(textbox_form_child_ctrl));
        
        form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_CTRL_ID;
        child_form_ctrl_id = 0;
        
        button_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        label_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        textbox_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //¶¯Ì¬´´½¨form1×Óform¿Ø¼þ
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_CTRL_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        
        GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        for (i = 0; i < 2; i++) {
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM2_CTRL_ID + i;
            childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
            childform_form_child_ctrl.init_data_ptr = &childform_init_data;
            GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
        
        //¶¯Ì¬´´½¨form1×Óform1(ÖúÁ¦ÁôÑÔ)¿Ø¼þLABELºÍ±à¼­°´Å¥
        label_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_LABEL_CTRL_ID;
        label_form_child_ctrl.init_data_ptr = &label_init_data;
        child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
        
        button_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_BUTTON_CTRL_ID;
        button_form_child_ctrl.init_data_ptr = &buttom_init_data;
        child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &button_form_child_ctrl);
        
        //¶¯Ì¬´´½¨form1×Óform2(ÖúÁ¦ÄÚÈÝ)¿Ø¼þTEXTBOX
        textbox_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM2_TEXTBOX_CTRL_ID;
        textbox_form_child_ctrl.init_data_ptr = &textbox_init_data;
        child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM2_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &textbox_form_child_ctrl);
        
        //¶¯Ì¬´´½¨form1×Óform2(ÖúÁ¦ÄÚÈÝ)¿Ø¼þÒ»¼ü·¢ËÍ°´Å¥
        button_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM3_BUTTON_CTRL_ID;
        button_form_child_ctrl.init_data_ptr = &buttom_init_data;
        child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM3_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &button_form_child_ctrl);
    }

    //5¶¯Ì¬´´½¨-»áÔ±anim¶¯»­
    {
        child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_CTRL_ID;
        
        // anim_init_data = {0};
        memset(&anim_init_data, 0x00, sizeof(anim_init_data));
        anim_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_ANIM_CTRL_ID;
        anim_form_child_ctrl.init_data_ptr = &anim_init_data;
        anim_form_child_ctrl.guid = SPRD_GUI_ANIM_ID;
        
        //¶¯Ì¬´´½¨form×Ó¿Ø¼þANIM
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &anim_form_child_ctrl);
    }

    ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, ctrl_id);
    
    return ctrl_handle;
}

LOCAL void PRV_DUERAPP_TransmitMessageUiDraw(void)
{
    MMI_HANDLE_T form_ctrl_handle = PRV_DUERAPP_TransmitMessageCtrlCreate(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_ID);
    int i = 0, j = 0, k =0;
    uint32_t handle_id = 0;

    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, MMI_BLACK_COLOR, FALSE};
    GUI_RECT_T form_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
    IGUICTRL_T *IGuiCtrl = MMK_GetCtrlPtr(form_ctrl_handle);
    
    // GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0xEFEFEF), FALSE};
    MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_CTRL_ID;
    GUIFORM_CHILD_WIDTH_T child_form_width = {235, GUIFORM_CHILD_WIDTH_FIXED};
    GUI_PADDING_T padding = {5, 5, 5, 5};

    GUI_BG_T param_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
    MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_CTRL_ID;
    // MMI_HANDLE_T child_form_ctrl_id = 0;
    
    GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
    GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    uint16 hor_space = 1;//Ë®Æ½ï¿½ï¿½ï¿?
    uint16 ver_space = 1;//ï¿½ï¿½Ö±ï¿½ï¿½ï¿?
    // GUI_PADDING_T padding = {0, 5, 0, 5};

    //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    MMI_HANDLE_T button_ctrl_id = 0;
    GUI_BG_T button_pre_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_EDIT_BUTTON, MMI_WHITE_COLOR, TRUE};
    GUI_BG_T button_rel_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_EDIT_BUTTON, MMI_WHITE_COLOR, TRUE};
    GUI_FONT_ALL_T font_info = {DUERAPP_ASSISTS_SEND_BUTTON_FONT, MMI_BLACK_COLOR};
    MMI_STRING_T button_text = {0};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    MMI_HANDLE_T label_ctrl_id = 0;
    GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
    GUI_FONT_T label_font_size = DUERAPP_ASSISTS_COMMON_FONT;
    MMI_STRING_T label_wshowtxt = {0};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½TEXTBOXï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM2_TEXTBOX_CTRL_ID;
    GUI_COLOR_T font_color = MMI_BLACK_COLOR;
    GUI_FONT_T font_size = WATCH_DEFAULT_NORMAL_FONT;
    wchar *wtxt = L"ï¿½ï¿½ï¿½ï¿½ï¿½Úºï¿½Í¬Ñ§ï¿½ï¿½PKÑ§Ï°ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò¹ï¿½ï¿½ï¿½ï¿½Ô±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½\n";

    // MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM_CTRL_ID;
    // MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_CTRL_ID;
    MMI_HANDLE_T anim_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_ANIM_CTRL_ID;
    GUIFORM_CHILD_WIDTH_T anim_width = {240, GUIFORM_CHILD_WIDTH_FIXED};
    GUIFORM_CHILD_HEIGHT_T anim_height = {DUERAPP_ASSISTS_LONG_PICTURE_HEIGHT, GUIFORM_CHILD_HEIGHT_FIXED};
    
    GUIANIM_DATA_INFO_T img_info = {0};
    GUIANIM_CTRL_INFO_T ctrl_info;
    GUIANIM_DISPLAY_INFO_T  display_info;

    if (assists_attr.binding_number == 0) {
        assists_attr.binding_number = 1;
    } else if (assists_attr.binding_number > 10) {
        assists_attr.binding_number = 10;
    }
    
    //ÉèÖÃform²ÎÊý
    {
        
        CTRLFORM_SetRect(form_ctrl_handle, &form_rect);
        CTRLFORM_SetBg(form_ctrl_handle, &form_bg);
        CTRLFORM_PermitChildBg(form_ctrl_handle, FALSE);
        CTRLFORM_PermitChildFont(form_ctrl_handle, FALSE);
        CTRLFORM_PermitChildBorder(form_ctrl_handle, FALSE);
        // GUICTRL_SetProgress(IGuiCtrl, FALSE);
    }

    //ÉèÖÃ×Óform²ÎÊý
    {
        // form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0xEFEFEF), FALSE};
        child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_CTRL_ID;
        // child_form_width = {235, GUIFORM_CHILD_WIDTH_FIXED};
        // padding = {5, 5, 5, 5};

        memset(&form_bg, 0x00, sizeof(form_bg));
        form_bg.bg_type = GUI_BG_COLOR;
        form_bg.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        form_bg.img_id = 0;
        form_bg.color = RGB8882RGB565(0xEFEFEF);
        form_bg.is_screen_img = FALSE;  //is all screen image,only for img

        memset(&child_form_width, 0x00, sizeof(child_form_width));
        child_form_width.add_data = 235;   /*!<?????????????[0,100]*/
        child_form_width.type = GUIFORM_CHILD_WIDTH_FIXED;       /*!<????????*/

        memset(&padding, 0x00, sizeof(padding));
        padding.left = 5;
        padding.top = 5;
        padding.right = 5;
        padding.bottom = 5;
        
#if DUERAPP_ASSISTS_SHOW_BIND_SUPPORT
        for(i = 0; i < 3; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_CTRL_ID + i;
            i == 2 ? (form_bg.color = RGB8882RGB565(0x0)) : 0;
            CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
            CTRLFORM_SetChildWidth(form_ctrl_handle, child_form_ctrl_id, &child_form_width);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
            CTRLFORM_SetPadding(child_form_ctrl_id, &padding);
        }
#else
        for(i = 0; i < 2; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_CTRL_ID + i;
            i == 1 ? (form_bg.color = RGB8882RGB565(0x0)) : 0;
            CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
            CTRLFORM_SetChildWidth(form_ctrl_handle, child_form_ctrl_id, &child_form_width);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
            CTRLFORM_SetPadding(child_form_ctrl_id, &padding);
        }
#endif
    }

#if DUERAPP_ASSISTS_SHOW_BIND_SUPPORT
    //3¶¯Ì¬´´½¨-·¢¸ø
    {
        GUI_BG_T param_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
        MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_CTRL_ID;
        MMI_HANDLE_T child_form_ctrl_id = 0;
        MMI_HANDLE_T child_form_ctrl_id2 = 0;
        int utf8_len = 0;
        wchar name_wstr[256];
        
        GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        uint16 hor_space = 1;//Ë®Æ½¼ä¾à
        uint16 ver_space = 1;//´¹Ö±¼ä¾à
        GUI_PADDING_T padding = {0};

        //³õÊ¼»¯BUTTON¿Ø¼þµÄ²ÎÊý
        MMI_HANDLE_T button_ctrl_id = 0;
        GUI_BG_T button_pre_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_SELECT, MMI_WHITE_COLOR, TRUE};
        GUI_BG_T button_rel_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_UNSELECT, MMI_WHITE_COLOR, TRUE};

        //³õÊ¼»¯LABEL¿Ø¼þµÄ²ÎÊý
        MMI_HANDLE_T label_ctrl_id = 0;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_ASSISTS_COMMON_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        param_bg.color = RGB8882RGB565(0xEFEFEF);
        for (i = 0; i < assists_attr.binding_number; i++) {//10¸ö°ó¶¨Î¢ÐÅºÅÎ»ÖÃ
            child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &param_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
        }
        
        for (i = 0; i < assists_attr.binding_number; i++) {
            form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_CTRL_ID + i;
            for(j = 0; j < 2; j++) {
                child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM1_CTRL_ID + k;
                k % 2 == 0 ? (param_bg.color = RGB8882RGB565(0xEFEFEF)) : (param_bg.color = RGB8882RGB565(0xFFFFFF));
                CTRLFORM_SetBg(child_form_ctrl_id, &param_bg);
                CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
                CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
                CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
                CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
                k++;
            }
        }
        param_bg.color = RGB8882RGB565(0xFFFFFF);
        
        handle_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM1_LABEL_CTRL_ID;
        for (i = 0; i < assists_attr.binding_number; i++) {
            form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_CTRL_ID + i;
            child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM1_CTRL_ID + i*2;
            child_form_ctrl_id2 = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM1_FORM1_FORM2_CTRL_ID + i*2;
            //label-·¢¸ø
            width.add_data = 50;//×Óform¿í¶È
            CTRLFORM_SetChildWidth(form_ctrl_id, child_form_ctrl_id, &width);
            label_ctrl_id = handle_id;
            label_font_color = RGB8882RGB565(0XFA6400);
            CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
            CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
            i == 0 ? CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE) : CTRLLABEL_SetVisible(label_ctrl_id, FALSE, FALSE);
            label_wshowtxt.wstr_ptr = L"·¢¸ø", label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
            CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
            handle_id++;
            
            //checkbox
            width.add_data = 36;//°´Å¥¿í¶È
            height.add_data = 36;//°´Å¥¸ß¶È
            button_ctrl_id = handle_id;
            CTRLFORM_SetChildWidth(child_form_ctrl_id2, button_ctrl_id, &width);
            CTRLFORM_SetChildHeight(child_form_ctrl_id2, button_ctrl_id, &height);
            CTRLBUTTON_SetPressedBg(button_ctrl_id, &button_pre_bg_attr);
            CTRLBUTTON_SetFg(button_ctrl_id, &button_rel_bg_attr);
            CTRLBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_TransmitMessageImgButtonCb);
            i == 0 ? CTRLCHECKBOX_SetSelect(button_ctrl_id, TRUE) : CTRLCHECKBOX_SetSelect(button_ctrl_id, FALSE);
            handle_id++;
            
            //label-Î¢ÐÅÃû
            label_ctrl_id = handle_id;
            label_font_color = MMI_BLACK_COLOR;
            CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
            CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
            CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
            utf8_len = strlen(assists_attr.bind_info[i].nickname);
            GUI_UTF8ToWstr(name_wstr, utf8_len, (const uint8 *)assists_attr.bind_info[i].nickname, utf8_len);
            label_wshowtxt.wstr_ptr = name_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
            CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
            handle_id++;
        }
    }
#endif

    //4¶¯Ì¬´´½¨-ÖúÁ¦ÁôÑÔ
    {
        // param_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
        form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_CTRL_ID;
        child_form_ctrl_id = 0;

        memset(&form_bg, 0x00, sizeof(form_bg));
        form_bg.bg_type = GUI_BG_COLOR;
        form_bg.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        form_bg.img_id = 0;
        form_bg.color = MMI_WHITE_COLOR;
        form_bg.is_screen_img = FALSE;  //is all screen image,only for img
        
        // width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        // height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        memset(&width, 0x00, sizeof(width));
        width.add_data = 0;   /*!<?????????????[0,100]*/
        width.type = GUIFORM_CHILD_WIDTH_FIXED;       /*!<????????*/

        memset(&height, 0x00, sizeof(height));
        height.add_data = 0;   /*!<?????????????[0,100]*/
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;       /*!<????????*/

        hor_space = 1;//Ë®Æ½ï¿½ï¿½ï¿?
        ver_space = 1;//ï¿½ï¿½Ö±ï¿½ï¿½ï¿?
        // padding = {0, 5, 0, 5};
        memset(&padding, 0x00, sizeof(padding));
        padding.left = 0;
        padding.top = 5;
        padding.right = 0;
        padding.bottom = 5;

        //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        button_ctrl_id = 0;
        // button_pre_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_EDIT_BUTTON, MMI_WHITE_COLOR, TRUE};
        // button_rel_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_EDIT_BUTTON, MMI_WHITE_COLOR, TRUE};
        // font_info = {DUERAPP_ASSISTS_SEND_BUTTON_FONT, MMI_BLACK_COLOR};
        // button_text = {0};
        memset(&button_pre_bg_attr, 0x00, sizeof(button_pre_bg_attr));
        button_pre_bg_attr.bg_type = GUI_BG_IMG;
        button_pre_bg_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        button_pre_bg_attr.img_id = IMAGE_DUERAPP_ASSISTS_EDIT_BUTTON;
        button_pre_bg_attr.color = MMI_WHITE_COLOR;
        button_pre_bg_attr.is_screen_img = TRUE;  //is all screen image,only for img

        memset(&button_rel_bg_attr, 0x00, sizeof(button_rel_bg_attr));
        button_rel_bg_attr.bg_type = GUI_BG_IMG;
        button_rel_bg_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        button_rel_bg_attr.img_id = IMAGE_DUERAPP_ASSISTS_EDIT_BUTTON;
        button_rel_bg_attr.color = MMI_WHITE_COLOR;
        button_rel_bg_attr.is_screen_img = TRUE;  //is all screen image,only for img

        memset(&font_info, 0x00, sizeof(font_info));
        font_info.font = DUERAPP_ASSISTS_SEND_BUTTON_FONT;
        font_info.color = MMI_BLACK_COLOR;

        memset(&button_text, 0x00, sizeof(button_text));
        
        //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        label_ctrl_id = 0;
        label_font_color = MMI_WHITE_COLOR;
        label_font_size = DUERAPP_ASSISTS_COMMON_FONT;
        // label_wshowtxt = {0};
        memset(&label_wshowtxt, 0x00, sizeof(label_wshowtxt));

        //ï¿½ï¿½Ê¼ï¿½ï¿½TEXTBOXï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        text_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM2_TEXTBOX_CTRL_ID;
        font_color = MMI_BLACK_COLOR;
        font_size = WATCH_DEFAULT_NORMAL_FONT;
        wtxt = L"ÎÒÕýÔÚºÍÍ¬Ñ§ÃÇPKÑ§Ï°£¬Çë°ïÎÒ¹ºÂò»áÔ±¿ªÆôÏÂÒ»½Ú\n";
        
        for(i = 0; i < 3; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_CTRL_ID + i;
            i%2 ? (param_bg.color = RGB8882RGB565(0xFFFFFF)) : (param_bg.color = RGB8882RGB565(0xEFEFEF));
            CTRLFORM_SetBg(child_form_ctrl_id, &param_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
            i<2 ? CTRLFORM_SetPadding(child_form_ctrl_id, &padding) : 0;
        }
        
        //label-ÖúÁ¦ÁôÑÔ
        width.add_data = 29;//°´Å¥¿í¶È
        label_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_LABEL_CTRL_ID;
        label_font_color = RGB8882RGB565(0XFA6400);
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        label_wshowtxt.wstr_ptr = L"ÖúÁ¦ÁôÑÔ", label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        
        //button-±à¼­
        width.add_data = 50;//°´Å¥¿í¶È-¼Ó´ó·½±ã´¥Ãþ
        height.add_data = 29;//°´Å¥¸ß¶È
        button_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_BUTTON_CTRL_ID;
        CTRLFORM_SetChildWidth(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_CTRL_ID, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM1_CTRL_ID, button_ctrl_id, &height);
        CTRLBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        CTRLBUTTON_SetPressedBg(button_ctrl_id, &button_pre_bg_attr);
        CTRLBUTTON_SetFg(button_ctrl_id, &button_rel_bg_attr);
        CTRLBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_TransmitMessageImgButtonCb);
        
        //textbox-ÄÚÈÝ
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        GUITEXT_SetString(text_ctrl_id, wtxt, MMIAPICOM_Wstrlen(wtxt), TRUE);
        
        //button
        width.add_data = 240;//162
        height.add_data = 60;
        button_pre_bg_attr.img_id = IMAGE_DUERAPP_ASSISTS_SEND_BUTTON;
        button_rel_bg_attr.img_id = IMAGE_DUERAPP_ASSISTS_SEND_BUTTON;
        button_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM3_BUTTON_CTRL_ID;
        CTRLFORM_SetChildWidth(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM3_CTRL_ID, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM2_FORM3_CTRL_ID, button_ctrl_id, &height);
        
        CTRLBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
        CTRLBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        CTRLBUTTON_SetBg(button_ctrl_id, &button_rel_bg_attr);
        CTRLBUTTON_SetPressedFg(button_ctrl_id, &button_pre_bg_attr);
        CTRLBUTTON_SetFg(button_ctrl_id, &button_rel_bg_attr);
        CTRLBUTTON_SetFont(button_ctrl_id, &font_info);
        button_text.wstr_ptr = L"Ò»¼ü·¢ËÍ",  button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
        CTRLBUTTON_SetText(button_ctrl_id, button_text.wstr_ptr, button_text.wstr_len);
        CTRLBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_TransmitMessageImgButtonCb);
        //ÅäÖÃÖúÁ¦»ù´¡ÐÅÏ¢
        assists_attr.checkbox_number = 1;
        assists_attr.bind_info[0].is_select = TRUE;
        memset(assists_attr.assists_wtxt_words, 0, sizeof(wchar)*256);
        MMIAPICOM_Wstrcpy(assists_attr.assists_wtxt_words, wtxt);
    }

    //5¶¯Ì¬´´½¨-»áÔ±½éÉÜ³¤Í¼
    {
        form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM_CTRL_ID;
        child_form_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_CTRL_ID;
        anim_ctrl_id = MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM3_ANIM_CTRL_ID;
        // anim_width = {240, GUIFORM_CHILD_WIDTH_FIXED};
        // anim_height = {DUERAPP_ASSISTS_LONG_PICTURE_HEIGHT, GUIFORM_CHILD_HEIGHT_FIXED};
        memset(&anim_width, 0x00, sizeof(anim_width));
        anim_width.add_data = 240;   /*!<?????????????[0,100]*/
        anim_width.type = GUIFORM_CHILD_WIDTH_FIXED;       /*!<????????*/

        memset(&anim_height, 0x00, sizeof(anim_height));
        anim_height.add_data = DUERAPP_ASSISTS_LONG_PICTURE_HEIGHT;   /*!<?????????????[0,100]*/
        anim_height.type = GUIFORM_CHILD_HEIGHT_FIXED;       /*!<????????*/
        
        // img_info = {0};
        memset(&img_info, 0x00, sizeof(img_info));
        ctrl_info.is_ctrl_id = TRUE;
        ctrl_info.ctrl_id = anim_ctrl_id;

        memset(&display_info, 0x00, sizeof(display_info));
        display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
        display_info.is_auto_zoom_in = TRUE;
        display_info.is_update = TRUE;
        display_info.is_disp_one_frame = TRUE;
        display_info.is_zoom = TRUE;
        display_info.is_handle_transparent = TRUE;
        display_info.is_syn_decode = TRUE;
        
        img_info.img_id = IMAGE_DUERAPP_TITLE;
        
        CTRLFORM_SetChildDisplay(form_ctrl_id, child_form_ctrl_id, GUIFORM_CHILD_DISP_NORMAL);
        CTRLFORM_SetChildWidth(child_form_ctrl_id, anim_ctrl_id, &anim_width);
        CTRLFORM_SetChildHeight(child_form_ctrl_id, anim_ctrl_id, &anim_height);
        
        CTRLANIM_SetBgColor(anim_ctrl_id, MMI_BLACK_COLOR);
        CTRLANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);
        CTRLANIM_SetVisible(anim_ctrl_id, FALSE, TRUE);
    }
    
    MMK_SetAtvCtrl(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_ID, MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_FORM_CTRL_ID);
}

#endif

#if defined(DUERAPP_ASSISTS_VIP_MESSAGE_SELECT)

LOCAL void PRV_DUERAPP_MessageSelectCtrlDestory(BOOLEAN is_create);

LOCAL void PRV_DUERAPP_MessageSelectRecordCntrl(BOOLEAN is_start, MMI_HANDLE_T ctrl_id)
{
    GUI_BG_T button_bg_attr ={GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, 0, FALSE};
    GUI_BG_T button_fg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, 0, 0, TRUE};
    MMI_STRING_T button_text = {0};
    
    if (!is_start) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:stop", __func__, __LINE__);
        //input voice
        MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        if (MMIDUERAPP_GetInitStatus()) {
            duer_asr_stop();
        }
        //ui
        assists_input_click_cnt = 0;
        button_bg_attr.color = RGB8882RGB565(0x68AFFD);
        button_fg_attr.color = RGB8882RGB565(0xFFFFFF);
        button_fg_attr.img_id = IMAGE_DUERAPP_ASSISTS_INPUT_MIC;
        button_fg_attr.is_screen_img = TRUE;
        button_text.wstr_ptr = L"ÓïÒôÊäÈë",  button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
        CTRLBUTTON_SetStyle(ctrl_id, GUIBUTTON_STYLE_LIRTL);
    } else {
        SCI_TraceLow("(%s)(%d)[duer_watch]:start", __func__, __LINE__);
        //input voice
        MMIDEFAULT_TurnOnLCDBackLight();
        MMIDEFAULT_AllowTurnOffBackLight(FALSE);
        if (MMIDUERAPP_GetInitStatus()) {
        duer_asr_cancel();
        duer_record_start(FALSE);
        }
        //ui
        button_bg_attr.color = RGB8882RGB565(0xB4B4B4);
        button_fg_attr.color = RGB8882RGB565(0xFFFFFF);
        button_fg_attr.img_id = 0;
        button_fg_attr.is_screen_img = FALSE;
        button_text.wstr_ptr = L"ÕýÔÚÊäÈëÓïÒô",  button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
        CTRLBUTTON_SetStyle(ctrl_id, GUIBUTTON_STYLE_DEFAULT);
    }
    CTRLBUTTON_SetBg(ctrl_id, &button_bg_attr);
    CTRLBUTTON_SetPressedBg(ctrl_id, &button_bg_attr);
    CTRLBUTTON_SetPressedFg(ctrl_id, &button_fg_attr);
    CTRLBUTTON_SetFg(ctrl_id, &button_fg_attr);
    CTRLBUTTON_SetText(ctrl_id, button_text.wstr_ptr, button_text.wstr_len);
    //REFRESH
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
}

LOCAL MMI_RESULT_E  PRV_DUERAPP_MessageSelectHandleMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
    char *pTmp = PNULL;
    int result = 0;
    
    SCI_TraceLow("(%s)(%d)[duer_watch]:win_id(0x%0x), msg_id(0x%0x)", __func__, __LINE__, win_id, msg_id);
    
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            PRV_DUERAPP_WinDrawBG(win_id, MMI_BLACK_COLOR);
            PRV_DUERAPP_MessageSelectUiDraw();
            //ÅäÖÃÊäÈë·¨
            MMIDUERAPP_StartInputAsrSet();
            break;
        }
        case MSG_FULL_PAINT:
        {
            break;
        }
        case MSG_CTL_OK:
        case MSG_CTL_MIDSK:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_DUERINPUT_MID_RESULT_SCREEN:
        {
            pTmp = (char *)param;
            if (pTmp) {
                PRV_DUERAPP_ScreenInputTexUpdate(pTmp);
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
            }
            break;
        }
        case MSG_DUERINPUT_FINAL_RESULT_SCREEN:
        {
            pTmp = (char *)param;
            if (pTmp) {
                PRV_DUERAPP_ScreenInputTexUpdate(pTmp);
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
            }
            break;
        }
        case MSG_DUERAPP_DUER_UPDATE://ASR finish
        {
            result = *((int *)param);
            PRV_DUERAPP_MessageSelectRecordCntrl(FALSE, MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_BUTTON1_CTRL_ID);
            if (result >= 0) {
                DUER_LOGI("(%s)[duer_watch]:normal", __func__);
            } else {
                PRV_DUERAPP_Toast(L"ÍøÂç³ö´íÁË");
            }
            break;
        }
        case MSG_CLOSE_WINDOW:
        {
            memset(assists_attr.button_info, 0, sizeof(words_info_t)*DUERAPP_ASSISTS_WORDS_NUMBER);
            assists_attr.last_button_id = -1;
            assists_input_click_cnt = 0;
            PRV_DUERAPP_MessageSelectCtrlDestory(FALSE);
            //input
            MMIDEFAULT_AllowTurnOffBackLight(TRUE);
            if (MMIDUERAPP_GetInitStatus()) {
                duer_asr_stop();
                duer_ext_stop_speak();
                duer_dcs_close_multi_dialog();
            }
            break;
        }
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }

    return recode;
}

LOCAL MMI_RESULT_E PRV_DUERAPP_MessageSelectImgButtonCb(MMI_HANDLE_T ctrl_handle)
{
    int index = -1;
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MMI_HANDLE_T ctrl_id = MMK_GetCtrlId(ctrl_handle);
    //button
    GUI_BG_T button_bg_attr ={GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, 0, FALSE};
    GUI_BG_T button_fg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, 0, 0, TRUE};
    //textbox
    GUI_BG_T text_bg ={GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0xFFFFFF), FALSE};
    GUI_BORDER_T text_border = {1, RGB8882RGB565(0x898989), GUI_BORDER_ROUNDED};
    GUI_COLOR_T text_font_color = MMI_BLACK_COLOR;
    GUI_FONT_T text_font_size = WATCH_DEFAULT_NORMAL_FONT;
    
    static wchar wstr_info[32] = {0};//ï¿½ï¿½ï¿?24ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½Ç°32max
    MMI_STRING_T wtxt = {0};

    switch (ctrl_id)
    {
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_BUTTON1_CTRL_ID :
        {
            assists_input_click_cnt+=1;
            if(assists_input_click_cnt%2 == 0) {
                PRV_DUERAPP_MessageSelectRecordCntrl(FALSE, ctrl_id);
            } else {
                PRV_DUERAPP_MessageSelectRecordCntrl(TRUE, ctrl_id);
            }
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_BUTTON2_CTRL_ID :
        {
            
            memset(wstr_info, 0, sizeof(wchar)*32);
            CTRLBASEFLEX_GetString(MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_EDIT_CTRL_ID, &wtxt);
            MMIAPICOM_Wstrcpy(wstr_info, wtxt.wstr_ptr);
            wtxt.wstr_ptr = wstr_info;
            wtxt.wstr_len = MMIAPICOM_Wstrlen(wtxt.wstr_ptr);
            if (wtxt.wstr_len > 0) {
                MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_ID, MSG_DUERAPP_ASSISTS_WORDS_UPDATE, &wtxt, sizeof(MMI_STRING_T));
            }
            MMK_CloseWin(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID);
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON1_CTRL_ID :
        {
            index = 0;
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON2_CTRL_ID :
        {
            index = 1;
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON3_CTRL_ID :
        {
            index = 2;
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON4_CTRL_ID :
        {
            index = 3;
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON5_CTRL_ID :
        {
            index = 4;
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON6_CTRL_ID :
        {
            index = 5;
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON7_CTRL_ID :
        {
            index = 6;
            break;
        }
        
        case MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON8_CTRL_ID :
        {
            index = 7;
            break;
        }
        
        default:
        {
            break;
        }
    }

    if (index >= 0) {
        if (assists_attr.last_button_id == index) {//ÏàÍ¬²»´¦Àí
            return result;
        }

        if (assists_attr.last_button_id == -1) {
            assists_attr.button_info[index].button_state = TRUE;
            assists_attr.button_info[index].button_ctrl_id = ctrl_id;
            assists_attr.button_info[index].textbox_ctrl_id = ctrl_id + DUERAPP_ASSISTS_WORDS_NUMBER;
            button_fg_attr.img_id = IMAGE_DUERAPP_ASSISTS_SELECT;
            button_fg_attr.is_screen_img = TRUE;
            button_fg_attr.color = RGB8882RGB565(0xFFFFFF);
            button_bg_attr.color = RGB8882RGB565(0xFFFFFF);
            text_font_color = RGB8882RGB565(0xFFFFFF);
            text_bg.color = RGB8882RGB565(0x68AFFD);
            text_border.color = RGB8882RGB565(0x68AFFD);
            goto REFRESH;
        } else {
            assists_attr.button_info[assists_attr.last_button_id].button_state = FALSE;
            button_fg_attr.img_id = IMAGE_DUERAPP_ASSISTS_UNSELECT;
            button_fg_attr.is_screen_img = TRUE;
            button_fg_attr.color = RGB8882RGB565(0xFFFFFF);
            button_bg_attr.color = RGB8882RGB565(0xFFFFFF);
            text_font_color = RGB8882RGB565(0x0);
            text_bg.color = RGB8882RGB565(0xFFFFFF);
            text_border.color = RGB8882RGB565(0x898989);
            //reset
            CTRLBUTTON_SetBg(assists_attr.button_info[assists_attr.last_button_id].button_ctrl_id, &button_bg_attr);
            CTRLBUTTON_SetPressedBg(assists_attr.button_info[assists_attr.last_button_id].button_ctrl_id, &button_bg_attr);
            CTRLBUTTON_SetFg(assists_attr.button_info[assists_attr.last_button_id].button_ctrl_id, &button_fg_attr);
            CTRLBUTTON_SetPressedFg(assists_attr.button_info[assists_attr.last_button_id].button_ctrl_id, &button_fg_attr);
            GUITEXT_SetFont(assists_attr.button_info[assists_attr.last_button_id].textbox_ctrl_id, &text_font_size, &text_font_color);
            GUITEXT_SetBg(assists_attr.button_info[assists_attr.last_button_id].textbox_ctrl_id, &text_bg);
            GUITEXT_SetBorder(&text_border, assists_attr.button_info[assists_attr.last_button_id].textbox_ctrl_id);
        }
        //set
        assists_attr.button_info[index].button_state = TRUE;
        assists_attr.button_info[index].button_ctrl_id = ctrl_id;
        assists_attr.button_info[index].textbox_ctrl_id = ctrl_id + DUERAPP_ASSISTS_WORDS_NUMBER;
        button_fg_attr.img_id = IMAGE_DUERAPP_ASSISTS_SELECT;
        button_fg_attr.is_screen_img = TRUE;
        button_fg_attr.color = RGB8882RGB565(0xFFFFFF);
        button_bg_attr.color = RGB8882RGB565(0xFFFFFF);
        text_font_color = RGB8882RGB565(0xFFFFFF);
        text_bg.color = RGB8882RGB565(0x68AFFD);
        text_border.color = RGB8882RGB565(0x68AFFD);
REFRESH:
        CTRLBUTTON_SetBg(assists_attr.button_info[index].button_ctrl_id, &button_bg_attr);
        CTRLBUTTON_SetPressedBg(assists_attr.button_info[index].button_ctrl_id, &button_bg_attr);
        CTRLBUTTON_SetFg(assists_attr.button_info[index].button_ctrl_id, &button_fg_attr);
        CTRLBUTTON_SetPressedFg(assists_attr.button_info[index].button_ctrl_id, &button_fg_attr);
        GUITEXT_SetFont(assists_attr.button_info[index].textbox_ctrl_id, &text_font_size, &text_font_color);
        GUITEXT_SetBg(assists_attr.button_info[index].textbox_ctrl_id, &text_bg);        
        GUITEXT_SetBorder(&text_border, assists_attr.button_info[index].textbox_ctrl_id);
        //set edit
        CTRLBASEFLEX_SetString(MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_EDIT_CTRL_ID, message_select_text_array[index], MMIAPICOM_Wstrlen(message_select_text_array[index]));
        assists_attr.last_button_id = index;
        //REFRESH
        MMK_SendMsg(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
    
    return result;
}

LOCAL void PRV_DUERAPP_MessageSelectCtrlDestory(BOOLEAN is_create)
{
    static BOOLEAN is_have_create = FALSE;
    BOOLEAN state = FALSE;
    int i = 0;
    int number = 0;
    
    if (is_create) {
        if (is_have_create) {
            state = TRUE;
        }
        is_have_create = is_create;
        if (state) {
            goto DESTORY;
        }
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            goto DESTORY;
        }
    }
    return;
DESTORY:
    number = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_TEXTBOX8_CTRL_ID - MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM_CTRL_ID;
    for (i = number; i >= 0; i--) {
        MMK_DestroyControl(MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_TEXTBOX8_CTRL_ID - (number - i));
    }
}

LOCAL MMI_HANDLE_T PRV_DUERAPP_MessageSelectCtrlCreate(MMI_HANDLE_T win_handle)
{
    MMI_HANDLE_T ctrl_handle = 0;
    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM_CTRL_ID;
    int i = 0;
    GUIFORM_INIT_DATA_T childform_init_data = {0};
    GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};

    // GUIFORM_INIT_DATA_T childform_init_data = {0};
    // GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
    
    GUILABEL_INIT_DATA_T label_init_data = {0};
    GUIFORM_DYNA_CHILD_T label_form_child_ctrl = {0};
    
    GUIEDIT_INIT_DATA_T edit_init_data = {0};
    GUIFORM_DYNA_CHILD_T edit_form_child_ctrl = {0};
    
    GUIBUTTON_INIT_DATA_T button_init_data = {0};
    GUIFORM_DYNA_CHILD_T button_form_child_ctrl = {0};
    
    MMI_HANDLE_T form_ctrl_id = 0;
    MMI_HANDLE_T child_form_ctrl_id = 0;

    // GUIFORM_INIT_DATA_T childform_init_data = {0};
    // GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
    
    // GUILABEL_INIT_DATA_T label_init_data = {0};
    // GUIFORM_DYNA_CHILD_T label_form_child_ctrl = {0};
    
    // GUIBUTTON_INIT_DATA_T button_init_data = {0};
    // GUIFORM_DYNA_CHILD_T button_form_child_ctrl = {0};
    
    GUITEXT_INIT_DATA_T textbox_init_data = {0};
    GUIFORM_DYNA_CHILD_T textbox_form_child_ctrl = {0};
    
    // MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_CTRL_ID;
    // MMI_HANDLE_T child_form_ctrl_id = 0;
    
    //0ï¿½Í·ï¿½
    PRV_DUERAPP_MessageSelectCtrlDestory(TRUE);

    //1¶¯Ì¬´´½¨form
    {
        GUIFORM_CreatDynaCtrl(win_handle, ctrl_id, GUIFORM_LAYOUT_ORDER);
    }

    //2¶¯Ì¬´´½¨×Óform
    {
        
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_CTRL_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        
        for(i = 0; i < 2; i++) {//2ï¿½ï¿½
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
    }

    //3ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    {
        // childform_init_data = {0};
        // childform_form_child_ctrl = {0};
        
        // label_init_data = {0};
        // label_form_child_ctrl = {0};
        
        // edit_init_data = {0};
        // edit_form_child_ctrl = {0};
        
        // button_init_data = {0};
        // button_form_child_ctrl = {0};
        memset(&childform_init_data, 0x00, sizeof(childform_init_data));
        memset(&childform_form_child_ctrl, 0x00, sizeof(childform_form_child_ctrl));

        memset(&label_init_data, 0x00, sizeof(label_init_data));
        memset(&label_form_child_ctrl, 0x00, sizeof(label_form_child_ctrl));

        memset(&edit_init_data, 0x00, sizeof(edit_init_data));
        memset(&edit_form_child_ctrl, 0x00, sizeof(edit_form_child_ctrl));

        memset(&button_init_data, 0x00, sizeof(button_init_data));
        memset(&button_form_child_ctrl, 0x00, sizeof(button_form_child_ctrl));
        
        form_ctrl_id = 0;
        child_form_ctrl_id = 0;
        
        label_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        edit_form_child_ctrl.guid = SPRD_GUI_TEXTEDIT_ID;
        button_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        
        //ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½form1ï¿½ï¿½formï¿½Ø¼ï¿½
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_CTRL_ID;
        
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        //ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½form1ï¿½ï¿½form1ï¿½Ø¼ï¿½LABELï¿½ï¿½EDIT
        child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_CTRL_ID;
        label_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_LABEL_CTRL_ID;
        label_form_child_ctrl.init_data_ptr = &label_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
        
        edit_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_EDIT_CTRL_ID;
        edit_form_child_ctrl.init_data_ptr = &edit_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &edit_form_child_ctrl);
        
        //ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½form1ï¿½ï¿½form2ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È·ï¿½Ï°ï¿½Å?
        child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_CTRL_ID;
        button_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_BUTTON1_CTRL_ID;
        button_form_child_ctrl.init_data_ptr = &button_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &button_form_child_ctrl);
        
        button_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_BUTTON2_CTRL_ID;
        button_form_child_ctrl.init_data_ptr = &button_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &button_form_child_ctrl);
    }

    //4ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½ï¿½ï¿½Ä£ï¿½ï¿½
    {
        // childform_init_data = {0};
        // childform_form_child_ctrl = {0};
        
        // label_init_data = {0};
        // label_form_child_ctrl = {0};
        
        // button_init_data = {0};
        // button_form_child_ctrl = {0};
        
        // textbox_init_data = {0};
        // textbox_form_child_ctrl = {0};
        memset(&childform_init_data, 0x00, sizeof(childform_init_data));
        memset(&childform_form_child_ctrl, 0x00, sizeof(childform_form_child_ctrl));

        memset(&label_init_data, 0x00, sizeof(label_init_data));
        memset(&label_form_child_ctrl, 0x00, sizeof(label_form_child_ctrl));

        memset(&button_init_data, 0x00, sizeof(button_init_data));
        memset(&button_form_child_ctrl, 0x00, sizeof(button_form_child_ctrl));

        memset(&textbox_init_data, 0x00, sizeof(textbox_init_data));
        memset(&textbox_form_child_ctrl, 0x00, sizeof(textbox_form_child_ctrl));

        
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_CTRL_ID;
        child_form_ctrl_id = 0;
        
        label_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        button_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        textbox_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½form2ï¿½ï¿½formï¿½Ø¼ï¿½
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM1_CTRL_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        
        GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_CTRL_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        //ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½form2ï¿½ï¿½form1ï¿½Ø¼ï¿½LABEL
        label_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM1_LABEL_CTRL_ID;
        label_form_child_ctrl.init_data_ptr = &label_init_data;
        child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &label_form_child_ctrl);
        
        //ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½form2ï¿½ï¿½form2ï¿½ï¿½ï¿½ï¿½formï¿½Ø¼ï¿½ï¿½ï¿½Å¥
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_FORM1_CTRL_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_CTRL_ID;
        for(i = 0; i < DUERAPP_ASSISTS_WORDS_NUMBER; i++) {
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_FORM1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
        
        //ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½form2ï¿½ï¿½form2ï¿½ï¿½ï¿½ï¿½formï¿½ï¿½8ï¿½ï¿½buttonï¿½ï¿½textbox
        button_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON1_CTRL_ID;
        button_form_child_ctrl.init_data_ptr = &button_init_data;
        textbox_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_TEXTBOX1_CTRL_ID;
        textbox_form_child_ctrl.init_data_ptr = &textbox_init_data;
        child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_FORM1_CTRL_ID;
        for (i = 0; i < DUERAPP_ASSISTS_WORDS_NUMBER; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_FORM1_CTRL_ID + i;
            button_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON1_CTRL_ID + i;
            textbox_form_child_ctrl.child_handle = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_TEXTBOX1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &textbox_form_child_ctrl);
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &button_form_child_ctrl);
        }
    }

    ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, ctrl_id);
    
    return ctrl_handle;
}

LOCAL void PRV_DUERAPP_MessageSelectUiDraw(void)
{
    MMI_HANDLE_T form_ctrl_handle = PRV_DUERAPP_MessageSelectCtrlCreate(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID);
    int i = 0;

    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, MMI_BLACK_COLOR, FALSE};
    GUI_RECT_T form_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
    IGUICTRL_T *IGuiCtrl = MMK_GetCtrlPtr(form_ctrl_handle);

    // GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0xEFEFEF), FALSE};
    MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_CTRL_ID;
    GUIFORM_CHILD_WIDTH_T child_form_width = {240, GUIFORM_CHILD_WIDTH_FIXED};
    GUI_PADDING_T padding = {5, 5, 5, 5};

    GUI_BG_T param_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
    MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_CTRL_ID;
    // MMI_HANDLE_T child_form_ctrl_id = 0;
    
    GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
    GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    uint16 hor_space = 1;//Ë®Æ½ï¿½ï¿½ï¿?
    uint16 ver_space = 1;//ï¿½ï¿½Ö±ï¿½ï¿½ï¿?
    // GUI_PADDING_T padding = {0};

    //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    MMI_HANDLE_T label_ctrl_id = 0;
    GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
    GUI_FONT_T label_font_size = DUERAPP_ASSISTS_COMMON_FONT;
    MMI_STRING_T label_wshowtxt = {0};

    //ï¿½ï¿½Ê¼ï¿½ï¿½EDITï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    MMI_HANDLE_T edit_ctrl_id = 0;
    GUI_BG_T edit_bg_info = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, MMI_WHITE_COLOR, FALSE};
    GUI_FONT_T edit_font_info = DUERAPP_ASSISTS_VOICE_TEXT_FONT;
    MMI_STRING_T edit_wshowtxt = {0};

    //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    MMI_HANDLE_T button_ctrl_id = 0;
    GUI_BG_T button_bg_attr ={GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0x68AFFD), FALSE};
    GUI_BG_T button_fg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0x68AFFD), TRUE};
    GUI_FONT_ALL_T font_info = {DUERAPP_ASSISTS_COMMON_FONT, MMI_WHITE_COLOR};
    MMI_STRING_T button_text = {0};

    MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_TEXTBOX1_CTRL_ID;
    GUI_BG_T text_bg ={GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0xFFFFFF), FALSE};
    GUI_BORDER_T text_border = {1, RGB8882RGB565(0x898989), GUI_BORDER_ROUNDED};
    GUI_COLOR_T text_font_color = MMI_BLACK_COLOR;
    GUI_FONT_T text_font_size = WATCH_DEFAULT_NORMAL_FONT;

    // GUI_BG_T param_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
    // MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_CTRL_ID;
    // MMI_HANDLE_T child_form_ctrl_id = 0;
    
    // GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
    // GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    // uint16 hor_space = 0;//Ë®Æ½ï¿½ï¿½ï¿?
    // uint16 ver_space = 0;//ï¿½ï¿½Ö±ï¿½ï¿½ï¿?
    // GUI_PADDING_T padding = {0};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    // MMI_HANDLE_T label_ctrl_id = 0;
    label_font_color = MMI_WHITE_COLOR;
    // GUI_FONT_T label_font_size = DUERAPP_ASSISTS_COMMON_FONT;
    // MMI_STRING_T label_wshowtxt = {0};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    // MMI_HANDLE_T button_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON1_CTRL_ID;
    // GUI_BG_T button_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_UNSELECT, RGB8882RGB565(0xFFFFFF), TRUE};
    // GUI_BG_T button_fg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_UNSELECT, RGB8882RGB565(0xFFFFFF), TRUE};
    
    //ï¿½ï¿½Ê¼ï¿½ï¿½TEXTBOXï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
    // MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_TEXTBOX1_CTRL_ID;
    // GUI_BG_T text_bg ={GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0xFFFFFF), FALSE};
    // GUI_BORDER_T text_border = {1, RGB8882RGB565(0x898989), GUI_BORDER_ROUNDED};
    // GUI_COLOR_T text_font_color = MMI_BLACK_COLOR;
    // GUI_FONT_T text_font_size = WATCH_DEFAULT_NORMAL_FONT;
    
    //ï¿½ï¿½ï¿½ï¿½formï¿½ï¿½ï¿½ï¿½
    {   
        CTRLFORM_SetRect(form_ctrl_handle, &form_rect);
        CTRLFORM_SetBg(form_ctrl_handle, &form_bg);
        CTRLFORM_PermitChildBg(form_ctrl_handle, TRUE);
        CTRLFORM_PermitChildFont(form_ctrl_handle, FALSE);
        CTRLFORM_PermitChildBorder(form_ctrl_handle, FALSE);
        // GUICTRL_SetProgress(IGuiCtrl, FALSE);
    }

    //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½formï¿½ï¿½ï¿½ï¿½
    {
        // form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0xEFEFEF), FALSE};
        // child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_CTRL_ID;
        // child_form_width = {240, GUIFORM_CHILD_WIDTH_FIXED};
        // padding = {5, 5, 5, 5};
        
        for(i = 0; i < 2; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_CTRL_ID + i;
            i == 0 ? (form_bg.color = RGB8882RGB565(0xEFEFEF)) : (form_bg.color = RGB8882RGB565(0xFFFFFF));
            CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
            CTRLFORM_SetChildWidth(form_ctrl_handle, child_form_ctrl_id, &child_form_width);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
            CTRLFORM_SetPadding(child_form_ctrl_id, &padding);
        }
    }

    //3ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    {
        // param_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_CTRL_ID;
        child_form_ctrl_id = 0;
        memset(&param_bg, 0x00, sizeof(param_bg));
        param_bg.bg_type = GUI_BG_COLOR;
        param_bg.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        param_bg.img_id = 0;
        param_bg.color = MMI_WHITE_COLOR;
        param_bg.is_screen_img = FALSE;  //is all screen image,only for img
        
        // width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        // height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        hor_space = 1;//Ë®Æ½ï¿½ï¿½ï¿?
        ver_space = 1;//ï¿½ï¿½Ö±ï¿½ï¿½ï¿?
        // padding = {0};
        memset(&width, 0x00, sizeof(width));
        width.add_data = 0;   /*!<?????????????[0,100]*/
        width.type = GUIFORM_CHILD_WIDTH_FIXED;       /*!<????????*/

        memset(&height, 0x00, sizeof(height));
        height.add_data = 0;   /*!<?????????????[0,100]*/
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;       /*!<????????*/

        memset(&padding, 0x00, sizeof(padding));

        //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        label_ctrl_id = 0;
        label_font_color = MMI_WHITE_COLOR;
        label_font_size = DUERAPP_ASSISTS_COMMON_FONT;
        // label_wshowtxt = {0};
        memset(&label_wshowtxt, 0x00, sizeof(label_wshowtxt));

        //ï¿½ï¿½Ê¼ï¿½ï¿½EDITï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        edit_ctrl_id = 0;
        // edit_bg_info = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, MMI_WHITE_COLOR, FALSE};
        memset(&edit_bg_info, 0x00, sizeof(edit_bg_info));
        edit_bg_info.bg_type = GUI_BG_COLOR;
        edit_bg_info.shape = GUI_SHAPE_RECT;          //only for color
        edit_bg_info.img_id = 0;
        edit_bg_info.color = MMI_WHITE_COLOR;
        edit_bg_info.is_screen_img = FALSE;  //is all screen image,only for img
        edit_font_info = DUERAPP_ASSISTS_VOICE_TEXT_FONT;
        // edit_wshowtxt = {0};
        memset(&edit_wshowtxt, 0x00, sizeof(edit_wshowtxt));

        //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        button_ctrl_id = 0;
        // button_bg_attr ={GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0x68AFFD), FALSE};
        // button_fg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0x68AFFD), TRUE};
        // font_info = {DUERAPP_ASSISTS_COMMON_FONT, MMI_WHITE_COLOR};
        // button_text = {0};
        memset(&button_bg_attr, 0x00, sizeof(button_bg_attr));
        button_bg_attr.bg_type = GUI_BG_COLOR;
        button_bg_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        button_bg_attr.img_id = 0;
        button_bg_attr.color = RGB8882RGB565(0x68AFFD);
        button_bg_attr.is_screen_img = FALSE;  //is all screen image,only for img

        memset(&edit_bg_info, 0x00, sizeof(edit_bg_info));
        edit_bg_info.bg_type = GUI_BG_IMG;
        edit_bg_info.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        edit_bg_info.img_id = 0;
        edit_bg_info.color = RGB8882RGB565(0x68AFFD);
        edit_bg_info.is_screen_img = TRUE;  //is all screen image,only for img

        memset(&font_info, 0x00, sizeof(font_info));
        font_info.font = DUERAPP_ASSISTS_COMMON_FONT;
        font_info.color = MMI_WHITE_COLOR;

        memset(&button_text, 0x00, sizeof(button_text));

        param_bg.color = RGB8882RGB565(0xEFEFEF);
        for(i = 0; i < 2; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &param_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
        }
        
        //label-ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        label_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_LABEL_CTRL_ID;
        label_font_color = RGB8882RGB565(0XFA6400);
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        label_wshowtxt.wstr_ptr = L"ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½", label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        
        //edit-ï¿½à¼­ï¿½ï¿½
        width.add_data = 230;
        height.add_data = 100;
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_CTRL_ID;
        edit_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM1_EDIT_CTRL_ID;
        CTRLFORM_SetChildWidth(form_ctrl_id, edit_ctrl_id, &width);
        CTRLFORM_SetChildHeight(form_ctrl_id, edit_ctrl_id, &height);
        
        GUIEDIT_SetImTp(edit_ctrl_id, FALSE);
        GUIEDIT_SetStyle(edit_ctrl_id, GUIEDIT_STYLE_ADAPTIVE);
        GUIEDIT_SetAlign(edit_ctrl_id, ALIGN_HVMIDDLE);
        GUIEDIT_SetCursorHideState(edit_ctrl_id, FALSE);
        GUIEDIT_SetClipboardEnabled(edit_ctrl_id, FALSE);
        GUIEDIT_SetLongCancelDelAll(edit_ctrl_id, FALSE);
        GUIEDIT_SetFont(edit_ctrl_id, edit_font_info);
        GUIEDIT_SetBg(edit_ctrl_id, &edit_bg_info);
        edit_wshowtxt.wstr_ptr = L"µã»÷¡°ÓïÒôÊäÈë¡±£¬Ö±½ÓËµ³öÄãµÄÖúÁ¦ÁôÑÔ¡£(×î¶à24×Ö)";
        edit_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(edit_wshowtxt.wstr_ptr);
        GUIEDIT_SetDefaultString(edit_ctrl_id, edit_wshowtxt.wstr_ptr, edit_wshowtxt.wstr_len);
        GUIEDIT_SetCursorCurPos(edit_ctrl_id, 0);
        GUIEDIT_SetMaxLen(edit_ctrl_id, 24);
        GUIEDIT_SetFontColor(edit_ctrl_id, RGB8882RGB565(0x333333));
        
        //button
        width.add_data = 145;//°´Å¥¿í¶È
        height.add_data = 44;//°´Å¥¸ß¶È
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_CTRL_ID;
        button_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_BUTTON1_CTRL_ID;
        button_fg_attr.img_id = IMAGE_DUERAPP_ASSISTS_INPUT_MIC;
        button_fg_attr.is_screen_img = TRUE;
        CTRLFORM_SetChildWidth(form_ctrl_id, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(form_ctrl_id, button_ctrl_id, &height);
        CTRLFORM_SetPadding(form_ctrl_id, &padding);
        
        CTRLBUTTON_SetStyle(button_ctrl_id, GUIBUTTON_STYLE_LIRTL);
        CTRLBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
        CTRLBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        CTRLBUTTON_SetBg(button_ctrl_id, &button_bg_attr);
        CTRLBUTTON_SetPressedBg(button_ctrl_id, &button_bg_attr);
        CTRLBUTTON_SetFg(button_ctrl_id, &button_fg_attr);
        CTRLBUTTON_SetPressedFg(button_ctrl_id, &button_fg_attr);
        CTRLBUTTON_SetFont(button_ctrl_id, &font_info);
        button_text.wstr_ptr = L"ÓïÒôÊäÈë",  button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
        CTRLBUTTON_SetText(button_ctrl_id, button_text.wstr_ptr, button_text.wstr_len);
        CTRLBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_MessageSelectImgButtonCb);
        
        //button
        width.add_data = 73;//°´Å¥¿í¶È
        height.add_data = 44;//°´Å¥¸ß¶È
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_CTRL_ID;
        button_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM1_FORM2_BUTTON2_CTRL_ID;
        button_fg_attr.img_id = 0;
        button_fg_attr.is_screen_img = FALSE;
        CTRLFORM_SetChildWidth(form_ctrl_id, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(form_ctrl_id, button_ctrl_id, &height);
        
        CTRLBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
        CTRLBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        CTRLBUTTON_SetBg(button_ctrl_id, &button_bg_attr);
        CTRLBUTTON_SetPressedBg(button_ctrl_id, &button_bg_attr);
        CTRLBUTTON_SetFg(button_ctrl_id, &button_fg_attr);
        CTRLBUTTON_SetPressedFg(button_ctrl_id, &button_fg_attr);
        CTRLBUTTON_SetFont(button_ctrl_id, &font_info);
        button_text.wstr_ptr = L"È·ÈÏ",  button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
        CTRLBUTTON_SetText(button_ctrl_id, button_text.wstr_ptr, button_text.wstr_len);
        CTRLBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_MessageSelectImgButtonCb);
    }

    //4ï¿½ï¿½Ì¬ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½ï¿½ï¿½Ä£ï¿½ï¿½
    {
        // GUI_BG_T param_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_CTRL_ID;
        child_form_ctrl_id = 0;
        memset(&param_bg, 0x00, sizeof(param_bg));
        param_bg.bg_type = GUI_BG_COLOR;
        param_bg.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        param_bg.img_id = 0;
        param_bg.color = MMI_WHITE_COLOR;
        param_bg.is_screen_img = FALSE;  //is all screen image,only for img
        
        // GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        // GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        memset(&width, 0x00, sizeof(width));
        width.add_data = 0;   /*!<?????????????[0,100]*/
        width.type = GUIFORM_CHILD_WIDTH_FIXED;       /*!<????????*/

        memset(&height, 0x00, sizeof(height));
        height.add_data = 0;   /*!<?????????????[0,100]*/
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;       /*!<????????*/
        
        hor_space = 0;//Ë®Æ½ï¿½ï¿½ï¿?
        ver_space = 0;//ï¿½ï¿½Ö±ï¿½ï¿½ï¿?
        // GUI_PADDING_T padding = {0};
        memset(&padding, 0x00, sizeof(padding));
        
        //ï¿½ï¿½Ê¼ï¿½ï¿½LABELï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        label_ctrl_id = 0;
        label_font_color = MMI_WHITE_COLOR;
        label_font_size = DUERAPP_ASSISTS_COMMON_FONT;
        // MMI_STRING_T label_wshowtxt = {0};
        memset(&label_wshowtxt, 0x00, sizeof(label_wshowtxt));
        
        //ï¿½ï¿½Ê¼ï¿½ï¿½BUTTONï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        button_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON1_CTRL_ID;
        // GUI_BG_T button_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_UNSELECT, RGB8882RGB565(0xFFFFFF), TRUE};
        // GUI_BG_T button_fg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_ASSISTS_UNSELECT, RGB8882RGB565(0xFFFFFF), TRUE};
        memset(&button_bg_attr, 0x00, sizeof(button_bg_attr));
        button_bg_attr.bg_type = GUI_BG_IMG;
        button_bg_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        button_bg_attr.img_id = IMAGE_DUERAPP_ASSISTS_UNSELECT;
        button_bg_attr.color = RGB8882RGB565(0xFFFFFF);
        button_bg_attr.is_screen_img = TRUE;  //is all screen image,only for img

        memset(&button_fg_attr, 0x00, sizeof(button_fg_attr));
        button_fg_attr.bg_type = GUI_BG_IMG;
        button_fg_attr.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        button_fg_attr.img_id = IMAGE_DUERAPP_ASSISTS_UNSELECT;
        button_fg_attr.color = RGB8882RGB565(0xFFFFFF);
        button_fg_attr.is_screen_img = TRUE;  //is all screen image,only for img

        //ï¿½ï¿½Ê¼ï¿½ï¿½TEXTBOXï¿½Ø¼ï¿½ï¿½Ä²ï¿½ï¿½ï¿½
        text_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_TEXTBOX1_CTRL_ID;
        // GUI_BG_T text_bg ={GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, RGB8882RGB565(0xFFFFFF), FALSE};
        memset(&text_bg, 0x00, sizeof(text_bg));
        text_bg.bg_type = GUI_BG_COLOR;
        text_bg.shape = GUI_SHAPE_ROUNDED_RECT;          //only for color
        text_bg.img_id = 0;
        text_bg.color = RGB8882RGB565(0xFFFFFF);
        text_bg.is_screen_img = FALSE;  //is all screen image,only for img

        // GUI_BORDER_T text_border = {1, RGB8882RGB565(0x898989), GUI_BORDER_ROUNDED};
        memset(&text_border, 0x00, sizeof(text_border));
        text_border.width = 1;      //border width
        text_border.color = RGB8882RGB565(0x898989);      //border color
        text_border.type = GUI_BORDER_ROUNDED;       //border type
        text_font_color = MMI_BLACK_COLOR;
        text_font_size = WATCH_DEFAULT_NORMAL_FONT;
        
        //form
        param_bg.color = RGB8882RGB565(0xFFFFFF);
        for(i = 0; i < 2; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &param_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
        }
        
        //form-8ï¿½ï¿½
        param_bg.color = RGB8882RGB565(0xFFFFFF);
        for(i = 0; i < DUERAPP_ASSISTS_WORDS_NUMBER; i++) {
            child_form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &param_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
        }
        
        //label-ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        label_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM1_LABEL_CTRL_ID;
        label_font_color = RGB8882RGB565(0XFA6400);
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        label_wshowtxt.wstr_ptr = L"¸ü¶àÄ£°å", label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        
        //button-Ä£ï¿½ï¿½Ñ¡ï¿½ï¿½
        form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_FORM1_CTRL_ID;
        button_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON1_CTRL_ID;

        for (i = 0; i < DUERAPP_ASSISTS_WORDS_NUMBER; i++) {
            form_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_FORM1_CTRL_ID + i;
            button_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_BUTTON1_CTRL_ID + i;
            text_ctrl_id = MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM2_FORM2_TEXTBOX1_CTRL_ID + i;
            //button-ï¿½ï¿½ï¿½ï¿½
            width.add_data = 30;
            height.add_data = 60;
            CTRLFORM_SetChildWidth(form_ctrl_id, button_ctrl_id, &width);
            CTRLFORM_SetChildHeight(form_ctrl_id, button_ctrl_id, &height);
            CTRLFORM_SetPadding(form_ctrl_id, &padding);
            CTRLBUTTON_SetRunSheen(button_ctrl_id, FALSE);
            CTRLBUTTON_SetBg(button_ctrl_id, &button_bg_attr);
            CTRLBUTTON_SetPressedBg(button_ctrl_id, &button_bg_attr);
            CTRLBUTTON_SetFg(button_ctrl_id, &button_fg_attr);
            CTRLBUTTON_SetPressedFg(button_ctrl_id, &button_fg_attr);
            CTRLBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_MessageSelectImgButtonCb);
            //textbox-ï¿½ï¿½ï¿½ï¿½
            width.add_data = 190;
            CTRLFORM_SetChildWidth(form_ctrl_id, text_ctrl_id, &width);
            GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
            GUITEXT_SetFont(text_ctrl_id, &text_font_size, &text_font_color);
            GUITEXT_SetBg(text_ctrl_id, &text_bg);
            GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
            GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
            GUITEXT_SetBorder(&text_border, text_ctrl_id);
            GUITEXT_SetString(text_ctrl_id, message_select_text_array[i], MMIAPICOM_Wstrlen(message_select_text_array[i]), TRUE);
        }
    }
    
    MMK_SetAtvCtrl(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID, MMI_DUERAPP_VIP_MESSAGE_SELECT_FORM_CTRL_ID);
}

#endif

#if defined(DUERAPP_ASSISTS_PUBLIC_FUNCTION)

PUBLIC void PUB_DUERAPP_ScreenInputMidResultEvent(char *text, int ret)
{
    int utf8_len = strlen(text) + 1;
    SCI_TraceLow("(%s)(%d)[duer_watch]:", __func__, __LINE__);
    
    if (utf8_len > 1) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:mid show", __func__, __LINE__);
        MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID, MSG_DUERINPUT_MID_RESULT_SCREEN, text, utf8_len);
    }
}

PUBLIC void PUB_DUERAPP_ScreenInputFinalResultEvent(char *text, int ret)
{
    int utf8_len = strlen(text) + 1;
    SCI_TraceLow("(%s)(%d)[duer_watch]:", __func__, __LINE__);
    
    if (utf8_len > 1) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:final show", __func__, __LINE__);
        MMK_duer_other_task_to_MMI(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID, MSG_DUERINPUT_FINAL_RESULT_SCREEN, text, utf8_len);
    }
}

PUBLIC int PUB_DUERAPP_UserListRequest(void)
{
    char temp_url[300] = {0};
    const char *cuid = MMIDUERAPP_GetDeviceUuid();
    
    DUER_LOGI("(%s)(%d)[duer_watch]:request userlist", __FUNCTION__, __LINE__);
    
    sprintf(temp_url,"%s%s?clientId=%s&deviceUuid=%s",
            WECHAT_API_URL_RELEASE,
            "/wechat/watch/user/list",
            DUER_CLIENT_ID,
            cuid);
    return duerapp_wechat_request(PRV_DUERAPP_UserListResponse, temp_url);
}

PUBLIC int PUB_DUERAPP_ParentsAssistsRequest(void)
{
    char tmp_url[300] = {0};
    char utf8_str[256] = {0};
    char tmp_str[512] = {0};
    uint32 wstr_len = 0;
    int i = 0;
    
#if DUERAPP_ASSISTS_SHOW_BIND_SUPPORT
    if (assists_attr.binding_number != 1) {
        for (i = 0; i < assists_attr.binding_number; i++) {
            if (assists_attr.bind_info[i].is_select) {
                DUER_LOGI("(%d)[duer_watch]:openid(%s), i(%d)", __LINE__, assists_attr.bind_info[i].openId, i);
                strcat(tmp_str, assists_attr.bind_info[i].openId);
                i == (assists_attr.binding_number - 1) ? 0 : strcat(tmp_str, ",");
            }
        }
    } else {
        for (i = 0; i < assists_attr.binding_number; i++) {
            DUER_LOGI("(%d)[duer_watch]:openid(%s)", __LINE__, assists_attr.bind_info[i].openId);
            if (assists_attr.bind_info[i].is_select) {
                DUER_LOGI("(%d)[duer_watch]:openid(%s), i(%d)", __LINE__, assists_attr.bind_info[i].openId, i);
                break;
            }
        }
        DUER_LOGI("(%d)[duer_watch]:openid(%s), i(%d)", __LINE__, assists_attr.bind_info[i].openId, i);
        strcpy(tmp_str, assists_attr.bind_info[i].openId);
    }
#else//send all
    for (i = 0; i < assists_attr.binding_number; i++) {
            DUER_LOGI("(%d)[duer_watch]:openid(%s), i(%d)", __LINE__, assists_attr.bind_info[i].openId, i);
            strcat(tmp_str, assists_attr.bind_info[i].openId);
            i == (assists_attr.binding_number - 1) ? 0 : strcat(tmp_str, ",");
    }
#endif
    DUER_LOGI("(%d)[duer_watch]:request parent assists, openid(%s)", __LINE__, tmp_str);
    
    sprintf(tmp_url,"%s%s",
            WECHAT_API_URL_RELEASE,
            "/wechat/watch/message/send");
    wstr_len = MMIAPICOM_Wstrlen(assists_attr.assists_wtxt_words);
    GUI_WstrToUTF8((uint8 *)utf8_str, wstr_len, assists_attr.assists_wtxt_words, wstr_len);
    return duerapp_device_assists_request(PRV_DUERAPP_ParentsAssistsResponse, tmp_url, tmp_str, utf8_str);
}

PUBLIC MMI_HANDLE_T PUB_DUERAPP_AssistsWinOpen(void)
{
    return MMK_CreateWin((uint32 *)MMI_DUERAPP_VIP_ASSISTS_WIN_TAB, PNULL);
}

#if defined(DUERAPP_ASSISTS_VIP_MESSAGE_SELECT)
PUBLIC BOOLEAN PUB_DUERAPP_MessageSelectWinIsOpen(void)
{
    if (!MMK_IsOpenWin(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID) ||
        !MMK_IsFocusWin(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID))
    {
        DUER_LOGI("(%s)(%d)[duer_watch]:not open", __FUNCTION__, __LINE__);
        return FALSE;
    }
    return TRUE;
}
#endif

#if defined(DUERAPP_ASSISTS_VIP_TRANSMIT_MESSAGE)
PUBLIC void PUB_DUERAPP_TransmitMessageWinOpen(void)
{
    MMK_CreateWin((uint32 *)MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_TAB, PNULL);
}
#endif

#endif

