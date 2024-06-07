#include "std_header.h"
#include <stdlib.h>
#include "cjson.h"
#include "dal_time.h"
#include "zmt_hanzi_id.h"
#include "zmt_hanzi_main.h"
#include "zmt_hanzi_text.h"
#include "zmt_hanzi_image.h"
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
#ifdef WORD_CARD_SUPPORT
#include "zmt_word_text.h"
#include "zmt_word_image.h"
#endif


LOCAL GUI_RECT_T hanzi_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};//窗口
LOCAL GUI_RECT_T hanzi_title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, HANZI_CARD_LINE_HIGHT};//顶部
LOCAL GUI_RECT_T hanzi_auto_play_rect = {4*HANZI_CARD_LINE_WIDTH, 0, MMI_MAINSCREEN_WIDTH, HANZI_CARD_LINE_HIGHT};//自动播放图标
LOCAL GUI_RECT_T hanzi_list_rect = {0, HANZI_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT-5};//列表
LOCAL GUI_RECT_T hanzi_dir_rect = {5*HANZI_CARD_LINE_WIDTH, 0, MMI_MAINSCREEN_WIDTH, HANZI_CARD_LINE_HIGHT};
LOCAL GUI_RECT_T hanzi_word_rect = {5, HANZI_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 2*HANZI_CARD_LINE_HIGHT};//汉字
LOCAL GUI_RECT_T hanzi_pinyin_rect = {5, 1.8*HANZI_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-5, 2.8*HANZI_CARD_LINE_HIGHT};//音标
LOCAL GUI_RECT_T hanzi_text_rect = {5,2.8*HANZI_CARD_LINE_HIGHT+1,MMI_MAINSCREEN_WIDTH-5,MMI_MAINSCREEN_HEIGHT-HANZI_CARD_LINE_HIGHT-5};//释义
LOCAL GUI_RECT_T hanzi_msg_rect = {HANZI_CARD_LINE_WIDTH, 3*HANZI_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-HANZI_CARD_LINE_WIDTH, 5*HANZI_CARD_LINE_HIGHT};//界面的信息提示
LOCAL GUI_RECT_T hanzi_left_rect = {10, MMI_MAINSCREEN_HEIGHT-HANZI_CARD_LINE_HIGHT, 3*HANZI_CARD_LINE_WIDTH-10, MMI_MAINSCREEN_HEIGHT-2};//汉字学习/掌握
LOCAL GUI_RECT_T hanzi_right_rect = {3*HANZI_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-HANZI_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-10, MMI_MAINSCREEN_HEIGHT-2};//生词本/未掌握
LOCAL GUI_RECT_T hanzi_pre_rect = {0.5*HANZI_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-HANZI_CARD_LINE_HIGHT, 1.5*HANZI_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-上一个
LOCAL GUI_RECT_T hanzi_del_rect = {2.5*HANZI_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-HANZI_CARD_LINE_HIGHT, 3.5*HANZI_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-删除
LOCAL GUI_RECT_T hanzi_next_rect = {4.5*HANZI_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-HANZI_CARD_LINE_HIGHT, 5.5*HANZI_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-下一个
LOCAL GUI_RECT_T hanzi_tip_rect = {HANZI_CARD_LINE_WIDTH,4*HANZI_CARD_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-HANZI_CARD_LINE_WIDTH,6*HANZI_CARD_LINE_HIGHT};//弹框提示

LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;

extern HANZI_CONTENT_INFO_T * hanzi_content_info[HANZI_CONTENT_CHAPTER_MAX];
extern int16 hanzi_chapter_count;
extern int16 hanzi_chapter_children_count[20];
extern HANZI_BOOK_HANZI_INFO * hanzi_detail_info[100];
extern int16 hanzi_detail_count;
extern int16 hanzi_detail_cur_idx;

HANZI_BOOK_INFO_T hanzi_book_info = {0};
BOOLEAN is_open_auto_play = TRUE;
LOCAL uint8 open_auto_play_timer = 0;
LOCAL BOOLEAN is_open_new_hanzi = FALSE;
LOCAL GUI_LCD_DEV_INFO hanzi_chapter_tip_layer = {0};
LOCAL GUI_LCD_DEV_INFO hanzi_detail_tip_layer = {0};
LOCAL int8 hanzi_is_display_tip = 0;
LOCAL uint8 hanzi_tip_timer = 0;
LOCAL MMISRV_HANDLE_T hanzi_player_handle = PNULL;
LOCAL uint8 cur_chapter_total_count = 0;
LOCAL uint8 cur_chapter_unmaster_count = 0;
uint8 cur_new_word_page_idx = 0;
int cur_chapter_unmaster_idx[HANZI_CHAPTER_WORD_MAX] = {0};

LOCAL MMI_RESULT_E MMI_CloseHanziWin(void);
LOCAL MMI_RESULT_E MMI_CloseHanziChapterWin(void);

LOCAL ZMT_HANZI_GRADE_NAME hanzi_grade_name[HANZI_BOOK_TOTAL] = {
    "一年级上册","一年级下册","二年级上册","二年级下册",
    "三年级上册","三年级下册","四年级上册","四年级下册","五年级上册",
    "五年级下册","六年级上册", "六年级下册","七年级上册","七年级下册",
    "八年级上册","八年级下册","九年级上册","九年级下册"
};

LOCAL void Hanzi_InitButton(MMI_CTRL_ID_T ctrl_id)
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


LOCAL void Hanzi_SetDiretionText(MMI_CTRL_ID_T ctrl_id, int cur_idx, int total)
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

LOCAL void Hanzi_DisplayBookList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 index = 0;
    uint8 num = 0;
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_str = {0};
    wchar name_wchar[50] = {0};
    char name_str[50] = {0};
    uint8 length = 0;
    MMI_STRING_T text_string = {0};
    char tmp[10]={0};
    uint16 wstr0[10] = {0};

    list_init.both_rect.v_rect = hanzi_list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, HANZI_BOOK_TOTAL, FALSE);

    for(index = 0;index < HANZI_BOOK_TOTAL; index++)
    {
        length = strlen(hanzi_grade_name[index].name);

    #ifdef WORD_CARD_SUPPORT
        item_t.item_style = GUIITEM_SYTLE_DSL_ENGLISH_BOOK;
    #else
        item_t.item_style = GUIITEM_DSL_HANZI_BOOK;
    #endif
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
		
        memset(name_wchar, 0, 50);
        memset(name_str, 0, 50);

    #ifdef WORD_CARD_SUPPORT
        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = IMG_ZMT_CONTACT_ICON;

        itoa(index+1,tmp, 10);
        GUI_UTF8ToWstr(wstr0,10,tmp, strlen(tmp)+1);
        text_str.wstr_len = MMIAPICOM_Wstrlen(wstr0);
        text_str.wstr_ptr = wstr0;
        item_data.item_content[1].is_default =TRUE;
        item_data.item_content[1].font_color_id = MMITHEME_COLOR_LIGHT_BLUE;
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[1].item_data.text_buffer = text_str;

        GUI_GBToWstr(name_wchar, hanzi_grade_name[index].name, length);
        text_str.wstr_len = MMIAPICOM_Wstrlen(name_wchar);
        text_str.wstr_ptr = name_wchar;
        item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[2].item_data.text_buffer = text_str;

        item_data.item_content[3].item_data_type = GUIITEM_DATA_TEXT_ID;
        item_data.item_content[3].item_data.text_id = HANZI_RENJIAO_EDITION;
    #else
        //book name
        GUI_GBToWstr(name_wchar, hanzi_grade_name[index].name, length);
        text_str.wstr_ptr = name_wchar;
        text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[0].item_data.text_buffer = text_str;
    #endif

        //不画分割线
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, TRUE);
        //不画高亮条
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);
        //文字显示特效
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_EFFECT_STR,TRUE);

        GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

        GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
}

LOCAL MMI_RESULT_E HandleHanziWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                GUI_FONT_ALL_T font = {0};
		
                font.font = DP_FONT_22;
                font.color = MMI_WHITE_COLOR;
        
                GUIBUTTON_SetRect(MMI_ZMT_HANZI_MAIN_LABEL_BACK_CTRL_ID, &hanzi_title_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_MAIN_LABEL_BACK_CTRL_ID, MMI_CloseHanziWin);
                GUIBUTTON_SetFont(MMI_ZMT_HANZI_MAIN_LABEL_BACK_CTRL_ID, &font);
                GUIBUTTON_SetTextAlign(MMI_ZMT_HANZI_MAIN_LABEL_BACK_CTRL_ID,ALIGN_HVMIDDLE);

                GUILABEL_SetRect(MMI_ZMT_HANZI_MAIN_LABEL_NUM_CTRL_ID, &hanzi_dir_rect, FALSE);
                GUILABEL_SetFont(MMI_ZMT_HANZI_MAIN_LABEL_NUM_CTRL_ID, DP_FONT_16,MMI_BLACK_COLOR);
                GUILABEL_SetAlign(MMI_ZMT_HANZI_MAIN_LABEL_NUM_CTRL_ID, GUILABEL_ALIGN_RIGHT);

                memset(&hanzi_book_info, 0, sizeof(HANZI_BOOK_INFO_T));
            }
            break;
        case MSG_FULL_PAINT:
            {
                GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
                GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
                GUISTR_STYLE_T text_style = {0};
                MMI_STRING_T text_string = {0};

                GUI_FillRect(&lcd_dev_info, hanzi_win_rect, GUI_RGB2RGB565(80, 162, 254));
                GUI_FillRect(&lcd_dev_info, hanzi_title_rect, GUI_RGB2RGB565(108, 181, 255));

                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_MAIN_LABEL_BACK_CTRL_ID, HANZI_CARD);              

                text_style.align = ALIGN_HVMIDDLE;
                text_style.font = DP_FONT_20;
                text_style.font_color = MMI_WHITE_COLOR;

                Hanzi_DisplayBookList(win_id, MMI_ZMT_HANZI_MAIN_LIST_CTRL_ID);
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
                uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_HANZI_MAIN_LIST_CTRL_ID);
                hanzi_book_info.cur_book_idx = cur_idx;
                SCI_TRACE_LOW("%s: hanzi_book_info.cur_book_idx = %d", __FUNCTION__, hanzi_book_info.cur_book_idx);
                MMI_CreateHanziChapterWin();
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                memset(&hanzi_book_info, 0, sizeof(HANZI_BOOK_INFO_T));
            }
            break;
         default:
            recode = MMI_RESULT_FALSE;
            break;
    }
      return recode;
}

WINDOW_TABLE(MMI_HANZI_WIN_TAB) = {
    WIN_ID(MMI_HANZI_MAIN_WIN_ID),
    WIN_FUNC((uint32)HandleHanziWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_MAIN_LABEL_BACK_CTRL_ID),
    //CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMI_ZMT_HANZI_MAIN_LIST_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_HANZI_MAIN_LABEL_NUM_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMI_CreateHanziWin(void)
{
    MMK_CreateWin((uint32 *)MMI_HANZI_WIN_TAB, PNULL);
}

PUBLIC MMI_RESULT_E MMI_CloseHanziWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CloseWin(MMI_HANZI_MAIN_WIN_ID);
    return result;
}

////////////////////////////////////////////////////
LOCAL void Hanzi_AutoDisplay_Tip_Show(uint8 type)
{
    UILAYER_APPEND_BLT_T append_layer = {0};	
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    wchar text_str[35] = {0};
    char count_str[35] = {0};

    append_layer.lcd_dev_info = hanzi_chapter_tip_layer;
    append_layer.layer_level = UILAYER_LEVEL_HIGH;
    UILAYER_AppendBltLayer(&append_layer);

    LCD_FillRoundedRect(&hanzi_chapter_tip_layer, hanzi_tip_rect, hanzi_tip_rect, MMI_WHITE_COLOR);

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
        (const GUI_LCD_DEV_INFO *)&hanzi_chapter_tip_layer,
        &hanzi_tip_rect,
        &hanzi_tip_rect,
        &text_string,
        &text_style,
        GUISTR_STATE_ALIGN,
        GUISTR_TEXT_DIR_AUTO
    );
}

LOCAL void Hanzi_AutoDisplay_Tip_Timeout(uint8 timer_id,uint32 param)
{
    if(0 != open_auto_play_timer)
    {
        MMK_StopTimer(open_auto_play_timer);
        open_auto_play_timer = 0;
    }
    UILAYER_RemoveBltLayer(&hanzi_chapter_tip_layer);
}

LOCAL void Hanzi_AutoDisplay_Tip(uint8 type)
{
    if(0 != open_auto_play_timer)
    {
        MMK_StopTimer(open_auto_play_timer);
        open_auto_play_timer = 0;
    }
    Hanzi_AutoDisplay_Tip_Show(type);
    open_auto_play_timer = MMK_CreateTimerCallback(2000, Hanzi_AutoDisplay_Tip_Timeout, PNULL, FALSE);
    MMK_StartTimerCallback(open_auto_play_timer, 2000, Hanzi_AutoDisplay_Tip_Timeout, PNULL, FALSE);
    
}

LOCAL MMI_RESULT_E Hanzi_clickAutoPlay()
{
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	is_open_auto_play = TRUE;
	Hanzi_AutoDisplay_Tip(1);
	MMK_SendMsg(MMI_HANZI_CHAPTER_WIN_ID, MSG_FULL_PAINT, PNULL);
	return result;
}
LOCAL MMI_RESULT_E Hanzi_clickDisAutoPlay()
{
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	is_open_auto_play = FALSE;
	Hanzi_AutoDisplay_Tip(2);
	MMK_SendMsg(MMI_HANZI_CHAPTER_WIN_ID, MSG_FULL_PAINT, PNULL);
	return result;
}

LOCAL void Hanzi_OpenNormalHanzi(void)
{
    is_open_new_hanzi = FALSE;
    MMI_CreateHanziDetailWin();
}

LOCAL void Hanzi_OpenNewHanzi(void)
{
    is_open_new_hanzi = TRUE;
    cur_new_word_page_idx = 0;
    MMI_CreateHanziDetailWin();
}

LOCAL void Hanzi_DisplayChapterList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i,j = 0;
    uint8 index = 0;
    uint8 num = 0;
    uint16 total_count = 0;
    GUI_RECT_T list_rect = {0};
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_str = {0};
    wchar name_wchar[100] = {0};
    char name_str[100] = {0};
    uint16 length = 0;
    MMI_STRING_T text_string = {0};

    list_rect = hanzi_list_rect;
    list_rect.bottom -= HANZI_CARD_LINE_HIGHT;
    list_init.both_rect.v_rect = list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    for(i = 0;i < hanzi_chapter_count;i++){
        total_count += hanzi_chapter_children_count[i];
    }
    cur_chapter_total_count = total_count;
    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, total_count, FALSE);

    for(i = 0;i < hanzi_chapter_count;i++)
    {
        for(j = 0; j < hanzi_chapter_children_count[i]; j++)
        {
            length = strlen(hanzi_content_info[i]->chapter[j]->chapter_name);
    		
            item_t.item_style = GUIITEM_SYTLE_DSL_CHECK;
            item_t.item_data_ptr = &item_data;
            item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
    		
            memset(name_wchar, 0, 100);
            memset(name_str, 0, 100);

            GUI_UTF8ToWstr(name_wchar, 100, hanzi_content_info[i]->chapter[j]->chapter_name, length);
            text_str.wstr_ptr = name_wchar;
            text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
            item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
            item_data.item_content[0].item_data.text_buffer = text_str;
        
            item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
            if(hanzi_book_info.cur_section_idx == i && hanzi_book_info.cur_section_children_idx == j){
                item_data.item_content[1].item_data.image_id = IMG_ZMT_SELECTED;
            }else{
                item_data.item_content[1].item_data.image_id = IMG_ZMT_UNSELECTED;
            }

            //不画分割线
            GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, TRUE);
            //不画高亮条
            GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

            GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

            GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));
            GUILIST_SetTextFont(ctrl_id, DP_FONT_16, MMI_WHITE_COLOR);

            GUILIST_AppendItem(ctrl_id, &item_t);
        }
    }
}

LOCAL MMI_RESULT_E HandleHanziChapterWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                GUI_FONT_ALL_T font = {0};
                GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
		
                font.font = DP_FONT_18;
                font.color = MMI_WHITE_COLOR;
        
                GUIBUTTON_SetRect(MMI_ZMT_HANZI_CHAPTER_LABEL_BACK_CTRL_ID, &hanzi_title_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_CHAPTER_LABEL_BACK_CTRL_ID, MMI_CloseHanziWin);
                GUIBUTTON_SetFont(MMI_ZMT_HANZI_CHAPTER_LABEL_BACK_CTRL_ID, &font);
                GUIBUTTON_SetTextAlign(MMI_ZMT_HANZI_CHAPTER_LABEL_BACK_CTRL_ID,ALIGN_LVMIDDLE);

                GUIBUTTON_SetRect(MMI_ZMT_HANZI_CHAPTER_AUTO_PLAY_CTRL_ID, &hanzi_auto_play_rect);
                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_CHAPTER_AUTO_PLAY_CTRL_ID,TRUE,TRUE);

                GUIBUTTON_SetRect(MMI_ZMT_HANZI_CHAPTER_LEFT_CTRL_ID, &hanzi_left_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_CHAPTER_LEFT_CTRL_ID, Hanzi_OpenNormalHanzi);
                GUIBUTTON_SetTextAlign(MMI_ZMT_HANZI_CHAPTER_LEFT_CTRL_ID,ALIGN_HVMIDDLE);
                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_CHAPTER_LEFT_CTRL_ID, HANZI_PACTISE);
                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_CHAPTER_LEFT_CTRL_ID,FALSE,FALSE);
                Hanzi_InitButton(MMI_ZMT_HANZI_CHAPTER_LEFT_CTRL_ID);

                GUIBUTTON_SetRect(MMI_ZMT_HANZI_CHAPTER_RIGHT_CTRL_ID, &hanzi_right_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_CHAPTER_RIGHT_CTRL_ID, Hanzi_OpenNewHanzi);
                GUIBUTTON_SetTextAlign(MMI_ZMT_HANZI_CHAPTER_RIGHT_CTRL_ID,ALIGN_HVMIDDLE);
                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_CHAPTER_RIGHT_CTRL_ID, HANZI_NEW_WORD);
                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_CHAPTER_RIGHT_CTRL_ID,FALSE,FALSE);
                Hanzi_InitButton(MMI_ZMT_HANZI_CHAPTER_RIGHT_CTRL_ID);

                GUILABEL_SetRect(MMI_ZMT_HANZI_CHAPTER_LABEL_NUM_CTRL_ID, &hanzi_dir_rect, FALSE);
                GUILABEL_SetFont(MMI_ZMT_HANZI_CHAPTER_LABEL_NUM_CTRL_ID, DP_FONT_16,MMI_BLACK_COLOR);
                GUILABEL_SetAlign(MMI_ZMT_HANZI_CHAPTER_LABEL_NUM_CTRL_ID, GUILABEL_ALIGN_RIGHT);

                Hanzi_requestChapterInfo(hanzi_book_info.cur_book_idx+1);

                if (UILAYER_IsMultiLayerEnable())
                {
                    UILAYER_CREATE_T create_info = {0};
			create_info.lcd_id = MAIN_LCD_ID;
			create_info.owner_handle = win_id;
			create_info.offset_x = hanzi_tip_rect.left;
			create_info.offset_y = hanzi_tip_rect.top;
			create_info.width = hanzi_tip_rect.right;
			create_info.height = hanzi_tip_rect.bottom;
			create_info.is_bg_layer = FALSE;
			create_info.is_static_layer = FALSE;   
			UILAYER_CreateLayer(&create_info, &hanzi_chapter_tip_layer);
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

                GUI_FillRect(&lcd_dev_info, hanzi_win_rect, GUI_RGB2RGB565(80, 162, 254));
                GUI_FillRect(&lcd_dev_info, hanzi_title_rect, GUI_RGB2RGB565(108, 181, 255));

                sprintf(text_str, "%s", hanzi_grade_name[hanzi_book_info.cur_book_idx].name);
                GUI_GBToWstr(text_wchar, text_str, strlen(text_str));
                text_string.wstr_ptr = text_wchar;
                text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                GUIBUTTON_SetText(MMI_ZMT_HANZI_CHAPTER_LABEL_BACK_CTRL_ID, text_string.wstr_ptr, text_string.wstr_len);              

                text_style.align = ALIGN_HVMIDDLE;
                text_style.font = DP_FONT_20;
                text_style.font_color = MMI_WHITE_COLOR;
                
                SCI_TRACE_LOW("%s: hanzi_chapter_count = %d", __FUNCTION__, hanzi_chapter_count);
                if(hanzi_chapter_count == 0)
                {
                    MMIRES_GetText(WORD_LOADING, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &hanzi_win_rect,
                        &hanzi_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
                else if(hanzi_chapter_count == -1)
                {
                    MMIRES_GetText(WORD_LOADING_FAILED, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &hanzi_win_rect,
                        &hanzi_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
                else if(hanzi_chapter_count == -2)
                {
                    MMIRES_GetText(WORD_NO_DATA, win_id, &text_string);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &hanzi_win_rect,
                        &hanzi_win_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
                else
                {
                    GUI_BG_T auto_play_bg = {0};
                    if(is_open_auto_play)
        		{
                        auto_play_bg.bg_type = GUI_BG_IMG;
                        auto_play_bg.img_id = IMG_AUTO_PLAY;
                        GUIBUTTON_SetBg(MMI_ZMT_HANZI_CHAPTER_AUTO_PLAY_CTRL_ID, &auto_play_bg);
                        GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_CHAPTER_AUTO_PLAY_CTRL_ID, Hanzi_clickDisAutoPlay);
        		}
        		else
        		{
                        auto_play_bg.bg_type = GUI_BG_IMG;
                        auto_play_bg.img_id = IMG_DISAUTO_PLAY;
                        GUIBUTTON_SetBg(MMI_ZMT_HANZI_CHAPTER_AUTO_PLAY_CTRL_ID, &auto_play_bg);
                        GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_CHAPTER_AUTO_PLAY_CTRL_ID, Hanzi_clickAutoPlay);
        		}
                    GUIBUTTON_SetVisible(MMI_ZMT_HANZI_CHAPTER_LEFT_CTRL_ID,TRUE,FALSE);
                    GUIBUTTON_SetVisible(MMI_ZMT_HANZI_CHAPTER_RIGHT_CTRL_ID,TRUE,FALSE);
                    Hanzi_DisplayChapterList(win_id, MMI_ZMT_HANZI_CHAPTER_LIST_CTRL_ID);
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
                uint8 i,j = 0;
                uint8 m = 0;
                BOOLEAN is_get = FALSE;
                uint8 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_HANZI_CHAPTER_LIST_CTRL_ID);
                SCI_TRACE_LOW("%s: cur_idx = %d", __FUNCTION__, cur_idx);
                hanzi_book_info.cur_chapter_idx = cur_idx;
                hanzi_book_info.cur_section_idx = 0;
                hanzi_book_info.cur_section_children_idx = 0;
                for(i = 0;i < hanzi_chapter_count;i++){
                    for(j = 0;j < hanzi_chapter_children_count[i];j++){
                        if(m == cur_idx){
                            hanzi_book_info.cur_section_idx = i;
                            hanzi_book_info.cur_section_children_idx = j;
                            is_get = TRUE;
                            break;
                        }
                        m++;
                    }
                    if(is_get){
                        break;
                    }
                }
                SCI_TRACE_LOW("%s: cur_section_idx = %d, cur_section_children_idx = %d", 
                    __FUNCTION__, hanzi_book_info.cur_section_idx, hanzi_book_info.cur_section_children_idx);
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                hanzi_book_info.cur_section_idx = 0;
                hanzi_book_info.cur_section_children_idx = 0;              
                Hanzi_ReleaseChapterInfo();
                hanzi_chapter_count = 0;
                memset(&hanzi_chapter_children_count, 0, sizeof(hanzi_chapter_children_count));             
            }
            break;
         default:
            recode = MMI_RESULT_FALSE;
            break;
    }
      return recode;
}

WINDOW_TABLE(MMI_HANZI_CHAPTER_WIN_TAB) = {
    WIN_ID(MMI_HANZI_CHAPTER_WIN_ID),
    WIN_FUNC((uint32)HandleHanziChapterWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_CHAPTER_LABEL_BACK_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_HANZI_CHAPTER_LABEL_NUM_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_CHAPTER_AUTO_PLAY_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_CHAPTER_LEFT_CTRL_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_CHAPTER_RIGHT_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMI_CreateHanziChapterWin(void)
{
    MMK_CreateWin((uint32 *)MMI_HANZI_CHAPTER_WIN_TAB, PNULL);
}

LOCAL MMI_RESULT_E MMI_CloseHanziChapterWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CloseWin(MMI_HANZI_CHAPTER_WIN_ID);
    return result;
}

///////////////////////////////////////////////////////////////
LOCAL BOOLEAN Hanzi_ChatPlayMp3DataNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
	MMISRVAUD_REPORT_T *report_ptr = PNULL;

	if(param != PNULL && handle > 0)
	{
		report_ptr = (MMISRVAUD_REPORT_T *)param->data;
		if(report_ptr != PNULL && handle == hanzi_player_handle)
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

LOCAL void Hanzi_StopPlayMp3Data(void)
{
    if(hanzi_player_handle != 0)
    {
        MMISRVAUD_Stop(hanzi_player_handle);
        MMISRVMGR_Free(hanzi_player_handle);
        hanzi_player_handle = 0;
    }
}

LOCAL void Hanzi_ChatPlayMp3Data(uint8 *data,uint32 data_len)
{
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    BOOLEAN result = FALSE;

    Hanzi_StopPlayMp3Data();

    req.is_auto_free = TRUE;
    req.notify = Hanzi_ChatPlayMp3DataNotify;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.info.type = MMISRVAUD_TYPE_RING_BUF;
    audio_srv.info.ring_buf.fmt = MMISRVAUD_RING_FMT_MP3;
    audio_srv.info.ring_buf.data = data;
    audio_srv.info.ring_buf.data_len = data_len;
    audio_srv.volume=MMIAPISET_GetMultimVolume();
    
    SCI_TRACE_LOW("%s: audio_srv.volume=%d", __FUNCTION__, audio_srv.volume);

    audio_srv.all_support_route = MMISRVAUD_ROUTE_SPEAKER | MMISRVAUD_ROUTE_EARPHONE;
    hanzi_player_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
	
    if(hanzi_player_handle > 0)
    {
        SCI_TRACE_LOW("%s mp3_player_handle > 0", __FUNCTION__);
        result = MMISRVAUD_Play(hanzi_player_handle, 0);
        if(!result)
        {
            SCI_TRACE_LOW("%s chat_player error", __FUNCTION__);
            MMISRVMGR_Free(hanzi_player_handle);
            hanzi_player_handle = 0;
        }
        if(result == MMISRVAUD_RET_OK)
        {
            SCI_TRACE_LOW("%s chat_player success", __FUNCTION__);
        }
    }
    else
    {
        SCI_TRACE_LOW("%s mp3_player_handle <=> 0", __FUNCTION__);
    }
}

LOCAL void HanziDetail_NextChapterInfo(void)
{
    hanzi_book_info.cur_section_children_idx++;
    if(hanzi_book_info.cur_section_children_idx < hanzi_chapter_children_count[hanzi_book_info.cur_section_idx])
    {
        Hanzi_WriteUnmasterHanzi(cur_chapter_unmaster_count);
        hanzi_detail_cur_idx = 0;
        hanzi_book_info.cur_chapter_idx++;
        Hanzi_requestDetailInfo(
            hanzi_book_info.cur_book_idx+1,
            hanzi_content_info[hanzi_book_info.cur_section_idx]->content_id
        );
    }
    else
    {
        hanzi_book_info.cur_section_idx++;
        if(hanzi_book_info.cur_section_idx < hanzi_chapter_count)
        {
            Hanzi_WriteUnmasterHanzi(cur_chapter_unmaster_count);
            hanzi_detail_cur_idx = 0;
            hanzi_book_info.cur_section_children_idx = 0;
            hanzi_book_info.cur_chapter_idx++;
            Hanzi_requestDetailInfo(
                hanzi_book_info.cur_book_idx+1,
                hanzi_content_info[hanzi_book_info.cur_section_idx]->content_id
            );
        }
        else
        {
            //书本已经学完
        }
    }
}

PUBLIC void HanziDetail_TipTimeout(uint8 timer_id,uint32 param)
{
    SCI_TRACE_LOW("DSLCHAT_WIN %s start", __FUNCTION__);
    if(0 != hanzi_tip_timer)
    {
        MMK_StopTimer(hanzi_tip_timer);
        hanzi_tip_timer = 0;
    }
    hanzi_is_display_tip = 0;
    MMK_PostMsg(MMI_HANZI_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
}

LOCAL void HanziDetail_DisplayTip(uint type)
{
    SCI_TRACE_LOW("DSLCHAT_WIN %s start", __FUNCTION__);
    if(0 != hanzi_tip_timer)
    {
        MMK_StopTimer(hanzi_tip_timer);
        hanzi_tip_timer = 0;
    }
    hanzi_is_display_tip = type;
    MMK_PostMsg(MMI_HANZI_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    hanzi_tip_timer = MMK_CreateTimerCallback(2000, HanziDetail_TipTimeout,(uint32)0, FALSE);
    MMK_StartTimerCallback(hanzi_tip_timer, 2000, HanziDetail_TipTimeout, (uint32)0, FALSE);
}

PUBLIC void HanziDetail_PlayPinyinAudio(void)
{
    if(hanzi_detail_info[hanzi_detail_cur_idx]->audio_len == 0)
    {
        //没有音频数据，去请求//这里有本地音频的话也可以去拉本地数据暂时没有实现
        HanziDetail_DisplayTip(1);
        MMIZDT_HTTP_AppSend(TRUE, hanzi_detail_info[hanzi_detail_cur_idx]->audio_uri, PNULL, 0, 1000, 0, 0, 6000, 0, 0, Hanzi_ParseMp3Response);
    }
    else if(hanzi_detail_info[hanzi_detail_cur_idx]->audio_len == -1)
    {
        //没有音频uri
        HanziDetail_DisplayTip(2);
    }
    else if(hanzi_detail_info[hanzi_detail_cur_idx]->audio_len == -2)
    {
        //加载音频失败
        HanziDetail_DisplayTip(3);
    }
    else
    {
        //加载成功，开始播放
        Hanzi_ChatPlayMp3Data(hanzi_detail_info[hanzi_detail_cur_idx]->audio_data ,hanzi_detail_info[hanzi_detail_cur_idx]->audio_len);
    }
}

LOCAL void HanziDetail_LeftDetail(void)//已掌握/上一个
{
    if(!is_open_new_hanzi){
        hanzi_detail_cur_idx++;
    }else{
        if(hanzi_detail_cur_idx == 0){
            return;
        }else{
            hanzi_detail_cur_idx--;
        }
    }
    HanziDetail_TipTimeout(hanzi_tip_timer, 0);
}

LOCAL void HanziDetail_RightDetail(void)//未掌握/下一个
{
    if(!is_open_new_hanzi){
        cur_chapter_unmaster_idx[cur_chapter_unmaster_count] = hanzi_detail_cur_idx + 1;
        cur_chapter_unmaster_count++;
        hanzi_detail_cur_idx++;
        HanziDetail_TipTimeout(hanzi_tip_timer, 0);
    }else{
        uint16 cur_idx = cur_new_word_page_idx * HANZI_CHAPTER_WORD_MAX + hanzi_detail_cur_idx;
        if(cur_idx + 1 == hanzi_detail_count){
            hanzi_detail_cur_idx++;
            HanziDetail_TipTimeout(hanzi_tip_timer, 0);
        }else{
            hanzi_detail_cur_idx++;
            if(hanzi_detail_cur_idx >= HANZI_CHAPTER_WORD_MAX){
                hanzi_detail_cur_idx = 0;
                cur_new_word_page_idx++;
                Hanzi_RequestNewWord();
            }
            else
            {
                HanziDetail_TipTimeout(hanzi_tip_timer, 0);
            }
        }
    }
}

LOCAL void HanziDetail_DeleteNewWord(void)
{
    uint16 idx = cur_new_word_page_idx * HANZI_CHAPTER_WORD_MAX + hanzi_detail_cur_idx;
    HanziDetail_DisplayTip(4);
    Hanzi_DeleteNewWordItem(idx);
}

LOCAL void HanziDetail_DisplayDtailInfo(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};    
    MMI_STRING_T text_word = {0};
    MMI_STRING_T text_pinyin = {0};
    MMI_STRING_T text_remark = {0};
    wchar wstr_word[10] = {0};
    wchar wstr_pinyin[30] = {0};
    wchar wstr_remark[2048] = {0};
    uint16 size = 0;
    uint16 text_width_piex = 0;
    GUI_RECT_T pinyin_rect = {0};
    GUI_RECT_T audio_rect = {0};

    size = strlen(hanzi_detail_info[hanzi_detail_cur_idx]->word);
    GUILABEL_SetRect(MMI_ZMT_HANZI_DETAIL_LABEL_WORD_CTRL_ID, &hanzi_word_rect, FALSE);
    GUI_UTF8ToWstr(wstr_word, 10, hanzi_detail_info[hanzi_detail_cur_idx]->word, size); 
    text_word.wstr_ptr = wstr_word;
    text_word.wstr_len = MMIAPICOM_Wstrlen(text_word.wstr_ptr);
    GUILABEL_SetText(MMI_ZMT_HANZI_DETAIL_LABEL_WORD_CTRL_ID, &text_word, FALSE);

    size = strlen(hanzi_detail_info[hanzi_detail_cur_idx]->pingy);
    GUI_UTF8ToWstr(wstr_pinyin, 30, hanzi_detail_info[hanzi_detail_cur_idx]->pingy, size);
    text_pinyin.wstr_ptr = wstr_pinyin;
    text_pinyin.wstr_len = MMIAPICOM_Wstrlen(text_pinyin.wstr_ptr);
    text_width_piex = GUI_CalculateStringPiexlNum(text_pinyin.wstr_ptr, text_pinyin.wstr_len, DP_FONT_16, 1);
    pinyin_rect = hanzi_pinyin_rect;
    pinyin_rect.right = pinyin_rect.left + text_width_piex + 10;
    GUILABEL_SetRect(MMI_ZMT_HANZI_DETAIL_LABEL_PINYIN_CTRL_ID, &pinyin_rect, FALSE);
    GUILABEL_SetText(MMI_ZMT_HANZI_DETAIL_LABEL_PINYIN_CTRL_ID, &text_pinyin, FALSE);
    
    audio_rect = hanzi_pinyin_rect;
    audio_rect.left = pinyin_rect.right;
    GUIBUTTON_SetRect(MMI_ZMT_HANZI_DETAIL_BUTTON_AUDIO_CTRL_ID, &audio_rect);

    LCD_DrawHLine(&lcd_dev_info, hanzi_pinyin_rect.left, hanzi_pinyin_rect.bottom, hanzi_pinyin_rect.right, MMI_WHITE_COLOR);

    size = strlen(hanzi_detail_info[hanzi_detail_cur_idx]->remark);
    GUITEXT_SetRect(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, &hanzi_text_rect);
    GUI_UTF8ToWstr(wstr_remark, 2048, hanzi_detail_info[hanzi_detail_cur_idx]->remark, size);
    text_remark.wstr_ptr = wstr_remark;
    text_remark.wstr_len = MMIAPICOM_Wstrlen(text_remark.wstr_ptr);
    GUITEXT_SetString(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, text_remark.wstr_ptr,text_remark.wstr_len, TRUE);
}

LOCAL MMI_RESULT_E HandleHanziDetailWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                GUI_FONT_ALL_T font = {0};
                GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
                GUI_FONT_T text_font = DP_FONT_16;
                GUI_COLOR_T text_color = MMI_WHITE_COLOR;
                GUI_BG_T bg = {0};
		
                font.font = DP_FONT_18;
                font.color = MMI_WHITE_COLOR;
        
                GUIBUTTON_SetRect(MMI_ZMT_HANZI_DETAIL_LABEL_BACK_CTRL_ID, &hanzi_title_rect);
                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_LABEL_BACK_CTRL_ID, MMI_CloseHanziDetailWin);
                GUIBUTTON_SetFont(MMI_ZMT_HANZI_DETAIL_LABEL_BACK_CTRL_ID, &font);
                GUIBUTTON_SetTextAlign(MMI_ZMT_HANZI_DETAIL_LABEL_BACK_CTRL_ID,ALIGN_LVMIDDLE);

                GUITEXT_SetRect(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, &hanzi_text_rect);
                GUITEXT_SetFont(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, &text_font,&text_color);
                GUITEXT_IsDisplayPrg(FALSE, MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID);
                GUITEXT_SetHandleTpMsg(FALSE, MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID);
                GUITEXT_SetClipboardEnabled(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID,FALSE);
                GUITEXT_IsSlide(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID,FALSE);
                bg.bg_type = GUI_BG_COLOR;
                bg.color = GUI_RGB2RGB565(80, 162, 254);
                GUITEXT_SetBg(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, &bg);

                GUIBUTTON_SetRect(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, &hanzi_msg_rect);
                Hanzi_InitButton(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID);
                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, FALSE, FALSE);

                font.font = DP_FONT_16;
                if(is_open_new_hanzi){
                    GUIBUTTON_SetRect(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID, &hanzi_pre_rect);
                    GUIBUTTON_SetRect(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, &hanzi_next_rect);
                    GUIBUTTON_SetRect(MMI_ZMT_HANZI_DETAIL_DELETE_CTRL_ID, &hanzi_del_rect);                    
                }else{
                    GUIBUTTON_SetRect(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID, &hanzi_left_rect);
                    GUIBUTTON_SetRect(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, &hanzi_right_rect);
                                  
                    GUIBUTTON_SetTextAlign(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID,ALIGN_HVMIDDLE);
                    GUIBUTTON_SetTextAlign(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID,ALIGN_HVMIDDLE);
                    
                    Hanzi_InitButton(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID);
                    Hanzi_InitButton(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID);
                }
                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_DELETE_CTRL_ID,FALSE,FALSE);
                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID,FALSE,FALSE);

                GUILABEL_SetRect(MMI_ZMT_HANZI_DETAIL_LABEL_NUM_CTRL_ID, &hanzi_dir_rect, FALSE);
                GUILABEL_SetFont(MMI_ZMT_HANZI_DETAIL_LABEL_NUM_CTRL_ID, DP_FONT_16,MMI_WHITE_COLOR);
                GUILABEL_SetAlign(MMI_ZMT_HANZI_DETAIL_LABEL_NUM_CTRL_ID, GUILABEL_ALIGN_MIDDLE);

                GUILABEL_SetFont(MMI_ZMT_HANZI_DETAIL_LABEL_WORD_CTRL_ID, DP_FONT_22,MMI_WHITE_COLOR);
                GUILABEL_SetFont(MMI_ZMT_HANZI_DETAIL_LABEL_PINYIN_CTRL_ID, DP_FONT_20,MMI_WHITE_COLOR);

                memset(cur_chapter_unmaster_idx, 0, sizeof(cur_chapter_unmaster_idx));
                if(is_open_new_hanzi){
                    Hanzi_RequestNewWord();
                }else{
                    Hanzi_requestDetailInfo(
                        hanzi_book_info.cur_book_idx+1,
                        hanzi_content_info[hanzi_book_info.cur_section_idx]->content_id
                     );
                }
                if (UILAYER_IsMultiLayerEnable())
                {
                    UILAYER_CREATE_T create_info = {0};
                    create_info.lcd_id = MAIN_LCD_ID;
                    create_info.owner_handle = win_id;
                    create_info.offset_x = hanzi_tip_rect.left;
                    create_info.offset_y = hanzi_tip_rect.top;
                    create_info.width = hanzi_tip_rect.right;
                    create_info.height = hanzi_tip_rect.bottom;
                    create_info.is_bg_layer = FALSE;
                    create_info.is_static_layer = FALSE;
                    UILAYER_CreateLayer(&create_info, &hanzi_detail_tip_layer);
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

                GUI_FillRect(&lcd_dev_info, hanzi_win_rect, GUI_RGB2RGB565(80, 162, 254));
                GUI_FillRect(&lcd_dev_info, hanzi_title_rect, GUI_RGB2RGB565(108, 181, 255));

                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, FALSE, FALSE);
                if(is_open_new_hanzi){
                    MMIRES_GetText(HANZI_NEW_WORD, win_id, &text_string);
                }else{
                    sprintf(text_str, "%s", hanzi_content_info[hanzi_book_info.cur_section_idx]->chapter[hanzi_book_info.cur_section_children_idx]->chapter_name);
                    SCI_TRACE_LOW("%s: info[%d].chapter_name = %s", __FUNCTION__, hanzi_book_info.cur_section_idx, text_str);
                    GUI_UTF8ToWstr(text_wchar, 100, text_str, strlen(text_str));
                    text_string.wstr_ptr = text_wchar;
                    text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);                  
                }
                GUIBUTTON_SetText(MMI_ZMT_HANZI_DETAIL_LABEL_BACK_CTRL_ID, text_string.wstr_ptr, text_string.wstr_len);

                text_style.align = ALIGN_HVMIDDLE;
                text_style.font = DP_FONT_22;
                text_style.font_color = MMI_WHITE_COLOR;

                SCI_TRACE_LOW("%s: hanzi_detail_count = %d", __FUNCTION__, hanzi_detail_count);
                if(hanzi_detail_count == 0)
                {
                    GUITEXT_SetFont(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, &text_style.font,&text_style.font_color);
                    MMIRES_GetText(WORD_LOADING, win_id, &text_string);
                    GUITEXT_SetAlign(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, ALIGN_HMIDDLE);
                    GUITEXT_SetString(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, text_string.wstr_ptr,text_string.wstr_len, TRUE);
                }
                else if(hanzi_detail_count == -1)
                {
                    GUITEXT_SetFont(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, &text_style.font,&text_style.font_color);
                    MMIRES_GetText(WORD_LOADING_FAILED, win_id, &text_string);
                    GUITEXT_SetAlign(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, ALIGN_HMIDDLE);
                    GUITEXT_SetString(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, text_string.wstr_ptr,text_string.wstr_len, TRUE);
                }
                else if(hanzi_detail_count == -2)
                {
                    GUITEXT_SetFont(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, &text_style.font,&text_style.font_color);
                    MMIRES_GetText(WORD_NO_DATA, win_id, &text_string);
                    GUITEXT_SetAlign(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, ALIGN_HMIDDLE);
                    GUITEXT_SetString(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, text_string.wstr_ptr,text_string.wstr_len, TRUE);
                }
                else if(hanzi_detail_count == -3)
                {
                    GUITEXT_SetFont(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, &text_style.font,&text_style.font_color);
                    MMIRES_GetText(HANZI_NO_UNMASTER, win_id, &text_string);
                    GUITEXT_SetAlign(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, ALIGN_HMIDDLE);
                    GUITEXT_SetString(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, text_string.wstr_ptr,text_string.wstr_len, TRUE);
                }
                else
                {                       
                    GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID,TRUE,FALSE);
                    GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID,TRUE,FALSE);

                    
                    if(!is_open_new_hanzi){
                        SCI_TRACE_LOW("%s: hanzi_detail_cur_idx = %d, hanzi_detail_count = %d", __FUNCTION__, hanzi_detail_cur_idx, hanzi_detail_count);
                        if(hanzi_detail_cur_idx > hanzi_detail_count){
                            is_end = TRUE;
                        }
                    }else{
                        uint16 cur_idx = cur_new_word_page_idx * HANZI_CHAPTER_WORD_MAX + hanzi_detail_cur_idx;
                        SCI_TRACE_LOW("%s: cur_idx = %d, hanzi_detail_count = %d", __FUNCTION__, cur_idx, hanzi_detail_count);
                        if(cur_idx + 1 > hanzi_detail_count){
                            is_end = TRUE;
                        }
                    }
                    if(!is_end)
                    {
                        GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_BUTTON_AUDIO_CTRL_ID,TRUE,FALSE);
                        GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_BUTTON_AUDIO_CTRL_ID, HanziDetail_PlayPinyinAudio);
                        if(is_open_new_hanzi)
                        {
                            GUI_BG_T but_bg = {0};
                            uint16 cur_idx = cur_new_word_page_idx * HANZI_CHAPTER_WORD_MAX + hanzi_detail_cur_idx;
                            but_bg.bg_type = GUI_BG_IMG;
                            if(cur_idx > 0){
                                but_bg.img_id = WORD_PRE_ICON;
                                GUIBUTTON_SetBg(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID, &but_bg);
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID, HanziDetail_LeftDetail);
                            }else{
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
                            }
                            but_bg.img_id = WORD_NEXT_ICON;
                            GUIBUTTON_SetBg(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, &but_bg);                          
                            GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, HanziDetail_RightDetail);
                            GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_DELETE_CTRL_ID,TRUE,TRUE);
                            GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_DELETE_CTRL_ID, HanziDetail_DeleteNewWord);
                        }
                        else
                        {
                            GUIBUTTON_SetTextId(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID, HANZI_MASTERED);
                            GUIBUTTON_SetTextId(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, HANZI_UNMASTERED);
                            GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID, HanziDetail_LeftDetail);
                            GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, HanziDetail_RightDetail);
                        }
                        Hanzi_SetDiretionText(MMI_ZMT_HANZI_DETAIL_LABEL_NUM_CTRL_ID, cur_new_word_page_idx * HANZI_CHAPTER_WORD_MAX + hanzi_detail_cur_idx, hanzi_detail_count);                   
                        HanziDetail_DisplayDtailInfo(win_id);
                    }
                    else
                    {
                         if(!is_open_new_hanzi)
                         {
                            if(hanzi_book_info.cur_chapter_idx + 1 < cur_chapter_total_count)
                            {
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, TRUE, TRUE);
                                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, WORD_FINISH);
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_BUTTON_AUDIO_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID, HANZI_BACK_CHAPTER);
                                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, HANZI_NEXT_CHAPTER);
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID, MMI_CloseHanziDetailWin);
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, HanziDetail_NextChapterInfo);
                            }
                            else
                            {
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, TRUE, TRUE);
                                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, WORD_BOOK_FINISH);
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_BUTTON_AUDIO_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, WORD_BACK_CH);            
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, MMI_CloseHanziDetailWin);
                            }
                         }
                         else
                         {
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_DELETE_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, TRUE, TRUE);
                                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID, NEW_WORD_BOOK_FINISH);
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_BUTTON_AUDIO_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetVisible(MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID,FALSE,FALSE);
                                GUIBUTTON_SetRect(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, &hanzi_right_rect);
                                GUIBUTTON_SetTextAlign(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID,ALIGN_HVMIDDLE);
                                Hanzi_InitButton(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID);
                                GUIBUTTON_SetTextId(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, WORD_BACK_CH);            
                                GUIBUTTON_SetCallBackFunc(MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID, MMI_CloseHanziDetailWin);
                         }
                    }
                }

                if(hanzi_is_display_tip != 0)
                {
                    UILAYER_APPEND_BLT_T append_layer = {0};
                    GUISTR_STYLE_T text_style = {0};
                    MMI_STRING_T text_string = {0};
                    wchar text_str[35] = {0};
                    char count_str[35] = {0};
                    GUI_RECT_T tip_rect = {0};

                    append_layer.lcd_dev_info = hanzi_detail_tip_layer;
                    append_layer.layer_level = UILAYER_LEVEL_HIGH;
                    UILAYER_AppendBltLayer(&append_layer);

                    LCD_FillRoundedRect(&hanzi_detail_tip_layer, hanzi_tip_rect, hanzi_tip_rect, MMI_WHITE_COLOR);

                    text_style.align = ALIGN_HVMIDDLE;
                    text_style.font = DP_FONT_18;
                    text_style.font_color = GUI_RGB2RGB565(80, 162, 254);

                    if(hanzi_is_display_tip == 1)
                    {
                        sprintf(count_str,"正在加载，请稍后");
                    }else if(hanzi_is_display_tip == 2)
                    {
                        sprintf(count_str,"暂无音频");
                    }else if(hanzi_is_display_tip == 3)
                    {
                        sprintf(count_str,"音频加载失败，请重试");
                    }else if(hanzi_is_display_tip == 4)
                    {
                        sprintf(count_str,"正在删除，请稍等");
                    }
                    GUI_GBToWstr(text_str, count_str, strlen(count_str));
                    text_string.wstr_ptr = text_str;
                    text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&hanzi_detail_tip_layer,
                        &hanzi_tip_rect,
                        &hanzi_tip_rect,
                        &text_string,
                        &text_style,
                        GUISTR_STATE_ALIGN,
                        GUISTR_TEXT_DIR_AUTO
                    );
                }
                else
                {
                    UILAYER_RemoveBltLayer(&hanzi_detail_tip_layer);
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
                        MMK_PostMsg(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, MSG_KEYREPEAT_UP, PNULL, 0);
                    }
                    else if(tp_offset_y < -40)
                    {
                        MMK_PostMsg(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID, MSG_KEYREPEAT_DOWN, PNULL, 0);
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
                Hanzi_WriteUnmasterHanzi(cur_chapter_unmaster_count);
                Hanzi_ReleaseDetailInfo();
                hanzi_detail_cur_idx = 0;
                hanzi_detail_count = 0;
                cur_chapter_unmaster_count = 0;
                cur_new_word_page_idx = 0;
                Hanzi_StopPlayMp3Data();
                memset(cur_chapter_unmaster_idx, 0, sizeof(cur_chapter_unmaster_idx));
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
}

WINDOW_TABLE(MMI_HANZI_DETAIL_WIN_TAB) = {
    WIN_ID(MMI_HANZI_DETAIL_WIN_ID),
    WIN_FUNC((uint32)HandleHanziDetailWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_DETAIL_LABEL_BACK_CTRL_ID),//单元名称
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_HANZI_DETAIL_LABEL_NUM_CTRL_ID),//导航页
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_HANZI_DETAIL_LABEL_WORD_CTRL_ID),//汉字
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_HANZI_DETAIL_LABEL_PINYIN_CTRL_ID),//拼音
    CREATE_BUTTON_CTRL(ZMT_LISTEN_VOLUME, MMI_ZMT_HANZI_DETAIL_BUTTON_AUDIO_CTRL_ID),//音频图标
    CREATE_TEXT_CTRL(MMI_ZMT_HANZI_DETAIL_TEXT_INFO_CTRL_ID),//释义
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_DETAIL_LEFT_CTRL_ID),//已掌握/上一个
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_DETAIL_RIGHT_CTRL_ID),//未掌握/下一个
    CREATE_BUTTON_CTRL(WORD_DELETE_ICON, MMI_ZMT_HANZI_DETAIL_DELETE_CTRL_ID),//删除
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_HANZI_MSG_TIPS_CTRL_ID),//已学完tip
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMI_CreateHanziDetailWin(void)
{
    MMK_CreateWin((uint32 *)MMI_HANZI_DETAIL_WIN_TAB, PNULL);
}

PUBLIC MMI_RESULT_E MMI_CloseHanziDetailWin(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CloseWin(MMI_HANZI_DETAIL_WIN_ID);
    return result;
}

