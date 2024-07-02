/*****************************************************************************
** File Name:      zmt_class_win.c                                               *
** Author:         fys                                               *
** Date:           2024/06/27                                                *
******************************************************************************/
#include "std_header.h"
#include <stdlib.h>
#include "cjson.h"
#include "dal_time.h"
#include "gps_drv.h"
#include "gps_interface.h"
#include "guibutton.h"
#include "guifont.h"
#include "guilcd.h"
#include "guitext.h"
#include "guiiconlist.h"
#include "mmi_textfun.h"
#include "mmiacc_text.h"
#include "mmicc_export.h"
#include "mmidisplay_data.h"
#include "mmipub.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmisrvrecord_export.h"
#include "zmt_class_main.h"
#include "zmt_class_id.h"
#include "zmt_class_text.h"
#include "zmt_class_image.h"
#ifdef LISTENING_PRATICE_SUPPORT
#include "zmt_main_file.h"
#include "zmt_listening_image.h"
#endif
#ifdef WORD_CARD_SUPPORT
#include "zmt_word_image.h"
#endif
#ifdef MATH_COUNT_SUPPORT
#include "math_count_image.h"
#endif

#define CLASS_SYN_WIN_BG_COLOR GUI_RGB2RGB565(80, 162, 254)
#define CLASS_SYN_TITLE_BG_COLOR GUI_RGB2RGB565(108, 181, 255)
#define CLASS_SYN_TEXT_SELECT_COLOR GUI_RGB2RGB565(253, 180, 80)

#define class_win_rect {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT}//窗口
#define class_title_rect {0, 0, MMI_MAINSCREEN_WIDTH, CLASS_SYN_LINE_HIGHT}//顶部
#define class_list_rect {0, CLASS_SYN_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT-5}//列表
#define class_left_rect {10, MMI_MAINSCREEN_HEIGHT-1.5*CLASS_SYN_LINE_HIGHT, 3*CLASS_SYN_LINE_WIDTH-10, MMI_MAINSCREEN_HEIGHT-5}
#define class_right_rect {3*CLASS_SYN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-1.5*CLASS_SYN_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-10, MMI_MAINSCREEN_HEIGHT-5}
LOCAL GUI_RECT_T class_volume_rect = {0};
LOCAL GUI_RECT_T calss_form_rect = {0};

CLASS_SYNC_INFO_T class_sync_info = {0};
CLASS_BOOK_INFO_T * class_book_info[CLASS_SYN_BOOK_NUM_MAX];
int8 class_book_count = 0;
CLASS_SECTION_INFO_T * class_section_info[CLASS_SYN_SECTION_NUM_MAX];
int8 class_section_count = 0;
CLASS_READ_INFO_T * class_read_info[CLASS_SYN_READ_NUM_MAX];
int8 class_read_count = 0;
CLASS_READ_CURRENT_INFO_T class_cur_info = {0};
int class_repeat_cnt = 0;
LOCAL GUI_FONT_T class_read_font = DP_FONT_20;
LOCAL MMISRV_HANDLE_T class_player_handle = PNULL;
LOCAL GUI_LCD_DEV_INFO class_volume_layer = {0};
LOCAL uint8 class_volume_timer_id = 0;
extern uint8 class_cur_down_idx;
extern BOOLEAN class_download_next_now;
extern BOOLEAN have_new_quest;
#ifdef WIN32
LOCAL uint8 class_test_timer = 0;
LOCAL void Class_StartWin32TestTimer(void);
#endif

LOCAL void Class_SetActiveTextFont(MMI_CTRL_ID_T last_ctrl_id, MMI_CTRL_ID_T ctrl_id);
LOCAL void ClassReadWin_UpdateButtonBgWin(BOOLEAN is_play, BOOLEAN is_single);

LOCAL void Class_DrawWinTitle(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id,MMI_STRING_T text_string, GUI_RECT_T title_rect, GUI_FONT_T font)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
    GUISTR_STYLE_T text_style = {0};
    GUI_RECT_T win_rect = class_win_rect;
    GUI_RECT_T text_rect = title_rect;

    GUI_FillRect(&lcd_dev_info, win_rect, CLASS_SYN_WIN_BG_COLOR);
    GUI_FillRect(&lcd_dev_info, title_rect, CLASS_SYN_TITLE_BG_COLOR);

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

LOCAL void Class_DrawWinTips(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, int8 type)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T win_rect = class_win_rect;
    GUI_RECT_T title_rect = class_title_rect;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_22;
    text_style.font_color = MMI_WHITE_COLOR;

    if(type == 0){
        MMIRES_GetText(CLASS_SYNR_LOADING_TXT, win_id, &text_string);
    }else if(type == -1){
        MMIRES_GetText(CLASS_SYNR_LOAD_FAIL_TXT, win_id, &text_string);
    }else if(type == -2){
        MMIRES_GetText(CLASS_SYNR_REQUEST_TXT, win_id, &text_string);
    }
    if(ctrl_id != 0)
    {
        GUITEXT_SetString(ctrl_id, text_string.wstr_ptr,text_string.wstr_len, TRUE);
    }
    else
    {
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &win_rect,
            &win_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
    }
}

LOCAL void Class_InitButtonBg(MMI_CTRL_ID_T ctrl_id)
{
	GUI_FONT_ALL_T font = {0};
	GUI_BG_T bg = {0};
	GUI_BORDER_T btn_border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
	font.font = DP_FONT_18;
	font.color = CLASS_SYN_WIN_BG_COLOR;
	GUIBUTTON_SetBorder(ctrl_id, &btn_border, FALSE);
	bg.bg_type = GUI_BG_COLOR;
	bg.color = MMI_WHITE_COLOR;
	GUIBUTTON_SetBg(ctrl_id, &bg);
	GUIBUTTON_SetFont(ctrl_id, &font);
}

LOCAL void Class_InitButton(MMI_CTRL_ID_T ctrl_id, GUI_RECT_T rect, MMI_TEXT_ID_T text_id, GUI_ALIGN_E text_align, BOOLEAN visable, GUIBUTTON_CALLBACK_FUNC func)
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

LOCAL void Class_InitListbox(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, GUI_RECT_T list_rect, uint16 max_item)
{
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};

    if(!MMK_GetCtrlHandleByWin(win_id, ctrl_id)){
        list_init.both_rect.v_rect = list_rect;
        list_init.type = GUILIST_TEXTLIST_E;
        GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);
        MMK_SetAtvCtrl(win_id, ctrl_id);
        GUILIST_SetMaxItem(ctrl_id, max_item, FALSE);

        GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_AUTO_SCROLL, FALSE);
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_EFFECT_STR,TRUE);
        GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);
        GUILIST_SetBgColor(ctrl_id, CLASS_SYN_WIN_BG_COLOR);
    }else{
        GUILIST_RemoveAllItems(ctrl_id);
    }
}

LOCAL void Class_InitIconlist(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, GUI_RECT_T list_rect, uint16 max_item)
{
    GUI_BORDER_T border={0};
    GUI_BG_T ctrl_bg = {0};
    GUI_FONT_ALL_T font_all = {0};
    GUIICONLIST_MARGINSPACE_INFO_T margin_space = {10,5,0,2};
    
    GUIICONLIST_SetTotalIcon(ctrl_id,max_item);
    GUIICONLIST_SetCurIconIndex(0,ctrl_id);
    GUIICONLIST_SetStyle(ctrl_id,GUIICONLIST_STYLE_ICON_UIDT);
    GUIICONLIST_SetIconWidthHeight(ctrl_id, 70, 60);
    GUIICONLIST_SetLayoutStyle(ctrl_id,GUIICONLIST_LAYOUT_V);
    border.type = GUI_BORDER_NONE;
    GUIICONLIST_SetItemBorderStyle(ctrl_id,FALSE,&border);
    border.type =GUI_BORDER_ROUNDED;
    border.width = 1;
    border.color = MMI_WHITE_COLOR;
    GUIICONLIST_SetItemBorderStyle(ctrl_id,TRUE,&border);
    GUIICONLIST_SetLoadType(ctrl_id,GUIICONLIST_LOAD_ALL);
    ctrl_bg.bg_type = GUI_BG_COLOR;
    ctrl_bg.color = CLASS_SYN_WIN_BG_COLOR;
    GUIICONLIST_SetBg(ctrl_id, &ctrl_bg);
    GUIICONLIST_SetSlideState(ctrl_id, FALSE);
    GUIICONLIST_SetIconItemSpace(ctrl_id, margin_space);
    GUIICONLIST_SetRect(ctrl_id, &list_rect);
    font_all.color = MMI_WHITE_COLOR;
    font_all.font = DP_FONT_20;
    GUIICONLIST_SetIconListTextInfo(ctrl_id, font_all);
    MMK_SetAtvCtrl(win_id, ctrl_id);
}
////////////////////////////////////////////////////////////////////////////////
LOCAL void ClassReadSetWin_UpdateButton(uint8 label_idx, uint8 button_idx)
{
    uint8 i = 0;
    GUI_BG_T bg = {0};
    GUI_FONT_ALL_T font = {0};
    GUI_BORDER_T btn_border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
    MMI_CTRL_ID_T ctrl_id[CLASS_SYN_READ_SET_REPEAT_MAX][CLASS_SYN_READ_SET_REPEAT_MAX] = {
        {ZMT_CLASS_READ_SET_BUTTON_1_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_2_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_3_CTRL_ID},
        {ZMT_CLASS_READ_SET_BUTTON_4_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_5_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_6_CTRL_ID},
        {ZMT_CLASS_READ_SET_BUTTON_7_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_8_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_9_CTRL_ID}
    };
    bg.bg_type = GUI_BG_COLOR;
    font.font = DP_FONT_18;
    for(i = 0;i < CLASS_SYN_READ_SET_REPEAT_MAX;i++)
    {
        if(i == button_idx){
            bg.color = CLASS_SYN_TEXT_SELECT_COLOR;
            font.color = MMI_WHITE_COLOR;
        }else{
            bg.color = MMI_WHITE_COLOR;
            font.color = CLASS_SYN_WIN_BG_COLOR;
        }
        GUIBUTTON_SetBg(ctrl_id[label_idx][i], &bg);
        GUIBUTTON_SetFont(ctrl_id[label_idx][i], &font);
        GUIBUTTON_SetBorder(ctrl_id[label_idx][i], &btn_border, FALSE);
        GUIBUTTON_Update(ctrl_id[label_idx][i]);
    }
    if(label_idx == 0){
        class_cur_info.repeat_cnt = button_idx + 1;
    }else if(label_idx == 1){
        GUI_FONT_T font[CLASS_SYN_READ_SET_REPEAT_MAX] = {DP_FONT_18, DP_FONT_20, DP_FONT_22};
        class_cur_info.font = font[button_idx];
    }else if(label_idx == 2){
        class_cur_info.speed = button_idx + 1;
    }
}

LOCAL void ClassReadSetWin_RepeatOneCallback(void)
{
    ClassReadSetWin_UpdateButton(0, 0);
}

LOCAL void ClassReadSetWin_RepeatTwoCallback(void)
{
    ClassReadSetWin_UpdateButton(0, 1);
}

LOCAL void ClassReadSetWin_RepeatThreeCallback(void)
{
    ClassReadSetWin_UpdateButton(0, 2);
}
LOCAL void ClassReadSetWin_FontOneCallback(void)
{
    ClassReadSetWin_UpdateButton(1, 0);
}

LOCAL void ClassReadSetWin_FontTwoCallback(void)
{
    ClassReadSetWin_UpdateButton(1, 1);
}

LOCAL void ClassReadSetWin_FontThreeCallback(void)
{
    ClassReadSetWin_UpdateButton(1, 2);
}

LOCAL void ClassReadSetWin_SpeedOneCallback(void)
{
    ClassReadSetWin_UpdateButton(2, 0);
}

LOCAL void ClassReadSetWin_SpeedTwoCallback(void)
{
    ClassReadSetWin_UpdateButton(2, 1);
}

LOCAL void ClassReadSetWin_SpeedThreeCallback(void)
{
    ClassReadSetWin_UpdateButton(2, 2);
}

LOCAL void ClassReadSetWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T title_rect = class_title_rect;
    GUI_RECT_T label_rect = {0};
    GUI_RECT_T button_rect = {0};
    uint8 i = 0;
    uint8 j = 0;
    MMI_CTRL_ID_T label_ctrl_id[CLASS_SYN_READ_SET_REPEAT_MAX] = {
        ZMT_CLASS_READ_SET_LABEL_1_CTRL_ID, ZMT_CLASS_READ_SET_LABEL_2_CTRL_ID, ZMT_CLASS_READ_SET_LABEL_3_CTRL_ID
    };
    MMI_TEXT_ID_T label_text_id[CLASS_SYN_READ_SET_REPEAT_MAX] = {
        CLASS_SYNR_READ_REPEAT_TXT, CLASS_SYNR_READ_FONT_TXT, CLASS_SYNR_READ_SPEED_TXT
    };
    MMI_CTRL_ID_T button_ctrl_id[CLASS_SYN_READ_SET_REPEAT_MAX][CLASS_SYN_READ_SET_REPEAT_MAX] = {
        {ZMT_CLASS_READ_SET_BUTTON_1_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_2_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_3_CTRL_ID},
        {ZMT_CLASS_READ_SET_BUTTON_4_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_5_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_6_CTRL_ID},
        {ZMT_CLASS_READ_SET_BUTTON_7_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_8_CTRL_ID, ZMT_CLASS_READ_SET_BUTTON_9_CTRL_ID}
    };
    MMI_TEXT_ID_T button_text_id[CLASS_SYN_READ_SET_REPEAT_MAX][CLASS_SYN_READ_SET_REPEAT_MAX] = {
        {CLASS_SYNR_READ_REPEAT_1_TXT, CLASS_SYNR_READ_REPEAT_2_TXT, CLASS_SYNR_READ_REPEAT_3_TXT},
        {CLASS_SYNR_READ_FONT_1_TXT, CLASS_SYNR_READ_FONT_2_TXT, CLASS_SYNR_READ_FONT_3_TXT},
        {CLASS_SYNR_READ_SPEED_1_TXT, CLASS_SYNR_READ_SPEED_2_TXT, CLASS_SYNR_READ_SPEED_3_TXT}
    };

    label_rect.left = 5;
    label_rect.right = MMI_MAINSCREEN_WIDTH - 5;
    label_rect.top = title_rect.bottom;
    label_rect.bottom = label_rect.top + CLASS_SYN_LINE_HIGHT;
    for(i = 0;i < CLASS_SYN_READ_SET_REPEAT_MAX - 1;i++)
    {
        GUILABEL_SetRect(label_ctrl_id[i], &label_rect, TRUE);
        GUILABEL_SetTextById(label_ctrl_id[i], label_text_id[i], TRUE);
        GUILABEL_SetFont(label_ctrl_id[i], DP_FONT_20, MMI_WHITE_COLOR);
        button_rect = label_rect;
        button_rect.top = button_rect.bottom;
        button_rect.bottom += 1.2*CLASS_SYN_LINE_HIGHT;
        button_rect.left = button_rect.right = 0;
        for(j = 0;j < CLASS_SYN_READ_SET_REPEAT_MAX;j++)
        {
            button_rect.left = j*2*CLASS_SYN_LINE_WIDTH + 5;
            button_rect.right = (j+1)*2*CLASS_SYN_LINE_WIDTH - 5;
            Class_InitButton(button_ctrl_id[i][j], button_rect, button_text_id[i][j], ALIGN_HVMIDDLE, TRUE, NULL);
            Class_InitButtonBg(button_ctrl_id[i][j]);
        }
        label_rect.top = button_rect.bottom;
        label_rect.bottom = label_rect.top + CLASS_SYN_LINE_HIGHT;
    }
    ClassReadSetWin_UpdateButton(0, class_cur_info.repeat_cnt-1);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[0][0], ClassReadSetWin_RepeatOneCallback);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[0][1], ClassReadSetWin_RepeatTwoCallback);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[0][2], ClassReadSetWin_RepeatThreeCallback);
    class_read_font = class_cur_info.font;
    ClassReadSetWin_UpdateButton(1, (class_cur_info.font-DP_FONT_18)/2);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[1][0], ClassReadSetWin_FontOneCallback);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[1][1], ClassReadSetWin_FontTwoCallback);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[1][2], ClassReadSetWin_FontThreeCallback);
    /*ClassReadSetWin_UpdateButton(2, class_cur_info.speed - 1);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[2][0], ClassReadSetWin_SpeedOneCallback);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[2][1], ClassReadSetWin_SpeedTwoCallback);
    GUIBUTTON_SetCallBackFunc(button_ctrl_id[2][2], ClassReadSetWin_SpeedThreeCallback);*/
}

LOCAL void ClassReadSetWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T title_rect = class_title_rect;

    MMIRES_GetText(CLASS_SYNR_BOOK_READ_SET_TXT, win_id, &text_string);
    Class_DrawWinTitle(win_id, 0, text_string, title_rect, DP_FONT_22);
}

LOCAL MMI_RESULT_E HandleClassReadSetWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ClassReadSetWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {
                ClassReadSetWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
        case MSG_CTL_MIDSK:
            { 
                
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
                if(class_read_font != class_cur_info.font){
                    MMK_DestroyDynaCtrl(ZMT_CLASS_READ_FORM_CTRL_ID);
                }
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_CLASS_READ_SET_WIN_TAB) = {
    WIN_ID(ZMT_CLASS_READ_SET_WIN_ID),
    WIN_FUNC((uint32)HandleClassReadSetWinMsg),
    CREATE_LABEL_CTRL(ALIGN_LVMIDDLE, ZMT_CLASS_READ_SET_LABEL_1_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_1_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_2_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_3_CTRL_ID),
    CREATE_LABEL_CTRL(ALIGN_LVMIDDLE, ZMT_CLASS_READ_SET_LABEL_2_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_4_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_5_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_6_CTRL_ID),
    /*CREATE_LABEL_CTRL(ALIGN_LVMIDDLE, ZMT_CLASS_READ_SET_LABEL_3_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_7_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_8_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SET_BUTTON_9_CTRL_ID),*/
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseClassReadSetWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(ZMT_CLASS_READ_SET_WIN_ID)){
        MMK_CloseWin(ZMT_CLASS_READ_SET_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateClassReadSetWin(void)
{
    MMI_CloseClassReadSetWin();
    MMK_CreateWin((uint32 *)MMI_CLASS_READ_SET_WIN_TAB, PNULL);
}
/////////////////////////////////////////////////////////////////////////
#ifdef WIN32
LOCAL void Class_StopWin32TestTimer(void)
{
    if(class_test_timer != 0)
    {
        MMK_StopTimer(class_test_timer);
        class_test_timer = 0;
    }
}

LOCAL void Class_Win32TestTimerCallback(uint8 timer_id, uint32 param)
{
    if(timer_id == class_test_timer)
    {
        Class_StopWin32TestTimer();
        if(class_cur_info.is_single)
        {
            Class_StartWin32TestTimer();
        }
        else if(class_cur_info.cur_idx + 1 < class_read_count)
        {
            if(class_repeat_cnt + 1 < class_cur_info.repeat_cnt)
            {
                class_repeat_cnt++;
            }
            else
            {
                class_cur_info.cur_idx++;
                class_cur_info.cur_ctrl_id++;
                if(MMK_IsFocusWin(ZMT_CLASS_READ_WIN_ID)){
                    Class_SetActiveTextFont(class_cur_info.cur_ctrl_id-1, class_cur_info.cur_ctrl_id);
                }
            }
            Class_StartWin32TestTimer();
        }
        else
        {
            class_cur_info.is_play = FALSE;
            class_cur_info.is_single = FALSE;
            ClassReadWin_UpdateButtonBgWin(FALSE, FALSE);
        }
    }
}

LOCAL void Class_StartWin32TestTimer(void)
{
    uint32 duration = 0;
    Class_StopWin32TestTimer();
    duration = class_read_info[class_cur_info.cur_idx]->audio_duration * 1000;//ms
    class_test_timer = MMK_CreateTimerCallback(duration, Class_Win32TestTimerCallback, PNULL, FALSE);
    MMK_StartTimerCallback(class_test_timer, duration, Class_Win32TestTimerCallback, PNULL, FALSE);
}
#endif

LOCAL void Class_StopPlayMp3Data(void)
{
    if(class_player_handle)
    {
        SCI_TRACE_LOW("%s: free class_player_handle", __FUNCTION__);
        MMISRVAUD_Stop(class_player_handle);
        MMISRVMGR_Free(class_player_handle);
        class_player_handle = PNULL;
    }
}

LOCAL void Class_PlayerReportEnd(void)
{
    if(class_cur_info.is_single)
    {
        Class_PlayAudioMp3();
    }
    else if(class_cur_info.cur_idx + 1 < class_read_count)
    {
        class_repeat_cnt = 0;
        class_cur_info.cur_idx++;
        class_cur_info.cur_ctrl_id++;
        if(Class_PlayAudioMp3()){
            if(MMK_IsFocusWin(ZMT_CLASS_READ_WIN_ID)){
                Class_SetActiveTextFont(class_cur_info.cur_ctrl_id-1, class_cur_info.cur_ctrl_id);
            }
        }else{
            SCI_TRACE_LOW("%s: REPORT_END, play mp3 fail!!, cur_idx = %d", class_cur_info.cur_idx);
            class_cur_info.cur_idx--;
            class_cur_info.cur_ctrl_id--;
        }
    }
    else
    {
        class_cur_info.is_play = FALSE;
        class_cur_info.is_single = FALSE;
        ClassReadWin_UpdateButtonBgWin(FALSE, FALSE);
        SCI_TRACE_LOW("%s: mp3 play end, stop mp3", __FUNCTION__);
        Class_StopPlayMp3Data();
    }
}

LOCAL BOOLEAN Class_PlayMp3DataNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
    MMISRVAUD_REPORT_T *report_ptr = PNULL;

    if(param != PNULL && handle > 0)
    {
        report_ptr = (MMISRVAUD_REPORT_T *)param->data;
        if(report_ptr != PNULL && handle == class_player_handle)
        {
            switch(report_ptr->report)
                {
                    case MMISRVAUD_REPORT_END:  
                        {
                            Class_PlayerReportEnd();
                        }
                        break;
                    default:
                        break;
                }
        }
    }
    return TRUE;
}

LOCAL BOOLEAN Class_Play_RequestHandle(MMISRV_HANDLE_T *audio_handle, MMISRVAUD_TYPE_U *audio_data, MMISRVMGR_NOTIFY_FUNC notify)
{
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    
    req.notify = notify;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.duation = 0;
    audio_srv.eq_mode = 0;
    audio_srv.is_mixing_enable = FALSE;
    audio_srv.all_support_route = MMISRVAUD_ROUTE_SPEAKER | MMISRVAUD_ROUTE_EARPHONE;
    audio_srv.volume = class_cur_info.volume;
    audio_srv.play_times = class_cur_info.repeat_cnt;

    audio_srv.info.record_file.type = audio_data->type;
    audio_srv.info.record_file.fmt  = audio_data->record_file.fmt;
    audio_srv.info.record_file.name = audio_data->record_file.name;
    audio_srv.info.record_file.name_len = audio_data->record_file.name_len;
    audio_srv.info.record_file.source   = audio_data->record_file.source;
    audio_srv.info.record_file.frame_len= audio_data->record_file.frame_len;

    *audio_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
    if(*audio_handle > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

LOCAL BOOLEAN Class_PlayMp3Data(BOOLEAN is_file, char * file_name, uint8 *data,uint32 data_len)
{
    BOOLEAN result = FALSE;
    MMIRECORD_SRV_RESULT_E srv_result = MMIRECORD_SRV_RESULT_SUCCESS;
    MMISRV_HANDLE_T audio_handle = PNULL;
    MMISRVAUD_TYPE_U    audio_data  = {0};
    uint16 full_path[60] = {0};
    uint16 full_path_len = 0;

    Class_StopPlayMp3Data();

    if(is_file){
        SCI_TRACE_LOW("%s: play file_name = %s", __FUNCTION__, file_name);
        full_path_len = GUI_GBToWstr(full_path, (const uint8*)file_name, SCI_STRLEN(file_name));
        audio_data.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
        audio_data.ring_file.name = full_path;
        audio_data.ring_file.name_len = full_path_len;
    }else{
        audio_data.ring_file.type = MMISRVAUD_TYPE_RING_BUF;
        audio_data.ring_buf.data = data;
        audio_data.ring_buf.data_len = data_len;
    }
    audio_data.ring_file.fmt  = (MMISRVAUD_RING_FMT_E)MMIAPICOM_GetMusicType(audio_data.ring_file.name, audio_data.ring_file.name_len);

    if(Class_Play_RequestHandle(&audio_handle, &audio_data, Class_PlayMp3DataNotify))
    {
        class_player_handle = audio_handle;
        if(!MMISRVAUD_Play(class_player_handle, 0))
        {
            SCI_TRACE_LOW("%s class_player_handle error", __FUNCTION__);
            MMISRVMGR_Free(class_player_handle);
            class_player_handle = PNULL;
        }
        else
        {
            SCI_TRACE_LOW("%s class_player_handle success", __FUNCTION__);
            result = TRUE;
        }
    }
    else
    {
        SCI_TRACE_LOW("%s class_player_handle <= 0", __FUNCTION__);
    }
    return result;
}

PUBLIC BOOLEAN Class_PlayAudioMp3(void)
{
    BOOLEAN result = FALSE;
    if(class_read_info[class_cur_info.cur_idx] == NULL)
    {
        SCI_TRACE_LOW("%s: class_read_info empty!!, cur_idx = %d", __FUNCTION__, class_cur_info.cur_idx);
        return result;
    }
    if(class_read_info[class_cur_info.cur_idx]->audio_len == 0)
    {
        char file_name[30] = {0};
        sprintf(file_name, CLASS_SYN_SENTECT_AUDIO_PATH, class_cur_info.cur_idx);
        if(zmt_file_exist(file_name)){
            class_read_info[class_cur_info.cur_idx]->audio_len = 2;
            Class_PlayAudioMp3();
            result = TRUE;
        }else{
            Class_RequestMp3Data(class_read_info[class_cur_info.cur_idx]->audio_url, class_cur_info.cur_idx, TRUE);
        }
    }
    else if(class_read_info[class_cur_info.cur_idx]->audio_len == -1)
    {
        SCI_TRACE_LOW("%s: audio error, [%d]->audio_len = -1", __FUNCTION__, class_cur_info.cur_idx);
    }
    else if(class_read_info[class_cur_info.cur_idx]->audio_len == -2)
    {
        SCI_TRACE_LOW("%s: audio error, [%d]->audio_len = -2", __FUNCTION__, class_cur_info.cur_idx);
    }
    else if(class_read_info[class_cur_info.cur_idx]->audio_len > 0)
    {
        char file_name[30] = {0};
        sprintf(file_name, CLASS_SYN_SENTECT_AUDIO_PATH, class_cur_info.cur_idx);
        result = Class_PlayMp3Data(TRUE, file_name, class_read_info[class_cur_info.cur_idx]->audio_data, class_read_info[class_cur_info.cur_idx]->audio_len);
    }
    else
    {
        SCI_TRACE_LOW("%s: audio error, [%d]->audio_len = %d", __FUNCTION__, class_cur_info.cur_idx, class_read_info[class_cur_info.cur_idx]->audio_len);
    }
    return result;
}

LOCAL void Class_SetActiveTextFont(MMI_CTRL_ID_T last_ctrl_id, MMI_CTRL_ID_T ctrl_id)
{
    GUI_COLOR_T last_font_color = CLASS_SYN_WIN_BG_COLOR;
    GUI_COLOR_T font_color = CLASS_SYN_TEXT_SELECT_COLOR;
    GUI_FONT_T font_size = class_cur_info.font;

    GUITEXT_SetFont(last_ctrl_id, &font_size, &last_font_color);
    MMK_SendMsg(last_ctrl_id, MSG_CTL_PAINT, PNULL);
    
    GUITEXT_SetFont(ctrl_id, &font_size, &font_color);
    MMK_SendMsg(ctrl_id, MSG_CTL_PAINT, PNULL);

    GUIFORM_SetActiveChild(ZMT_CLASS_READ_FORM_CTRL_ID, ctrl_id);
}

LOCAL void ClassReadWin_UpdateButtonBgWin(BOOLEAN is_play, BOOLEAN is_single)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUI_RECT_T button_rect = class_list_rect;
    GUI_BG_T bg = {0};
    bg.bg_type = GUI_BG_IMG;
    
    button_rect.top = calss_form_rect.bottom + 2;
    button_rect.bottom = MMI_MAINSCREEN_HEIGHT;
    
    GUI_FillRect(&lcd_dev_info, button_rect, CLASS_SYN_WIN_BG_COLOR);

    if(is_play){
        bg.img_id = IMG_CLASS_PLAY;
    }else{
        bg.img_id = IMG_CLASS_PAUSE;
    }
    GUIBUTTON_SetBg(ZMT_CLASS_READ_PLAY_CTRL_ID, &bg);
    GUIBUTTON_Update(ZMT_CLASS_READ_PLAY_CTRL_ID);

    if(is_single){
        bg.img_id = IMG_CLASS_SINGLE_TRUE;
    }else{
        bg.img_id = IMG_CLASS_SINGLE_FALSE;
    }
    GUIBUTTON_SetBg(ZMT_CLASS_READ_SIGLE_CTRL_ID, &bg);
    GUIBUTTON_Update(ZMT_CLASS_READ_SIGLE_CTRL_ID);
    
    bg.img_id = IMG_CLASS_SETTING;
    GUIBUTTON_SetBg(ZMT_CLASS_READ_SETTING_CTRL_ID, &bg);
    GUIBUTTON_Update(ZMT_CLASS_READ_SETTING_CTRL_ID);
}

LOCAL void ClassReadWin_ButtonSingleCallback(void)
{
#ifdef WIN32
    Class_StopWin32TestTimer();
#endif
    if(class_cur_info.is_single)
    {
        class_cur_info.is_single = FALSE;
        Class_StopPlayMp3Data();
    }
    else
    {
        class_cur_info.is_single = TRUE;
    #ifdef WIN32
        Class_StartWin32TestTimer();
    #else
        Class_PlayAudioMp3();
    #endif
    }
    class_cur_info.is_play = FALSE;
    ClassReadWin_UpdateButtonBgWin(FALSE, class_cur_info.is_single);
}

LOCAL void ClassReadWin_ButtonPlayCallback(void)
{
#ifdef WIN32
    Class_StopWin32TestTimer();
#endif
    if(class_cur_info.is_play)
    {
        class_cur_info.is_play = FALSE;
        Class_StopPlayMp3Data();
    }
    else
    {
        class_cur_info.is_play = TRUE;
    #ifdef WIN32
        Class_StartWin32TestTimer();
    #else
        Class_PlayAudioMp3();
    #endif
    }
    class_cur_info.is_single = FALSE;
    ClassReadWin_UpdateButtonBgWin(class_cur_info.is_play, FALSE);
}

LOCAL void ClassReadWin_InitCurPlayerInfo(void)
{
    class_read_count = 0;
    class_cur_down_idx = 0;
    class_download_next_now = FALSE;
    have_new_quest = FALSE;
    class_cur_info.cur_idx = 0;
    class_cur_info.cur_ctrl_id = ZMT_CLASS_READ_FORM_TEXT_CTRL_ID;
    if(class_cur_info.repeat_cnt == 0){
        class_cur_info.repeat_cnt = 1;
    }
    if(class_cur_info.font == 0){
        class_cur_info.font = DP_FONT_20;
    }
    if(class_cur_info.volume == 0){
        class_cur_info.volume = MMISRVAUD_VOLUME_LEVEL_MAX;
    }
    if(class_cur_info.speed == 0){
        class_cur_info.speed = 1;
    }
    Class_ReuestReadInfo(class_section_info[class_sync_info.section_idx]->type_id);
}

LOCAL void ClassReadWin_InitBottomButton(void)
{
    GUI_RECT_T button_rect = class_list_rect;
    GUI_BG_T bg = {0};
    bg.bg_type = GUI_BG_IMG;

    button_rect.top = button_rect.bottom - CLASS_SYN_LINE_HIGHT;
    button_rect.bottom = MMI_MAINSCREEN_HEIGHT;
    button_rect.left = 0;
    button_rect.right = 2*CLASS_SYN_LINE_WIDTH;
    Class_InitButton(ZMT_CLASS_READ_SIGLE_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, FALSE, ClassReadWin_ButtonSingleCallback);
    bg.img_id = IMG_CLASS_SINGLE_FALSE;
    GUIBUTTON_SetBg(ZMT_CLASS_READ_SIGLE_CTRL_ID, &bg);
    button_rect.left = button_rect.right;
    button_rect.right += 2*CLASS_SYN_LINE_WIDTH;
    Class_InitButton(ZMT_CLASS_READ_PLAY_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, FALSE, ClassReadWin_ButtonPlayCallback);
    bg.img_id = IMG_CLASS_PAUSE;
    GUIBUTTON_SetBg(ZMT_CLASS_READ_PLAY_CTRL_ID, &bg);
    button_rect.left = button_rect.right;
    button_rect.right += 2*CLASS_SYN_LINE_WIDTH;
    Class_InitButton(ZMT_CLASS_READ_SETTING_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, FALSE, MMI_CreateClassReadSetWin);
    bg.img_id = IMG_CLASS_SETTING;
    GUIBUTTON_SetBg(ZMT_CLASS_READ_SETTING_CTRL_ID, &bg);
}

LOCAL void ClassReadWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T volume_rect = class_list_rect;

    ClassReadWin_InitCurPlayerInfo();
    ClassReadWin_InitBottomButton();

    volume_rect.top = volume_rect.bottom - CLASS_SYN_LINE_HIGHT + 3;
    volume_rect.bottom = MMI_MAINSCREEN_HEIGHT - 3;
    class_volume_rect = volume_rect;
    if (UILAYER_IsMultiLayerEnable())
    {
        UILAYER_CREATE_T create_info = {0};
        create_info.lcd_id = MAIN_LCD_ID;
        create_info.owner_handle = win_id;
        create_info.offset_x = volume_rect.left;
        create_info.offset_y = volume_rect.top;
        create_info.width = volume_rect.right - volume_rect.left;
        create_info.height = volume_rect.bottom;
        create_info.is_bg_layer = FALSE;
        create_info.is_static_layer = FALSE;   
        UILAYER_CreateLayer(&create_info, &class_volume_layer);
    }
}

LOCAL void ClassReadWin_DisplayFormList(MMI_WIN_ID_T win_id)
{
    uint8 i = 0;
    MMI_CTRL_ID_T form_ctrl_id = ZMT_CLASS_READ_FORM_CTRL_ID;
    MMI_CTRL_ID_T text_ctrl_id = 0;
    MMI_HANDLE_T ctrl_handle = 0;
    uint8 form_margin = 5;
    GUI_RECT_T form_rect = class_list_rect;
    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
    GUI_BG_T text_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
    GUI_COLOR_T font_color = CLASS_SYN_WIN_BG_COLOR;
    GUI_FONT_T font_size = DP_FONT_20;
    GUIFORM_CHILD_WIDTH_T child_width = {0};
    GUIFORM_CHILD_HEIGHT_T child_height = {0};
    MMI_STRING_T text_string = {0};
    wchar text_str[2048] = {0};
    uint16 PixelNum = 0;
    uint16 line_num = 0;
    uint16 width = 0;
    BOOLEAN result = FALSE;
    uint8 span = 2;
    
    font_size = class_cur_info.font;
    if(font_size == DP_FONT_22){
        span = 4;
    }
    
    if(!MMK_GetCtrlHandleByWin(win_id, form_ctrl_id))
    {
        GUIFORM_CreatDynaCtrl(win_id, form_ctrl_id, GUIFORM_LAYOUT_ORDER);
        for(i = 0;i < class_read_count && i < CLASS_SYN_READ_NUM_MAX;i++)
        {
            GUITEXT_INIT_DATA_T text_init_data = {0};
            GUIFORM_DYNA_CHILD_T text_form_child_ctrl = {0};
            text_form_child_ctrl.child_handle = ZMT_CLASS_READ_FORM_TEXT_CTRL_ID + i;
            text_form_child_ctrl.init_data_ptr = &text_init_data;
            text_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
            result = GUIFORM_CreatDynaChildCtrl(win_id, form_ctrl_id, &text_form_child_ctrl);
        }
        ctrl_handle = MMK_GetCtrlHandleByWin(win_id, form_ctrl_id);
        form_rect.bottom -= CLASS_SYN_LINE_HIGHT;
        form_rect.left += form_margin;
        form_rect.right -= form_margin;
        calss_form_rect = form_rect;
        GUIFORM_SetBg(ctrl_handle, &form_bg);
        GUIFORM_SetRect(ctrl_handle, &form_rect);
        GUIFORM_SetDisplayScrollBar(ctrl_handle, FALSE);
        GUIFORM_PermitChildBg(ctrl_handle,TRUE);
        GUIFORM_PermitChildFont(ctrl_handle,TRUE);
        GUIFORM_PermitChildFontColor(ctrl_handle, TRUE);
        GUIFORM_PermitChildBorder(ctrl_handle, FALSE);
        MMK_SetActiveCtrl(ctrl_handle, FALSE);
        if(class_sync_info.subject_type == SUBJECT_CHINESE_TYPE){
            PixelNum = 200;
        }else{
            PixelNum = 160;
        }
        for(i = 0;i < class_read_count && i < CLASS_SYN_READ_NUM_MAX;i++)
        {
            text_ctrl_id = ZMT_CLASS_READ_FORM_TEXT_CTRL_ID + i;
            memset(text_str, 0, 2048);
            GUI_UTF8ToWstr(text_str, 2048, class_read_info[i]->text, strlen(class_read_info[i]->text));
            text_string.wstr_ptr = text_str;
            text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
            line_num = GUI_CalculateStringLinesByPixelNum(PixelNum, text_string.wstr_ptr,text_string.wstr_len,font_size,0,TRUE);
            child_height.type = GUIFORM_CHILD_HEIGHT_FIXED;
            //SCI_TRACE_LOW("%s: line_num = %d", __FUNCTION__, line_num);
            child_height.add_data = line_num * (font_size + span);
            GUIFORM_SetChildHeight(ctrl_handle, text_ctrl_id, &child_height);
            child_width.type = GUIFORM_CHILD_WIDTH_FIXED;
            child_width.add_data = form_rect.right - form_rect.left;
            GUIFORM_SetChildWidth(ctrl_handle, text_ctrl_id, &child_width);
            GUIFORM_SetChildAlign(ctrl_handle, text_ctrl_id, GUIFORM_CHILD_ALIGN_LEFT);
            GUITEXT_SetAlign(text_ctrl_id, ALIGN_LVMIDDLE);
            GUITEXT_SetBg(text_ctrl_id, &text_bg);
            if(i == class_cur_info.cur_idx){
                font_color = CLASS_SYN_TEXT_SELECT_COLOR;
            }else{
                font_color = CLASS_SYN_TITLE_BG_COLOR;
            }
            GUITEXT_SetMarginEx(text_ctrl_id, 10, 0, 0, 0);
            GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
            GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
            GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
            GUITEXT_SetString(text_ctrl_id, text_string.wstr_ptr, text_string.wstr_len, TRUE);
            GUITEXT_IsSlide(text_ctrl_id, FALSE);
            GUITEXT_SetHandleTpMsg(TRUE, text_ctrl_id);
        }
        GUIFORM_SetActiveChild(ctrl_handle, class_cur_info.cur_ctrl_id);
    }
    else
    {
        for(i = 0;i < class_read_count && i < CLASS_SYN_READ_NUM_MAX;i++)
        {
            text_ctrl_id = ZMT_CLASS_READ_FORM_TEXT_CTRL_ID + i;
            memset(text_str, 0, 2048);
            GUI_UTF8ToWstr(text_str, 2048, class_read_info[i]->text, strlen(class_read_info[i]->text));
            text_string.wstr_ptr = text_str;
            text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
             if(i == class_cur_info.cur_idx){
                font_color = CLASS_SYN_TEXT_SELECT_COLOR;
            }else{
                font_color = CLASS_SYN_TITLE_BG_COLOR;
            }
            GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        }
    }
}

PUBLIC void ClassReadWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    wchar section_str[100] = {0};
    GUI_RECT_T win_rect = class_win_rect;
    GUI_RECT_T title_rect = class_title_rect;
    
    GUI_FillRect(&lcd_dev_info, win_rect, CLASS_SYN_WIN_BG_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_20;
    text_style.font_color = MMI_WHITE_COLOR;
    
    GUI_UTF8ToWstr(section_str, 100, class_section_info[class_sync_info.section_idx]->section_name, strlen(class_section_info[class_sync_info.section_idx]->section_name));
    text_string.wstr_ptr = section_str;
    text_string.wstr_len = MMIAPICOM_Wstrlen(section_str);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &title_rect,
        &title_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
        );

    if(class_read_count <= 0)
    {
        Class_DrawWinTips(win_id, 0, class_read_count);
    }
    else
    {
        ClassReadWin_DisplayFormList(win_id);
        GUIBUTTON_SetVisible(ZMT_CLASS_READ_PLAY_CTRL_ID, TRUE, TRUE);
        GUIBUTTON_SetVisible(ZMT_CLASS_READ_SIGLE_CTRL_ID, TRUE, TRUE);
        GUIBUTTON_SetVisible(ZMT_CLASS_READ_SETTING_CTRL_ID, TRUE, TRUE);
    }
}

LOCAL void ClassReadWin_CTL_PENOK(MMI_WIN_ID_T win_id, DPARAM param)
{
    int8 cur_idx = 0;
    MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T *)param)->src_id;
    MMI_CTRL_ID_T last_ctrl_id = class_cur_info.cur_ctrl_id;
    cur_idx = ctrl_id - ZMT_CLASS_READ_FORM_TEXT_CTRL_ID;
    if(cur_idx < 0){
        cur_idx = class_cur_info.cur_idx;
    }
    if(cur_idx == class_cur_info.cur_idx){
        ctrl_id = cur_idx + ZMT_CLASS_READ_FORM_TEXT_CTRL_ID;
        if(class_cur_info.is_single){
            ClassReadWin_ButtonSingleCallback();
        }else{
            ClassReadWin_ButtonPlayCallback();
        }
        if(cur_idx > 0){
            Class_SetActiveTextFont(last_ctrl_id, ctrl_id);
        }
        return;
    }
    Class_StopPlayMp3Data();
    class_cur_info.cur_ctrl_id = ctrl_id;
    class_cur_info.cur_idx = cur_idx;
    class_cur_info.is_play = FALSE;
    class_cur_info.is_single = FALSE;
    ClassReadWin_UpdateButtonBgWin(FALSE, FALSE);
    Class_SetActiveTextFont(last_ctrl_id, class_cur_info.cur_ctrl_id);
}

LOCAL void ClassReadWin_VOL_TimerCallback(uint8 timer_id, uint32 param)
{
    if(class_volume_timer_id != 0)
    {
        MMK_StopTimer(class_volume_timer_id);
        class_volume_timer_id = 0;
    }
    UILAYER_RemoveBltLayer(&class_volume_layer);
}

LOCAL void ClassReadWin_VOL_LayerShow(MMI_WIN_ID_T win_id, uint8 volume)
{
    UILAYER_APPEND_BLT_T append_layer = {0};
    uint8 img_width = 7;
    uint8 img_space = 5;
    uint8 count = 2*volume;
    uint8 i = 0;
    GUI_RECT_T rect_volume = {15, 0, 22, 0};

    UILAYER_RemoveBltLayer(&class_volume_layer);
    append_layer.lcd_dev_info = class_volume_layer;
    append_layer.layer_level = UILAYER_LEVEL_HIGH;
    UILAYER_AppendBltLayer(&append_layer);

    rect_volume.top = class_volume_rect.top + 5;
    rect_volume.bottom = class_volume_rect.bottom - 1;
    class_volume_rect.left = 10;
    class_volume_rect.right = MMI_MAINSCREEN_WIDTH - 10;

    LCD_FillRoundedRect(&class_volume_layer, class_volume_rect, class_volume_rect, GUI_RGB2RGB565(43, 121, 208));
    LCD_DrawRoundedRect(&class_volume_layer, class_volume_rect, class_volume_rect, GUI_RGB2RGB565(43, 121, 208));
    for(i = 0; i < count && i < 2*MMISRVAUD_VOLUME_LEVEL_MAX; i++)
    {
        GUIRES_DisplayImg(PNULL,&rect_volume, PNULL,win_id, MATH_COUNT_PROCESS_IMG, &class_volume_layer);
        rect_volume.left = rect_volume.right + img_space;
        rect_volume.right += img_width + img_space;
    }
}

LOCAL void ClassReadWin_KEYDOWN_VOL(MMI_WIN_ID_T win_id, BOOLEAN is_up)
{
    if(is_up){
        if(class_cur_info.volume < AUD_MAX_SPEAKER_VOLUME){
            class_cur_info.volume++;
         }else{
            return;
         }
    }else{
         if(class_cur_info.volume > 0){
            class_cur_info.volume--;
        }else{
            return;
        }
    }
    ClassReadWin_VOL_LayerShow(win_id, class_cur_info.volume);
    if(class_player_handle != 0){
        MMISRVAUD_SetVolume(class_player_handle, class_cur_info.volume);
    }
    
    if(class_volume_timer_id != 0)
    {
        MMK_StopTimer(class_volume_timer_id);
        class_volume_timer_id = 0;
    }
    class_volume_timer_id = MMK_CreateTimerCallback(5000, ClassReadWin_VOL_TimerCallback, PNULL, FALSE);
    MMK_StartTimerCallback(class_volume_timer_id, 5000, ClassReadWin_VOL_TimerCallback, PNULL, FALSE);
}

LOCAL void ClassReadWin_CLOSE_WINDOW(void)
{
    Class_DeleteReadAudioFile();
    class_read_count = 0;
    Class_RealeaseReadInfo();
    Class_StopPlayMp3Data();
#ifdef WIN32
    Class_StopWin32TestTimer();
#endif
}

LOCAL MMI_RESULT_E HandleClassReadWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ClassReadWin_OPEN_WINDOW(win_id);
                WATCHCOM_Backlight(TRUE);
            }
            break;
        case MSG_FULL_PAINT:
            {
                ClassReadWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                ClassReadWin_CTL_PENOK(win_id, param);
            }
            break;
        case MSG_KEYDOWN_UPSIDE:
        case MSG_KEYDOWN_VOL_UP:
            {
                ClassReadWin_KEYDOWN_VOL(win_id, TRUE);
            }
            break;
        case MSG_KEYDOWN_DOWNSIDE:
        case MSG_KEYDOWN_VOL_DOWN:
            {
                ClassReadWin_KEYDOWN_VOL(win_id, FALSE);
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
                ClassReadWin_CLOSE_WINDOW();
                WATCHCOM_Backlight(FALSE);
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_CLASS_READ_WIN_TAB) = {
    WIN_ID(ZMT_CLASS_READ_WIN_ID),
    WIN_FUNC((uint32)HandleClassReadWinMsg),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SIGLE_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_PLAY_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_READ_SETTING_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseClassReadWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(ZMT_CLASS_READ_WIN_ID)){
        MMK_CloseWin(ZMT_CLASS_READ_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateClassReadWin(void)
{
    MMI_CloseClassReadWin();
    MMK_CreateWin((uint32 *)MMI_CLASS_READ_WIN_TAB, PNULL);
}

LOCAL void ClassSectionWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T left_rect = class_left_rect;
    GUI_RECT_T right_rect = class_right_rect;
    
    Class_InitButton(ZMT_CLASS_SECTION_LEFT_BUTTON_CTRL_ID, left_rect, CLASS_SYNR_BOOK_READ_TXT, ALIGN_HVMIDDLE, FALSE, MMI_CreateClassReadWin);
    Class_InitButtonBg(ZMT_CLASS_SECTION_LEFT_BUTTON_CTRL_ID);

    Class_InitButton(ZMT_CLASS_SECTION_RIGHT_BUTTON_CTRL_ID, right_rect, CLASS_SYNR_AI_EVALUATE_TXT, ALIGN_HVMIDDLE, FALSE, MMI_CreateClassReadWin);
    Class_InitButtonBg(ZMT_CLASS_SECTION_RIGHT_BUTTON_CTRL_ID);
    
    Class_ReuestSectionInfo(class_book_info[class_sync_info.book_idx]->course_id);
}

LOCAL void ClassSectionWin_DisplayList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, BOOLEAN is_single)
{
    uint8 i = 0;
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_string = {0};
    wchar section_str[100] = {0};
    uint16 length = 0;
    GUI_RECT_T list_rect = class_list_rect;

    if(!is_single){
        list_rect.top += 0.5*CLASS_SYN_LINE_HIGHT;
    }
    list_rect.bottom -= 1.5*CLASS_SYN_LINE_HIGHT;
    Class_InitListbox(win_id, ctrl_id, list_rect, CLASS_SYN_SECTION_NUM_MAX);
    
    for(i = 0;i < class_section_count && i < CLASS_SYN_SECTION_NUM_MAX;i++)
    {
        item_t.item_style = GUIITEM_STYLE_CLASS_SECTION_LIST_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_EXPAND;

        memset(&section_str, 0, 100);
        length = strlen(class_section_info[i]->section_name);
        GUI_UTF8ToWstr(section_str, 100, class_section_info[i]->section_name, length);
        text_string.wstr_ptr = section_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(section_str);
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[0].item_data.text_buffer = text_string;
        
        item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
        if(class_sync_info.section_idx == i){
            item_data.item_content[1].item_data.image_id = IMG_ZMT_SELECTED;
        }else{
            item_data.item_content[1].item_data.image_id = IMG_ZMT_UNSELECTED;
        }

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, TRUE);
    GUILIST_SetCurItemIndex(ctrl_id, class_sync_info.section_idx);
}

LOCAL void ClassSectionWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    wchar course_str[100] = {0};
    uint8 line_num = 0;
    GUI_RECT_T title_rect = class_title_rect;

    GUI_UTF8ToWstr(course_str, 100, class_book_info[class_sync_info.book_idx]->name, strlen(class_book_info[class_sync_info.book_idx]->name));
    text_string.wstr_ptr = course_str;
    text_string.wstr_len = MMIAPICOM_Wstrlen(course_str);
    line_num = GUI_CalculateStringLinesByPixelNum(160,text_string.wstr_ptr,text_string.wstr_len,DP_FONT_18,0,TRUE);
    if(line_num > 1){
        title_rect.bottom += 0.5*CLASS_SYN_LINE_HIGHT;
    }
    Class_DrawWinTitle(win_id, 0, text_string, title_rect, DP_FONT_22);
    
    if(class_section_count <= 0)
    {
        Class_DrawWinTips(win_id, 0, class_section_count);
    }
    else
    {
        GUIBUTTON_SetVisible(ZMT_CLASS_SECTION_LEFT_BUTTON_CTRL_ID, TRUE, TRUE);
        GUIBUTTON_SetVisible(ZMT_CLASS_SECTION_RIGHT_BUTTON_CTRL_ID, TRUE, TRUE);
        if(line_num > 1){
            ClassSectionWin_DisplayList(win_id, ZMT_CLASS_SECTION_LIST_CTRL_ID, FALSE);
        }else{
            ClassSectionWin_DisplayList(win_id, ZMT_CLASS_SECTION_LIST_CTRL_ID, TRUE);
        }
    }
}

LOCAL void ClassSectionWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    uint16 cur_idx = GUILIST_GetCurItemIndex(ZMT_CLASS_SECTION_LIST_CTRL_ID);
    if(cur_idx == class_sync_info.section_idx)
    {
        MMI_CreateClassReadWin();
    }
    else
    {
        class_sync_info.section_idx = cur_idx;
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL MMI_RESULT_E HandleClassSectionWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ClassSectionWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {
                ClassSectionWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_APP_OK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                ClassSectionWin_CTL_PENOK(win_id);
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
                class_sync_info.book_idx = 0;
                class_sync_info.section_idx = 0;
                class_section_count = 0;
                Class_ReleaseSectionInfo();
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_CLASS_SECTION_WIN_TAB) = {
    WIN_ID(ZMT_CLASS_SECTION_WIN_ID),
    WIN_FUNC((uint32)HandleClassSectionWinMsg),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_SECTION_LEFT_BUTTON_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, ZMT_CLASS_SECTION_RIGHT_BUTTON_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseClassSectionWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(ZMT_CLASS_SECTION_WIN_ID)){
        MMK_CloseWin(ZMT_CLASS_SECTION_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateClassSectionWin(void)
{
    MMI_CloseClassSectionWin();
    MMK_CreateWin((uint32 *)MMI_CLASS_SECTION_WIN_TAB, PNULL);
}

LOCAL void ClassBookWin_DisplayList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_IMAGE_ID_T img_id = 0;
    MMI_STRING_T text_string = {0};
    wchar book_str[100] = {0};
    uint16 length = 0;
    GUI_RECT_T list_rect = class_list_rect;
    list_rect.top += 10;

    Class_InitListbox(win_id, ctrl_id, list_rect, CLASS_SYN_BOOK_NUM_MAX);

    if(class_sync_info.subject_type == SUBJECT_CHINESE_TYPE){
        img_id = IMG_CLASS_CHINESE_BOOK;
    }else{
        img_id = IMG_CLASS_ENGLISH_BOOK;
    }
    
    for(i = 0;i < class_book_count && i < CLASS_SYN_BOOK_NUM_MAX;i++)
    {
        item_t.item_style = GUIITEM_STYLE_CLASS_MAIN_LIST_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_EXPAND;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = IMG_CLASS_MAIN_ITEM_BG;

        item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[1].item_data.image_id = img_id;

        memset(&book_str, 0, 100);
        length = strlen(class_book_info[i]->name);
        GUI_UTF8ToWstr(book_str, 100, class_book_info[i]->name, length);
        text_string.wstr_ptr = book_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(book_str);
        item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[2].item_data.text_buffer = text_string;

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    GUILIST_SetTextFont(ctrl_id, DP_FONT_18, CLASS_SYN_WIN_BG_COLOR);
    GUILIST_SetCurItemIndex(ctrl_id, class_sync_info.book_idx);
}

LOCAL void ClassBookWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T title_rect = class_title_rect;

    MMIRES_GetText(CLASS_SYNR_BOOK_TXT, win_id, &text_string);
    Class_DrawWinTitle(win_id, 0, text_string, title_rect, DP_FONT_22);

    if(class_book_count <= 0)
    {
        Class_DrawWinTips(win_id, 0, class_book_count);
    }
    else
    {
        ClassBookWin_DisplayList(win_id, ZMT_CLASS_BOOK_LIST_CTRL_ID);
    }
}

LOCAL void ClassBookWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    uint16 cur_idx = GUILIST_GetCurItemIndex(ZMT_CLASS_BOOK_LIST_CTRL_ID);
    class_sync_info.book_idx = cur_idx;
    MMI_CreateClassSectionWin();
}

LOCAL MMI_RESULT_E HandleClassBookWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                Class_ReuestBookInfo(class_sync_info.subject_type, class_sync_info.grade_idx+1, 1);
            }
            break;
        case MSG_FULL_PAINT:
            {
                ClassBookWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                ClassBookWin_CTL_PENOK(win_id);
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
                class_sync_info.grade_idx = 0;
                class_book_count = 0;
                Class_ReleaseBookInfo();
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_CLASS_BOOK_WIN_TAB) = {
    WIN_ID(ZMT_CLASS_BOOK_WIN_ID),
    WIN_FUNC((uint32)HandleClassBookWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseClassBookWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(ZMT_CLASS_BOOK_WIN_ID)){
        MMK_CloseWin(ZMT_CLASS_BOOK_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateClassBookWin(void)
{
    MMI_CloseClassBookWin();
    MMK_CreateWin((uint32 *)MMI_CLASS_BOOK_WIN_TAB, PNULL);
}

LOCAL void ClassGradeWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T list_rect = class_list_rect;
    list_rect.top += 10;
    Class_InitIconlist(win_id, ZMT_CLASS_GRADE_ICONLIST_CTRL_ID, list_rect, CLASS_SYN_GRADE_NUM_MAX);
}

LOCAL void ClassGradeWin_APPEND_ICON(MMI_WIN_ID_T win_id, DPARAM param)
{
    uint16 icon_index = *((uint16 *)param);
    GUIANIM_DATA_INFO_T data_info = {0};
    data_info.img_id = IMG_CLASS_GRADE_ICON;
    GUIICONLIST_AppendIcon(icon_index, ZMT_CLASS_GRADE_ICONLIST_CTRL_ID, &data_info,PNULL);
}

LOCAL void ClassGradeWin_APPEND_TEXT(MMI_WIN_ID_T win_id, DPARAM param)
{
    uint16 icon_index = *((uint16 *)param);
    GUIICONLIST_DISP_T icon_list = {0};
    MMI_TEXT_ID_T text_id[CLASS_SYN_GRADE_NUM_MAX] = {
        CLASS_SYNR_GRADE_1_TXT, CLASS_SYNR_GRADE_2_TXT, CLASS_SYNR_GRADE_3_TXT,
        CLASS_SYNR_GRADE_4_TXT, CLASS_SYNR_GRADE_5_TXT, CLASS_SYNR_GRADE_6_TXT,
        CLASS_SYNR_GRADE_7_TXT, CLASS_SYNR_GRADE_8_TXT, CLASS_SYNR_GRADE_9_TXT
    };
    MMIRES_GetText(text_id[icon_index], win_id, &icon_list.name_str);
    GUIICONLIST_AppendText(icon_index, ZMT_CLASS_GRADE_ICONLIST_CTRL_ID,&icon_list);
}

LOCAL void ClassGradeWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T title_rect = class_title_rect;
    
    if(class_sync_info.subject_type == SUBJECT_CHINESE_TYPE){
        MMIRES_GetText(CLASS_SYNR_GRADE_CHINESE_TXT, win_id, &text_string);
    }else{
        MMIRES_GetText(CLASS_SYNR_GRADE_ENGLISH_TXT, win_id, &text_string);
    }
    Class_DrawWinTitle(win_id, 0, text_string, title_rect, DP_FONT_22);
}

LOCAL void ClassGradeWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    uint16 cur_idx = GUIICONLIST_GetCurIconIndex(ZMT_CLASS_GRADE_ICONLIST_CTRL_ID);
    class_sync_info.grade_idx = cur_idx;
    MMI_CreateClassBookWin();
}


LOCAL MMI_RESULT_E HandleClassGradeWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ClassGradeWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {
                ClassGradeWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_CTL_ICONLIST_APPEND_ICON:
            {
                ClassGradeWin_APPEND_ICON(win_id, param);
            }
            break;
        case MSG_CTL_ICONLIST_APPEND_TEXT:
            {
                ClassGradeWin_APPEND_TEXT(win_id, param);
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
        case MSG_CTL_MIDSK:
            { 
                ClassGradeWin_CTL_PENOK(win_id);
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
                class_sync_info.subject_type = 0;
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_CLASS_GRADE_WIN_TAB) = {
    WIN_ID(ZMT_CLASS_GRADE_WIN_ID),
    WIN_FUNC((uint32)HandleClassGradeWinMsg),
    CREATE_ICONLIST_CTRL(ZMT_CLASS_GRADE_ICONLIST_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseClassGradeWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(ZMT_CLASS_GRADE_WIN_ID)){
        MMK_CloseWin(ZMT_CLASS_GRADE_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateClassGradeWin(void)
{
    MMI_CloseClassGradeWin();
    MMK_CreateWin((uint32 *)MMI_CLASS_GRADE_WIN_TAB, PNULL);
}

LOCAL void ClassMainWin_DisplayList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_IMAGE_ID_T img_id[CLASS_SYN_SUBJECT_NUM_MAX] = {IMG_CLASS_CHINESE, IMG_CLASS_ENGLISH};
    MMI_TEXT_ID_T text_id[CLASS_SYN_SUBJECT_NUM_MAX] = {CLASS_SYNR_CHINESE_TXT, CLASS_SYNR_ENGLISH_TXT};
    
    for(i = 0;i < CLASS_SYN_SUBJECT_NUM_MAX;i++)
    {
        item_t.item_style = GUIITEM_STYLE_CLASS_MAIN_LIST_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = IMG_CLASS_MAIN_ITEM_BG;

        item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[1].item_data.image_id = img_id[i];

        item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_ID;
        item_data.item_content[2].item_data.text_id = text_id[i];

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
}

LOCAL void ClassMainWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T list_rect = class_list_rect;
    list_rect.top += 10;
    Class_InitListbox(win_id, ZMT_CLASS_MAIN_LIST_CTRL_ID, list_rect, CLASS_SYN_SUBJECT_NUM_MAX);
    GUILIST_SetSlideState(ZMT_CLASS_MAIN_LIST_CTRL_ID, FALSE);
    GUILIST_SetTextFont(ZMT_CLASS_MAIN_LIST_CTRL_ID, DP_FONT_24, CLASS_SYN_WIN_BG_COLOR);

    ClassMainWin_DisplayList(win_id, ZMT_CLASS_MAIN_LIST_CTRL_ID);
}

LOCAL void ClassMainWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T title_rect = class_title_rect;

    MMIRES_GetText(CLASS_SYNR_TXT, win_id, &text_string);
    Class_DrawWinTitle(win_id, 0, text_string, title_rect, DP_FONT_22);
}

LOCAL void ClassMainWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    uint16 cur_idx = GUILIST_GetCurItemIndex(ZMT_CLASS_MAIN_LIST_CTRL_ID);
    if(cur_idx == 0){
        class_sync_info.subject_type = SUBJECT_CHINESE_TYPE;
    }else{
        class_sync_info.subject_type = SUBJECT_ENGLISH_TYPE;
    }
    MMI_CreateClassGradeWin();
}

LOCAL MMI_RESULT_E HandleClassMainWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ClassMainWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {
                ClassMainWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                ClassMainWin_CTL_PENOK(win_id);
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
                memset(&class_sync_info, 0, sizeof(CLASS_SYNC_INFO_T));
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_CLASS_MAIN_WIN_TAB) = {
    WIN_ID(ZMT_CLASS_MAIN_WIN_ID),
    WIN_FUNC((uint32)HandleClassMainWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseClassMainWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(ZMT_CLASS_MAIN_WIN_ID)){
        MMK_CloseWin(ZMT_CLASS_MAIN_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateClassMainWin(void)
{
    MMI_CloseClassMainWin();
    MMK_CreateWin((uint32 *)MMI_CLASS_MAIN_WIN_TAB, PNULL);
}

