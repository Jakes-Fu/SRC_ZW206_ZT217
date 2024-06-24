#include "std_header.h"

#include <stdlib.h>

#include "cjson.h"
#include "dal_time.h"
#include "zmt_word_id.h"
#include "zmt_word_main.h"
#include "zmt_word_text.h"
#include "zmt_word_image.h"
#include "gps_drv.h"
#include "gps_interface.h"
#include "guibutton.h"
#include "guifont.h"
#include "guilcd.h"
#include "guistring.h"
#include "guitext.h"
#include "mmi_textfun.h"
#include "mmiacc_text.h"
#include "mmicc_export.h"
#include "mmidisplay_data.h"
#include "mmipub.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmisrvrecord_export.h"
#ifdef LISTENING_PRATICE_SUPPORT
#include "zmt_main_file.h"
#include "zmt_listening_image.h"
#endif
#ifdef HANZI_CARD_SUPPORT
#include "zmt_hanzi_image.h"
#include "zmt_hanzi_text.h"
#endif
#ifdef POETRY_LISTEN_SUPPORT
#include "zmt_poetry_image.h"
#endif

LOCAL GUI_RECT_T word_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};//窗口
LOCAL GUI_RECT_T word_title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, WORD_CARD_LINE_HIGHT};//顶部
LOCAL GUI_RECT_T word_auto_play_rect = {4*WORD_CARD_LINE_WIDTH, 0, MMI_MAINSCREEN_WIDTH, WORD_CARD_LINE_HIGHT};//自动播放图标
LOCAL GUI_RECT_T word_list_rect = {0, WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT-5};//列表
LOCAL GUI_RECT_T word_dir_rect = {5*WORD_CARD_LINE_WIDTH-10, 0, MMI_MAINSCREEN_WIDTH, WORD_CARD_LINE_HIGHT};
LOCAL GUI_RECT_T word_word_rect = {5, WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 2*WORD_CARD_LINE_HIGHT};//汉字
LOCAL GUI_RECT_T word_pinyin_rect = {5, 1.8*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-5, 2.8*WORD_CARD_LINE_HIGHT};//音标
LOCAL GUI_RECT_T word_pinyin_audio_rect = {5*WORD_CARD_LINE_WIDTH, 1.8*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-5, 2.8*WORD_CARD_LINE_HIGHT};//音标icon
LOCAL GUI_RECT_T word_text_rect = {5,2.8*WORD_CARD_LINE_HIGHT+1,MMI_MAINSCREEN_WIDTH-5,MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT-5};//释义
LOCAL GUI_RECT_T word_msg_rect = {WORD_CARD_LINE_WIDTH, 3*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-WORD_CARD_LINE_WIDTH, 5*WORD_CARD_LINE_HIGHT};//界面的信息提示
LOCAL GUI_RECT_T word_left_rect = {10, MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT, 3*WORD_CARD_LINE_WIDTH-10, MMI_MAINSCREEN_HEIGHT-5};//汉字学习/掌握
LOCAL GUI_RECT_T word_right_rect = {3*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-10, MMI_MAINSCREEN_HEIGHT-5};//生词本/未掌握
LOCAL GUI_RECT_T word_pre_rect = {0.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 1.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-上一个
LOCAL GUI_RECT_T word_del_rect = {2.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 3.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-删除
LOCAL GUI_RECT_T word_next_rect = {4.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 5.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-下一个
LOCAL GUI_RECT_T word_tip_rect = {WORD_CARD_LINE_WIDTH,4*WORD_CARD_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-WORD_CARD_LINE_WIDTH,6*WORD_CARD_LINE_HIGHT};//弹框提示
LOCAL GUI_RECT_T word_Hor_line_rect = {0};

LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;

WORD_BOOK_INFO_T word_book_info = {0};
int8 word_publish_count = 0;
int8 word_book_count = 0;
int8 word_chapter_count = 0;
int16 word_detail_count = 0;
WORD_BOOK_PUBLISH_INFO_T * word_publish_info[WORD_PUBLISH_MAX];
WORD_GRADE_BOOK_T word_publish_grade_book_count[WORD_PUBLISH_BOOK_TOTAL_MAX];
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
LOCAL uint8 chapter_unmaster_count = 0;
LOCAL BOOLEAN new_word_haved_delete = FALSE;
int16 word_detail_cur_idx = 0;
int chapter_unmaster_idx[WORD_CHAPTER_WORD_MAX] = {0};

LOCAL MMI_RESULT_E MMI_CloseWordChapterWin(void);
LOCAL MMI_RESULT_E MMI_CloseWordDetailWin(void);
LOCAL void WordDetail_ShowTip(void);

LOCAL void Word_InitButton(MMI_CTRL_ID_T ctrl_id)
{
	GUI_FONT_ALL_T font = {0};
	GUI_BG_T bg = {0};
	GUI_BORDER_T btn_border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
	font.font = DP_FONT_16;
	font.color = GUI_RGB2RGB565(80, 162, 254);
	GUIBUTTON_SetBorder(ctrl_id, &btn_border, FALSE);
	bg.bg_type = GUI_BG_COLOR;
	bg.color = MMI_WHITE_COLOR;
	GUIBUTTON_SetBg(ctrl_id, &bg);
	GUIBUTTON_SetFont(ctrl_id, &font);
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

LOCAL BOOLEAN Word_GetPublishAndBookIdx(uint16 idx)
{
    uint8 i = 0;
    uint8 j = 0;
    uint16 k = 0;
    BOOLEAN is_get = FALSE;
    for(i = 0;i < word_publish_count && i < WORD_PUBLISH_MAX; i++)
    {
        for(j = 0;j < word_publish_info[i]->item_count && j < WORD_PUBLISH_BOOK_MAX;j++)
        {
            if(k == idx){
                is_get = TRUE;
                break;
            }
            k++;
        }
        if(is_get){
            break;
        }
    }
    SCI_TRACE_LOW("%s: i = %d, j = %d", __FUNCTION__, i, j);
    if(is_get){
        word_book_info.cur_publish_idx = i;
        word_book_info.cur_book_idx = j;
    }
    return is_get;
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
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    
    list_init.both_rect.v_rect = word_list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    
    cur_publish_idx = word_book_info.cur_publish_grade_idx;
    if(cur_publish_idx == 0){
        book_count = word_publish_grade_book_count[0].count;
        index = 0;
    }else{
        book_count = word_publish_grade_book_count[cur_publish_idx].count - word_publish_grade_book_count[cur_publish_idx-1].count;
        index = word_publish_grade_book_count[cur_publish_idx-1].count;
    }
    GUILIST_SetMaxItem(ctrl_id, book_count, FALSE);

    for(i = 0;i < word_publish_count && i < WORD_PUBLISH_MAX; i++)
    {
        for(j = 0;j < word_publish_info[i]->item_count && j < WORD_PUBLISH_BOOK_MAX;j++)
        {
            if(k >= index && k <  index + book_count)
            {
                item_t.item_style = GUIITEM_STYLE_POETRY_ITEM_LIST_MS;
                item_t.item_data_ptr = &item_data;
                item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

                item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
                item_data.item_content[0].item_data.image_id = IMG_POETRY_ITEM_BG;

                GUI_UTF8ToWstr(text_str, 100, word_publish_info[i]->item_info[j]->book_name, strlen(word_publish_info[i]->item_info[j]->book_name));
                text_string.wstr_ptr = text_str;
                text_string.wstr_len = MMIAPICOM_Wstrlen(text_str);
                item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
                item_data.item_content[1].item_data.text_buffer = text_string;

                GUILIST_AppendItem(ctrl_id, &item_t);
            }
            else
            {
                if(k >= index + book_count){
                    break;
                }
            }
            k++;
        }
    }
    //不画分割线
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
    //不画高亮条
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_AUTO_SCROLL, TRUE);
    GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);
    GUILIST_SetBgColor(ctrl_id, GUI_RGB2RGB565(80, 162, 254));
    GUILIST_SetTextFont(ctrl_id, DP_FONT_20, GUI_RGB2RGB565(80, 162, 254));
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
                GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
                GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
                GUISTR_STYLE_T text_style = {0};
                MMI_STRING_T text_string = {0};

                GUI_FillRect(&lcd_dev_info, word_win_rect, GUI_RGB2RGB565(80, 162, 254));
                GUI_FillRect(&lcd_dev_info, word_title_rect, GUI_RGB2RGB565(108, 181, 255));

                text_style.align = ALIGN_HVMIDDLE;
                text_style.font = DP_FONT_22;
                text_style.font_color = MMI_WHITE_COLOR;

                MMIRES_GetText(WORD_TITLE, win_id, &text_string);
                GUISTR_DrawTextToLCDInRect(
                    (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                    &word_title_rect,
                    &word_title_rect,
                    &text_string,
                    &text_style,
                    text_state,
                    GUISTR_TEXT_DIR_AUTO
                );

                Word_DisplayBookList(win_id, MMI_ZMT_WORD_MAIN_LIST_BOOK_CTRL_ID);
            }
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_WORD_MAIN_LIST_BOOK_CTRL_ID);
                uint16 cur_publish_idx = word_book_info.cur_publish_grade_idx;
                uint8 start_idx = 0;
                if(cur_publish_idx == 0){
                    start_idx = 0;
                }else{
                    start_idx = word_publish_grade_book_count[cur_publish_idx-1].count;
                }
                if(Word_GetPublishAndBookIdx(start_idx + cur_idx))
                {
                    MMI_CreateWordChapterWin();
                }
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

PUBLIC void MMI_CreateWordBookWin(void)
{
    if(MMK_IsOpenWin(MMI_WORD_BOOK_MAIN_WIN_ID)){
        MMK_CloseWin(MMI_WORD_BOOK_MAIN_WIN_ID);
    }
    MMK_CreateWin((uint32 *)MMI_WORD_BOOK_WIN_TAB, PNULL);
}

PUBLIC MMI_RESULT_E MMI_CloseWordBookWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CloseWin(MMI_WORD_BOOK_MAIN_WIN_ID);
    return result;
}

LOCAL void Word_DisplayPublishList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    uint8 j = 0;
    uint8 k = 0;
    uint8 grade_num = 0;
    uint8 cur_grade = 0;
    GUILIST_INIT_DATA_T list_init = {0};
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
    MMI_TEXT_ID_T grade_text[9] = {
        WORD_GRADE_1, WORD_GRADE_2, WORD_GRADE_3, WORD_GRADE_4, WORD_GRADE_5,
        WORD_GRADE_6, WORD_GRADE_7, WORD_GRADE_8, WORD_GRADE_9
    };

    memset(word_publish_grade_book_count, 0 ,WORD_PUBLISH_BOOK_TOTAL_MAX*sizeof(WORD_GRADE_BOOK_T));
    list_init.both_rect.v_rect = word_list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, WORD_PUBLISH_BOOK_TOTAL_MAX, FALSE);

    for(i = 0;i < word_publish_count && i < WORD_PUBLISH_BOOK_MAX; i++)
    {
        cur_grade = 0;
        for(j = 0;j < word_publish_info[i]->item_count && j < WORD_PUBLISH_BOOK_MAX;j++)
        {
            if(word_publish_info[i]->item_info[j]->grade != cur_grade)
            {
                cur_grade = word_publish_info[i]->item_info[j]->grade;
                
                item_t.item_style = GUIITEM_SYTLE_DSL_ENGLISH_BOOK;
                item_t.item_data_ptr = &item_data;
                item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
        		
                memset(name_wchar, 0, 50);
                memset(name_str, 0, 50);

                item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
                item_data.item_content[0].item_data.image_id = IMG_ZMT_CONTACT_ICON;

                itoa(k+1,tmp, 10);
                GUI_UTF8ToWstr(wstr0,10,tmp, strlen(tmp)+1);
                text_str.wstr_len = MMIAPICOM_Wstrlen(wstr0);
                text_str.wstr_ptr = wstr0;
                item_data.item_content[1].is_default =TRUE;
                item_data.item_content[1].font_color_id = MMITHEME_COLOR_LIGHT_BLUE;
                item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
                item_data.item_content[1].item_data.text_buffer = text_str;

                item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_ID;
                item_data.item_content[2].item_data.text_id = grade_text[cur_grade - 1];

                length = strlen(word_publish_info[i]->publish_name);
                GUI_UTF8ToWstr(name_str, 50, word_publish_info[i]->publish_name, length);
                text_str2.wstr_len = MMIAPICOM_Wstrlen(name_str);
                text_str2.wstr_ptr = name_str;
                item_data.item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
                item_data.item_content[3].item_data.text_buffer = text_str2;

                GUILIST_AppendItem(ctrl_id, &item_t);
                word_publish_grade_book_count[k].count++;
                k++;
                word_publish_grade_book_count[k].count = word_publish_grade_book_count[k-1].count;
            }
            else
            {
                word_publish_grade_book_count[k-1].count++;
                word_publish_grade_book_count[k].count = word_publish_grade_book_count[k-1].count;
            }
            SCI_TRACE_LOW("%s: book_count[%d].count = %d", __FUNCTION__, k-1, word_publish_grade_book_count[k-1].count);
        }
    }
    //不画分割线
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, TRUE);
    //不画高亮条
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);
    //文字显示特效
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_EFFECT_STR,TRUE);

    GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

    GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));

    GUILIST_SetCurItemIndex(ctrl_id, word_book_info.cur_publish_grade_idx);
}

LOCAL MMI_RESULT_E HandleWordMainWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                memset(&word_book_info, 0, sizeof(WORD_BOOK_INFO_T));
                Word_requestBookInfo();
            }
            break;
        case MSG_FULL_PAINT:
            {
                GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
                GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
                GUISTR_STYLE_T text_style = {0};
                MMI_STRING_T text_string = {0};

                GUI_FillRect(&lcd_dev_info, word_win_rect, GUI_RGB2RGB565(80, 162, 254));
                GUI_FillRect(&lcd_dev_info, word_title_rect, GUI_RGB2RGB565(108, 181, 255));

                text_style.align = ALIGN_HVMIDDLE;
                text_style.font = DP_FONT_22;
                text_style.font_color = MMI_WHITE_COLOR;

                MMIRES_GetText(WORD_TITLE, win_id, &text_string);
                GUISTR_DrawTextToLCDInRect(
                    (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                    &word_title_rect,
                    &word_title_rect,
                    &text_string,
                    &text_style,
                    text_state,
                    GUISTR_TEXT_DIR_AUTO
                );

                if(word_publish_count > 0)
                {
                    Word_DisplayPublishList(win_id, MMI_ZMT_WORD_MAIN_LIST_CTRL_ID);
                }
                else if (word_publish_count == 0)
                {
                    MMIRES_GetText(WORD_LOADING, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &word_win_rect,
                        &word_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
                else if(word_publish_count == -1)
                {
                    MMIRES_GetText(WORD_LOADING_FAILED, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &word_win_rect,
                        &word_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
                else if(word_publish_count == -2)
                {
                    MMIRES_GetText(WORD_NO_DATA, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &word_win_rect,
                        &word_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
            }
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_WORD_MAIN_LIST_CTRL_ID);
                word_book_info.cur_publish_grade_idx = cur_idx;
                MMI_CreateWordBookWin();
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                memset(&word_book_info, 0, sizeof(WORD_BOOK_INFO_T));
                memset(word_publish_grade_book_count, 0 ,WORD_PUBLISH_BOOK_TOTAL_MAX*sizeof(WORD_GRADE_BOOK_T));
                Word_ReleaseBookInfo();
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
LOCAL void WordChapter_AutoDisplay_Tip_Show(uint8 type)
{
    UILAYER_APPEND_BLT_T append_layer = {0};	
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    wchar text_str[35] = {0};
    char count_str[35] = {0};

    append_layer.lcd_dev_info = word_chapter_tip_layer;
    append_layer.layer_level = UILAYER_LEVEL_HIGH;
    UILAYER_AppendBltLayer(&append_layer);

    LCD_FillRoundedRect(&word_chapter_tip_layer, word_tip_rect, word_tip_rect, MMI_WHITE_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_16;
    text_style.font_color = GUI_RGB2RGB565(80, 162, 254);

    if(type==1)
    {
        sprintf(count_str,"已开启自动发音");
    }else
    {
        sprintf(count_str,"已关闭自动发音");
    }
    GUI_GBToWstr(text_str, count_str, strlen(count_str));
    text_string.wstr_ptr = text_str;
    text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&word_chapter_tip_layer,
        &word_tip_rect,
        &word_tip_rect,
        &text_string,
        &text_style,
        GUISTR_STATE_ALIGN,
        GUISTR_TEXT_DIR_AUTO
    );
}

LOCAL void WordChapter_AutoDisplay_Tip_Timeout(uint8 timer_id,uint32 param)
{
    if(0 != open_auto_play_timer)
    {
        MMK_StopTimer(open_auto_play_timer);
        open_auto_play_timer = 0;
    }
    UILAYER_RemoveBltLayer(&word_chapter_tip_layer);
}

LOCAL void WordChapter_AutoDisplay_Tip(uint8 type)
{
    if(0 != open_auto_play_timer)
    {
        MMK_StopTimer(open_auto_play_timer);
        open_auto_play_timer = 0;
    }
    WordChapter_AutoDisplay_Tip_Show(type);
    open_auto_play_timer = MMK_CreateTimerCallback(2000, WordChapter_AutoDisplay_Tip_Timeout, PNULL, FALSE);
    MMK_StartTimerCallback(open_auto_play_timer, 2000, WordChapter_AutoDisplay_Tip_Timeout, PNULL, FALSE);
}

LOCAL MMI_RESULT_E WordChapter_clickAutoPlay()
{
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	word_open_auto_play = TRUE;
	WordChapter_AutoDisplay_Tip(1);
	MMK_SendMsg(MMI_WORD_CHAPTER_WIN_ID, MSG_FULL_PAINT, PNULL);
	return result;
}
LOCAL MMI_RESULT_E WordChapter_clickDisAutoPlay()
{
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	word_open_auto_play = FALSE;
	WordChapter_AutoDisplay_Tip(2);
	MMK_SendMsg(MMI_WORD_CHAPTER_WIN_ID, MSG_FULL_PAINT, PNULL);
	return result;
}

LOCAL void WordChapter_OpenNormalWord(void)
{
    is_open_new_word = FALSE;
    MMI_CreateWordDetailWin();
}

LOCAL void WordChapter_OpenNewWord(void)
{
    is_open_new_word = TRUE;
    MMI_CreateWordDetailWin();
}

LOCAL void WordChapter_DisplayChapterList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    GUI_RECT_T list_rect = {0};
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_str = {0};
    wchar name_wchar[100] = {0};
    uint16 length = 0;
    MMI_STRING_T text_string = {0};

    list_rect = word_list_rect;
    list_rect.bottom -= 1.5*WORD_CARD_LINE_HIGHT;
    list_init.both_rect.v_rect = list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, word_chapter_count, FALSE);
    for(i = 0;i < word_chapter_count && i < WORD_CHAPTER_NUM_MAX;i++)
    {
        item_t.item_style = GUIITEM_SYTLE_DSL_CHECK;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
    		
        memset(name_wchar, 0, 100);
        length = strlen(word_chapter_info[i]->chapter_name);
        GUI_UTF8ToWstr(name_wchar, 100, word_chapter_info[i]->chapter_name, length);
        text_str.wstr_ptr = name_wchar;
        text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
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
    //不画分割线
    GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, TRUE);
    //不画高亮条
    GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

    GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

    GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));
    GUILIST_SetTextFont(ctrl_id, DP_FONT_16, MMI_WHITE_COLOR);
    GUILIST_SetCurItemIndex(ctrl_id, word_book_info.cur_chapter_idx);
}

LOCAL MMI_RESULT_E HandleWordChapterWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                GUI_FONT_ALL_T font = {0};
                GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
		
                font.font = DP_FONT_16;
                font.color = MMI_WHITE_COLOR;
        
                GUIBUTTON_SetRect(MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID, &word_title_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID, MMI_CloseWordChapterWin);
                GUIBUTTON_SetFont(MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID, &font);
                GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID,ALIGN_LVMIDDLE);

                GUIBUTTON_SetRect(MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID, &word_auto_play_rect);
                GUIBUTTON_SetVisible(MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID,TRUE,TRUE);

                GUIBUTTON_SetRect(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID, &word_left_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID, WordChapter_OpenNormalWord);
                GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID,ALIGN_HVMIDDLE);
                GUIBUTTON_SetTextId(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID, WORD_PACTISE);
                GUIBUTTON_SetVisible(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID,FALSE,FALSE);
                Word_InitButton(MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID);

                GUIBUTTON_SetRect(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID, &word_right_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID, WordChapter_OpenNewWord);
                GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID,ALIGN_HVMIDDLE);
                GUIBUTTON_SetTextId(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID, HANZI_NEW_WORD);
                GUIBUTTON_SetVisible(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID,FALSE,FALSE);
                Word_InitButton(MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID);

                GUILABEL_SetRect(MMI_ZMT_WORD_CHAPTER_LABEL_NUM_CTRL_ID, &word_dir_rect, FALSE);
                GUILABEL_SetFont(MMI_ZMT_WORD_CHAPTER_LABEL_NUM_CTRL_ID, DP_FONT_16,MMI_BLACK_COLOR);
                GUILABEL_SetAlign(MMI_ZMT_WORD_CHAPTER_LABEL_NUM_CTRL_ID, GUILABEL_ALIGN_RIGHT);

                Word_requestChapterDetailInfo(word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id);

                if (UILAYER_IsMultiLayerEnable())
                {
                    UILAYER_CREATE_T create_info = {0};
			create_info.lcd_id = MAIN_LCD_ID;
			create_info.owner_handle = win_id;
			create_info.offset_x = word_tip_rect.left;
			create_info.offset_y = word_tip_rect.top;
			create_info.width = word_tip_rect.right;
			create_info.height = word_tip_rect.bottom;
			create_info.is_bg_layer = FALSE;
			create_info.is_static_layer = FALSE;   
			UILAYER_CreateLayer(&create_info, &word_chapter_tip_layer);
                }
            }
            break;
        case MSG_FULL_PAINT:
            {
                GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
                GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
                GUISTR_STYLE_T text_style = {0};
                MMI_STRING_T text_string = {0};
                char text_str[100] = {0};
                wchar text_wchar[100] = {0};

                GUI_FillRect(&lcd_dev_info, word_win_rect, GUI_RGB2RGB565(80, 162, 254));
                GUI_FillRect(&lcd_dev_info, word_title_rect, GUI_RGB2RGB565(108, 181, 255));

                sprintf(text_str, "%s", word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_name);
                GUI_UTF8ToWstr(text_wchar, 100, text_str, strlen(text_str));
                text_string.wstr_ptr = text_wchar;
                text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                GUIBUTTON_SetText(MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID, text_string.wstr_ptr, text_string.wstr_len);              

                text_style.align = ALIGN_HVMIDDLE;
                text_style.font = DP_FONT_20;
                text_style.font_color = MMI_WHITE_COLOR;
                
                SCI_TRACE_LOW("%s: word_chapter_count = %d", __FUNCTION__, word_chapter_count);
                if(word_chapter_count == 0)
                {
                    MMIRES_GetText(WORD_LOADING, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &word_win_rect,
                        &word_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
                else if(word_chapter_count == -1)
                {
                    MMIRES_GetText(WORD_LOADING_FAILED, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &word_win_rect,
                        &word_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
                else if(word_chapter_count == -2)
                {
                    MMIRES_GetText(WORD_NO_DATA, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &word_win_rect,
                        &word_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
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
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            { 
                uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_WORD_CHAPTER_LIST_CTRL_ID);
                word_book_info.cur_chapter_idx = cur_idx;
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                word_chapter_count = 0;
                Word_ReleaseChapterDetailInfo();
                word_book_info.cur_chapter_idx = 0;
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
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_WORD_CHAPTER_LABEL_NUM_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMI_CreateWordChapterWin(void)
{
    if(MMK_IsOpenWin(MMI_WORD_CHAPTER_WIN_ID)){
        MMK_CloseWin(MMI_WORD_CHAPTER_WIN_ID);
    }
    MMK_CreateWin((uint32 *)MMI_WORD_CHAPTER_WIN_TAB, PNULL);
}

LOCAL MMI_RESULT_E MMI_CloseWordChapterWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CloseWin(MMI_WORD_CHAPTER_WIN_ID);
    return result;
}

/////////////////////////////////////////////////////////////////////////////
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

LOCAL void Word_StopPlayMp3Data(void)
{
    if(word_player_handle != 0)
    {
        MMISRVAUD_Stop(word_player_handle);
        MMISRVMGR_Free(word_player_handle);
        word_player_handle = 0;
    }
}

LOCAL void Word_ChatPlayMp3Data(uint8 *data,uint32 data_len)
{
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    BOOLEAN result = FALSE;

    Word_StopPlayMp3Data();

    req.is_auto_free = TRUE;
    req.notify = Word_ChatPlayMp3DataNotify;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.info.type = MMISRVAUD_TYPE_RING_BUF;
    audio_srv.info.ring_buf.fmt = MMISRVAUD_RING_FMT_MP3;
    audio_srv.info.ring_buf.data = data;
    audio_srv.info.ring_buf.data_len = data_len;
    audio_srv.volume=MMIAPISET_GetMultimVolume();
    
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
    WordDetail_ShowTip();
}

LOCAL void WordDetail_DisplayTip(uint type)
{
    if(0 != word_tip_timer)
    {
        MMK_StopTimer(word_tip_timer);
        word_tip_timer = 0;
    }
    word_is_display_tip = type;
    WordDetail_ShowTip();
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
            WordDetail_DisplayTip(2);
        }
        else if(new_word_detail_info[word_detail_cur_idx]->audio_len == -2)
        {
            //加载音频失败
            WordDetail_DisplayTip(3);
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
                SCI_TRACE_LOW("%s: [%d]audio_uri = %s", __FUNCTION__, word_detail_cur_idx, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_uri);    
                MMIZDT_HTTP_AppSend(TRUE, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_uri, PNULL, 0, 1000, 0, 0, 6000, 0, 0, Word_ParseMp3Response);
            }
        }
    }
    else if(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_len == -1)
    {
        //没有音频uri
        WordDetail_DisplayTip(2);
    }
    else if(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->audio_len == -2)
    {
        //加载音频失败
        WordDetail_DisplayTip(3);
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
    word_book_info.cur_chapter_idx++;
    if(word_book_info.cur_chapter_idx < word_chapter_count)
    {
        Word_WriteUnmasterChapterWord(
            word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_id,
            word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name,
            chapter_unmaster_count
        );
        chapter_unmaster_count = 0;
        word_detail_cur_idx = 0;
        MMK_SendMsg(MMI_WORD_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void WordDetail_DeleteNewWord(void)
{
    Word_DeleteOneNewWord(word_detail_cur_idx, word_detail_count);
    new_word_haved_delete = TRUE;
    WordDetail_DisplayTip(4);
}

LOCAL void WordDetail_ShowTip(void)
{
    if(word_is_display_tip != 0)
    {
        UILAYER_APPEND_BLT_T append_layer = {0};
        GUISTR_STYLE_T text_style = {0};
        MMI_STRING_T text_string = {0};
        wchar text_str[35] = {0};
        char count_str[35] = {0};
        GUI_RECT_T tip_rect = {0};

        append_layer.lcd_dev_info = word_detail_tip_layer;
        append_layer.layer_level = UILAYER_LEVEL_HIGH;
        UILAYER_AppendBltLayer(&append_layer);

        LCD_FillRoundedRect(&word_detail_tip_layer, word_msg_rect, word_msg_rect, MMI_WHITE_COLOR);

        text_style.align = ALIGN_HVMIDDLE;
        text_style.font = DP_FONT_18;
        text_style.font_color = GUI_RGB2RGB565(80, 162, 254);

        if(word_is_display_tip == 1)
        {
            sprintf(count_str,"正在加载，请稍后");
        }else if(word_is_display_tip == 2)
        {
            sprintf(count_str,"暂无音频");
        }else if(word_is_display_tip == 3)
        {
            sprintf(count_str,"音频加载失败，请重试");
        }else if(word_is_display_tip == 4)
        {
            sprintf(count_str,"正在删除，请稍等");
        }
        GUI_GBToWstr(text_str, count_str, strlen(count_str));
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&word_detail_tip_layer,
            &word_msg_rect,
            &word_msg_rect,
            &text_string,
            &text_style,
            GUISTR_STATE_ALIGN,
            GUISTR_TEXT_DIR_AUTO
        );
    }
    else
    {
        UILAYER_RemoveBltLayer(&word_detail_tip_layer);
    }
}

LOCAL void WordDetail_DisplayDtailInfo(MMI_WIN_ID_T win_id)
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
    GUI_RECT_T word_rect = {0};
    GUI_RECT_T pinyin_rect = {0};
    GUI_RECT_T audio_rect = {0};
    GUI_RECT_T remark_rect = {0};
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
    word_rect = word_word_rect;
    width_rect = word_rect.left + text_width_piex + 5;
    word_rect.right = width_rect;
    GUILABEL_SetRect(MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID, &word_rect, FALSE);
    GUILABEL_SetText(MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID, &text_word, FALSE);

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
    if(width_rect < MMI_MAINSCREEN_WIDTH / 2 || 
        word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->phonetic == NULL)
    {
        text_width_piex = GUI_CalculateStringPiexlNum(text_pinyin.wstr_ptr, text_pinyin.wstr_len, DP_FONT_20, 1);
        pinyin_rect = word_rect;
        pinyin_rect.left = word_rect.right;
        pinyin_rect.right = pinyin_rect.left + text_width_piex;
        GUILABEL_SetRect(MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID, &pinyin_rect, FALSE);
        audio_rect = pinyin_rect;
    }else{
        GUILABEL_SetRect(MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID, &word_pinyin_rect, FALSE);
        audio_rect = word_pinyin_rect;
    }
    GUILABEL_SetText(MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID, &text_pinyin, FALSE);

    if(is_phonetic_null){
        audio_rect = word_rect;
    }
    audio_rect.left = 5*WORD_CARD_LINE_WIDTH;
    audio_rect.right = 6*WORD_CARD_LINE_WIDTH;
    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID,TRUE,FALSE);
    GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID, &audio_rect);
    LCD_DrawHLine(&lcd_dev_info, word_pinyin_rect.left, audio_rect.bottom, word_pinyin_rect.right, MMI_WHITE_COLOR);
    word_Hor_line_rect = word_pinyin_rect;
    word_Hor_line_rect.top = audio_rect.top;
    word_Hor_line_rect.bottom = audio_rect.bottom;

    if(is_open_new_word){
        size = strlen(new_word_detail_info[word_detail_cur_idx]->translation);
        GUI_UTF8ToWstr(wstr_remark, 2048, new_word_detail_info[word_detail_cur_idx]->translation, size);
    }else{
        size = strlen(word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->translation);
        GUI_UTF8ToWstr(wstr_remark, 2048, word_chapter_info[word_book_info.cur_chapter_idx]->detail[word_detail_cur_idx]->translation, size);
    }
    remark_rect = word_text_rect;
    remark_rect.top = audio_rect.bottom + 1;
    GUITEXT_SetRect(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, &remark_rect);
    text_remark.wstr_ptr = wstr_remark;
    text_remark.wstr_len = MMIAPICOM_Wstrlen(text_remark.wstr_ptr);
    GUITEXT_SetAlign(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, ALIGN_LEFT);
    GUITEXT_SetString(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, text_remark.wstr_ptr,text_remark.wstr_len, TRUE);
    GUITEXT_SetResetTopDisplay(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, 0);
}

LOCAL MMI_RESULT_E HandleWordDetailWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                GUI_FONT_ALL_T font = {0};
                GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
                GUI_FONT_T text_font = DP_FONT_20;
                GUI_COLOR_T text_color = MMI_WHITE_COLOR;
                GUI_BG_T bg = {0};
		
                font.font = DP_FONT_18;
                font.color = MMI_WHITE_COLOR;
        
                GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID, &word_title_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID, MMI_CloseWordDetailWin);
                GUIBUTTON_SetFont(MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID, &font);
                GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID,ALIGN_LVMIDDLE);

                GUITEXT_SetRect(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, &word_text_rect);
                GUITEXT_SetFont(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, &text_font,&text_color);
                GUITEXT_SetAlign(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, ALIGN_HVMIDDLE);
                GUITEXT_IsDisplayPrg(FALSE, MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID);
                GUITEXT_SetHandleTpMsg(FALSE, MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID);
                GUITEXT_SetClipboardEnabled(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID,FALSE);
                GUITEXT_IsSlide(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID,FALSE);
                bg.bg_type = GUI_BG_COLOR;
                bg.color = GUI_RGB2RGB565(80, 162, 254);
                GUITEXT_SetBg(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, &bg);

                GUIBUTTON_SetRect(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, &word_msg_rect);
                Word_InitButton(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID);
                GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, FALSE, FALSE);

                font.font = DP_FONT_16;
                if(is_open_new_word){
                    GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, &word_pre_rect);
                    GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, &word_next_rect);
                    GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID, &word_del_rect);                    
                }else{
                    GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, &word_left_rect);
                    GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, &word_right_rect);
                                  
                    GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,ALIGN_HVMIDDLE);
                    GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID,ALIGN_HVMIDDLE);
                    
                    Word_InitButton(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID);
                    Word_InitButton(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID);
                }
                GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID,FALSE,FALSE);
                GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
                GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID,FALSE,FALSE);

                GUILABEL_SetRect(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, &word_dir_rect, FALSE);
                GUILABEL_SetFont(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, DP_FONT_16,MMI_WHITE_COLOR);
                GUILABEL_SetAlign(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, GUILABEL_ALIGN_MIDDLE);

                GUILABEL_SetFont(MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID, DP_FONT_20,MMI_WHITE_COLOR);
                GUILABEL_SetFont(MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID, DP_FONT_20,MMI_WHITE_COLOR);

                new_word_haved_delete = FALSE;
                word_detail_count = 0;
                memset(chapter_unmaster_idx, 0, sizeof(chapter_unmaster_idx));
                if(is_open_new_word){
                    Word_RequestNewWord(
                        word_publish_info[word_book_info.cur_publish_idx]->item_info[word_book_info.cur_book_idx]->book_id,
                        word_chapter_info[word_book_info.cur_chapter_idx]->chapter_id
                    );
                }else{
                    if(word_open_auto_play && word_chapter_info[word_book_info.cur_chapter_idx]->detail_count > 0)
                    {
                        WordDetail_PlayPinyinAudio();
                    }
                }
                if (UILAYER_IsMultiLayerEnable())
                {
                    UILAYER_CREATE_T create_info = {0};
                    create_info.lcd_id = MAIN_LCD_ID;
                    create_info.owner_handle = win_id;
                    create_info.offset_x = word_msg_rect.left;
                    create_info.offset_y = word_msg_rect.top;
                    create_info.width = word_msg_rect.right - word_msg_rect.left;
                    create_info.height = word_msg_rect.bottom;
                    create_info.is_bg_layer = FALSE;
                    create_info.is_static_layer = FALSE;
                    UILAYER_CreateLayer(&create_info, &word_detail_tip_layer);
                }
            }
            break;
        case MSG_FULL_PAINT:
            {
                GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
                GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
                GUISTR_STYLE_T text_style = {0};
                MMI_STRING_T text_string = {0};
                char text_str[100] = {0};
                wchar text_wchar[100] = {0};
                BOOLEAN is_end = FALSE;

                GUI_FillRect(&lcd_dev_info, word_win_rect, GUI_RGB2RGB565(80, 162, 254));
                GUI_FillRect(&lcd_dev_info, word_title_rect, GUI_RGB2RGB565(108, 181, 255));

                GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, FALSE, FALSE);
                if(is_open_new_word){
                    MMIRES_GetText(WORD_EXERCISE, win_id, &text_string);
                }else{
                    strcpy(text_str, word_chapter_info[word_book_info.cur_chapter_idx]->chapter_name);
                    GUI_UTF8ToWstr(text_wchar, 100, text_str, strlen(text_str));
                    text_string.wstr_ptr = text_wchar;
                    text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);                  
                }
                GUIBUTTON_SetText(MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID, text_string.wstr_ptr, text_string.wstr_len);

                text_style.align = ALIGN_HVMIDDLE;
                text_style.font = DP_FONT_20;
                text_style.font_color = MMI_WHITE_COLOR;

                if(!is_open_new_word){
                    word_detail_count = word_chapter_info[word_book_info.cur_chapter_idx]->detail_count;
                }
                //SCI_TRACE_LOW("%s: word_detail_count = %d", __FUNCTION__, word_detail_count);
                if(word_detail_count == 0)
                {
                    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID, FALSE, FALSE);
                    MMIRES_GetText(WORD_LOADING, win_id, &text_string); 
                    GUITEXT_SetString(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, text_string.wstr_ptr,text_string.wstr_len, TRUE);
                }
                else if(word_detail_count == -1)
                {
                    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID, FALSE, FALSE);
                    MMIRES_GetText(WORD_LOADING_FAILED, win_id, &text_string);
                    GUITEXT_SetString(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, text_string.wstr_ptr,text_string.wstr_len, TRUE);
                }
                else if(word_detail_count == -2)
                {
                    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID, FALSE, FALSE);
                    MMIRES_GetText(WORD_NO_DATA, win_id, &text_string);
                    GUITEXT_SetString(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, text_string.wstr_ptr,text_string.wstr_len, TRUE);
                }
                else if(word_detail_count == -3)
                {
                    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID, FALSE, FALSE);
                    MMIRES_GetText(NEW_WORD_BOOK_NO_DATA, win_id, &text_string);
                    GUITEXT_SetString(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID, text_string.wstr_ptr,text_string.wstr_len, TRUE);
                }
                else
                {
                    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,TRUE,FALSE);
                    GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID,TRUE,FALSE);
                    if(word_detail_cur_idx < word_detail_count)
                    {
                        GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID, WordDetail_PlayPinyinAudio);
                        if(is_open_new_word)
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
                        }
                        else
                        {
                            GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, HANZI_MASTERED);
                            GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, HANZI_UNMASTERED);
                            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, WordDetail_LeftDetail);
                            GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WordDetail_RightDetail);
                        }     
                        Word_SetDiretionText(MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID, word_detail_cur_idx, word_detail_count);    
                        WordDetail_DisplayDtailInfo(win_id);
                    }
                    else
                    {
                         if(!is_open_new_word)
                         {
                            if(word_book_info.cur_chapter_idx + 1 < word_chapter_count)
                            {
                                GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, TRUE, TRUE);
                                GUIBUTTON_SetTextId(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, WORD_FINISH);
                                GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, HANZI_BACK_CHAPTER);
                                GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, HANZI_NEXT_CHAPTER);
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID, MMI_CloseWordDetailWin);
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WordDetail_NextChapterInfo);
                            }
                            else
                            {
                                GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, TRUE, TRUE);
                                GUIBUTTON_SetTextId(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, WORD_BOOK_FINISH);
                                GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WORD_BACK_CH);            
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, MMI_CloseWordDetailWin);
                            }
                         }
                         else
                         {
                                GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, TRUE, TRUE);
                                GUIBUTTON_SetTextId(MMI_ZMT_WORD_MSG_TIPS_CTRL_ID, NEW_WORD_BOOK_FINISH);
                                GUIBUTTON_SetVisible(MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetRect(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, &word_right_rect);
                                GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID,ALIGN_HVMIDDLE);
                                Word_InitButton(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID);
                                GUIBUTTON_SetTextId(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, WORD_BACK_CH);            
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID, MMI_CloseWordDetailWin);
                         }
                         LCD_DrawHLine(&lcd_dev_info, word_Hor_line_rect.left, word_Hor_line_rect.bottom, word_Hor_line_rect.right, MMI_WHITE_COLOR);
                    }
                }
            }
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T point = {0};
                int16 tp_offset_x = 0;
                int16 tp_offset_y = 0;
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
            break;
        case MSG_TP_PRESS_DOWN:
            {
                main_tp_down_x = MMK_GET_TP_X(param);
                main_tp_down_y = MMK_GET_TP_Y(param);
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
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
}

WINDOW_TABLE(MMI_WORD_DETAIL_WIN_TAB) = {
    WIN_ID(MMI_WORD_DETAIL_WIN_ID),
    WIN_FUNC((uint32)HandleWordDetailWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID),//单元名称
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID),//导航页
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID),//汉字
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID),//拼音
    CREATE_BUTTON_CTRL(ZMT_LISTEN_VOLUME, MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID),//音频图标
    CREATE_TEXT_CTRL(MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID),//释义
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID),//已掌握/上一个
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID),//未掌握/下一个
    CREATE_BUTTON_CTRL(WORD_DELETE_ICON, MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID),//删除
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_MSG_TIPS_CTRL_ID),//已学完tip
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMI_CreateWordDetailWin(void)
{
    MMK_CreateWin((uint32 *)MMI_WORD_DETAIL_WIN_TAB, PNULL);
}

LOCAL MMI_RESULT_E MMI_CloseWordDetailWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CloseWin(MMI_WORD_DETAIL_WIN_ID);
    return result;
}

