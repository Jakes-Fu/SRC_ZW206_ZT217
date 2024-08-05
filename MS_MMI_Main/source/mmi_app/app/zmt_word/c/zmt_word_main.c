#include "std_header.h"

#include <stdlib.h>

#include "dal_time.h"
#include "zmt_word_id.h"
#include "zmt_word_main.h"
#include "zmt_word_text.h"
#include "zmt_word_image.h"
#include "gps_drv.h"
#include "gps_interface.h"
#include "guires.h"
#include "guibutton.h"
#include "guifont.h"
#include "guilcd.h"
#include "guistring.h"
#include "guitext.h"
#include "ui_layer.h"
#include "mmi_textfun.h"
#include "mmiacc_text.h"
#include "mmicc_export.h"
#include "mmidisplay_data.h"
#include "mmipub.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmisrvrecord_export.h"
#ifdef HANZI_CARD_SUPPORT
#include "zmt_main_file.h"
#include "zmt_hanzi_image.h"
#include "zmt_hanzi_text.h"
#endif
#ifdef POETRY_LISTEN_SUPPORT
#include "zmt_poetry_image.h"
#endif
#ifdef FORMULA_SUPPORT
#include "formula_image.h"
#endif

#define WORD_WIN_BG_COLOR GUI_RGB2RGB565(80, 162, 254)
#define WORD_TITLE_BG_COLOR GUI_RGB2RGB565(108, 181, 255)
#define WORD_BUTTON_BG_COLOR GUI_RGB2RGB565(255, 154, 0)

#define word_msg_tips_rect {WORD_CARD_LINE_WIDTH, 3*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH - WORD_CARD_LINE_WIDTH, 7*WORD_CARD_LINE_HIGHT}
#define word_msg_tips_left_rect {1.2*WORD_CARD_LINE_WIDTH, 5.5*WORD_CARD_LINE_HIGHT, 2.7*WORD_CARD_LINE_WIDTH, 6.5*WORD_CARD_LINE_HIGHT}
#define word_msg_tips_right_rect {3.3*WORD_CARD_LINE_WIDTH, 5.5*WORD_CARD_LINE_HIGHT, 4.8*WORD_CARD_LINE_WIDTH, 6.5*WORD_CARD_LINE_HIGHT}
#define word_win_rect {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT}
#define word_title_rect {0, 0, MMI_MAINSCREEN_WIDTH, WORD_CARD_LINE_HIGHT}
#define word_auto_play_rect {4.5*WORD_CARD_LINE_WIDTH, 0, MMI_MAINSCREEN_WIDTH, WORD_CARD_LINE_HIGHT}
#define word_list_rect {0, WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT-5}
#define word_dir_rect {5*WORD_CARD_LINE_WIDTH-10, 0, MMI_MAINSCREEN_WIDTH, WORD_CARD_LINE_HIGHT}
#define word_word_rect {5, WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 2*WORD_CARD_LINE_HIGHT}
#define word_pinyin_rect {5, 1.8*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-5, 2.8*WORD_CARD_LINE_HIGHT}
#define word_pinyin_audio_rect {5*WORD_CARD_LINE_WIDTH, 1.8*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-5, 2.8*WORD_CARD_LINE_HIGHT}
#define word_text_rect {5,2.8*WORD_CARD_LINE_HIGHT+1,MMI_MAINSCREEN_WIDTH-5,MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT-5}
#define word_msg_rect {0.8*WORD_CARD_LINE_WIDTH, 3*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-0.8*WORD_CARD_LINE_WIDTH, 5*WORD_CARD_LINE_HIGHT}
#define word_left_rect {10, MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT, 3*WORD_CARD_LINE_WIDTH-10, MMI_MAINSCREEN_HEIGHT-5}
#define word_right_rect {3*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-10, MMI_MAINSCREEN_HEIGHT-5}
#define word_pre_rect {0.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 1.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2}
#define word_del_rect {2.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 3.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2}
#define word_next_rect {4.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 5.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2}
#define word_tip_rect {WORD_CARD_LINE_WIDTH,4*WORD_CARD_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-WORD_CARD_LINE_WIDTH,6*WORD_CARD_LINE_HIGHT}
#define word_form_rect {0, WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT - 1.5*WORD_CARD_LINE_HIGHT}
#define word_bottom_rect {1.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT, 4.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-5}
#define word_left_button_rect {9, MMI_MAINSCREEN_HEIGHT - 1.5*WORD_CARD_LINE_HIGHT, 115, MMI_MAINSCREEN_HEIGHT - 2}
#define word_right_button_rect {125, MMI_MAINSCREEN_HEIGHT - 1.5*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-9, MMI_MAINSCREEN_HEIGHT - 2}
LOCAL GUI_RECT_T word_Hor_line_rect = {0};

LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;

WORD_LEARN_INFO_T * word_learn_info = NULL;
WORD_BOOK_INFO_T word_book_info = {0};
int8 word_publish_count = 0;
int8 word_chapter_count = 0;
int16 word_detail_count = 0;
WORD_BOOK_PUBLISH_INFO_T * word_publish_info[WORD_PUBLISH_MAX];
WORD_BOOK_CHAPTER_T * word_chapter_info[WORD_CHAPTER_NUM_MAX];
WORD_BOOK_DETAIL_T * new_word_detail_info[WORD_CHAPTER_WORD_MAX];
BOOLEAN word_open_auto_play = TRUE;
BOOLEAN is_open_new_word = FALSE;
LOCAL uint8 open_auto_play_timer = 0;
LOCAL GUI_LCD_DEV_INFO word_chapter_tip_layer = {0};
LOCAL GUI_LCD_DEV_INFO word_detail_tip_layer = {0};
LOCAL int8 word_is_display_tip = 0;
LOCAL uint8 word_tip_timer = 0;
LOCAL MMISRV_HANDLE_T word_player_handle = PNULL;
LOCAL int word_player_volume = 0;
LOCAL uint8 chapter_unmaster_count = 0;
LOCAL BOOLEAN new_word_haved_delete = FALSE;
int16 word_detail_cur_idx = 0;
int chapter_unmaster_idx[WORD_CHAPTER_WORD_MAX] = {0};
LOCAL int16 word_listen_cur_idx = 0;
LOCAL uint8 word_listen_repeat_count = 0;
LOCAL WORD_LISTEN_INFO_T word_listen_info = {0};
LOCAL uint8 word_listen_timer_id = 0;
LOCAL uint16 word_listen_set[WORD_LISTEN_SET_SYMBOL_NUM] = {0};
LOCAL uint16 word_listen_idx[WORD_CHAPTER_WORD_MAX] = {0};
LOCAL int word_listen_set_idx = 0;
LOCAL int word_click_btn = 0;

LOCAL void MMI_CreateWordTipsWin(int type);
LOCAL MMI_RESULT_E MMI_CloseWordChapterWin(void);
LOCAL MMI_RESULT_E MMI_CloseWordListenWin(void);
LOCAL MMI_RESULT_E MMI_CloseWordListenInfoWin(void);
LOCAL BOOLEAN MMI_IsOpenWordListenWin(void);
LOCAL void WordListenWin_PlayAudioFail(void);
LOCAL void WordListenWin_CreateIntervalTimer(void);

LOCAL void Word_DrawWinTitle(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id,MMI_STRING_T text_string)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    GUI_RECT_T win_rect = word_win_rect;
    GUI_RECT_T title_rect = word_title_rect;

    GUI_FillRect(&lcd_dev_info, win_rect, WORD_WIN_BG_COLOR);
    GUI_FillRect(&lcd_dev_info, title_rect, WORD_TITLE_BG_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;

    if(ctrl_id != 0)
    {
        GUIBUTTON_SetText(ctrl_id, text_string.wstr_ptr, text_string.wstr_len); 
    }
    else
    {
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &title_rect,
            &title_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
    }
}

LOCAL void Word_DrawWinTips(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, MMI_TEXT_ID_T text_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T win_rect = word_win_rect;
    GUI_RECT_T title_rect = word_title_rect;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;

    MMIRES_GetText(text_id, win_id, &text_string);
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

LOCAL void Word_InitListbox(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, GUI_RECT_T list_rect, uint16 max_item)
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

        GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, TRUE);
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, TRUE);
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_EFFECT_STR,TRUE);
        GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);
        GUILIST_SetBgColor(ctrl_id, WORD_WIN_BG_COLOR);
    }else{
        GUILIST_RemoveAllItems(ctrl_id);
    }
}

LOCAL void Word_InitButtonBg(MMI_CTRL_ID_T ctrl_id)
{
	GUI_FONT_ALL_T font = {0};
	GUI_BG_T bg = {0};
	GUI_BORDER_T btn_border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
	font.font = DP_FONT_20;
	font.color = WORD_WIN_BG_COLOR;
	GUIBUTTON_SetBorder(ctrl_id, &btn_border, FALSE);
	bg.bg_type = GUI_BG_COLOR;
	bg.color = MMI_WHITE_COLOR;
	GUIBUTTON_SetBg(ctrl_id, &bg);
	GUIBUTTON_SetFont(ctrl_id, &font);
}

LOCAL void Word_InitButton(MMI_CTRL_ID_T ctrl_id, GUI_RECT_T rect, MMI_TEXT_ID_T text_id, GUI_ALIGN_E text_align, BOOLEAN visable, GUIBUTTON_CALLBACK_FUNC func)
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

LOCAL void Word_InitTextBox(MMI_CTRL_ID_T ctrl_id, GUI_RECT_T rect)
{
    GUI_BG_T bg = {0};
    GUI_FONT_ALL_T font = {0};
    
    GUITEXT_SetRect(ctrl_id, &rect);
    GUITEXT_SetAlign(ctrl_id, ALIGN_HVMIDDLE);
    GUITEXT_IsDisplayPrg(FALSE, ctrl_id);
    GUITEXT_SetHandleTpMsg(FALSE, ctrl_id);
    GUITEXT_SetClipboardEnabled(ctrl_id,FALSE);
    GUITEXT_IsSlide(ctrl_id,FALSE);
    
    font.font = DP_FONT_18;
    font.color = MMI_WHITE_COLOR;
    GUITEXT_SetFont(ctrl_id, &font.font,&font.color);
    
    bg.bg_type = GUI_BG_COLOR;
    bg.color = WORD_WIN_BG_COLOR;
    GUITEXT_SetBg(ctrl_id, &bg);
}

LOCAL void Word_SetDiretionText(MMI_CTRL_ID_T ctrl_id, int cur_idx, int total)
{
    char str[20] = {0};
    MMI_STRING_T text = {0};
    uint16 wstr[10] = {0};
    sprintf(str, "%d/%d", cur_idx + 1, total);

    MMI_STRNTOWSTR(wstr, strlen(str)+1, (uint8 *)str, strlen(str)+1, strlen(str)+1);
    text.wstr_ptr = wstr;
    text.wstr_len = MMIAPICOM_Wstrlen(wstr);
    GUILABEL_SetText(ctrl_id, &text, TRUE);
}

LOCAL void WordPopupWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUISTR_STYLE_T text_style = {0};
    GUI_BORDER_T border  = {0};
    MMI_STRING_T str_left = {0};
    MMI_STRING_T str_right = {0};
    MMI_STRING_T tipstr = {0};
    GUI_RECT_T rect = {0};
    GUI_RECT_T msg_tip_rect = word_msg_tips_rect;
    GUI_RECT_T msg_rect = word_msg_rect;
    GUI_RECT_T tips_left_rect = word_msg_tips_left_rect;
    GUI_RECT_T tips_right_rect = word_msg_tips_right_rect;

    int type = MMK_GetWinAddDataPtr(win_id);
    
    if(type == 0){
        rect = msg_tip_rect;
    }else{
        rect = msg_rect;
    }

    border.width = 1;
    border.color = MMI_WHITE_COLOR;
    border.type =  GUI_BORDER_ROUNDED;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_16;
    text_style.font_color = WORD_WIN_BG_COLOR;

    LCD_FillRoundedRect(&lcd_dev_info,rect,rect,MMI_WHITE_COLOR);
    
    GUI_DisplayBorder(rect,rect,&border,&lcd_dev_info);
    
    if(type == 0){
        rect.bottom = tips_left_rect.top;
        MMI_GetLabelTextByLang(HANZI_CONTINUE_TIPS, &tipstr);
    }else if(type == 1){
        MMI_GetLabelTextByLang(HANZI_OPEN_AUTOPLAY, &tipstr);
    }else if(type == 2){
        MMI_GetLabelTextByLang(HANZI_CLOSE_AUTOPLAY, &tipstr);
    }else if(type == 3){
        MMI_GetLabelTextByLang(HANZI_LOADING_WAIT, &tipstr);
    }else if(type == 4){
        MMI_GetLabelTextByLang(HANZI_NONE_AUDIO, &tipstr);
    }else if(type == 5){
        MMI_GetLabelTextByLang(HANZI_LOAD_FAIL_WAIT, &tipstr);
    }else if(type == 6){
        MMI_GetLabelTextByLang(HANZI_DELETING_WAIT, &tipstr);
    }
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &rect,
        &rect,
        &tipstr,
        &text_style,
        GUISTR_STATE_ALIGN,
        GUISTR_TEXT_DIR_AUTO
    );

    if(type == 0){
        border.color = WORD_WIN_BG_COLOR;
        GUI_DisplayBorder(tips_left_rect,tips_left_rect,&border,&lcd_dev_info);
        MMI_GetLabelTextByLang(WORD_TRUE,&str_left);
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &tips_left_rect,
            &tips_left_rect,
            &str_left,
            &text_style,
            GUISTR_STATE_ALIGN,
            GUISTR_TEXT_DIR_AUTO
        );

        GUI_DisplayBorder(tips_right_rect,tips_right_rect,&border,&lcd_dev_info);
        MMI_GetLabelTextByLang(WORD_FALSE,&str_right);
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &tips_right_rect,
            &tips_right_rect,
            &str_right,
            &text_style,
            GUISTR_STATE_ALIGN,
            GUISTR_TEXT_DIR_AUTO
        );
    }
}

LOCAL void WordPopupWin_TP_PRESS_UP(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
    int type = MMK_GetWinAddDataPtr(win_id);
    if(type == 0){
        GUI_RECT_T tip_left_rect = word_msg_tips_left_rect;
        GUI_RECT_T tips_right_rect = word_msg_tips_right_rect;
        if(GUI_PointIsInRect(point, tips_right_rect))
        {
            MMK_CloseWin(win_id);
        }
        else if(GUI_PointIsInRect(point, tip_left_rect))
        {
            MMK_CloseWin(win_id);
            if(word_learn_info != NULL)
            {
                uint8 i = 0;
                uint8 j = 0;
                BOOLEAN is_get = FALSE;
                for(i = 0;i < word_publish_count && i < WORD_PUBLISH_MAX;i++)
                {
                    if(word_publish_info[i]->publish_id == word_learn_info->publish_id){
                        word_book_info.cur_publish_idx = i;
                        for(j = 0;j < word_publish_info[i]->item_count && j < WORD_PUBLISH_BOOK_MAX;j++)
                        {
                            if(word_publish_info[i]->item_info[j]->book_id == word_learn_info->book_id){
                                word_book_info.cur_book_idx = j;
                                is_get = TRUE;
                                break;
                            }
                        }
                    }
                    if(is_get){
                        MMI_CreateWordChapterWin();
                        break;
                    }
                }
            }
        }
    }
}

LOCAL void WordPopupWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    int type = MMK_GetWinAddDataPtr(win_id);
    if(type == 0){
        MMK_CloseWin(win_id);
        if(word_learn_info != NULL)
        {
            uint8 i = 0;
            uint8 j = 0;
            BOOLEAN is_get = FALSE;
            for(i = 0;i < word_publish_count && i < WORD_PUBLISH_MAX;i++)
            {
                if(word_publish_info[i]->publish_id == word_learn_info->publish_id){
                    word_book_info.cur_publish_idx = i;
                    for(j = 0;j < word_publish_info[i]->item_count && j < WORD_PUBLISH_BOOK_MAX;j++)
                    {
                        if(word_publish_info[i]->item_info[j]->book_id == word_learn_info->book_id){
                            word_book_info.cur_book_idx = j;
                            is_get = TRUE;
                            break;
                        }
                    }
                }
                if(is_get){
                    MMI_CreateWordChapterWin();
                    break;
                }
            }
        }
    }
}

LOCAL MMI_RESULT_E HandleWordPopupWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:  
            {
                
            }
            break;
        case MSG_FULL_PAINT:
            {    
                WordPopupWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                WordPopupWin_CTL_PENOK(win_id);
            }
            break;
        case MSG_KEYDOWN_CANCEL:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            MMK_CloseWin(win_id);
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T point = {0};
                GUI_RECT_T rect = word_msg_tips_left_rect;
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(point.y > rect.top){
                    WordPopupWin_TP_PRESS_UP(win_id, point);
                }
            }
            break;
        default:
            result = MMI_RESULT_FALSE;
            break;
    }
    return result;
}

WINDOW_TABLE(MMI_WORD_TIPS_TAB) = {
    WIN_FUNC((uint32)HandleWordPopupWinMsg),
    WIN_ID(MMI_WORD_MAIN_TIPS_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL void MMI_CreateWordTipsWin(int type)
{
    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = {0};
    GUI_RECT_T msg_tip_rect = word_msg_tips_rect;
    GUI_RECT_T msg_rect = word_msg_rect;
    if(type == 0){
        rect = msg_tip_rect;
    }else{
        rect = msg_rect;
    }
    if(MMK_IsOpenWin(MMI_WORD_MAIN_TIPS_WIN_ID)){
        MMK_CloseWin(MMI_WORD_MAIN_TIPS_WIN_ID);
    }
    win_handle = MMK_CreateWin((uint32 *)MMI_WORD_TIPS_TAB, (ADD_DATA)type);
    MMK_SetWinRect(win_handle,&rect);
}

LOCAL void Word_DisplayBookList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    uint8 j = 0;
    uint16 k = 0;
    uint8 index = 0;
    uint8 book_count = 0;
    uint16 cur_publish_idx = 0;
    wchar text_str[100] = {0};
    MMI_STRING_T text_string = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    GUI_RECT_T list_rect = word_list_rect;

    Word_InitListbox(win_id, ctrl_id, list_rect, WORD_PUBLISH_BOOK_MAX);

    i = word_book_info.cur_publish_idx;
    for(j = 0;j < word_publish_info[i]->item_count && j < WORD_PUBLISH_BOOK_MAX; j++)
    {
        item_t.item_style = GUIITEM_STYLE_POETRY_ITEM_LIST_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = IMG_POETRY_ITEM_BG;

        GUI_UTF8ToWstr(text_str, 100, word_publish_info[i]->item_info[j]->book_name, strlen(word_publish_info[i]->item_info[j]->book_name));
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_str);
        item_data.item_content[1].is_default =TRUE;
        item_data.item_content[1].font_color_id = MMITHEME_COLOR_LIGHT_BLUE;
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[1].item_data.text_buffer = text_string;

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
    GUILIST_SetTextFont(ctrl_id, DP_FONT_18, WORD_WIN_BG_COLOR);
    GUILIST_SetCurItemIndex(ctrl_id, word_book_info.cur_book_idx);
}

LOCAL void WordBookMainWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    MMI_STRING_T text_string = {0};
    wchar name_str[50] = {0};
    uint16 length = 0;
    
    length = strlen(word_publish_info[word_book_info.cur_publish_idx]->publish_name);
    GUI_UTF8ToWstr(name_str, 50, word_publish_info[word_book_info.cur_publish_idx]->publish_name, length);
    text_string.wstr_len = MMIAPICOM_Wstrlen(name_str);
    text_string.wstr_ptr = name_str;

    Word_DrawWinTitle(win_id, 0, text_string);
    
    Word_DisplayBookList(win_id, MMI_ZMT_WORD_MAIN_LIST_BOOK_CTRL_ID);
}

LOCAL void WordBookMainWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_WORD_MAIN_LIST_BOOK_CTRL_ID);
    word_book_info.cur_book_idx = cur_idx;
    MMI_CreateWordChapterWin();
}

LOCAL MMI_RESULT_E HandleWordBookMainWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                
            }
            break;
        case MSG_FULL_PAINT:
            {
                WordBookMainWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_KEYDOWN_CANCEL:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CTL_MIDSK:
        case MSG_APP_WEB:
        case MSG_APP_OK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                WordBookMainWin_CTL_PENOK(win_id);
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

WINDOW_TABLE(MMI_WORD_BOOK_WIN_TAB) = {
    WIN_ID(MMI_WORD_BOOK_MAIN_WIN_ID),
    WIN_FUNC((uint32)HandleWordBookMainWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC MMI_RESULT_E MMI_CloseWordBookWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(MMI_WORD_BOOK_MAIN_WIN_ID)){
        MMK_CloseWin(MMI_WORD_BOOK_MAIN_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateWordBookWin(void)
{
    MMI_CloseWordBookWin();
    MMK_CreateWin((uint32 *)MMI_WORD_BOOK_WIN_TAB, PNULL);
}

LOCAL void Word_DisplayPublishList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    uint8 j = 0;
    uint8 k = 0;
    uint8 grade_num = 0;
    uint8 cur_grade = 0;
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_str = {0};
    MMI_STRING_T text_str2 = {0};
    wchar name_wchar[50] = {0};
    wchar name_str[50] = {0};
    uint8 length = 0;
    MMI_STRING_T text_string = {0};
    char tmp[10]={0};
    uint16 wstr0[10] = {0};
    GUI_RECT_T list_rect = word_list_rect;

    Word_InitListbox(win_id, ctrl_id, list_rect, WORD_PUBLISH_MAX);

    for(i = 0;i < word_publish_count && i < WORD_PUBLISH_MAX; i++)
    {
        item_t.item_style = GUIITEM_SYTLE_ZMT_PUBLISH_LIST_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = IMG_ZMT_CONTACT_ICON;

        itoa(i+1,tmp, 10);
        GUI_UTF8ToWstr(wstr0,10,tmp, strlen(tmp)+1);
        text_str.wstr_len = MMIAPICOM_Wstrlen(wstr0);
        text_str.wstr_ptr = wstr0;
        item_data.item_content[1].is_default =TRUE;
        item_data.item_content[1].font_color_id = MMITHEME_COLOR_LIGHT_BLUE;
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[1].item_data.text_buffer = text_str;

        memset(name_str, 0, 50);
        length = strlen(word_publish_info[i]->publish_name);
        GUI_UTF8ToWstr(name_str, 50, word_publish_info[i]->publish_name, length);
        text_str2.wstr_len = MMIAPICOM_Wstrlen(name_str);
        text_str2.wstr_ptr = name_str;
        item_data.item_content[2].is_default =TRUE;
        item_data.item_content[2].font_color_id = MMITHEME_COLOR_LIGHT_WHITE;
        item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[2].item_data.text_buffer = text_str2;

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    GUILIST_SetCurItemIndex(ctrl_id, word_book_info.cur_publish_idx);
}

LOCAL void WordMainWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    MMI_STRING_T text_string = {0};

    MMIRES_GetText(WORD_TITLE, win_id, &text_string);
    Word_DrawWinTitle(win_id, 0, text_string);

    if (word_publish_count == 0)
    {
        Word_DrawWinTips(win_id, 0, WORD_LOADING);
    }
    else if(word_publish_count == -1)
    {
        Word_DrawWinTips(win_id, 0, WORD_LOADING_FAILED);
    }
    else if(word_publish_count == -2)
    {
        Word_DrawWinTips(win_id, 0, WORD_NO_DATA);
    }
    else
    {
        Word_DisplayPublishList(win_id, MMI_ZMT_WORD_MAIN_LIST_CTRL_ID);
    }
}

LOCAL void WordMainWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    if(word_publish_count > 0){
        uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_WORD_MAIN_LIST_CTRL_ID);
        word_book_info.cur_publish_idx = cur_idx;
        MMI_CreateWordBookWin();
    }
}
LOCAL void WordMainWin_CLOSE_WINDOW(void)
{
    memset(&word_listen_info, 0, sizeof(WORD_LISTEN_INFO_T));
    memset(&word_book_info, 0, sizeof(WORD_BOOK_INFO_T));
    Word_ReleaseBookInfo();
    Word_ReleaseLearnInfo();
    word_publish_count = 0;
}
LOCAL MMI_RESULT_E HandleWordMainWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                memset(&word_listen_info, 0, sizeof(WORD_LISTEN_INFO_T));
                memset(&word_book_info, 0, sizeof(WORD_BOOK_INFO_T));
                Word_requestBookInfo();
            }
            break;
        case MSG_FULL_PAINT:
            {
                WordMainWin_FULL_PAINT(win_id);
                if(word_publish_count > 0 && Word_LoadLearnInfo()){
                    MMI_CreateWordTipsWin(0);
                }
            }
            break;
        case MSG_KEYDOWN_CANCEL:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CTL_MIDSK:
        case MSG_APP_WEB:
        case MSG_APP_OK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                WordMainWin_CTL_PENOK(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                WordMainWin_CLOSE_WINDOW();
            }
            break;
         default:
            recode = MMI_RESULT_FALSE;
            break;
    }
      return recode;
}

WINDOW_TABLE(MMI_WORD_WIN_TAB) = {
    WIN_ID(MMI_WORD_MAIN_WIN_ID),
    WIN_FUNC((uint32)HandleWordMainWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMI_CreateWordWin(void)
{
    if(MMK_IsOpenWin(MMI_WORD_MAIN_WIN_ID)){
        MMK_CloseWin(MMI_WORD_MAIN_WIN_ID);
    }
    MMK_CreateWin((uint32 *)MMI_WORD_WIN_TAB, PNULL);
}

PUBLIC MMI_RESULT_E MMI_CloseWordWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CloseWin(MMI_WORD_MAIN_WIN_ID);
    return result;
}

/////////////////////////////////////////////////////////////////////////

LOCAL void WordChapter_AutoDisplay_Tip_Timeout(uint8 timer_id,uint32 param)
{
    if(0 != open_auto_play_timer)
    {
        MMK_StopTimer(open_auto_play_timer);
        open_auto_play_timer = 0;
    }
    if(MMK_IsOpenWin(MMI_WORD_MAIN_TIPS_WIN_ID)){
        MMK_CloseWin(MMI_WORD_MAIN_TIPS_WIN_ID);
    }
}

LOCAL void WordChapter_AutoDisplay_Tip(uint8 type)
{
    if(0 != open_auto_play_timer)
    {
        MMK_StopTimer(open_auto_play_timer);
        open_auto_play_timer = 0;
    }
    MMI_CreateWordTipsWin(type);
    open_auto_play_timer = MMK_CreateTimerCallback(2000, WordChapter_AutoDisplay_Tip_Timeout, PNULL, FALSE);
    MMK_StartTimerCallback(open_auto_play_timer, 2000, WordChapter_AutoDisplay_Tip_Timeout, PNULL, FALSE);
}

LOCAL MMI_RESULT_E WordChapter_clickAutoPlay()
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    word_open_auto_play = TRUE;
    MMK_SendMsg(MMI_WORD_CHAPTER_WIN_ID, MSG_FULL_PAINT, PNULL);
    WordChapter_AutoDisplay_Tip(1);
    return result;
}
LOCAL MMI_RESULT_E WordChapter_clickDisAutoPlay()
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    word_open_auto_play = FALSE;
    MMK_SendMsg(MMI_WORD_CHAPTER_WIN_ID, MSG_FULL_PAINT, PNULL);
    WordChapter_AutoDisplay_Tip(2);
    return result;
}

LOCAL void WordChapter_OpenNormalWord(void)
{
    if(word_chapter_count > 0){
        is_open_new_word = FALSE;
        MMI_CreateWordDetailWin();
    }
}

LOCAL void WordChapter_OpenNewWord(void)
{
    if(word_chapter_count > 0){
        is_open_new_word = TRUE;
        MMI_CreateWordDetailWin();
    }
}

LOCAL void WordChapter_DisplayChapterList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_str = {0};
    wchar name_wchar[100] = {0};
    uint16 length = 0;
    MMI_STRING_T text_string = {0};
    GUI_RECT_T list_rect = word_list_rect;

    list_rect.bottom -= 1.5*WORD_CARD_LINE_HIGHT;
    list_init.both_rect.v_rect = list_rect;
    Word_InitListbox(win_id, ctrl_id, list_rect, WORD_CHAPTER_NUM_MAX);
    
    for(i = 0;i < word_chapter_count && i < WORD_CHAPTER_NUM_MAX;i++)
    {
        item_t.item_style = GUIITEM_SYTLE_ZMT_UNIT_LIST_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELECTED;
    		
        memset(name_wchar, 0, 100);
        length = strlen(word_chapter_info[i]->chapter_name);
        GUI_UTF8ToWstr(name_wchar, 100, word_chapter_info[i]->chapter_name, length);
        text_str.wstr_ptr = name_wchar;
        text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
        item_data.item_content[0].is_default =TRUE;
        item_data.item_content[0].font_color_id = MMITHEME_COLOR_LIGHT_WHITE;
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[0].item_data.text_buffer = text_str;
        
        item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
        if(word_book_info.cur_chapter_idx == i){
            item_data.item_content[1].item_data.image_id = IMG_ZMT_SELECTED;
        }else{
            item_data.item_content[1].item_data.image_id = IMG_ZMT_UNSELECTED;
        }
        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    GUILIST_SetCurItemIndex(ctrl_id, word_book_info.cur_chapter_idx);
}

LOCAL void WordChapterWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_FONT_ALL_T font = {0};
    GUI_RECT_T title_rect = word_title_rect;
    GUI_RECT_T auto_play_rect = word_auto_play_rect;
    GUI_RECT_T left_rect = word_left_rect;
    GUI_RECT_T right_rect = word_right_rect;
    GUI_RECT_T dir_rect = word_dir_rect;
    GUI_RECT_T tip_rect = word_tip_rect;
		
    font.font = DP_FONT_16;
    font.color = MMI_WHITE_COLOR;
    GUIBUTTON_SetFont(MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID, &font);
    Word_InitButton(MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID, title_rect, NULL, ALIGN_LVMIDDLE, TRUE, MMI_CloseWordChapterWin);

    GUIBUTTON_SetRect(MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID, &auto_play_rect);
    GUIBUTTON_SetVisible(MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID,TRUE,TRUE);

    Word_InitButton(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID, left_rect, WORD_PACTISE, ALIGN_HVMIDDLE, FALSE, WordChapter_OpenNormalWord);
    Word_InitButtonBg(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID);

    Word_InitButton(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID, right_rect, WORD_EXERCISE, ALIGN_HVMIDDLE, FALSE, WordChapter_OpenNewWord);
    Word_InitButtonBg(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID);

    Word_requestChapterDetailInfo(word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id);
}

LOCAL void WordChapterWin_DrawTitle(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    char text_str[100] = {0};
    wchar text_wchar[100] = {0};

    sprintf(text_str, "%s", word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_name);
    GUI_UTF8ToWstr(text_wchar, 100, text_str, strlen(text_str));
    text_string.wstr_ptr = text_wchar;
    text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
    Word_DrawWinTitle(win_id, MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID, text_string);
}

LOCAL void WordChapterWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    WordChapterWin_DrawTitle(win_id);
    
    //SCI_TRACE_LOW("%s: word_chapter_count = %d", __FUNCTION__, word_chapter_count);
    if(word_chapter_count == 0)
    {
        Word_DrawWinTips(win_id, 0, WORD_LOADING);
    }
    else if(word_chapter_count == -1)
    {
        Word_DrawWinTips(win_id, 0, WORD_LOADING_FAILED);
    }
    else if(word_chapter_count == -2)
    {
        Word_DrawWinTips(win_id, 0, WORD_NO_DATA);
    }
    else
    {
        GUI_BG_T auto_play_bg = {0};
        if(word_open_auto_play)
        {
            auto_play_bg.bg_type = GUI_BG_IMG;
            auto_play_bg.img_id = IMG_AUTO_PLAY;
            GUIBUTTON_SetBg(MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID, &auto_play_bg);
            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID, WordChapter_clickDisAutoPlay);
        }
        else
        {
            auto_play_bg.bg_type = GUI_BG_IMG;
            auto_play_bg.img_id = IMG_DISAUTO_PLAY;
            GUIBUTTON_SetBg(MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID, &auto_play_bg);
            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID, WordChapter_clickAutoPlay);
        }
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID,TRUE,FALSE);
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID,TRUE,FALSE);
        WordChapter_DisplayChapterList(win_id, MMI_ZMT_WORD_CHAPTER_LIST_CTRL_ID);
    }
}

LOCAL void WordChapterWin_CTL_PENOK(MMI_WIN_ID_T win_id, DPARAM param)
{
    if(word_chapter_count >0){
        uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_WORD_CHAPTER_LIST_CTRL_ID);
        if(cur_idx == word_book_info.cur_chapter_idx){
            is_open_new_word = FALSE;
            MMI_CreateWordDetailWin();
        }else{
            word_book_info.cur_chapter_idx = cur_idx;
            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
    }
}

LOCAL void WordChapterWin_CLOSE_WINDOW(MMI_WIN_ID_T win_id)
{
    word_chapter_count = 0;
    Word_ReleaseChapterDetailInfo();
    word_book_info.cur_chapter_idx = 0;
    Word_WriteLearnInfo();
    word_click_btn = 0;
}

LOCAL MMI_RESULT_E HandleWordChapterWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                WordChapterWin_OPEN_WINDOW(win_id);
                Word_UpdateLearnInfo(
                    word_publish_info[word_book_info.cur_publish_idx]->publish_id,
                    word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id,
                    0
                );
            }
            break;
        case MSG_FULL_PAINT:
            {
                WordChapterWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_APP_0:
        case MSG_APP_HASH:
        case MSG_KEYDOWN_UPSIDE:
        case MSG_KEYDOWN_VOL_UP:
        case MSG_KEYDOWN_DOWNSIDE:
        case MSG_KEYDOWN_VOL_DOWN:
            {
                if(word_open_auto_play){
                    WordChapter_clickDisAutoPlay();
                }else{
                    WordChapter_clickAutoPlay();
                }
            }
            break; 
        case MSG_APP_LEFT:
            {
                WordChapter_OpenNormalWord();
            }
            break;
        case MSG_APP_RIGHT:
            {
                WordChapter_OpenNewWord();
            }
            break;
        case MSG_KEYDOWN_CANCEL:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CTL_PENOK:
        case MSG_CTL_OK:
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            {
                WordChapterWin_CTL_PENOK(win_id, param);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                WordChapterWin_CLOSE_WINDOW(win_id);
            }
            break;
         default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_WORD_CHAPTER_WIN_TAB) = {
    WIN_ID(MMI_WORD_CHAPTER_WIN_ID),
    WIN_FUNC((uint32)HandleWordChapterWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseWordChapterWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(MMI_WORD_CHAPTER_WIN_ID)){
        MMK_CloseWin(MMI_WORD_CHAPTER_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateWordChapterWin(void)
{
    MMI_CloseWordChapterWin();
    MMK_CreateWin((uint32 *)MMI_WORD_CHAPTER_WIN_TAB, PNULL);
}

/////////////////////////////////////////////////////////////////////////////
LOCAL void Word_StopPlayMp3Data(void)
{
    if(word_player_handle != 0)
    {
        MMISRVAUD_Stop(word_player_handle);
        MMISRVMGR_Free(word_player_handle);
        word_player_handle = 0;
    }
}

LOCAL BOOLEAN Word_ListenPlayMp3DataNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
    MMISRVAUD_REPORT_T *report_ptr = PNULL;

    if(param != PNULL && handle > 0)
    {
        report_ptr = (MMISRVAUD_REPORT_T *)param->data;
        if(report_ptr != PNULL && handle == word_player_handle)
        {
            switch(report_ptr->report)
                {
                    case MMISRVAUD_REPORT_END:  
                        {
                            if(MMI_IsOpenWordListenWin()){
                                WordListenWin_CreateIntervalTimer();
                            }
                        }
                        break;
                    default:
                        break;
                }
        }
    }
    return TRUE;
}

LOCAL BOOLEAN Word_ChatPlayMp3DataNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
    MMISRVAUD_REPORT_T *report_ptr = PNULL;

    if(param != PNULL && handle > 0)
    {
        report_ptr = (MMISRVAUD_REPORT_T *)param->data;
        if(report_ptr != PNULL && handle == word_player_handle)
        {
            switch(report_ptr->report)
                {
                    case MMISRVAUD_REPORT_END:  
                        {
                            
                        }
                        break;
                    default:
                        break;
                }
        }
    }
    return TRUE;
}

LOCAL void Word_ChatPlayMp3Data(uint8 *data,uint32 data_len)
{
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    BOOLEAN result = FALSE;

    Word_StopPlayMp3Data();
    
    if(MMK_IsOpenWin(MMI_WORD_LISTEN_WIN_ID)){
        req.is_auto_free = FALSE;
        req.notify = Word_ListenPlayMp3DataNotify;
    }else{
        req.is_auto_free = TRUE;
        req.notify = Word_ChatPlayMp3DataNotify;
    }
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.info.type = MMISRVAUD_TYPE_RING_BUF;
    audio_srv.info.ring_buf.fmt = MMISRVAUD_RING_FMT_MP3;
    audio_srv.info.ring_buf.data = data;
    audio_srv.info.ring_buf.data_len = data_len;
    audio_srv.volume = word_player_volume;
    
    SCI_TRACE_LOW("%s: audio_srv.volume=%d", __FUNCTION__, audio_srv.volume);
    audio_srv.all_support_route = MMISRVAUD_ROUTE_SPEAKER | MMISRVAUD_ROUTE_EARPHONE;
    word_player_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
	
    if(word_player_handle > 0)
    {
        SCI_TRACE_LOW("%s word_player_handle > 0", __FUNCTION__);
        result = MMISRVAUD_Play(word_player_handle, 0);
        if(!result)
        {
            SCI_TRACE_LOW("%s chat_player error", __FUNCTION__);
            MMISRVMGR_Free(word_player_handle);
            word_player_handle = 0;
        }
        if(result == MMISRVAUD_RET_OK)
        {
            SCI_TRACE_LOW("%s chat_player success", __FUNCTION__);
        }
    }
    else
    {
        SCI_TRACE_LOW("%s word_player_handle <= 0", __FUNCTION__);
    }
}

PUBLIC void WordDetail_TipTimeout(uint8 timer_id,uint32 param)
{
    if(0 != word_tip_timer)
    {
        MMK_StopTimer(word_tip_timer);
        word_tip_timer = 0;
    }
    word_is_display_tip = 0;
    if(MMK_IsOpenWin(MMI_WORD_MAIN_TIPS_WIN_ID)){
        MMK_CloseWin(MMI_WORD_MAIN_TIPS_WIN_ID);
    }
}

LOCAL void WordDetail_DisplayTip(uint type)
{
    if(0 != word_tip_timer)
    {
        MMK_StopTimer(word_tip_timer);
        word_tip_timer = 0;
    }
    word_is_display_tip = type;
    MMI_CreateWordTipsWin(type);
    word_tip_timer = MMK_CreateTimerCallback(2000, WordDetail_TipTimeout,(uint32)0, FALSE);
    MMK_StartTimerCallback(word_tip_timer, 2000, WordDetail_TipTimeout, (uint32)0, FALSE);
}

PUBLIC void WordDetail_PlayPinyinAudio(void)
{
    //生词本
    if(is_open_new_word)
    {
        if(new_word_detail_info[word_detail_cur_idx] == NULL){
            SCI_TRACE_LOW("%s: 01empty detail info!!", __FUNCTION__);
            return;
        }
        //SCI_TRACE_LOW("%s: 01audio_len = %d", __FUNCTION__, new_word_detail_info[word_detail_cur_idx]->audio_len);
        if(new_word_detail_info[word_detail_cur_idx]->audio_len == 0)
        {
            char file_path[40] = {0};
            sprintf(file_path, WORD_BOOK_AUDIO_PATH, 
                word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id, 
                new_word_detail_info[word_detail_cur_idx]->word
            );
            if(zmt_file_exist(file_path)){
                new_word_detail_info[word_detail_cur_idx]->audio_data = zmt_file_data_read(file_path, &new_word_detail_info[word_detail_cur_idx]->audio_len);
                WordDetail_PlayPinyinAudio();
            }else{
                if(new_word_detail_info[word_detail_cur_idx]->audio_uri != NULL){
                    MMIZDT_HTTP_AppSend(TRUE, new_word_detail_info[word_detail_cur_idx]->audio_uri, PNULL, 0, 1000, 0, 0, 6000, 0, 0, Word_ParseMp3Response);
                }
            }
        }
        else if(new_word_detail_info[word_detail_cur_idx]->audio_len == -1)
        {
            //没有音频uri
            WordListenWin_PlayAudioFail();
            WordDetail_DisplayTip(4);
        }
        else if(new_word_detail_info[word_detail_cur_idx]->audio_len == -2)
        {
            //加载音频失败
            WordListenWin_PlayAudioFail();
            WordDetail_DisplayTip(5);
        }
        else
        {
            //加载成功，开始播放
            if(new_word_detail_info[word_detail_cur_idx]->audio_data != NULL){
                Word_ChatPlayMp3Data(new_word_detail_info[word_detail_cur_idx]->audio_data ,new_word_detail_info[word_detail_cur_idx]->audio_len);
            }
        }
        return;
    }

    //正常单词练习
    if(word_chapter_info[word_book_info.cur_chapter_idx] == NULL ||
        word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx] == NULL)
    {
        SCI_TRACE_LOW("%s: 02empty detail info!!", __FUNCTION__);
        return;
    }
    //SCI_TRACE_LOW("%s: 02audio_len = %d", __FUNCTION__, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_len);
    if(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_len == 0)
    {
        char file_path[40] = {0};
        sprintf(file_path, WORD_BOOK_AUDIO_PATH, 
            word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id, 
            word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->word
        );
        if(zmt_file_exist(file_path)){
            word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_data = zmt_file_data_read(file_path, &word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_len);
            WordDetail_PlayPinyinAudio();
        }else{
            if(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_uri != NULL){
                //SCI_TRACE_LOW("%s: [%d]audio_uri = %s", __FUNCTION__, word_detail_cur_idx, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_uri);    
                MMIZDT_HTTP_AppSend(TRUE, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_uri, PNULL, 0, 1000, 0, 0, 6000, 0, 0, Word_ParseMp3Response);
            }
        }
    }
    else if(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_len == -1)
    {
        //没有音频uri
        WordListenWin_PlayAudioFail();
        WordDetail_DisplayTip(4);
    }
    else if(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_len == -2)
    {
        //加载音频失败
        
        WordListenWin_PlayAudioFail();
        WordDetail_DisplayTip(5);
    }
    else
    {
        //加载成功，开始播放
        if(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_data != NULL){
            Word_ChatPlayMp3Data(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_data ,word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_len);
        }
    }
}

LOCAL void WordDetail_LeftDetail(void)//已掌握/上一个
{
    if(!is_open_new_word){
        word_detail_cur_idx++;
        if(word_open_auto_play){
            WordDetail_PlayPinyinAudio();
        }
    }else{
        if(word_detail_cur_idx == 0){
            return;
        }else{
            word_detail_cur_idx--;
            if(word_open_auto_play){
                WordDetail_PlayPinyinAudio();
            }
        }
    }
    MMK_PostMsg(MMI_WORD_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
}

LOCAL void WordDetail_RightDetail(void)//未掌握/下一个
{
    if(!is_open_new_word){
        chapter_unmaster_idx[chapter_unmaster_count] = word_detail_cur_idx + 1;
        chapter_unmaster_count++;
        word_detail_cur_idx++;
        if(word_open_auto_play){
            WordDetail_PlayPinyinAudio();
        }
        MMK_PostMsg(MMI_WORD_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }else{
        word_detail_cur_idx++;
        if(word_open_auto_play){
            WordDetail_PlayPinyinAudio();
        }
        MMK_PostMsg(MMI_WORD_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }
}

LOCAL void WordDetail_NextChapterInfo(void)
{
    if(word_book_info.cur_chapter_idx + 1 < word_chapter_count)
    {
        Word_WriteUnmasterChapterWord(
            word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name,
            chapter_unmaster_count
        );
        word_book_info.cur_chapter_idx++;
        memset(&chapter_unmaster_idx, 0, sizeof(chapter_unmaster_idx));
        chapter_unmaster_count = 0;
        word_detail_cur_idx = 0;
        MMK_SendMsg(MMI_WORD_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL);
        if(word_open_auto_play){
            WordDetail_PlayPinyinAudio();
        }
    }
}

LOCAL void WordDetail_GoToListenWord(void)
{
    if(!is_open_new_word){
        Word_WriteUnmasterChapterWord(
            word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name,
            chapter_unmaster_count
        );
        chapter_unmaster_count = 0;
    }
    MMI_CreateWordListenWin();
}

LOCAL void WordDetail_DeleteNewWord(void)
{
    new_word_haved_delete = TRUE;
    Word_DeleteOneNewWord(word_detail_cur_idx, word_detail_count);
    WordDetail_DisplayTip(6);
}

LOCAL void WordDetail_KeyLeft(void)
{
    if(word_detail_count <= 0){
        return;
    }
    if(is_open_new_word){
        if(word_detail_cur_idx < word_detail_count){
            WordDetail_LeftDetail();
        }else{
            WordDetail_GoToListenWord();
        }
    }else{
        if(word_detail_cur_idx < word_detail_count){
            WordDetail_LeftDetail();
        }else{
            WordDetail_GoToListenWord();
        }
    }
}

LOCAL void WordDetail_KeyRight(void)
{
    if(word_detail_count <= 0){
        return;
    }
    if(is_open_new_word){
        if(word_detail_cur_idx < word_detail_count){
            WordDetail_RightDetail();
        }else{
            MMI_CloseWordDetailWin();
        }
    }else{
        if(word_detail_cur_idx < word_detail_count){
            WordDetail_RightDetail();
        }else{
            if(word_book_info.cur_chapter_idx + 1 < word_chapter_count){
                WordDetail_NextChapterInfo();
            }else{
                MMI_CloseWordDetailWin();
            }
        }
    }
}

LOCAL void WordDetail_DisplayDtailInfo(
    MMI_WIN_ID_T win_id, 
    MMI_CTRL_ID_T ctrl_label_word, 
    MMI_CTRL_ID_T ctrl_label_pinyin, 
    MMI_CTRL_ID_T ctrl_button_audio, 
    MMI_CTRL_ID_T ctrl_text_info)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUI_FONT_T text_font = DP_FONT_16;
    GUI_COLOR_T text_color = MMI_WHITE_COLOR;
    
    MMI_STRING_T text_word = {0};
    MMI_STRING_T text_pinyin = {0};
    MMI_STRING_T text_remark = {0};
    wchar wstr_word[30] = {0};
    wchar wstr_pinyin[30] = {0};
    wchar wstr_remark[2048] = {0};
    uint16 size = 0;
    uint16 text_width_piex = 0;
    uint16 width_rect = 0;
    GUI_RECT_T word_rect = word_word_rect;
    GUI_RECT_T pinyin_rects = word_pinyin_rect;
    GUI_RECT_T pinyin_rect = {0};
    GUI_RECT_T audio_rect = {0};
    GUI_RECT_T remark_rect = word_text_rect;
    BOOLEAN is_phonetic_null = FALSE;

    if(is_open_new_word){
        size = strlen(new_word_detail_info[word_detail_cur_idx]->word);
        GUI_UTF8ToWstr(wstr_word, 30, new_word_detail_info[word_detail_cur_idx]->word, size);
    }else{
        size = strlen(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->word);
        GUI_UTF8ToWstr(wstr_word, 30, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->word, size); 
    }
    text_word.wstr_ptr = wstr_word;
    text_word.wstr_len = MMIAPICOM_Wstrlen(text_word.wstr_ptr);
    text_width_piex = GUI_CalculateStringPiexlNum(text_word.wstr_ptr, text_word.wstr_len, DP_FONT_20, 1);
    width_rect = word_rect.left + text_width_piex + 5;
    word_rect.right = width_rect;
    GUILABEL_SetRect(ctrl_label_word, &word_rect, FALSE);
    GUILABEL_SetText(ctrl_label_word, &text_word, FALSE);

    if(is_open_new_word){
        if(new_word_detail_info[word_detail_cur_idx]->phonetic != NULL){
            size = strlen(new_word_detail_info[word_detail_cur_idx]->phonetic);
            GUI_UTF8ToWstr(wstr_pinyin, 30, new_word_detail_info[word_detail_cur_idx]->phonetic, size);
        }else{
            is_phonetic_null = TRUE;
        }
    }else{
        if(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->phonetic != NULL){
            size = strlen(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->phonetic);
            GUI_UTF8ToWstr(wstr_pinyin, 30, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->phonetic, size);
        }else{
            is_phonetic_null = TRUE;
        }
    }
    text_pinyin.wstr_ptr = wstr_pinyin;
    text_pinyin.wstr_len = MMIAPICOM_Wstrlen(text_pinyin.wstr_ptr);
    if(width_rect < MMI_MAINSCREEN_WIDTH / 2.5 || 
        word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->phonetic == NULL)
    {
        text_width_piex = GUI_CalculateStringPiexlNum(text_pinyin.wstr_ptr, text_pinyin.wstr_len, DP_FONT_20, 1);
        pinyin_rect = word_rect;
        pinyin_rect.left = word_rect.right;
        pinyin_rect.right = pinyin_rect.left + 2*text_width_piex;
        GUILABEL_SetRect(ctrl_label_pinyin, &pinyin_rect, FALSE);
        audio_rect = pinyin_rect;
    }else{
        pinyin_rect.right += 20;
        GUILABEL_SetRect(ctrl_label_pinyin, &pinyin_rects, FALSE);
        audio_rect = pinyin_rects;
    }
    GUILABEL_SetText(ctrl_label_pinyin, &text_pinyin, FALSE);

    if(is_phonetic_null){
        audio_rect = word_rect;
    }
    audio_rect.left = 5*WORD_CARD_LINE_WIDTH;
    audio_rect.right = 6*WORD_CARD_LINE_WIDTH;
    GUIBUTTON_SetVisible(ctrl_button_audio,TRUE,FALSE);
    GUIBUTTON_SetRect(ctrl_button_audio, &audio_rect);
    LCD_DrawHLine(&lcd_dev_info, pinyin_rects.left, audio_rect.bottom, pinyin_rects.right, MMI_WHITE_COLOR);
    word_Hor_line_rect = pinyin_rects;
    word_Hor_line_rect.top = audio_rect.top;
    word_Hor_line_rect.bottom = audio_rect.bottom;

    if(is_open_new_word){
        size = strlen(new_word_detail_info[word_detail_cur_idx]->translation);
        GUI_UTF8ToWstr(wstr_remark, 2048, new_word_detail_info[word_detail_cur_idx]->translation, size);
    }else{
        size = strlen(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->translation);
        GUI_UTF8ToWstr(wstr_remark, 2048, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->translation, size);
    }
    remark_rect.top = audio_rect.bottom + 1;
    GUITEXT_SetRect(ctrl_text_info, &remark_rect);
    text_remark.wstr_ptr = wstr_remark;
    text_remark.wstr_len = MMIAPICOM_Wstrlen(text_remark.wstr_ptr);
    GUITEXT_SetAlign(ctrl_text_info, ALIGN_LEFT);
    GUITEXT_SetString(ctrl_text_info, text_remark.wstr_ptr,text_remark.wstr_len, TRUE);
    GUITEXT_SetResetTopDisplay(ctrl_text_info, 0);
}

LOCAL void WordDetailWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_FONT_ALL_T font = {0};
    GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
    GUI_BG_T bg = {0};
    GUI_RECT_T title_rect = word_title_rect;
    GUI_RECT_T text_rect = word_text_rect;
    GUI_RECT_T msg_rect = word_msg_rect;
    GUI_RECT_T dir_rect = word_dir_rect;
    GUI_RECT_T left_rect = word_left_rect;
    GUI_RECT_T right_rect = word_right_rect;
    GUI_RECT_T pre_rect = word_pre_rect;
    GUI_RECT_T next_rect = word_next_rect;
    GUI_RECT_T del_rect = word_del_rect;
		
    font.font = DP_FONT_18;
    font.color = MMI_WHITE_COLOR; 
    GUIBUTTON_SetFont(MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID, &font);
    Word_InitButton(MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID, title_rect, NULL, ALIGN_LVMIDDLE, TRUE, MMI_CloseWordDetailWin);

    Word_InitTextBox(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, text_rect);
    
    Word_InitButton(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, msg_rect, NULL, ALIGN_HVMIDDLE, FALSE, NULL);
    Word_InitButtonBg(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID);

    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID,FALSE,FALSE);
    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID,FALSE,FALSE);

    GUILABEL_SetRect(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, &dir_rect, FALSE);
    GUILABEL_SetFont(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, DP_FONT_16,MMI_WHITE_COLOR);
    GUILABEL_SetAlign(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, GUILABEL_ALIGN_MIDDLE);

    GUILABEL_SetFont(MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID, DP_FONT_20,MMI_WHITE_COLOR);
    GUILABEL_SetFont(MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID, DP_FONT_20,MMI_WHITE_COLOR);

    new_word_haved_delete = FALSE;
    word_detail_count = 0;
    word_player_volume = MMIAPISET_GetMultimVolume();
    memset(chapter_unmaster_idx, 0, sizeof(chapter_unmaster_idx));
    if(is_open_new_word){
        Word_RequestNewWord(
            word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_id
        );
        Word_InitButton(MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID, del_rect, NULL, ALIGN_HVMIDDLE, FALSE, WordDetail_DeleteNewWord);
        Word_InitButton(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, pre_rect, NULL, ALIGN_HVMIDDLE, FALSE, WordDetail_LeftDetail);
        Word_InitButton(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, next_rect, NULL, ALIGN_HVMIDDLE, FALSE, WordDetail_RightDetail);
    }else{
        if(word_open_auto_play && word_chapter_info[word_book_info.cur_chapter_idx]->detail_count > 0)
        {
            WordDetail_PlayPinyinAudio();
        }
        Word_InitButton(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, left_rect, NULL, ALIGN_HVMIDDLE, FALSE, WordDetail_LeftDetail);
        Word_InitButton(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, right_rect, NULL, ALIGN_HVMIDDLE, FALSE, WordDetail_RightDetail);
        Word_InitButtonBg(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID);
        Word_InitButtonBg(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID);
    }
}

LOCAL void WordDetailWin_NewWordFullPaint(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    if(word_detail_cur_idx < word_detail_count)
    {
        GUI_BG_T but_bg = {0};
        but_bg.bg_type = GUI_BG_IMG;
        if(word_detail_cur_idx > 0){
            but_bg.img_id = WORD_PRE_ICON;
            GUIBUTTON_SetBg(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, &but_bg);
            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WordDetail_LeftDetail);
        }else{
            GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
        }
        but_bg.img_id = WORD_NEXT_ICON;
        GUIBUTTON_SetBg(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, &but_bg);                          
        GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WordDetail_RightDetail);
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID,TRUE,TRUE);
        GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID, WordDetail_DeleteNewWord);
        Word_SetDiretionText(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, word_detail_cur_idx, word_detail_count);    
        WordDetail_DisplayDtailInfo(win_id, 
            MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID, 
            MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID,
            MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID,
            MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID
            );
    }
    else
    {
        GUI_RECT_T right_rect = word_right_rect;
        GUI_RECT_T left_rect = word_left_rect;
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID,FALSE,FALSE);
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, TRUE, TRUE);
        GUIBUTTON_SetTextId(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, NEW_WORD_BOOK_FINISH);
        GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, &left_rect);
        Word_InitButtonBg(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID);
        GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WORD_TO_LISTENING);
        GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WordDetail_GoToListenWord);
        GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, &right_rect);
        GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID,ALIGN_HVMIDDLE);
        Word_InitButtonBg(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID);
        GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WORD_BACK_CH);
        GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, MMI_CloseWordDetailWin);
        LCD_DrawHLine(&lcd_dev_info, word_Hor_line_rect.left, word_Hor_line_rect.bottom, word_Hor_line_rect.right, MMI_WHITE_COLOR);
    }
}

LOCAL void WordDetailWin_NormalWordFullPaint(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    if(word_detail_cur_idx < word_detail_count)
    {
        GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, HANZI_MASTERED);
        GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, HANZI_UNMASTERED);
        GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WordDetail_LeftDetail);
        GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WordDetail_RightDetail);
        Word_SetDiretionText(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, word_detail_cur_idx, word_detail_count);    
        WordDetail_DisplayDtailInfo(win_id, 
            MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID, 
            MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID,
            MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID,
            MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID
        );
    }
    else
    {
        if(word_book_info.cur_chapter_idx + 1 < word_chapter_count)
        {
            GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, TRUE, TRUE);
            GUIBUTTON_SetTextId(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, WORD_FINISH);
            GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WORD_TO_LISTENING);
            GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, HANZI_NEXT_CHAPTER);
            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WordDetail_GoToListenWord);
            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WordDetail_NextChapterInfo);
        }
        else
        {
            GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, TRUE, TRUE);
            GUIBUTTON_SetTextId(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, WORD_BOOK_FINISH);
            GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WORD_TO_LISTENING);
            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WordDetail_GoToListenWord);
            GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WORD_BACK_CH);            
            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, MMI_CloseWordDetailWin);
        }
        LCD_DrawHLine(&lcd_dev_info, word_Hor_line_rect.left, word_Hor_line_rect.bottom, word_Hor_line_rect.right, MMI_WHITE_COLOR);
    }
}

LOCAL void WordDetailWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    char text_str[100] = {0};
    wchar text_wchar[100] = {0};
    BOOLEAN is_end = FALSE;

    GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, FALSE, FALSE);
    if(is_open_new_word){
        MMIRES_GetText(WORD_EXERCISE, win_id, &text_string);
    }else{
        strcpy(text_str, word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name);
        GUI_UTF8ToWstr(text_wchar, 100, text_str, strlen(text_str));
        text_string.wstr_ptr = text_wchar;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
    }
    Word_DrawWinTitle(win_id, MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID, text_string);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_20;
    text_style.font_color = MMI_WHITE_COLOR;

    if(!is_open_new_word){
        word_detail_count = word_chapter_info[word_book_info.cur_chapter_idx]->detail_count;
    }

    //SCI_TRACE_LOW("%s: word_detail_count = %d", __FUNCTION__, word_detail_count);
    if(word_detail_count <= 0){
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID, FALSE, FALSE);
    }
    if(word_detail_count == 0)
    {
        Word_DrawWinTips(win_id, MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, WORD_LOADING);
    }
    else if(word_detail_count == -1)
    {
        Word_DrawWinTips(win_id, MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, WORD_LOADING_FAILED);
    }
    else if(word_detail_count == -2)
    {
        Word_DrawWinTips(win_id, MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, WORD_NO_DATA);
    }
    else if(word_detail_count == -3)
    {
        Word_DrawWinTips(win_id, MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, NEW_WORD_BOOK_NO_DATA);
    }
    else
    {
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,TRUE,TRUE);
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID,TRUE,TRUE);
        GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID, WordDetail_PlayPinyinAudio);
        if(is_open_new_word){
            WordDetailWin_NewWordFullPaint(win_id);
        }else{
            WordDetailWin_NormalWordFullPaint(win_id);
        }
    }
}

LOCAL void WordDetailWin_TP_PRESS_UP(MMI_WIN_ID_T win_id, DPARAM param)
{
    int16 tp_offset_x = 0;
    int16 tp_offset_y = 0;
    GUI_POINT_T point = {0};
    point.x = MMK_GET_TP_X(param);
    point.y = MMK_GET_TP_Y(param);
    tp_offset_x = point.x - main_tp_down_x;
    tp_offset_y = point.y - main_tp_down_y;
    if(ABS(tp_offset_x) <= ABS(tp_offset_y))
    {
        if(tp_offset_y > 40)
        {
            MMK_PostMsg(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, MSG_KEYREPEAT_UP, PNULL, 0);
        }
        else if(tp_offset_y < -40)
        {
            MMK_PostMsg(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, MSG_KEYREPEAT_DOWN, PNULL, 0);
        }
    }
}

LOCAL void WordDetailWin_CLOSE_WINDOW(void)
{
    if(is_open_new_word && new_word_haved_delete)
    {
        Word_SaveDeleteNewWord(
            word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name                        
        );
    }
    else if(!is_open_new_word && chapter_unmaster_count > 0)
    {
        Word_WriteUnmasterChapterWord(
            word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name,
            chapter_unmaster_count
        );
    }
    new_word_haved_delete = FALSE;
    word_detail_count = 0;
    word_detail_cur_idx = 0;
    chapter_unmaster_count = 0;
    memset(chapter_unmaster_idx, 0, sizeof(chapter_unmaster_idx));
    Word_StopPlayMp3Data();
    Word_ReleaseNewWordInfo();
}

LOCAL MMI_RESULT_E HandleWordDetailWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                WordDetailWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {
                WordDetailWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_TP_PRESS_UP:
            {
                WordDetailWin_TP_PRESS_UP(win_id, param);
            }
            break;
        case MSG_TP_PRESS_DOWN:
            {
                main_tp_down_x = MMK_GET_TP_X(param);
                main_tp_down_y = MMK_GET_TP_Y(param);
            }
            break;
        case MSG_APP_LEFT:
            {
                WordDetail_KeyLeft();
            }
            break;
        case MSG_APP_RIGHT:
            {
                WordDetail_KeyRight();
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
        case MSG_CTL_MIDSK:
        case MSG_APP_WEB:
        case MSG_APP_OK:
            {
                if(is_open_new_word && word_detail_cur_idx < word_detail_count){
                    WordDetail_DeleteNewWord();
                }
            }
            break;
        case MSG_APP_0:
        case MSG_APP_1:
        case MSG_APP_2:
        case MSG_APP_3:
        case MSG_APP_4:
        case MSG_APP_5:
        case MSG_APP_6:
        case MSG_APP_7:
        case MSG_APP_8:
        case MSG_APP_9:
        case MSG_APP_HASH:
        case MSG_APP_STAR:
            {
                WordDetail_PlayPinyinAudio();
            }
            break;
        case MSG_KEYDOWN_UPSIDE:
        case MSG_KEYDOWN_VOL_UP:
            {
                word_player_volume = ZmtApp_VolumeChange(word_player_handle, TRUE, word_player_volume);
            }
            break;
        case MSG_KEYDOWN_DOWNSIDE:
        case MSG_KEYDOWN_VOL_DOWN:
            {
                word_player_volume = ZmtApp_VolumeChange(word_player_handle, FALSE, word_player_volume);
            }
            break;
        case MSG_KEYDOWN_CANCEL:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                WordDetailWin_CLOSE_WINDOW();
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_WORD_DETAIL_WIN_TAB) = {
    WIN_ID(MMI_WORD_DETAIL_WIN_ID),
    WIN_FUNC((uint32)HandleWordDetailWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID),//单元名称
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID),//导航页
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID),//汉字
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID),//拼音
    CREATE_BUTTON_CTRL(IMG_LISTEN_VOLUME, MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID),//音频图标
    CREATE_TEXT_CTRL(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID),//释义
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID),//已掌握/上一个
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID),//未掌握/下一个
    CREATE_BUTTON_CTRL(WORD_DELETE_ICON, MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID),//删除
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_MSG_TIPS_CTRL_ID),//已学完tip
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC MMI_RESULT_E MMI_CloseWordDetailWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(MMI_WORD_DETAIL_WIN_ID)){
        MMK_CloseWin(MMI_WORD_DETAIL_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateWordDetailWin(void)
{
    MMI_CloseWordDetailWin();
    MMK_CreateWin((uint32 *)MMI_WORD_DETAIL_WIN_TAB, PNULL);
}

/////////////////////////////////////////////////////////////////////
LOCAL void WordListenInfoWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_FONT_ALL_T font = {0};
    GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
    GUI_BG_T bg = {0};
    GUI_RECT_T title_rect = word_title_rect;
    GUI_RECT_T text_rect = word_text_rect;
    GUI_RECT_T dir_rect = word_dir_rect;
    GUI_RECT_T bottom_rect = word_bottom_rect;
		
    font.font = DP_FONT_18;
    font.color = MMI_WHITE_COLOR; 
    GUIBUTTON_SetFont(MMI_ZMT_WORD_LISTEN_INFO_LABEL_BACK_CTRL_ID, &font);
    Word_InitButton(MMI_ZMT_WORD_LISTEN_INFO_LABEL_BACK_CTRL_ID, title_rect, NULL, ALIGN_LVMIDDLE, TRUE, MMI_CloseWordListenInfoWin);

    Word_InitTextBox(MMI_ZMT_WORD_LISTEN_INFO_TEXT_INFO_CTRL_ID, text_rect);

    GUILABEL_SetRect(MMI_ZMT_WORD_LISTEN_INFO_LABEL_NUM_CTRL_ID, &dir_rect, FALSE);
    GUILABEL_SetFont(MMI_ZMT_WORD_LISTEN_INFO_LABEL_NUM_CTRL_ID, DP_FONT_16,MMI_WHITE_COLOR);
    GUILABEL_SetAlign(MMI_ZMT_WORD_LISTEN_INFO_LABEL_NUM_CTRL_ID, GUILABEL_ALIGN_MIDDLE);

    GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_LISTEN_INFO_BUTTON_AUDIO_CTRL_ID, WordDetail_PlayPinyinAudio);
    GUILABEL_SetFont(MMI_ZMT_WORD_LISTEN_INFO_LABEL_WORD_CTRL_ID, DP_FONT_20,MMI_WHITE_COLOR);
    GUILABEL_SetFont(MMI_ZMT_WORD_LISTEN_INFO_LABEL_PINYIN_CTRL_ID, DP_FONT_20,MMI_WHITE_COLOR);

    Word_InitButton( MMI_ZMT_WORD_LISTEN_INFO_BUTTON_STATUS_CTRL_ID,  bottom_rect, WORD_LEARN_LISTENING_BACK, ALIGN_HVMIDDLE, TRUE, MMI_CloseWordListenInfoWin);
    Word_InitButtonBg(MMI_ZMT_WORD_LISTEN_INFO_BUTTON_STATUS_CTRL_ID);
}

LOCAL void WordListenInfoWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    char text_str[100] = {0};
    wchar text_wchar[100] = {0};
    BOOLEAN is_end = FALSE;

    if(is_open_new_word){
        MMIRES_GetText(WORD_EXERCISE, win_id, &text_string);
    }else{
        strcpy(text_str, word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name);
        GUI_UTF8ToWstr(text_wchar, 100, text_str, strlen(text_str));
        text_string.wstr_ptr = text_wchar;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);                  
    }
    Word_DrawWinTitle(win_id, MMI_ZMT_WORD_LISTEN_INFO_LABEL_BACK_CTRL_ID, text_string);

    if(!is_open_new_word){
        word_detail_count = word_chapter_info[word_book_info.cur_chapter_idx]->detail_count;
    }
    
    if(word_detail_cur_idx < word_detail_count)
    {
        Word_SetDiretionText(MMI_ZMT_WORD_LISTEN_INFO_LABEL_NUM_CTRL_ID, word_detail_cur_idx, word_detail_count);    
        WordDetail_DisplayDtailInfo(win_id,
            MMI_ZMT_WORD_LISTEN_INFO_LABEL_WORD_CTRL_ID,
            MMI_ZMT_WORD_LISTEN_INFO_LABEL_PINYIN_CTRL_ID,
            MMI_ZMT_WORD_LISTEN_INFO_BUTTON_AUDIO_CTRL_ID,
            MMI_ZMT_WORD_LISTEN_INFO_TEXT_INFO_CTRL_ID
        );
    }
}

LOCAL void WordListenInfoWin_TP_PRESS_UP(MMI_WIN_ID_T win_id, DPARAM param)
{
    int16 tp_offset_x = 0;
    int16 tp_offset_y = 0;
    GUI_POINT_T point = {0};
    point.x = MMK_GET_TP_X(param);
    point.y = MMK_GET_TP_Y(param);
    tp_offset_x = point.x - main_tp_down_x;
    tp_offset_y = point.y - main_tp_down_y;
    if(ABS(tp_offset_x) <= ABS(tp_offset_y))
    {
        if(tp_offset_y > 40)
        {
            MMK_PostMsg(MMI_ZMT_WORD_LISTEN_INFO_TEXT_INFO_CTRL_ID, MSG_KEYREPEAT_UP, PNULL, 0);
        }
        else if(tp_offset_y < -40)
        {
            MMK_PostMsg(MMI_ZMT_WORD_LISTEN_INFO_TEXT_INFO_CTRL_ID, MSG_KEYREPEAT_DOWN, PNULL, 0);
        }
    }
}

LOCAL MMI_RESULT_E HandleWordListenInfoWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                WordListenInfoWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {
                WordListenInfoWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_TP_PRESS_UP:
            {
                WordListenInfoWin_TP_PRESS_UP(win_id, param);
            }
            break;
        case MSG_TP_PRESS_DOWN:
            {
                main_tp_down_x = MMK_GET_TP_X(param);
                main_tp_down_y = MMK_GET_TP_Y(param);
            }
            break;
        case MSG_CTL_MIDSK:
        case MSG_CTL_OK:
        case MSG_APP_WEB:
        case MSG_APP_OK:
        case MSG_CTL_PENOK:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_APP_0:
        case MSG_APP_1:
        case MSG_APP_2:
        case MSG_APP_3:
        case MSG_APP_4:
        case MSG_APP_5:
        case MSG_APP_6:
        case MSG_APP_7:
        case MSG_APP_8:
        case MSG_APP_9:
        case MSG_APP_HASH:
        case MSG_APP_STAR:
            {
                WordDetail_PlayPinyinAudio();
            }
            break;
        case MSG_KEYDOWN_CANCEL:
        case MSG_KEYDOWN_RED:
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

WINDOW_TABLE(MMI_WORD_LISTEN_INFO_WIN_TAB) = {
    WIN_ID(MMI_WORD_LISTEN_INFO_WIN_ID),
    WIN_FUNC((uint32)HandleWordListenInfoWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_LISTEN_INFO_LABEL_BACK_CTRL_ID),//单元名称
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_WORD_LISTEN_INFO_LABEL_NUM_CTRL_ID),//导航页
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_WORD_LISTEN_INFO_LABEL_WORD_CTRL_ID),//汉字
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_WORD_LISTEN_INFO_LABEL_PINYIN_CTRL_ID),//拼音
    CREATE_BUTTON_CTRL(IMG_LISTEN_VOLUME, MMI_ZMT_WORD_LISTEN_INFO_BUTTON_AUDIO_CTRL_ID),//音频图标
    CREATE_TEXT_CTRL(MMI_ZMT_WORD_LISTEN_INFO_TEXT_INFO_CTRL_ID),//释义
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_LISTEN_INFO_BUTTON_STATUS_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseWordListenInfoWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(MMI_WORD_LISTEN_INFO_WIN_ID)){
        MMK_CloseWin(MMI_WORD_LISTEN_INFO_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateWordListenInfoWin(void)
{
    MMI_CloseWordListenInfoWin();
    MMK_CreateWin((uint32 *)MMI_WORD_LISTEN_INFO_WIN_TAB, PNULL);
}

/////////////////////////////////////////////////////////////////////
LOCAL void WordListenSetWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUIFORM_CHILD_WIDTH_T list_ctrl_width  = {0};
    GUIFORM_CHILD_HEIGHT_T list_ctrl_height = {0};
    MMI_CTRL_ID_T form_ctrl_id = 0;
    MMI_CTRL_ID_T label_ctrl_id = 0;
    MMI_CTRL_ID_T list_ctrl_id = 0;
    GUI_RECT_T form_rect = word_form_rect;
    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
    uint16 form_hspace = 0;
    uint16 form_vspave = 10;
    CAF_COLOR_T bg_color = WORD_WIN_BG_COLOR;
    MMI_STRING_T text_string = {0};
    GUISTR_STYLE_T text_style = {0};

    memset(word_listen_set, 0, sizeof(word_listen_set));
    if(word_listen_info.interval == 0){
        word_listen_info.interval = WORD_LISTEN_SET_INTERVAL_3;
    }
    if(word_listen_info.repeat == 0){
        word_listen_info.repeat =WORD_LISTEN_SET_REPEAT_1;
    }
    word_listen_info.status = 0;
    word_listen_info.listen_idx = 0;
    word_listen_set[0] = word_listen_info.style;
    word_listen_set[1] = word_listen_info.interval;
    word_listen_set[2] = word_listen_info.repeat;
}

LOCAL void WordListenSetWin_DrawList(MMI_WIN_ID_T win_id)
{
    MMI_CTRL_ID_T ctrl_id = MMI_ZMT_WORD_LISTEN_SET_LIST_CTRL_ID;
    GUI_RECT_T list_rect = word_form_rect;
    GUILIST_ITEM_T item_info = {0};
    GUILIST_ITEM_DATA_T item_data= {0};
    uint8 i,j = 0;
    MMI_TEXT_ID_T item_title_text[WORD_LISTEN_SET_SYMBOL_NUM]= {
        WORD_LISTENING_STYLE, WORD_LISTENING_TIMES, WORD_LISTENING_REPEAT
    };
    MMI_TEXT_ID_T item_text[WORD_LISTEN_SET_SYMBOL_NUM][WORD_LISTEN_SET_SYMBOL_NUM] = {
        {WORD_LISTENING_NORMAL, WORD_LISTENING_RANDOM, WORD_LISTENING_RANDOM},
        {WORD_LISTENING_3S, WORD_LISTENING_5S, WORD_LISTENING_10S},
        {WORD_LISTENING_REPEAT_1, WORD_LISTENING_REPEAT_3, WORD_LISTENING_REPEAT_5},
    };
    uint16 item_num[WORD_LISTEN_SET_SYMBOL_NUM][WORD_LISTEN_SET_SYMBOL_NUM] = {
        {0, 1, 2},
        {WORD_LISTEN_SET_INTERVAL_3, WORD_LISTEN_SET_INTERVAL_5, WORD_LISTEN_SET_INTERVAL_10}, 
        {WORD_LISTEN_SET_REPEAT_1, WORD_LISTEN_SET_REPEAT_3, WORD_LISTEN_SET_REPEAT_5}
    };
    
    Word_InitListbox(win_id, ctrl_id, list_rect, WORD_LISTEN_SET_SYMBOL_NUM);
    for(i = 0;i < WORD_LISTEN_SET_SYMBOL_NUM;i++)
    {
        item_info.item_style = GUIITEM_SYTLE_ZMT_LISTEN_SET_MS;
        item_info.item_data_ptr = &item_data;

        item_data.item_content[0].is_default =TRUE;
        item_data.item_content[0].font_color_id = MMITHEME_COLOR_LIGHT_WHITE;
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_ID;
        item_data.item_content[0].item_data.text_id= item_title_text[i];

        item_data.item_content[1].is_default =TRUE;
        item_data.item_content[1].font_color_id = MMITHEME_COLOR_LIGHT_WHITE;
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_ID;
        for(j = 0; j < WORD_LISTEN_SET_SYMBOL_NUM;j++){
            if(item_num[i][j] == word_listen_set[i]){
                item_data.item_content[1].item_data.text_id = item_text[i][j];
                break;
            }
        }
        GUILIST_AppendItem(ctrl_id, &item_info);
    }
    GUILIST_SetCurItemIndex(ctrl_id, word_listen_set_idx);
}

LOCAL void WordListenSetWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    uint8 i = 0;
    GUI_RECT_T left_rect = word_left_button_rect;
    GUI_RECT_T right_rect = word_right_button_rect;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_22;
    text_style.font_color = MMI_WHITE_COLOR;
    
    MMIRES_GetText(WORD_LISTENING_SETTING, win_id, &text_string);
    Word_DrawWinTitle(win_id, 0, text_string);
    
    WordListenSetWin_DrawList(win_id);

    text_style.font = DP_FONT_20;
    GUIRES_DisplayImg(PNULL, &left_rect, PNULL, win_id, FORMULA_BOTTOM_BG_IMG, &lcd_dev_info);
    MMIRES_GetText(WORD_TRUE, win_id, &text_string);
    text_style.font_color = WORD_WIN_BG_COLOR;
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &left_rect,
        &left_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );

    GUIRES_DisplayImg(PNULL, &right_rect, PNULL, win_id, FORMULA_BOTTOM_BG_IMG, &lcd_dev_info);
    MMIRES_GetText(WORD_FALSE, win_id, &text_string);
    text_style.font_color = WORD_WIN_BG_COLOR;
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &right_rect,
        &right_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
}

LOCAL void WordListenSetWin_CTL_PENOK(MMI_WIN_ID_T win_id, DPARAM param)
{
    word_listen_info.style = word_listen_set[0];
    word_listen_info.interval = word_listen_set[1];
    word_listen_info.repeat = word_listen_set[2];
    word_listen_info.listen_idx = 0;
    word_listen_cur_idx = 0;
    MMK_CloseWin(win_id);
}

LOCAL void WordListenSetWin_TP_PRESS_UP(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
    GUI_RECT_T left_rect = word_left_button_rect;
    GUI_RECT_T right_rect = word_right_button_rect;
    if(GUI_PointIsInRect(point, right_rect))
    {
        MMK_CloseWin(win_id);
    }
    else if(GUI_PointIsInRect(point, left_rect))
    {
        word_listen_info.style = word_listen_set[0];
        word_listen_info.interval = word_listen_set[1];
        word_listen_info.repeat = word_listen_set[2];
        word_listen_info.listen_idx = 0;
        word_listen_cur_idx = 0;
        MMK_CloseWin(win_id);
    }
}

LOCAL void WordListenSetWin_KeyLeftRight(MMI_WIN_ID_T win_id, BOOLEAN is_left)
{
    word_listen_set_idx = GUILIST_GetCurItemIndex(MMI_ZMT_WORD_LISTEN_SET_LIST_CTRL_ID);
    if(is_left)
    {
        switch(word_listen_set_idx)
        {
            case 0:
            {
                if(word_listen_set[0] == 0){
                    word_listen_set[0] = 1;
                }else{
                    word_listen_set[0] = 0;
                }
            }
            break;
            case 1:
            {
                if(word_listen_set[1] == WORD_LISTEN_SET_INTERVAL_3){
                    word_listen_set[1] = WORD_LISTEN_SET_INTERVAL_10;
                }else if(word_listen_set[1] == WORD_LISTEN_SET_INTERVAL_10){
                    word_listen_set[1] = WORD_LISTEN_SET_INTERVAL_5;
                }else if(word_listen_set[1] == WORD_LISTEN_SET_INTERVAL_5){
                    word_listen_set[1] = WORD_LISTEN_SET_INTERVAL_3;
                }
            }
            break;
            case 2:
            {
                if(word_listen_set[2] == WORD_LISTEN_SET_REPEAT_1){
                    word_listen_set[2] = WORD_LISTEN_SET_REPEAT_5;
                }else if(word_listen_set[2] == WORD_LISTEN_SET_REPEAT_5){
                    word_listen_set[2] = WORD_LISTEN_SET_REPEAT_3;
                }else if(word_listen_set[2] == WORD_LISTEN_SET_REPEAT_3){
                    word_listen_set[2] = WORD_LISTEN_SET_REPEAT_1;
                }
            }
            break;
        }
    }
    else
    {
        switch(word_listen_set_idx)
        {
            case 0:
            {
                if(word_listen_set[0] == 0){
                    word_listen_set[0] = 1;
                }else{
                    word_listen_set[0] = 0;
                }
            }
            break;
            case 1:
            {
                if(word_listen_set[1] == WORD_LISTEN_SET_INTERVAL_3){
                    word_listen_set[1] = WORD_LISTEN_SET_INTERVAL_5;
                }else if(word_listen_set[1] == WORD_LISTEN_SET_INTERVAL_5){
                    word_listen_set[1] = WORD_LISTEN_SET_INTERVAL_10;
                }else if(word_listen_set[1] == WORD_LISTEN_SET_INTERVAL_10){
                    word_listen_set[1] = WORD_LISTEN_SET_INTERVAL_3;
                }
            }
            break;
            case 2:
            {
                if(word_listen_set[2] == WORD_LISTEN_SET_REPEAT_1){
                    word_listen_set[2] = WORD_LISTEN_SET_REPEAT_3;
                }else if(word_listen_set[2] == WORD_LISTEN_SET_REPEAT_3){
                    word_listen_set[2] = WORD_LISTEN_SET_REPEAT_5;
                }else if(word_listen_set[2] == WORD_LISTEN_SET_REPEAT_5){
                    word_listen_set[2] = WORD_LISTEN_SET_REPEAT_1;
                }
            }
            break;
        }
    }
    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
}

LOCAL void WordListenSetWin_KeyUpDown(MMI_WIN_ID_T win_id, BOOLEAN is_up)
{
    if(is_up)
    {
        if(word_listen_set_idx == 0){
            word_listen_set_idx = 2;
        }else{
            word_listen_set_idx--;
        }
    }
    else
    {
        if(word_listen_set_idx == 2){
            word_listen_set_idx = 0;
        }else{
            word_listen_set_idx++;
        }
    }
}

LOCAL MMI_RESULT_E HandleWordListenSetWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                WordListenSetWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {
                WordListenSetWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_RECT_T right_rect = word_right_button_rect;
                GUI_POINT_T point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(point.y >= right_rect.top)
                {
                    WordListenSetWin_TP_PRESS_UP(win_id, point);
                }
            }
            break;
        case MSG_APP_LEFT:
            {
                WordListenSetWin_KeyLeftRight(win_id, TRUE);
            }
            break;
        case MSG_APP_RIGHT:
            {
                WordListenSetWin_KeyLeftRight(win_id, FALSE);
            }
            break;
        case MSG_KEYUP_UP:
            {
                //WordListenSetWin_KeyUpDown(win_id, FALSE);
            }
            break;
        case MSG_KEYUP_DOWN:
            {
                //WordListenSetWin_KeyUpDown(win_id, FALSE);
            }
            break;
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            {
                WordListenSetWin_CTL_PENOK(win_id, param);
            }
            break;
        case MSG_KEYDOWN_CANCEL:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                memset(word_listen_set, 0, sizeof(word_listen_set));
                word_listen_set_idx = 0;
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_WORD_LISTEN_SET_WIN_TAB) = {
    WIN_ID(MMI_WORD_LISTEN_SET_WIN_ID),
    WIN_FUNC((uint32)HandleWordListenSetWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseWordListenSetWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(MMI_WORD_LISTEN_SET_WIN_ID)){
        MMK_CloseWin(MMI_WORD_LISTEN_SET_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateWordListenSetWin(void)
{
    MMI_CloseWordListenSetWin();
    MMK_CreateWin((uint32 *)MMI_WORD_LISTEN_SET_WIN_TAB, PNULL);
}

////////////////////////////////////////////////////////////////////
LOCAL void Word_GetOneArrayRandNum(uint8 count, uint8 min, uint8 max)
{
    int i = 0;
    int j = 0;
    int temp = 0;
    
    srand(MMIAPICOM_GetCurTime());
    for (i = 0; i < count; i++) {
        word_listen_idx[i] = i + min;
    }
    for (i = max; i > 0; i--) {
        j = rand() % (i + 1);
        temp = word_listen_idx[i];
        word_listen_idx[i] = word_listen_idx[j];
        word_listen_idx[j] = temp;
    }
    //for (i = 0; i < count; i++) {
    //    SCI_TRACE_LOW("%d ", word_listen_idx[i]);
    //}
}

LOCAL void WordListenWin_PlayAudioFail(void)
{
    if(MMI_IsOpenWordListenWin() && word_listen_info.status == WORD_LISTEN_NOW)
    {
        uint8 listen_idx = 0;
        if(word_listen_info.style == 0){
            word_listen_cur_idx++;
        }else{
            word_listen_info.listen_idx++;
            word_listen_cur_idx = word_listen_idx[word_listen_info.listen_idx];
        }
        word_detail_cur_idx = word_listen_cur_idx;
        MMK_SendMsg(MMI_WORD_LISTEN_WIN_ID, MSG_FULL_PAINT, PNULL);
        SCI_TRACE_LOW("%s: word_listen_cur_idx = %d", __FUNCTION__, word_listen_cur_idx);
        if(word_listen_info.style == 0){
            listen_idx = word_listen_cur_idx;
        }else{
            listen_idx = word_listen_info.listen_idx;
        }
        if(listen_idx < word_detail_count){
            WordDetail_PlayPinyinAudio();
        }
    }
}

LOCAL void WordListenWin_StopIntervalTimer(void)
{
    if(word_listen_timer_id != 0)
    {
        MMK_StopTimer(word_listen_timer_id);
        word_listen_timer_id = 0;
    }
}

LOCAL void WordListenWin_IntervalTimerCallback(uint8 timer_id, uint32 param)
{
    if(word_listen_timer_id == timer_id)
    {
        uint8 listen_idx = 0;
        WordListenWin_StopIntervalTimer();
        SCI_TRACE_LOW("%s: word_listen_repeat_count= %d", __FUNCTION__, word_listen_repeat_count);
        if(word_listen_repeat_count + 1 >= word_listen_info.repeat){
            word_listen_repeat_count = 0;
            if(word_listen_info.style == 0){
                word_listen_cur_idx++;
            }else{
                word_listen_info.listen_idx++;
                word_listen_cur_idx = word_listen_idx[word_listen_info.listen_idx];
            }
            word_detail_cur_idx = word_listen_cur_idx;
            if(MMK_IsFocusWin(MMI_WORD_LISTEN_WIN_ID)){
                MMK_SendMsg(MMI_WORD_LISTEN_WIN_ID, MSG_FULL_PAINT, PNULL);
            }
            SCI_TRACE_LOW("%s: word_listen_cur_idx = %d", __FUNCTION__, word_listen_cur_idx);
        }else{
            word_listen_repeat_count++;
        }
        if(word_listen_info.style == 0){
            listen_idx = word_listen_cur_idx;
        }else{
            listen_idx = word_listen_info.listen_idx;
        }
        if(listen_idx < word_detail_count){
            if(MMK_IsFocusWin(MMI_WORD_LISTEN_WIN_ID)){
                WordDetail_PlayPinyinAudio();
            }
        }
    }
}

LOCAL void WordListenWin_CreateIntervalTimer(void)
{
    SCI_TRACE_LOW("%s: start", __FUNCTION__);
    WordListenWin_StopIntervalTimer();
    word_listen_timer_id = MMK_CreateTimerCallback(word_listen_info.interval, WordListenWin_IntervalTimerCallback, PNULL, FALSE);
    MMK_StartTimerCallback(word_listen_timer_id, word_listen_info.interval, WordListenWin_IntervalTimerCallback, PNULL, FALSE);
}

LOCAL void WordListenWin_SetClickFunc(void)
{
    if(word_listen_info.status == WORD_LISTEN_NOW)
    {
        word_listen_info.status = WORD_LISTEN_PAUSE;
    }
    WordListenWin_StopIntervalTimer();
    Word_StopPlayMp3Data();
    MMI_CreateWordListenSetWin();
}

LOCAL void WordListenWin_ImgClickFunc(void)
{
    if(word_listen_info.status == WORD_LISTEN_NOW || word_listen_info.status == WORD_LISTEN_PAUSE)
    {
        word_listen_info.status = WORD_LISTEN_PAUSE;
        WordListenWin_StopIntervalTimer();
        Word_StopPlayMp3Data();
        word_detail_cur_idx = word_listen_cur_idx;
        if(word_detail_cur_idx < word_detail_count){
            MMI_CreateWordListenInfoWin();
        }
    }
}

LOCAL void WordListenWin_BottomActionFunc(void)
{
    if(word_listen_info.status == WORD_LISTEN_NONE)
    {
        word_listen_info.status = WORD_LISTEN_NOW;
        word_listen_cur_idx = 0;
        word_detail_cur_idx = word_listen_cur_idx;
        WordDetail_PlayPinyinAudio();
    }
    else if(word_listen_info.status == WORD_LISTEN_PAUSE)
    {
        word_listen_info.status = WORD_LISTEN_NOW;
        if(word_listen_info.style == 1){
            word_listen_cur_idx = word_listen_idx[word_listen_info.listen_idx];
        }
        word_detail_cur_idx = word_listen_cur_idx;
        WordDetail_PlayPinyinAudio();
    }
    else if(word_listen_info.status == WORD_LISTEN_NOW)
    {
        word_listen_info.status = WORD_LISTEN_PAUSE;
        WordListenWin_StopIntervalTimer();
        Word_StopPlayMp3Data();
    }
    else if(word_listen_info.status == WORD_LISTEN_END)
    {
        word_listen_info.status = WORD_LISTEN_NOW;
        word_listen_cur_idx = 0;
        word_detail_cur_idx = word_listen_cur_idx;
        WordDetail_PlayPinyinAudio();
    }
    MMK_SendMsg(MMI_WORD_LISTEN_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void WordListenWin_ListenAgainFunc(void)
{
    word_listen_info.status = WORD_LISTEN_NOW;
    word_listen_cur_idx = 0;
    word_listen_info.listen_idx = 0;
    memset(word_listen_idx, 0, sizeof(word_listen_idx));
    Word_GetOneArrayRandNum(word_detail_count, 0, word_detail_count-1);
    if(word_listen_info.style == 1){
        word_detail_cur_idx = word_listen_idx[0];
    }
    word_detail_cur_idx = word_listen_cur_idx;
    WordDetail_PlayPinyinAudio();
    MMK_SendMsg(MMI_WORD_LISTEN_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void WordListenWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    GUI_FONT_ALL_T font = {0};
    GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
    GUI_FONT_T text_font = DP_FONT_20;
    GUI_COLOR_T text_color = MMI_WHITE_COLOR;
    GUI_RECT_T img_rect = {0};
    GUI_RECT_T tip_rect = {0};
    GUI_RECT_T dir_rect = word_dir_rect;
    GUI_RECT_T text_rect = word_text_rect;
    GUI_RECT_T left_rect = word_left_rect;
    GUI_RECT_T right_rect = word_right_rect;
    GUI_RECT_T bottom_rect = word_bottom_rect;

    GUIBUTTON_SetRect(MMI_ZMT_WORD_LISTEN_BUTTON_SET_CTRL_ID, &dir_rect);               
    GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_LISTEN_BUTTON_SET_CTRL_ID, WordListenWin_SetClickFunc);

    dir_rect.top = dir_rect.bottom;
    dir_rect.bottom = dir_rect.top + WORD_CARD_LINE_HIGHT;
    GUILABEL_SetRect(MMI_ZMT_WORD_LISTEN_LABEL_NUM_CTRL_ID, &dir_rect, FALSE);
    GUILABEL_SetFont(MMI_ZMT_WORD_LISTEN_LABEL_NUM_CTRL_ID, DP_FONT_20,MMI_WHITE_COLOR);
    GUILABEL_SetAlign(MMI_ZMT_WORD_LISTEN_LABEL_NUM_CTRL_ID, GUILABEL_ALIGN_MIDDLE);

    img_rect = text_rect;
    img_rect.top = 2*WORD_CARD_LINE_HIGHT;
    img_rect.bottom = 7*WORD_CARD_LINE_HIGHT;
    GUIBUTTON_SetRect(MMI_ZMT_WORD_LISTEN_BUTTON_IMG_CTRL_ID, &img_rect);               
    GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_LISTEN_BUTTON_IMG_CTRL_ID, WordListenWin_ImgClickFunc);

    tip_rect = text_rect;
    tip_rect.top = 7*WORD_CARD_LINE_HIGHT;
    tip_rect.bottom = tip_rect.top + WORD_CARD_LINE_HIGHT;
    GUILABEL_SetRect(MMI_ZMT_WORD_LISTEN_LABEL_TIPS_CTRL_ID, &tip_rect, FALSE);
    GUILABEL_SetFont(MMI_ZMT_WORD_LISTEN_LABEL_TIPS_CTRL_ID, DP_FONT_18,MMI_WHITE_COLOR);
    GUILABEL_SetAlign(MMI_ZMT_WORD_LISTEN_LABEL_TIPS_CTRL_ID, GUILABEL_ALIGN_MIDDLE);

    Word_InitButton(MMI_ZMT_WORD_LISTEN_BUTTON_STATUS_CTRL_ID, bottom_rect, NULL, ALIGN_HVMIDDLE, TRUE, WordListenWin_BottomActionFunc);
    Word_InitButtonBg(MMI_ZMT_WORD_LISTEN_BUTTON_STATUS_CTRL_ID);

    Word_InitButton(MMI_ZMT_WORD_LISTEN_LEFT_CTRL_ID, left_rect, WORD_LISTENING_AGAIN, ALIGN_HVMIDDLE, FALSE, WordListenWin_ListenAgainFunc);
    Word_InitButtonBg(MMI_ZMT_WORD_LISTEN_LEFT_CTRL_ID);
    Word_InitButton(MMI_ZMT_WORD_LISTEN_RIGHT_CTRL_ID, right_rect, WORD_LISTENING_EXIT, ALIGN_HVMIDDLE, FALSE, MMI_CloseWordListenWin);
    Word_InitButtonBg(MMI_ZMT_WORD_LISTEN_RIGHT_CTRL_ID);

    if(word_listen_info.interval == 0){
        word_listen_info.interval = WORD_LISTEN_SET_INTERVAL_3;
    }
    if(word_listen_info.repeat == 0){
        word_listen_info.repeat =WORD_LISTEN_SET_REPEAT_1;
    }
    word_listen_info.status = 0;
    word_listen_info.listen_idx = 0;
}

LOCAL void WordListenWin_DrawListenStatus(MMI_WIN_ID_T win_id, WORD_LISTEN_STAUS_TYPE_E status)
{
    MMI_TEXT_ID_T label_text = 0;
    MMI_TEXT_ID_T button_text = 0;

    switch(status)
    {
        case WORD_LISTEN_NONE:
            {
                label_text = WORD_LEARN_LISTENING_TIPS_1;
                button_text = WORD_LEARN_LISTENING_ACTION;
            }
            break;
        case WORD_LISTEN_NOW:
            {
                label_text = WORD_LEARN_LISTENING_TIPS_2;
                button_text = WORD_LEARN_LISTENING;
            }
            break;
        case WORD_LISTEN_PAUSE:
            {
                label_text = WORD_LEARN_LISTENING_TIPS_2;
                button_text = WORD_LEARN_LISTENING_PAUSE;
            }
            break;
        default:
            return;
    }
    GUILABEL_SetTextById(MMI_ZMT_WORD_LISTEN_LABEL_TIPS_CTRL_ID, label_text, TRUE);
    GUIBUTTON_SetTextId(MMI_ZMT_WORD_LISTEN_BUTTON_STATUS_CTRL_ID, button_text);
    GUIBUTTON_SetVisible(MMI_ZMT_WORD_LISTEN_BUTTON_STATUS_CTRL_ID, TRUE, TRUE);
}

LOCAL void WordListenWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    char text_str[100] = {0};
    wchar text_wchar[100] = {0};
    BOOLEAN is_end = FALSE;
    uint8 listen_idx = 0;

    if(is_open_new_word){
        MMIRES_GetText(WORD_EXERCISE, win_id, &text_string);
    }else{
        strcpy(text_str, word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name);
        GUI_UTF8ToWstr(text_wchar, 100, text_str, strlen(text_str));
        text_string.wstr_ptr = text_wchar;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
    }
    Word_DrawWinTitle(win_id, 0, text_string);

    if(word_listen_info.style == 0){
        listen_idx = word_listen_cur_idx;
    }else{
        listen_idx = word_listen_info.listen_idx;
    }
    if(listen_idx < word_detail_count)
    {
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_LISTEN_LEFT_CTRL_ID,FALSE,FALSE);
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_LISTEN_RIGHT_CTRL_ID,FALSE,FALSE);

        WordListenWin_DrawListenStatus(win_id, word_listen_info.status);
            
        if(word_listen_info.style == 1){
            GUILABEL_SetVisible(MMI_ZMT_WORD_LISTEN_LABEL_NUM_CTRL_ID, FALSE, TRUE);
        }else{
            GUILABEL_SetVisible(MMI_ZMT_WORD_LISTEN_LABEL_NUM_CTRL_ID, TRUE, TRUE);
            //Word_SetDiretionText(MMI_ZMT_WORD_LISTEN_LABEL_NUM_CTRL_ID, word_listen_cur_idx, word_detail_count);
        }
    }
    else
    {
        GUILABEL_SetVisible(MMI_ZMT_WORD_LISTEN_LABEL_NUM_CTRL_ID, FALSE, TRUE);
        GUILABEL_SetTextById(MMI_ZMT_WORD_LISTEN_LABEL_TIPS_CTRL_ID, WORD_LISTENING_END, TRUE);
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_LISTEN_BUTTON_STATUS_CTRL_ID, FALSE, TRUE);

        GUIBUTTON_SetVisible(MMI_ZMT_WORD_LISTEN_LEFT_CTRL_ID,TRUE,TRUE);
        GUIBUTTON_SetVisible(MMI_ZMT_WORD_LISTEN_RIGHT_CTRL_ID,TRUE,TRUE);

        word_listen_info.status = WORD_LISTEN_END;
    }
}

LOCAL void WordListenWin_CLOSE_WINDOW(void)
{
    word_listen_cur_idx = 0;
    word_detail_cur_idx = 0;
    word_listen_repeat_count = 0;
    word_listen_info.status = 0;
    word_listen_info.listen_idx = 0;
    memset(word_listen_idx, 0, sizeof(word_listen_idx));
    Word_StopPlayMp3Data();
    WordListenWin_StopIntervalTimer();
    MMI_CloseWordDetailWin();
}

LOCAL MMI_RESULT_E HandleWordListenWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                WordListenWin_OPEN_WINDOW(win_id);
                WATCHCOM_Backlight(TRUE);
                Word_GetOneArrayRandNum(word_detail_count, 0, word_detail_count-1);
            }
            break;
        case MSG_FULL_PAINT:
            {
                WordListenWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_KEYDOWN_UPSIDE:
        case MSG_KEYDOWN_VOL_UP:
            {
                word_player_volume = ZmtApp_VolumeChange(word_player_handle, TRUE, word_player_volume);
            }
            break;
        case MSG_KEYDOWN_DOWNSIDE:
        case MSG_KEYDOWN_VOL_DOWN:
            {
                word_player_volume = ZmtApp_VolumeChange(word_player_handle, FALSE, word_player_volume);
            }
            break;
        case MSG_APP_HASH:
        case MSG_APP_STAR:
            {
                WordListenWin_SetClickFunc();
            }
            break;
        case MSG_KEYDOWN_0:
        case MSG_KEYDOWN_1:
        case MSG_KEYDOWN_2:
        case MSG_KEYDOWN_3:
        case MSG_KEYDOWN_4:
        case MSG_KEYDOWN_5:
        case MSG_KEYDOWN_6:
        case MSG_KEYDOWN_7:
        case MSG_KEYDOWN_8:
        case MSG_KEYDOWN_9:
            {
                WordListenWin_ImgClickFunc();
            }
            break;
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            {
                WordListenWin_BottomActionFunc();
            }
            break;
        case MSG_KEYDOWN_CANCEL:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                WordListenWin_CLOSE_WINDOW();
                WATCHCOM_Backlight(FALSE);
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_WORD_LISTEN_WIN_TAB) = {
    WIN_ID(MMI_WORD_LISTEN_WIN_ID),
    WIN_FUNC((uint32)HandleWordListenWinMsg),
    CREATE_BUTTON_CTRL(IMG_ZMT_LISTEN_SET, MMI_ZMT_WORD_LISTEN_BUTTON_SET_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_ZMT_WORD_LISTEN_LABEL_NUM_CTRL_ID),
    CREATE_BUTTON_CTRL(IMG_ZMT_LISTEN_PEN, MMI_ZMT_WORD_LISTEN_BUTTON_IMG_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_ZMT_WORD_LISTEN_LABEL_TIPS_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_LISTEN_BUTTON_STATUS_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_LISTEN_LEFT_CTRL_ID),//退出
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_LISTEN_RIGHT_CTRL_ID),//再听一遍
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E MMI_CloseWordListenWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    if(MMK_IsOpenWin(MMI_WORD_LISTEN_WIN_ID)){
        MMK_CloseWin(MMI_WORD_LISTEN_WIN_ID);
    }
    return result;
}

PUBLIC void MMI_CreateWordListenWin(void)
{
    MMI_CloseWordListenWin();
    MMK_CreateWin((uint32 *)MMI_WORD_LISTEN_WIN_TAB, PNULL);
}

LOCAL BOOLEAN MMI_IsOpenWordListenWin(void)
{
    if(MMK_IsOpenWin(MMI_WORD_LISTEN_WIN_ID)){
        return TRUE;
    }
    return FALSE;
}

PUBLIC void ZMTWord_CloseWordPlayer(void)
{
    Word_StopPlayMp3Data();
    WordListenWin_StopIntervalTimer();
    word_listen_info.status = WORD_LISTEN_PAUSE;
}

