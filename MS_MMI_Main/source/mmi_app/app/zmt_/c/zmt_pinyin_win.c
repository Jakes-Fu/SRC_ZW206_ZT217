/*****************************************************************************
** File Name:      zmt_pinyin_win.c                                              *
** Author:         fys                                               *
** Date:           2024/07/08                                                *
******************************************************************************/
#include "std_header.h"
#include <stdlib.h>
#include "cjson.h"
#include "sci_api.h"
#include "dal_time.h"
#include "guibutton.h"
#include "guifont.h"
#include "guilcd.h"
#include "guitext.h"
#include "guiiconlist.h"
#include "mmi_textfun.h"
#include "mmidisplay_data.h"
#include "mmipub.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmi_filemgr.h"
#include "mmisrvrecord_export.h"
#include "zmt_pinyin_main.h"
#include "zmt_pinyin_id.h"
#include "zmt_pinyin_text.h"
#include "zmt_pinyin_image.h"

#define pinyin_win_rect {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT}//窗口
#define pinyin_title_rect {0, 0, MMI_MAINSCREEN_WIDTH, PINYIN_LINE_HIGHT}//顶部
#define pinyin_list_rect {0, PINYIN_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT-5}//列表
#define pinyin_yinbiao_rect {10, 2.5*PINYIN_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-10, 8*PINYIN_LINE_HIGHT}

PINYIN_INFO_TEXT_T pinyin_info_text[PINYIN_ICON_LIST_ITEM_MAX][PINYIN_SHENG_ITEM_MAX] = {
    {"a", "o", "e", "i", "u", "ü"}, {"ai", "ei", "ao", "ou",  "ie", "iu",  "üe"}, {"an", "en", "in", "un",  "ün"},
    {"ang", "eng", "ing", "ong"}, {"b", "p", "m", "f", "d", "t", "n", "l", "g", "k", "h", "j", "q", "x", "zh", "ch", "sh", "r", "z", "c", "s"},
    {"zhi", "chi", "shi", "ri", "zi", "ci", "si", "yi", "wu", "yu", "ye", "yue", "yuan", "yin", "yun", "ying"}
};

PINYIN_INFO_NUM_T pinyin_info_num[PINYIN_ICON_LIST_ITEM_MAX] = {
    PINYIN_DAN_ITEM_MAX, PINYIN_FU_ITEM_MAX, PINYIN_QIAN_ITEM_MAX, 
    PINYIN_HOU_ITEM_MAX, PINYIN_SHENG_ITEM_MAX, PINYIN_ZHENG_ITEM_MAX
};

LOCAL PINYIN_READ_INFO_T pinyin_read_info = {0};
LOCAL MMISRV_HANDLE_T pinyin_player_handle = PNULL;
LOCAL uint8 pinyin_player_timer_id = 0;
LOCAL MMI_CTRL_ID_T pinyin_cur_select_id = ZMT_PINYIN_BUTTON_1_CTRL_ID;

LOCAL void Pinyin_StopMp3Data(void);
LOCAL void Pinyin_PlayMp3Data(uint8 idx, char * text);
LOCAL void PinyinReadWin_PreCallback(void);
LOCAL void PinyinReadWin_PlayCallback(void);
LOCAL void PinyinReadWin_NextCallback(void);
LOCAL void PinyinReadWin_UpdateButtonBgWin(BOOLEAN is_play);

LOCAL void Pinyin_InitIconlist(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, GUI_RECT_T list_rect, uint16 max_item)
{
    GUI_BORDER_T border={0};
    GUI_BG_T ctrl_bg = {0};
    GUI_FONT_ALL_T font_all = {0};
    GUIICONLIST_MARGINSPACE_INFO_T margin_space = {10,15,0,2};
    
    GUIICONLIST_SetTotalIcon(ctrl_id,max_item);
    GUIICONLIST_SetCurIconIndex(0,ctrl_id);
    GUIICONLIST_SetStyle(ctrl_id,GUIICONLIST_STYLE_ICON);
    GUIICONLIST_SetIconWidthHeight(ctrl_id, 100, 48);
    GUIICONLIST_SetLayoutStyle(ctrl_id,GUIICONLIST_LAYOUT_V);
    border.type = GUI_BORDER_NONE;
    GUIICONLIST_SetItemBorderStyle(ctrl_id,FALSE,&border);
    border.type =GUI_BORDER_ROUNDED;
    border.width = 1;
    border.color = MMI_WHITE_COLOR;
    GUIICONLIST_SetItemBorderStyle(ctrl_id,TRUE,&border);
    GUIICONLIST_SetLoadType(ctrl_id,GUIICONLIST_LOAD_ALL);
    ctrl_bg.bg_type = GUI_BG_COLOR;
    ctrl_bg.color = PINYIN_WIN_BG_COLOR;
    GUIICONLIST_SetBg(ctrl_id, &ctrl_bg);
    GUIICONLIST_SetSlideState(ctrl_id, FALSE);
    GUIICONLIST_SetIconItemSpace(ctrl_id, margin_space);
    GUIICONLIST_SetRect(ctrl_id, &list_rect);
    font_all.color = MMI_WHITE_COLOR;
    font_all.font = DP_FONT_20;
    GUIICONLIST_SetIconListTextInfo(ctrl_id, font_all);
    MMK_SetAtvCtrl(win_id, ctrl_id);
}

LOCAL void Pinyin_InitButton(MMI_CTRL_ID_T ctrl_id, GUI_RECT_T rect, MMI_TEXT_ID_T text_id, GUI_ALIGN_E text_align, BOOLEAN visable, GUIBUTTON_CALLBACK_FUNC func)
{
    GUIBUTTON_SetRect(ctrl_id, &rect);
    GUIBUTTON_SetTextAlign(ctrl_id, text_align);
    GUIBUTTON_SetVisible(ctrl_id, visable, visable);
    if(func != NULL){
        GUIBUTTON_SetCallBackFunc(ctrl_id, func);
    }
    if(text_id != NULL){
        GUIBUTTON_SetTextId(ctrl_id, text_id);
    }
}

LOCAL void Pinyin_DrawWinTitle(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id,MMI_STRING_T text_string, GUI_RECT_T title_rect, GUI_FONT_T font)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
    GUISTR_STYLE_T text_style = {0};
    GUI_RECT_T win_rect = pinyin_win_rect;
    GUI_RECT_T text_rect = title_rect;

    GUI_FillRect(&lcd_dev_info, win_rect, PINYIN_WIN_BG_COLOR);
    GUI_FillRect(&lcd_dev_info, title_rect, PINYIN_TITLE_BG_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = font;
    text_style.font_color = MMI_WHITE_COLOR;

    if(ctrl_id != 0)
    {
        GUILABEL_SetText(ctrl_id, &text_string, TRUE);
    }
    else
    {
        text_rect.left += 10;
        text_rect.right -= 10;
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &text_rect,
            &text_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
    }
}

LOCAL void Pinyin_StopIntervalTimer(void)
{
    if(pinyin_player_timer_id != 0)
    {
        MMK_StopTimer(pinyin_player_timer_id);
        pinyin_player_timer_id = 0;
    }
}

LOCAL void Pinyin_IntervalTimerCallback(uint8 timer_id, uint32 param)
{
    if(pinyin_player_timer_id == timer_id)
    {
        uint8 idx = MMK_GetWinAddDataPtr(ZMT_PINYIN_READ_WIN_ID);
        Pinyin_StopIntervalTimer();
        if(pinyin_read_info.is_single)
        {
            pinyin_read_info.is_play = TRUE;
            Pinyin_PlayMp3Data(idx, pinyin_info_text[idx][pinyin_read_info.cur_read_idx].text);
        }
        else if(pinyin_read_info.is_circulate)
        {
            pinyin_read_info.is_play = TRUE;
            PinyinReadWin_NextCallback();
        }
        else
        {
            pinyin_read_info.is_play = FALSE;
            Pinyin_StopMp3Data();
        }
        PinyinReadWin_UpdateButtonBgWin(pinyin_read_info.is_play);
    }
}

LOCAL void Pinyin_CreateIntervalTimer(void)
{
    uint8 idx = MMK_GetWinAddDataPtr(ZMT_PINYIN_READ_WIN_ID);
    
    Pinyin_StopIntervalTimer();
    pinyin_player_timer_id = MMK_CreateTimerCallback(2000, Pinyin_IntervalTimerCallback, PNULL, FALSE);
    MMK_StartTimerCallback(pinyin_player_timer_id, 2000, Pinyin_IntervalTimerCallback, PNULL, FALSE);
    
    pinyin_read_info.is_play = FALSE;
    PinyinReadWin_UpdateButtonBgWin(pinyin_read_info.is_play);
}

LOCAL BOOLEAN Pinyin_PlayMp3DataCallback(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
    MMISRVAUD_REPORT_T *report_ptr = PNULL;
    if(param != PNULL && handle > 0)
    {
        report_ptr = (MMISRVAUD_REPORT_T *)param->data;
        if(report_ptr != PNULL && handle == pinyin_player_handle)
        {
            switch(report_ptr->report)
            {
                case MMISRVAUD_REPORT_END:  
                    {
                        if(MMK_IsOpenWin(ZMT_PINYIN_READ_WIN_ID)){
                            Pinyin_CreateIntervalTimer();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

LOCAL void Pinyin_StopMp3Data(void)
{
    if(pinyin_player_handle != 0)
    {
        MMISRVAUD_Stop(pinyin_player_handle);
        MMISRVMGR_Free(pinyin_player_handle);
        pinyin_player_handle = 0;
    }
}

LOCAL void Pinyin_PlayMp3Data(uint8 idx, char * text)
{
    MMIAUD_RING_DATA_INFO_T ring_data = {MMISRVAUD_RING_FMT_MIDI, 0, NULL};
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    BOOLEAN result = FALSE;
    char file_path[50] = {0};
    wchar file_name[50] = {0};

    Pinyin_StopMp3Data();

    sprintf(file_path, PINYIN_MP3_DATA_BASE_PATH, idx, text);
    GUI_GBToWstr(file_name, file_path, strlen(file_path));
    if(!MMIFILE_IsFileExist(file_name, MMIAPICOM_Wstrlen(file_name))){
        pinyin_read_info.is_play = FALSE;
        PinyinReadWin_UpdateButtonBgWin(pinyin_read_info.is_play);
        return;
    }
    
    req.is_auto_free = FALSE;
    req.notify = Pinyin_PlayMp3DataCallback;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.info.type = MMISRVAUD_TYPE_RING_FILE;
    audio_srv.info.ring_file.fmt  = MMISRVAUD_RING_FMT_MP3;
    audio_srv.info.ring_file.name = file_name;
    audio_srv.info.ring_file.name_len = MMIAPICOM_Wstrlen(file_name);
    audio_srv.volume=MMIAPISET_GetMultimVolume();

    audio_srv.all_support_route = MMISRVAUD_ROUTE_SPEAKER | MMISRVAUD_ROUTE_EARPHONE;
    pinyin_player_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
    if(pinyin_player_handle > 0)
    {
        result = MMISRVAUD_Play(pinyin_player_handle, 0);
        if(!result)
        {
            SCI_TRACE_LOW("%s pinyin_player_handle error", __FUNCTION__);
            MMISRVMGR_Free(pinyin_player_handle);
            pinyin_player_handle = 0;
        }
        if(result == MMISRVAUD_RET_OK)
        {
            SCI_TRACE_LOW("%s pinyin_player_handle = %d", __FUNCTION__, pinyin_player_handle);
        }
    }
    else
    {
        SCI_TRACE_LOW("%s pinyin_player_handle <= 0", __FUNCTION__);
    }
}

LOCAL void PinyinReadWin_UpdateTopButton(BOOLEAN is_circulate, BOOLEAN is_single)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUI_RECT_T single_rect = pinyin_list_rect;
    GUI_RECT_T fill_rect = {0};
    GUI_BG_T bg = {0};
    bg.bg_type = GUI_BG_IMG;
    
    single_rect.bottom = single_rect.top + PINYIN_LINE_HIGHT;
    single_rect.left = MMI_MAINSCREEN_WIDTH - PINYIN_LINE_WIDTH;
    single_rect.right = MMI_MAINSCREEN_WIDTH;

    fill_rect.top = single_rect.top;
    fill_rect.bottom = single_rect.bottom;
    fill_rect.left = 0;
    fill_rect.right = MMI_MAINSCREEN_WIDTH;
    GUI_FillRect(&lcd_dev_info, fill_rect, PINYIN_WIN_BG_COLOR);

    if(is_circulate){
        bg.img_id = IMG_PINYIN_CIRCULATE_PRE;
    }else{
        bg.img_id = IMG_PINYIN_CIRCULATE_DEF;
    }
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_CIRCULATE_CTRL_ID, &bg);
    GUIBUTTON_Update(ZMT_PINYIN_READ_CIRCULATE_CTRL_ID);

    if(is_single){
        bg.img_id = IMG_PINYIN_SINGLE_PRE;
    }else{
        bg.img_id = IMG_PINYIN_SINGLE_DEF;
    }
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_SINGLE_CTRL_ID, &bg);
    GUIBUTTON_Update(ZMT_PINYIN_READ_SINGLE_CTRL_ID);
}

LOCAL void PinyinReadWin_UpdateButtonBgWin(BOOLEAN is_play)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUI_RECT_T button_rect = pinyin_list_rect;
    GUI_BG_T bg = {0};
    bg.bg_type = GUI_BG_IMG;
    
    button_rect.top = button_rect.bottom - 2*PINYIN_LINE_HIGHT + 5;
    button_rect.bottom = MMI_MAINSCREEN_HEIGHT;
    
    GUI_FillRect(&lcd_dev_info, button_rect, PINYIN_WIN_BG_COLOR);

    if(is_play){
        bg.img_id = IMG_PINYIN_PLAY;
    }else{
        bg.img_id = IMG_PINYIN_STOP;
    }
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_PLAY_CTRL_ID, &bg);
    GUIBUTTON_Update(ZMT_PINYIN_READ_PLAY_CTRL_ID);
    
    bg.img_id = IMG_PINYIN_PREVIEW;
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_PRE_CTRL_ID, &bg);
    GUIBUTTON_Update(ZMT_PINYIN_READ_PRE_CTRL_ID);

    bg.img_id = IMG_PINYIN_NEXT;
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_NEXT_CTRL_ID, &bg);
    GUIBUTTON_Update(ZMT_PINYIN_READ_NEXT_CTRL_ID);
}

LOCAL void PinyinReadWin_CirculateCallback(void)
{
    if(pinyin_read_info.is_circulate){
        pinyin_read_info.is_circulate = FALSE;
    }else{
        pinyin_read_info.is_circulate = TRUE;
        pinyin_read_info.is_single = FALSE;
    }
    PinyinReadWin_UpdateTopButton(pinyin_read_info.is_circulate, pinyin_read_info.is_single);
}

LOCAL void PinyinReadWin_SingleCallback(void)
{
    if(pinyin_read_info.is_single){
        pinyin_read_info.is_single = FALSE;
    }else{
        pinyin_read_info.is_circulate = FALSE;
        pinyin_read_info.is_single = TRUE;
    }
    PinyinReadWin_UpdateTopButton(pinyin_read_info.is_circulate, pinyin_read_info.is_single);
}

LOCAL void PinyinReadWin_PlayCallback(void)
{
    SCI_TRACE_LOW("%s: pinyin_read_info.cur_read_idx = %d", __FUNCTION__, pinyin_read_info.cur_read_idx);
    if(pinyin_read_info.is_play){
        pinyin_read_info.is_play = FALSE;
        Pinyin_StopMp3Data();
    }else{
        uint8 idx = MMK_GetWinAddDataPtr(ZMT_PINYIN_READ_WIN_ID);
        pinyin_read_info.is_play = TRUE;
        Pinyin_PlayMp3Data(idx, pinyin_info_text[idx][pinyin_read_info.cur_read_idx].text);
    }
    Pinyin_StopIntervalTimer();
    PinyinReadWin_UpdateButtonBgWin(pinyin_read_info.is_play);
}

LOCAL void PinyinReadWin_PreCallback(void)
{
    uint8 idx = MMK_GetWinAddDataPtr(ZMT_PINYIN_READ_WIN_ID);
    Pinyin_StopMp3Data();
    Pinyin_StopIntervalTimer();
    if(pinyin_read_info.cur_read_idx > 0)
    {
        pinyin_read_info.cur_read_idx--;
    }
    else
    {
        pinyin_read_info.cur_read_idx = pinyin_info_num[idx].num - 1;
    }
    if(pinyin_read_info.is_play){
        Pinyin_PlayMp3Data(idx, pinyin_info_text[idx][pinyin_read_info.cur_read_idx].text);
    }
    if(MMK_IsFocusWin(ZMT_PINYIN_READ_WIN_ID)){
        MMK_SendMsg(ZMT_PINYIN_READ_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void PinyinReadWin_NextCallback(void)
{
    uint8 idx = MMK_GetWinAddDataPtr(ZMT_PINYIN_READ_WIN_ID);
    Pinyin_StopMp3Data();
    Pinyin_StopIntervalTimer();
    if(pinyin_read_info.cur_read_idx + 1 < pinyin_info_num[idx].num)
    {
        pinyin_read_info.cur_read_idx++;
    }
    else
    {
        pinyin_read_info.cur_read_idx = 0;
    }
    if(pinyin_read_info.is_play){
        Pinyin_PlayMp3Data(idx, pinyin_info_text[idx][pinyin_read_info.cur_read_idx].text);
    }
    if(MMK_IsFocusWin(ZMT_PINYIN_READ_WIN_ID)){
        MMK_SendMsg(ZMT_PINYIN_READ_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void PinyinReadWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T yinbiao_rect = pinyin_yinbiao_rect;
    GUI_RECT_T single_rect = pinyin_list_rect;
    GUI_RECT_T button_rect = pinyin_list_rect;
    GUI_BG_T bg = {0};
    bg.bg_type = GUI_BG_IMG;

    single_rect.top += 1;
    single_rect.bottom = single_rect.top + 1.5*PINYIN_LINE_HIGHT;
    single_rect.left = 0;
    single_rect.right = 2*PINYIN_LINE_WIDTH;
    Pinyin_InitButton(ZMT_PINYIN_READ_CIRCULATE_CTRL_ID, single_rect, NULL, ALIGN_HVMIDDLE, TRUE, PinyinReadWin_CirculateCallback);
    bg.img_id = IMG_PINYIN_CIRCULATE_DEF;
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_CIRCULATE_CTRL_ID, &bg);
    
    single_rect.left = MMI_MAINSCREEN_WIDTH - 2*PINYIN_LINE_WIDTH;
    single_rect.right = MMI_MAINSCREEN_WIDTH;
    Pinyin_InitButton(ZMT_PINYIN_READ_SINGLE_CTRL_ID, single_rect, NULL, ALIGN_HVMIDDLE, TRUE, PinyinReadWin_SingleCallback);
    bg.img_id = IMG_PINYIN_SINGLE_DEF;
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_SINGLE_CTRL_ID, &bg);

    button_rect.top = button_rect.bottom - 2*PINYIN_LINE_HIGHT + 5;
    button_rect.bottom = MMI_MAINSCREEN_HEIGHT;
    button_rect.left = 0;
    button_rect.right = 2*PINYIN_LINE_WIDTH;
    Pinyin_InitButton(ZMT_PINYIN_READ_PRE_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, TRUE, PinyinReadWin_PreCallback);
    bg.img_id = IMG_PINYIN_PREVIEW;
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_PRE_CTRL_ID, &bg);

    button_rect.left = button_rect.right;
    button_rect.right += 2*PINYIN_LINE_WIDTH;
    Pinyin_InitButton(ZMT_PINYIN_READ_PLAY_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, TRUE, PinyinReadWin_PlayCallback);
    bg.img_id = IMG_PINYIN_STOP;
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_PLAY_CTRL_ID, &bg);

    button_rect.left = button_rect.right;
    button_rect.right += 2*PINYIN_LINE_WIDTH;
    Pinyin_InitButton(ZMT_PINYIN_READ_NEXT_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, TRUE, PinyinReadWin_NextCallback);
    bg.img_id = IMG_PINYIN_NEXT;
    GUIBUTTON_SetBg(ZMT_PINYIN_READ_NEXT_CTRL_ID, &bg);

    GUIBUTTON_SetRect(ZMT_PINYIN_READ_YINBIAO_CTRL_ID, &yinbiao_rect);
}

LOCAL void PinyinReadWin_DisplayPinyinTie(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUI_RECT_T yinbiao_rect = pinyin_yinbiao_rect;
    GUI_FONT_ALL_T font = {DP_FONT_24, MMI_WHITE_COLOR};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    wchar text_str[20] = {0};
    uint8 size = 0;
    uint8 idx = MMK_GetWinAddDataPtr(win_id);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;

    LCD_FillRoundedRect(&lcd_dev_info, yinbiao_rect, yinbiao_rect, PINYIN_TITLE_BG_COLOR);

    size = strlen(pinyin_info_text[idx][pinyin_read_info.cur_read_idx].text);
    GUI_GBToWstr(text_str, pinyin_info_text[idx][pinyin_read_info.cur_read_idx].text, size);
    text_string.wstr_ptr = text_str;
    text_string.wstr_len = MMIAPICOM_Wstrlen(text_str);
    yinbiao_rect.top -= 5;
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &yinbiao_rect,
        &yinbiao_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
        );
}

LOCAL void PinyinReadWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T title_rect = pinyin_title_rect;
    MMI_TEXT_ID_T text_id[PINYIN_ICON_LIST_ITEM_MAX] = {
        PINYIN_DAN_YM_TXT, PINYIN_FU_YM_TXT, PINYIN_QIAN_YM_TXT,
        PINYIN_HOU_YM_TXT, PINYIN_SEHNG_MU_TXT, PINYIN_ZHENG_TI_TXT
    };
    uint8 idx = (uint8)MMK_GetWinAddDataPtr(win_id);

    MMIRES_GetText(text_id[idx], win_id, &text_string);
    Pinyin_DrawWinTitle(win_id, 0, text_string, title_rect, DP_FONT_24);

    PinyinReadWin_DisplayPinyinTie(win_id);
}

LOCAL void PinyinReadWin_CLOSE_WINDOW(void)
{
    pinyin_read_info.cur_read_idx = 0;
    Pinyin_StopMp3Data();
    Pinyin_StopIntervalTimer();
}

LOCAL MMI_RESULT_E HandlePinyinReadWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                PinyinReadWin_OPEN_WINDOW(win_id);
                WATCHCOM_Backlight(TRUE);
            }
            break;
        case MSG_FULL_PAINT:
            {
                PinyinReadWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                PinyinReadWin_PlayCallback();
            }
            break;
        case MSG_KEYDOWN_BACKWARD:
        case MSG_KEYDOWN_FORWARD:
            {
                PinyinReadWin_SingleCallback();
            }
            break;
        case MSG_APP_LEFT:
            {
                PinyinReadWin_PreCallback();
            }
            break;
        case MSG_APP_RIGHT:
            {
                PinyinReadWin_NextCallback();
            }
            break;
        case MSG_KEYDOWN_CANCEL:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                PinyinReadWin_CLOSE_WINDOW();
                WATCHCOM_Backlight(FALSE);
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_PINYIN_READ_WIN_TAB) = {
    WIN_ID(ZMT_PINYIN_READ_WIN_ID),
    WIN_FUNC((uint32)HandlePinyinReadWinMsg),
    CREATE_BUTTON_CTRL(PNULL, ZMT_PINYIN_READ_CIRCULATE_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_PINYIN_READ_SINGLE_CTRL_ID),
    CREATE_BUTTON_CTRL(IMG_PINYIN_TIE_BG, ZMT_PINYIN_READ_YINBIAO_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_PINYIN_READ_NEXT_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_PINYIN_READ_PLAY_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_PINYIN_READ_PRE_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_ClosePinyinReadWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(ZMT_PINYIN_READ_WIN_ID)){
        MMK_CloseWin(ZMT_PINYIN_READ_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreatePinyinReadWin(uint8 idx)
{
    MMI_ClosePinyinReadWin();
    MMK_CreateWin((uint32 *)MMI_PINYIN_READ_WIN_TAB, (ADD_DATA)idx);
}

LOCAL void PinyinMainWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    memset(&pinyin_read_info, 0, sizeof(PINYIN_READ_INFO_T));
    pinyin_cur_select_id = ZMT_PINYIN_BUTTON_1_CTRL_ID;
}

LOCAL void PinyinMainWin_DisplayButton(MMI_WIN_ID_T win_id)
{
    uint8 i = 0;
    uint8 j = 0;
    uint8 k = 0;
    MMI_CTRL_ID_T ctrl_id = ZMT_PINYIN_BUTTON_1_CTRL_ID;
    GUI_FONT_ALL_T font = {DP_FONT_24, PINYIN_WIN_BG_COLOR};
    GUI_BORDER_T last_border = {1, MMI_WHITE_COLOR, GUI_BORDER_NONE};
    GUI_BORDER_T border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
    GUI_RECT_T button_rect = pinyin_list_rect;
    MMI_TEXT_ID_T text_id[PINYIN_ICON_LIST_ITEM_MAX] = {
        PINYIN_DAN_YM_TXT, PINYIN_FU_YM_TXT, PINYIN_QIAN_YM_TXT,
        PINYIN_HOU_YM_TXT, PINYIN_SEHNG_MU_TXT, PINYIN_ZHENG_TI_TXT
    };
    button_rect.top += 5;
    button_rect.bottom = button_rect.top + 1.5*PINYIN_LINE_HIGHT;
    for(i = 0;i < 3;i++)
    {
        button_rect.left = 5;
        button_rect.right = MMI_MAINSCREEN_WIDTH / 2 - 5;
        for(j = 0;j < 2;j++)
        {
            ctrl_id = ZMT_PINYIN_BUTTON_1_CTRL_ID + k;
            Pinyin_InitButton(ctrl_id, button_rect, text_id[k], ALIGN_HVMIDDLE, TRUE, NULL);
            if(k == 5){
                font.font = DP_FONT_18;
            }
            GUIBUTTON_SetFont(ctrl_id, &font);
            MMK_SetActiveCtrl(ctrl_id, FALSE);
            button_rect.left = button_rect.right + 5;
            button_rect.right = MMI_MAINSCREEN_WIDTH - 5;
            k++;
        }
        button_rect.top = button_rect.bottom + 10;
        button_rect.bottom = button_rect.top + 1.5*PINYIN_LINE_HIGHT;
    }
    GUIBUTTON_SetBorder(pinyin_cur_select_id, &border, FALSE);
}

LOCAL void PinyinMainWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T title_rect = pinyin_title_rect;

    MMIRES_GetText(PINYIN_CLASS_TXT, win_id, &text_string);
    Pinyin_DrawWinTitle(win_id, 0, text_string, title_rect, DP_FONT_22);

    PinyinMainWin_DisplayButton(win_id);
}

LOCAL void PinyinMainWin_UpdateSelectButton(MMI_CTRL_ID_T ctrl_id)
{
    GUI_BORDER_T last_border = {1, MMI_WHITE_COLOR, GUI_BORDER_NONE};
    GUI_BORDER_T border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
    
    GUIBUTTON_SetBorder(pinyin_cur_select_id, &last_border, TRUE);
    GUIBUTTON_Update(pinyin_cur_select_id);
    GUIBUTTON_SetBorder(ctrl_id, &border, TRUE);
    GUIBUTTON_Update(ctrl_id);
}

LOCAL void PinyinMainWin_APP_LEFT(MMI_WIN_ID_T win_id)
{
    int8 idx = pinyin_cur_select_id - ZMT_PINYIN_BUTTON_1_CTRL_ID; 
    idx--;
    if(idx < 0){
        idx = 1;
    }else{
        idx = -1;
    }
    PinyinMainWin_UpdateSelectButton(pinyin_cur_select_id+idx);
    pinyin_cur_select_id += idx;
    PinyinMainWin_FULL_PAINT(win_id);
}

LOCAL void PinyinMainWin_APP_RIGHT(MMI_WIN_ID_T win_id)
{
    int8 idx = pinyin_cur_select_id - ZMT_PINYIN_BUTTON_1_CTRL_ID;
    idx++;
    if(idx % 2 == 0){
        idx = -1;
    }else{
        idx = 1;
    }
    PinyinMainWin_UpdateSelectButton(pinyin_cur_select_id+idx);
    pinyin_cur_select_id += idx;
    PinyinMainWin_FULL_PAINT(win_id);
}

LOCAL void PinyinMainWin_APP_UP(MMI_WIN_ID_T win_id)
{
    int8 idx = 0;
    if(pinyin_cur_select_id == ZMT_PINYIN_BUTTON_1_CTRL_ID ||
        pinyin_cur_select_id == ZMT_PINYIN_BUTTON_2_CTRL_ID ){
        idx = 4;
    }else{
        idx = -2;
    }
    PinyinMainWin_UpdateSelectButton(pinyin_cur_select_id+idx);
    pinyin_cur_select_id += idx;
    PinyinMainWin_FULL_PAINT(win_id);
}

LOCAL void PinyinMainWin_APP_DOWN(MMI_WIN_ID_T win_id)
{
    int8 idx = 0;
    if(pinyin_cur_select_id == ZMT_PINYIN_BUTTON_5_CTRL_ID ||
        pinyin_cur_select_id == ZMT_PINYIN_BUTTON_6_CTRL_ID ){
        idx = -4;
    }else{
        idx = 2;
    }
    PinyinMainWin_UpdateSelectButton(pinyin_cur_select_id+idx);
    pinyin_cur_select_id += idx;
    PinyinMainWin_FULL_PAINT(win_id);
}

LOCAL void PinyinMainWin_APP_OK(MMI_WIN_ID_T win_id)
{
    int8 cur_idx = pinyin_cur_select_id - ZMT_PINYIN_BUTTON_1_CTRL_ID;
    if(cur_idx < 0){
        cur_idx = 0;
    }
    SCI_TRACE_LOW("%s: cur_idx = %d", __FUNCTION__, cur_idx);
    pinyin_read_info.cur_icon_idx = cur_idx;
    MMI_CreatePinyinReadWin(cur_idx);
}

LOCAL void PinyinMainWin_CTL_PENOK(MMI_WIN_ID_T win_id, DPARAM param)
{
    MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T *)param)->src_id;
    int8 cur_idx = ctrl_id - ZMT_PINYIN_BUTTON_1_CTRL_ID;
    if(cur_idx < 0){
        cur_idx = 0;
        ctrl_id = ZMT_PINYIN_BUTTON_1_CTRL_ID;
    }
    PinyinMainWin_UpdateSelectButton(ctrl_id);
    pinyin_cur_select_id = ctrl_id;
    SCI_TRACE_LOW("%s: cur_idx = %d", __FUNCTION__, cur_idx);
    pinyin_read_info.cur_icon_idx = cur_idx;
    MMI_CreatePinyinReadWin(cur_idx);
}

LOCAL MMI_RESULT_E HandlePinyinMainWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                PinyinMainWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {
                PinyinMainWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_APP_LEFT:
            {
                PinyinMainWin_APP_LEFT(win_id);
            }
            break;
        case MSG_APP_RIGHT:
            {
                PinyinMainWin_APP_RIGHT(win_id);
            }
            break;
        case MSG_APP_UP:
            {
                PinyinMainWin_APP_UP(win_id);
            }
            break;
        case MSG_APP_DOWN:
            {
                PinyinMainWin_APP_DOWN(win_id);
            }
            break;
        case MSG_APP_WEB:
        case MSG_APP_OK:
            {
                PinyinMainWin_APP_OK(win_id);
            }
            break;
        case MSG_CTL_MIDSK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                PinyinMainWin_CTL_PENOK(win_id, param);
            }
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_PINYIN_MAIN_WIN_TAB) = {
    WIN_ID(ZMT_PINYIN_MAIN_WIN_ID),
    WIN_FUNC((uint32)HandlePinyinMainWinMsg),
    CREATE_BUTTON_CTRL(IMG_PINYIN_ICON_BG, ZMT_PINYIN_BUTTON_1_CTRL_ID),
    CREATE_BUTTON_CTRL(IMG_PINYIN_ICON_BG, ZMT_PINYIN_BUTTON_2_CTRL_ID),
    CREATE_BUTTON_CTRL(IMG_PINYIN_ICON_BG, ZMT_PINYIN_BUTTON_3_CTRL_ID),
    CREATE_BUTTON_CTRL(IMG_PINYIN_ICON_BG, ZMT_PINYIN_BUTTON_4_CTRL_ID),
    CREATE_BUTTON_CTRL(IMG_PINYIN_ICON_BG, ZMT_PINYIN_BUTTON_5_CTRL_ID),
    CREATE_BUTTON_CTRL(IMG_PINYIN_ICON_BG, ZMT_PINYIN_BUTTON_6_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_ClosePinyinMainWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(ZMT_PINYIN_MAIN_WIN_ID)){
        MMK_CloseWin(ZMT_PINYIN_MAIN_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreatePinyinMainWin(void)
{
#ifdef LISTENING_PRATICE_SUPPORT
    if(!zmt_tfcard_exist())
    {
        MMI_CreateListeningTipWin(ZMT_PINYIN_MAIN_WIN_ID);
    }
    else
#endif
    {
        MMI_ClosePinyinMainWin();
        MMK_CreateWin((uint32 *)MMI_PINYIN_MAIN_WIN_TAB, PNULL);
    }
}

