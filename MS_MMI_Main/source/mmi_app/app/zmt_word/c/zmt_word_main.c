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
/*****************************************************************/
// Description : open hello sprd windows
// Global resource dependence :
// Author: xiaoqing.lu
// Note:
/**********************************************************/
LOCAL GUI_RECT_T word_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};//窗口
LOCAL GUI_RECT_T word_title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, WORD_CARD_LINE_HIGHT};//顶部
LOCAL GUI_RECT_T word_list_rect = {0, WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT-5};//列表
LOCAL GUI_RECT_T word_left_rect = {10, MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT-5, 3*WORD_CARD_LINE_WIDTH-10, MMI_MAINSCREEN_HEIGHT-2};//单词学习/掌握
LOCAL GUI_RECT_T word_right_rect = {3*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT-5, MMI_MAINSCREEN_WIDTH-10, MMI_MAINSCREEN_HEIGHT-2};//生词本/未掌握
LOCAL GUI_RECT_T word_pre_rect = {0.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 1.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-上一个
LOCAL GUI_RECT_T word_del_rect = {3*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 4*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-删除
LOCAL GUI_RECT_T word_next_rect = {4.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-WORD_CARD_LINE_HIGHT, 5.5*WORD_CARD_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-2};//生词本-下一个
LOCAL GUI_RECT_T word_msg_rect = {WORD_CARD_LINE_WIDTH, 3*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-WORD_CARD_LINE_WIDTH, 5*WORD_CARD_LINE_HIGHT};//界面的信息提示
LOCAL GUI_RECT_T word_tip_rect = {WORD_CARD_LINE_WIDTH,3*WORD_CARD_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-WORD_CARD_LINE_WIDTH,5*WORD_CARD_LINE_HIGHT};//弹框提示
LOCAL GUI_RECT_T word_auto_tip_rect = {WORD_CARD_LINE_WIDTH, 2*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-WORD_CARD_LINE_WIDTH, 5*WORD_CARD_LINE_HIGHT};//自动播放提示
LOCAL GUI_RECT_T word_auto_play_rect = {4*WORD_CARD_LINE_WIDTH, 0, 6*WORD_CARD_LINE_WIDTH, WORD_CARD_LINE_HIGHT};//自动播放图标
LOCAL GUI_RECT_T word_word_rect = {5, WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 2*WORD_CARD_LINE_HIGHT};//单词
LOCAL GUI_RECT_T word_uk_rect = {5, 2*WORD_CARD_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 3*WORD_CARD_LINE_HIGHT};//音标
LOCAL GUI_RECT_T word_text_rect = {5,3*WORD_CARD_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-5,MMI_MAINSCREEN_HEIGHT-1.5*WORD_CARD_LINE_HIGHT-8};//单词释义

#define MAX_WORD_NUM 10
#define MAX_BOOK_NUM 20
#define MAX_CHAPTER_NUM 30
#define MAX_AUDIO_NUM 50
LOCAL ZMT_BOOK_T *books[MAX_BOOK_NUM];
LOCAL ZMT_CHAPTER_T *chapters[MAX_CHAPTER_NUM];
LOCAL ZMT_WORD_T *words[MAX_WORD_NUM];
LOCAL ZMT_WORD_AUDIO_T * words_audio[MAX_AUDIO_NUM];
LOCAL ZMT_PAGE_INFO *pageInfo;
#define DSL_WORD_MAIN_DISPLAY_NUM 2
LOCAL int dsl_word_main_display_count=0;
LOCAL int dsl_word_main_display_idx=0;
#define DSL_CHAPTER_DISPLAY_NUM 3
LOCAL int dsl_chapter_select_idx=0;
LOCAL int dsl_chapter_display_count=0;
LOCAL int dsl_chapter_display_idx=0;

LOCAL uint32 request_http_win_main_idx=0;
LOCAL uint32 request_http_win_ch_idx=0;
LOCAL uint32 request_http_win_word_idx=0;

LOCAL uint mp3_idx=0;
LOCAL int8 is_display_tip=0;
LOCAL uint8 display_tip_timer=0;

LOCAL uint8 is_autoplay=1;
LOCAL int8 is_autodisplay_tip=0;
LOCAL uint8 autodisplay_tip_timer=0;

LOCAL uint8 is_load_local=0;

LOCAL uint8 is_new_word=0;;

LOCAL char *newWords=PNULL;
LOCAL char *newWords_ex=PNULL;
LOCAL int newWordsReNum=0;
LOCAL void parseNewWordResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void ChatStopMp3Data(void);
LOCAL MMI_RESULT_E PlayWordUkSound(void);

LOCAL MMI_RESULT_E closeWordCardWin();
LOCAL MMI_RESULT_E openWordCardWin();
LOCAL MMI_RESULT_E openNewWordCardWin();
LOCAL MMI_RESULT_E openNewWordPreCard();
LOCAL MMI_RESULT_E openNewWordNextCard();
LOCAL MMI_RESULT_E deleteNewWordCard();
LOCAL MMI_RESULT_E openNextCard();
LOCAL MMI_RESULT_E setWordToNew();
LOCAL MMI_RESULT_E closeWordWin();
LOCAL MMI_RESULT_E openCHWin();
LOCAL MMI_RESULT_E closeWordCHWin();
LOCAL void requestBookInfo();
LOCAL void requestWordInfo();
LOCAL void addChapterInfo(uint8 position, char *name, int id,char * path);
LOCAL void addBookInfo(uint8 position, char *name , char *editionName, int id,char *path,char *basepath);
LOCAL void addWordInfo(uint8 position, char *word,char *sentence,char *sentence_explain,char *us,char *us_sound, char *uk,char *uk_sound,char *explain);
LOCAL void releaseWords();
LOCAL void releaseChapters();
LOCAL void releaseBooks();
LOCAL void setNumberText(MMI_CTRL_ID_T ctrl_id,int cur,int count);

LOCAL void parseBookResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void parseChapterResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void parseWordResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL parseListenAudioResponse(BOOLEAN success, uint32 idx);
LOCAL parseMp3Response(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);

LOCAL MMI_RESULT_E wordMainRefresh();
LOCAL MMI_RESULT_E wordChRefresh();
LOCAL MMI_RESULT_E wordRefresh();

LOCAL MMI_RESULT_E HandleWordWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
LOCAL MMI_RESULT_E HandleWordCHWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
LOCAL MMI_RESULT_E HandleWordCardWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

// 将strRes中的from替换为to，替换成功返回1，否则返回0。
LOCAL int StrReplace(char strRes[],char from[], char to[]) ;

LOCAL void loadLocalWords(char *path);

LOCAL void getWordAudioPath(char *path,char *audio_url);

LOCAL BOOLEAN ChatPlayAudioDataNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param);
/**--------------------------------------------------------------------------*/
/** GLOBAL DEFINITION */
/**--------------------------------------------------------------------------*/
LOCAL uint8 listening_total_count = 0;
LOCAL BOOLEAN is_homework_task = FALSE;
LOCAL char homework_task_id[50] = {0};
LOCAL uint8 homework_task_page = 0;
LOCAL char homework_task_bookid[20] = {0};
LOCAL uint8 homework_task_last_idx = 0;
LOCAL uint8 homework_task_target_num = 0;
LOCAL char *word_task_msg_id = NULL;
LOCAL uint8 listening_index = 0;

LOCAL uint8 is_can_click=0;

LOCAL void ClickTimeOut(uint8 timer_id, uint32 param) {
	if (0 != is_can_click) {
		MMK_StopTimer(is_can_click);
		is_can_click= 0;
	}
}

LOCAL uint8 GetCanntClick(uint is_click) 
{
	// if(0 == is_can_click)
	// {
	// 	if(is_click)
	// 	{
	// 		is_can_click = MMK_CreateTimerCallback(500, ClickTimeOut, 0, FALSE);
	// 		MMK_StartTimerCallback(is_can_click, 500,ClickTimeOut,0,FALSE);
	// 	}
		return 0;
	// }
	// return 1;
}

LOCAL uint8 is_record = 0;//0没有 1有 2加载

LOCAL OpenWordRecordTips();

LOCAL void saveRecord(void)
{
	char json[500]={0};
	sprintf(json,"{\"bookid\": \"%s\" ,\"chapterIdx\": %d ,\"curPage\": %d ,\"realIdx\": %d ,\"curIdx\": %d }",
	books[pageInfo->bookIdx]->id,pageInfo->chapterIdx,pageInfo->curPage,pageInfo->realIdx,pageInfo->curIdx);
	if(!zmt_dir_exsit("E:/Word"))
	{
		zmt_dir_creat("E:/Word");
	}
	zmt_file_save("E:/Word/userdata",json,strlen(json),FALSE);
}

LOCAL uint8 loadRecord(void)
{
	char * data = PNULL;
	int data_size;
	if(zmt_file_exist("E:/Word/userdata"))
	{
		data = zmt_file_data_read("E:/Word/userdata",&data_size);
		if(data_size>0){
			cJSON *root = cJSON_Parse(data);
			cJSON *bookid = cJSON_GetObjectItem(root, "bookid");
			cJSON *chapterIdx = cJSON_GetObjectItem(root, "chapterIdx");
			cJSON *curPage = cJSON_GetObjectItem(root, "curPage");
			cJSON *realIdx = cJSON_GetObjectItem(root, "realIdx");
			cJSON *curIdx = cJSON_GetObjectItem(root, "curIdx");
			strcpy(pageInfo->bookId,bookid->valuestring);
			pageInfo->chapterIdx=chapterIdx->valueint;
			pageInfo->curPage=curPage->valueint;
			pageInfo->realIdx=realIdx->valueint;
			pageInfo->curIdx=curIdx->valueint;
			SCI_FREE(data);
			return 1;
		}
	}
	return 0;
}

WINDOW_TABLE(MMI_WORD_WIN_TAB) = {
    WIN_ID(MMI_WORD_WIN_ID),
    WIN_FUNC((uint32)HandleWordWinMsg),
    //CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_MAIN_BUTTON_REFRESH),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_MAIN_LABEL_BACK),
    //CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_MAIN_PAD_LABEL_BACK),
    //CREATE_BUTTON_CTRL(IMG_BACK, MMI_ZMT_WORD_MAIN_BUTTON_BACK),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMI_ZMT_WORD_MAIN_LIST_CONTENT),
	CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_WORD_MAIN_LABEL_NUM_ID),
	WIN_HIDE_STATUS,
    END_WIN};
WINDOW_TABLE(MMI_WORD_CH_WIN_TAB) = {
    WIN_ID(MMI_WORD_CH_WIN_ID),
    WIN_FUNC((uint32)HandleWordCHWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CH_BUTTON_REFRESH),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CH_LABEL_BACK),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CH_PAD_LABEL_BACK),
    CREATE_BUTTON_CTRL(PNULL,MMI_ZMT_WORD_CH_BUTTON_AUTOPLAY),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMI_ZMT_WORD_CH_LIST_CONTENT),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CH_BUTTON_EXERCISE),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_CH_BUTTON_PACTISE),
	WIN_HIDE_STATUS,
    END_WIN};
WINDOW_TABLE(MMI_WORD_CARD_WIN_TAB) = {
    WIN_ID(MMI_WORD_CARD_WIN_ID),
    WIN_FUNC((uint32)HandleWordCardWinMsg),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_BUTTON_REFRESH),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_LABEL_TITLE_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_BUTTON_OK_ID),
    CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_BUTTON_CACEL_ID),
	CREATE_BUTTON_CTRL(WORD_NEXT_ICON, MMI_ZMT_WORD_BUTTON_NEXT_ID),
    CREATE_BUTTON_CTRL(WORD_PRE_ICON, MMI_ZMT_WORD_BUTTON_PRE_ID),
    CREATE_BUTTON_CTRL(WORD_DELETE_ICON, MMI_ZMT_WORD_BUTTON_DELETE_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_RIGHT, MMI_ZMT_WORD_LABEL_NUM_ID),
	CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_WORD_MSG_LABEL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_ZMT_WORD_MSG_SP1_ID),
	CREATE_BUTTON_CTRL(ZMT_LISTEN_VOLUME, MMI_ZMT_WORD_MSG_SP1_BTN_ID),
    CREATE_TEXT_CTRL(MMI_ZMT_WORD_INFO_TEXT_ID),
	CREATE_BUTTON_CTRL(PNULL, MMI_ZMT_WORD_LABEL_MESSAGE_ID),
	WIN_HIDE_STATUS,
    END_WIN};

/**--------------------------------------------------------------------------*
** FUNCTION DEFINITION *
**--------------------------------------------------------------------------*/
/******************************************************************/
// Description : open hello sprd windows
// Global resource dependence :
// Author: xiaoqing.lu
// Note:
/****************************************************************/
LOCAL void AppendDSLListItemByTextId(MMI_CTRL_ID_T ctrl_id, MMI_IMAGE_ID_T image_id, int index) 
{
	GUILIST_ITEM_T item_t = {0};
	GUILIST_ITEM_DATA_T item_data = {0};
	GUI_COLOR_T list_color = {0};

	char tmp[10]={0};
	MMI_STRING_T text0 = {0};
	uint16 wstr0[10] = {0};

	MMI_STRING_T text = {0};
	uint16 wstr[24] = {0};

	MMI_STRING_T text1 = {0};
	uint16 wstr1[24] = {0};
  
 
	item_t.item_style = GUIITEM_SYTLE_DSL_ENGLISH_BOOK;
	item_t.item_data_ptr = &item_data;

	item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
	item_data.item_content[0].item_data.image_id = image_id;

	itoa(index+1,tmp, 10);
	GUI_UTF8ToWstr(wstr0,10,tmp, strlen(tmp)+1);
	text0.wstr_len = MMIAPICOM_Wstrlen(wstr0);
	text0.wstr_ptr = wstr0;
	item_data.item_content[1].font_color_id = MMITHEME_COLOR_LIGHT_BLUE;
	item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
	item_data.item_content[1].item_data.text_buffer = text0;

	GUI_UTF8ToWstr(wstr1,24,books[index]->name, strlen(books[index]->name)+1);
	text1.wstr_len = MMIAPICOM_Wstrlen(wstr1);
	text1.wstr_ptr = wstr1;
	item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
	item_data.item_content[2].item_data.text_buffer = text1;

	GUI_UTF8ToWstr(wstr,24,books[index]->editionName, strlen(books[index]->editionName)+1);
	text.wstr_len = MMIAPICOM_Wstrlen(wstr);
	text.wstr_ptr = wstr;
	item_data.item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
	item_data.item_content[3].item_data.text_buffer = text;
	
	GUILIST_AppendItem(ctrl_id, &item_t);
}
LOCAL void appendCHItem(MMI_CTRL_ID_T ctrl_id, char *data,MMI_IMAGE_ID_T image_id)
{
	GUILIST_ITEM_T item_t = {0};
	GUILIST_ITEM_DATA_T item_data = {0};
	GUI_COLOR_T list_color = {0};

	MMI_STRING_T text = {0};
	uint16 wstr[100] = {0};
	GUI_UTF8ToWstr(wstr,30, data, strlen(data)+1);
	text.wstr_len = MMIAPICOM_Wstrlen(wstr);
	text.wstr_ptr = wstr;

	item_t.item_style = GUIITEM_SYTLE_DSL_CHECK;

	item_t.item_data_ptr = &item_data;
	item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
	item_data.item_content[0].item_data.text_buffer = text;

	item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
	item_data.item_content[1].item_data.image_id = image_id;

	GUILIST_AppendItem(ctrl_id, &item_t);
}
LOCAL void LayoutItem(MMI_CTRL_ID_T ctrl_id, GUI_RECT_T rect) 
{
	GUI_BOTH_RECT_T both_rect = {0};
	both_rect.h_rect.right = rect.right;
	both_rect.h_rect.left = rect.left;
	both_rect.h_rect.top = rect.top;
	both_rect.h_rect.bottom = rect.bottom;

	both_rect.v_rect.right = rect.right;
	both_rect.v_rect.left = rect.left;
	both_rect.v_rect.top = rect.top;
	both_rect.v_rect.bottom = rect.bottom;
	GUIAPICTRL_SetBothRect(ctrl_id, &both_rect);

	return;
}
LOCAL void InitButton(MMI_CTRL_ID_T ctrl_id)
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

LOCAL void loadLocalBookCH(char * path,char *basepath)
{
	int i = 0;

	char * data = PNULL;
	int data_size;

	dsl_chapter_display_count=0;
	dsl_chapter_display_idx=0;
	releaseChapters();

	data = zmt_file_data_read(path,&data_size);
	if(data_size>0){
		cJSON *root = cJSON_Parse(data);
		cJSON *chList = cJSON_GetObjectItem(root, "F_list");
		if (chList != NULL && chList->type != cJSON_NULL) 
		{
			for (i = 0; i < cJSON_GetArraySize(chList)&&i<MAX_CHAPTER_NUM; i++) 
			{
				char chpath[255]={0};
				char path_h[255]={0};
				uint16 wstr[255] = {0};
				cJSON *item = cJSON_GetArrayItem(chList, i);
				cJSON *name = cJSON_GetObjectItem(item, "F_chapter_name");
				cJSON *id = cJSON_GetObjectItem(item, "F_chapter_id");
				strcat(chpath,basepath);
				GUI_UTF8ToWstr(wstr, 255, name->valuestring, strlen(name->valuestring)+1); 
				GUI_WstrToGB(path_h,wstr, MMIAPICOM_Wstrlen(wstr));
				strcat(chpath,path_h);
				strcat(chpath,"\\");
				strcat(chpath, path_h);
				StrReplace(chpath,"?","");
				StrReplace(chpath,".","");
				strcat(chpath,".json");
				addChapterInfo(i,  name->valuestring,  id->valueint,chpath);
				dsl_chapter_display_count++;
			}
			cJSON_Delete(root);
		}
		SCI_FREE(data);
	}
	if(dsl_chapter_display_count==0)
	{
		dsl_chapter_display_count=-2;
	}
	MMK_PostMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	
}

LOCAL void requestWordCh(){
	char url[255]={0};
	if(is_load_local)
	{
		loadLocalBookCH(books[pageInfo->bookIdx]->path,books[pageInfo->bookIdx]->basepath);
	}
	else
	{
		sprintf(url, "v1/card/book/chapterList?F_book_id=%s&F_filter_word=1",books[pageInfo->bookIdx]->id);
		SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
		request_http_win_ch_idx++;
		MMIZDT_HTTP_AppSend(TRUE, BASE_URL_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, parseChapterResponse);
	}
}

LOCAL void openChRequest()
{
	dsl_chapter_display_count=0;
	dsl_chapter_display_idx=0;
	requestWordCh();
	openCHWin();
}

LOCAL MMI_RESULT_E HandleWordWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
  MMI_RESULT_E recode = MMI_RESULT_TRUE;
  switch (msg_id) {
    case MSG_OPEN_WINDOW:
    {
		GUI_FONT_ALL_T font = {0};
		GUI_RECT_T title_rect = {0};
		is_load_local=0;
		is_record=0;

		if (pageInfo == PNULL) {
			pageInfo = SCI_ALLOCA(sizeof(ZMT_PAGE_INFO));
		}
		requestBookInfo();
		
		font.font = DP_FONT_22;
		font.color = MMI_WHITE_COLOR;
        
		GUIBUTTON_SetRect(MMI_ZMT_WORD_MAIN_LABEL_BACK, &word_title_rect);      
		GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_MAIN_LABEL_BACK, closeWordWin);
		GUIBUTTON_SetFont(MMI_ZMT_WORD_MAIN_LABEL_BACK, &font);
		GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_MAIN_LABEL_BACK,ALIGN_HVMIDDLE);

		GUILIST_SetRect(MMI_ZMT_WORD_MAIN_LIST_CONTENT, &word_list_rect);
		GUILIST_SetListState(MMI_ZMT_WORD_MAIN_LIST_CONTENT,GUILIST_STATE_SPLIT_LINE, TRUE);
		GUILIST_SetListState(MMI_ZMT_WORD_MAIN_LIST_CONTENT,GUILIST_STATE_NEED_HIGHTBAR, FALSE);
		GUILIST_SetListState(MMI_ZMT_WORD_MAIN_LIST_CONTENT, GUILIST_STATE_EFFECT_STR,TRUE);
		GUILIST_SetTopItemOffset(MMI_ZMT_WORD_MAIN_LIST_CONTENT, 10);
		GUILIST_SetMaxItem(MMI_ZMT_WORD_MAIN_LIST_CONTENT, DSL_WORD_MAIN_DISPLAY_NUM, FALSE);
		GUILIST_SetBgColor(MMI_ZMT_WORD_MAIN_LIST_CONTENT,GUI_RGB2RGB565(80, 162, 254));

		title_rect.top = word_title_rect.top + 2;
		GUILABEL_SetRect(MMI_ZMT_WORD_MAIN_LABEL_NUM_ID, &title_rect, FALSE);
		GUILABEL_SetFont(MMI_ZMT_WORD_MAIN_LABEL_NUM_ID, DP_FONT_16,MMI_WHITE_COLOR);
		GUILABEL_SetAlign(MMI_ZMT_WORD_MAIN_LABEL_NUM_ID, GUILABEL_ALIGN_RIGHT);

	}
	break;
    case MSG_GET_FOCUS:
	break;
    case MSG_LOSE_FOCUS:
	break;
	case MSG_FULL_PAINT: 
	{
		int i = 0;
		GUILIST_EMPTY_INFO_T empty_info = {0};
		GUISTR_STYLE_T empty_style = {0};

		GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};

		GUI_FillRect(&lcd_dev_info, word_win_rect, GUI_RGB2RGB565(80, 162, 254));
		GUI_FillRect(&lcd_dev_info, word_title_rect, GUI_RGB2RGB565(108, 181, 255));
		
		GUIBUTTON_SetVisible(MMI_ZMT_WORD_MAIN_BUTTON_REFRESH,FALSE,FALSE);

		GUIBUTTON_SetTextId(MMI_ZMT_WORD_MAIN_LABEL_BACK, WORD_TITLE);

		empty_info.text_id = WORD_LOADING;
		empty_style.font = DP_FONT_22;
		empty_style.font_color = MMI_WHITE_COLOR;
		GUILIST_SetEmptyStyle(MMI_ZMT_WORD_MAIN_LIST_CONTENT, &empty_style);
		GUILIST_SetEmptyInfo(MMI_ZMT_WORD_MAIN_LIST_CONTENT, &empty_info);
		GUILIST_RemoveAllItems(MMI_ZMT_WORD_MAIN_LIST_CONTENT);
		if(dsl_word_main_display_count>0)
		{
			for (i = 0;i < dsl_word_main_display_count; i++) 
			{
			    if (books[i] != PNULL) 
			    {
			        AppendDSLListItemByTextId(MMI_ZMT_WORD_MAIN_LIST_CONTENT,IMG_ZMT_CONTACT_ICON, i);
			    }
			}
			GUILIST_SetCurItemIndex(MMI_ZMT_WORD_MAIN_LIST_CONTENT, pageInfo->bookIdx);
			//setNumberText(MMI_ZMT_WORD_MAIN_LABEL_NUM_ID,dsl_word_main_display_idx, (dsl_word_main_display_count+DSL_WORD_MAIN_DISPLAY_NUM-1)/DSL_WORD_MAIN_DISPLAY_NUM);
			if(is_record==2)
			{
				OpenWordRecordTips();
			}
		}
		else if(dsl_word_main_display_count==-1)
		{
			empty_info.text_id = WORD_LOADING_FAILED;
			GUILIST_SetEmptyInfo(MMI_ZMT_WORD_MAIN_LIST_CONTENT, &empty_info);
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_MAIN_BUTTON_REFRESH,TRUE,TRUE);
		}
		else if(dsl_word_main_display_count==-2)
		{
			empty_info.text_id = WORD_NO_DATA;
			GUILIST_SetEmptyInfo(MMI_ZMT_WORD_MAIN_LIST_CONTENT, &empty_info);
		}
	}
	break;
    case MSG_KEYUP_RED:
    case MSG_KEYUP_CANCEL:
      closeWordWin();
      break;
    case MSG_CTL_PENOK:
	{
		if(GetCanntClick(0))
		{
			SCI_TRACE_LOW("%s can not click", __FUNCTION__);
			break;
		}
		pageInfo->bookIdx =GUILIST_GetCurItemIndex(MMI_ZMT_WORD_MAIN_LIST_CONTENT);
		openChRequest();
	}
	break;
	case MSG_CLOSE_WINDOW:
	{
		request_http_win_main_idx++;
		dsl_word_main_display_count=0;
		dsl_word_main_display_idx=0;
	}
	break;
	default:
        recode = MMI_RESULT_FALSE;
	break;
  }
  return recode;
}

LOCAL void autodisplay_loading_tip_timeout(uint8 timer_id,uint32 param)
{
	SCI_TRACE_LOW("DSLCHAT_WIN %s start", __FUNCTION__);
	if(0 != autodisplay_tip_timer)
	{
		MMK_StopTimer(autodisplay_tip_timer);
		autodisplay_tip_timer = 0;
	}
	is_autodisplay_tip=0;
	MMK_PostMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
}

LOCAL void autodisplay_loading_tip(uint type)
{
	SCI_TRACE_LOW("DSLCHAT_WIN %s start", __FUNCTION__);
	if(0 != autodisplay_tip_timer)
	{
		MMK_StopTimer(autodisplay_tip_timer);
		autodisplay_tip_timer = 0;
	}
	is_autodisplay_tip=type;
	MMK_PostMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	autodisplay_tip_timer = MMK_CreateTimerCallback(2000, autodisplay_loading_tip_timeout,(uint32)0, FALSE);
	MMK_StartTimerCallback(autodisplay_tip_timer, 2000, autodisplay_loading_tip_timeout, (uint32)0, FALSE);
}

LOCAL MMI_RESULT_E clickAutoPlay()
{
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	is_autoplay=1;
	autodisplay_loading_tip(1);
	MMK_SendMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL);
	return result;
}
LOCAL MMI_RESULT_E clickDisAutoPlay()
{
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	is_autoplay=0;
	autodisplay_loading_tip(2);
	MMK_SendMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL);
	return result;
}

GUI_LCD_DEV_INFO auto_tip_layer={0};

LOCAL MMI_RESULT_E HandleWordCHWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param) 
{
  MMI_RESULT_E recode = MMI_RESULT_TRUE;
  switch (msg_id) {
    case MSG_OPEN_WINDOW: 
	{
		GUI_RECT_T list_rect = {0, 25, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
		GUI_RECT_T title_rect = {0};
		GUI_BORDER_T btn_border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
		GUI_FONT_ALL_T font = {0};
		GUI_BG_T button_bg = {0};
		font.font = DP_FONT_18;
		font.color = MMI_WHITE_COLOR;

		GUIBUTTON_SetRect(MMI_ZMT_WORD_CH_PAD_LABEL_BACK, &word_title_rect);
		GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CH_PAD_LABEL_BACK, closeWordCHWin);
        
		GUIBUTTON_SetRect(MMI_ZMT_WORD_CH_LABEL_BACK, &word_title_rect);
		GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CH_LABEL_BACK, closeWordCHWin);
		GUIBUTTON_SetFont(MMI_ZMT_WORD_CH_LABEL_BACK, &font);
		GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_CH_LABEL_BACK,ALIGN_LVMIDDLE);

		GUIBUTTON_SetRect(MMI_ZMT_WORD_CH_BUTTON_AUTOPLAY, &word_auto_play_rect);
		GUIBUTTON_SetVisible(MMI_ZMT_WORD_CH_BUTTON_AUTOPLAY,TRUE,TRUE);
		
		font.font = DP_FONT_16;
		list_rect = word_list_rect;
		list_rect.bottom -= 1.5*WORD_CARD_LINE_HIGHT+5;
		GUILIST_SetRect(MMI_ZMT_WORD_CH_LIST_CONTENT, &list_rect);
		GUILIST_SetListState(MMI_ZMT_WORD_CH_LIST_CONTENT,GUILIST_STATE_SPLIT_LINE, TRUE);
		GUILIST_SetListState(MMI_ZMT_WORD_CH_LIST_CONTENT, GUILIST_STATE_FOCUS,TRUE);
		GUILIST_SetListState(MMI_ZMT_WORD_CH_LIST_CONTENT,GUILIST_STATE_NEED_HIGHTBAR, FALSE);
		GUILIST_SetMaxItem(MMI_ZMT_WORD_CH_LIST_CONTENT, MAX_CHAPTER_NUM, FALSE);
		GUILIST_SetTextFont(MMI_ZMT_WORD_CH_LIST_CONTENT, DP_FONT_16, MMI_WHITE_COLOR);
		GUILIST_SetBgColor(MMI_ZMT_WORD_CH_LIST_CONTENT,GUI_RGB2RGB565(80, 162, 254));	

		GUIBUTTON_SetRect(MMI_ZMT_WORD_CH_BUTTON_PACTISE, &word_left_rect);//单词学习
		GUIBUTTON_SetRect(MMI_ZMT_WORD_CH_BUTTON_EXERCISE, &word_right_rect);//生词本
		InitButton(MMI_ZMT_WORD_CH_BUTTON_PACTISE);
		InitButton(MMI_ZMT_WORD_CH_BUTTON_EXERCISE);

		GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CH_BUTTON_PACTISE,openWordCardWin);
		//SCI_TRACE_LOW("%s: is_load_local = %d", __FUNCTION__, is_load_local);
		if(is_load_local)
		{
			GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CH_BUTTON_EXERCISE,openWordCardWin);
		}else
		{
			GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CH_BUTTON_EXERCISE,openNewWordCardWin);
		}

		LayoutItem(MMI_ZMT_WORD_CH_BUTTON_REFRESH, word_tip_rect);
		InitButton(MMI_ZMT_WORD_CH_BUTTON_REFRESH);
		GUIBUTTON_SetTextId(MMI_ZMT_WORD_CH_BUTTON_REFRESH, WORD_REFRESH);
		GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CH_BUTTON_REFRESH,wordChRefresh);

		if (UILAYER_IsMultiLayerEnable())
		{
			UILAYER_CREATE_T create_info = {0};
			create_info.lcd_id = MAIN_LCD_ID;
			create_info.owner_handle = win_id;
			create_info.offset_x = word_auto_tip_rect.left;
			create_info.offset_y = word_auto_tip_rect.top;
			create_info.width = word_auto_tip_rect.right;
			create_info.height = word_auto_tip_rect.bottom;
			create_info.is_bg_layer = FALSE;
			create_info.is_static_layer = FALSE;   
			UILAYER_CreateLayer(&create_info, &auto_tip_layer);
		}
		dsl_chapter_select_idx=0;
	}
	break;
    case MSG_GET_FOCUS:
      break;
    case MSG_LOSE_FOCUS:
      break;
    case MSG_FULL_PAINT: 
	{
		int i = 0;
		GUILIST_EMPTY_INFO_T empty_info = {0};
		GUISTR_STYLE_T empty_style = {0};
		GUI_BG_T auto_play_bg = {0};
		GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
		MMI_STRING_T string = {0};
		wchar text[100] = {0};
		wchar text_str[200] = {0};
        
		GUI_FillRect(&lcd_dev_info, word_win_rect, GUI_RGB2RGB565(80, 162, 254));
		GUI_FillRect(&lcd_dev_info, word_title_rect, GUI_RGB2RGB565(108, 181, 255));

		GUIBUTTON_SetVisible(MMI_ZMT_WORD_CH_BUTTON_REFRESH,FALSE,FALSE);
		if(is_autoplay)
		{
                    auto_play_bg.bg_type = GUI_BG_IMG;
                    auto_play_bg.img_id = IMG_AUTO_PLAY;
                    GUIBUTTON_SetBg(MMI_ZMT_WORD_CH_BUTTON_AUTOPLAY, &auto_play_bg);
                    GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CH_BUTTON_AUTOPLAY, clickDisAutoPlay);
		}
		else
		{
                    auto_play_bg.bg_type = GUI_BG_IMG;
                    auto_play_bg.img_id = IMG_DISAUTO_PLAY;
                    GUIBUTTON_SetBg(MMI_ZMT_WORD_CH_BUTTON_AUTOPLAY, &auto_play_bg);
			GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_CH_BUTTON_AUTOPLAY, clickAutoPlay);
		}

		empty_info.text_id = WORD_LOADING;
		empty_style.font = DP_FONT_22;
		empty_style.font_color = MMI_WHITE_COLOR;
		GUILIST_SetEmptyStyle(MMI_ZMT_WORD_CH_LIST_CONTENT, &empty_style);
		GUILIST_SetEmptyInfo(MMI_ZMT_WORD_CH_LIST_CONTENT, &empty_info);
		
		sprintf(text, "%s%s", books[pageInfo->bookIdx]->name, books[pageInfo->bookIdx]->editionName);
		GUI_UTF8ToWstr(text_str, 200, text, strlen(text));
		string.wstr_ptr = text_str;
		string.wstr_len = MMIAPICOM_Wstrlen(text_str);
		GUIBUTTON_SetText(MMI_ZMT_WORD_CH_LABEL_BACK, string.wstr_ptr, string.wstr_len);
		
		GUIBUTTON_SetTextId(MMI_ZMT_WORD_CH_BUTTON_PACTISE, WORD_PACTISE);
		GUIBUTTON_SetTextId(MMI_ZMT_WORD_CH_BUTTON_EXERCISE, WORD_EXERCISE);
		GUIBUTTON_SetVisible(MMI_ZMT_WORD_CH_BUTTON_EXERCISE,FALSE,FALSE);
		GUIBUTTON_SetVisible(MMI_ZMT_WORD_CH_BUTTON_PACTISE,FALSE,FALSE);
		
		GUILIST_RemoveAllItems(MMI_ZMT_WORD_CH_LIST_CONTENT);
		if(dsl_chapter_display_count>0)
		{
			for (i = 0; i < dsl_chapter_display_count; i++) {
			    if (chapters[i] != PNULL) {
			        if (i == dsl_chapter_select_idx) {
					appendCHItem(MMI_ZMT_WORD_CH_LIST_CONTENT, chapters[i]->name,IMG_ZMT_SELECTED);
			        }
			        else
			        {
					appendCHItem(MMI_ZMT_WORD_CH_LIST_CONTENT, chapters[i]->name,IMG_ZMT_UNSELECTED);
			        }
			    }
			}
			GUILIST_SetCurItemIndex(MMI_ZMT_WORD_CH_LIST_CONTENT, dsl_chapter_select_idx);
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_CH_BUTTON_EXERCISE,TRUE,TRUE);
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_CH_BUTTON_PACTISE,TRUE,TRUE);
		}
		else if(dsl_chapter_display_count==-2)
		{
			GUILIST_SetRect(MMI_ZMT_WORD_CH_LIST_CONTENT, &word_list_rect);
			empty_info.text_id = WORD_NO_DATA;
			GUILIST_SetEmptyInfo(MMI_ZMT_WORD_CH_LIST_CONTENT, &empty_info);
		}
		else if(dsl_chapter_display_count==-1)
		{
			GUILIST_SetRect(MMI_ZMT_WORD_CH_LIST_CONTENT, &word_list_rect);
			empty_info.text_id = WORD_LOADING_FAILED;
			GUILIST_SetEmptyInfo(MMI_ZMT_WORD_CH_LIST_CONTENT, &empty_info);
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_CH_BUTTON_REFRESH,TRUE,TRUE);
		}

		if(is_autodisplay_tip!=0)
			{
				UILAYER_APPEND_BLT_T append_layer = {0};
				
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar text_str[35] = {0};
				char count_str[35] = {0};

				append_layer.lcd_dev_info = auto_tip_layer;
				append_layer.layer_level = UILAYER_LEVEL_HIGH;
				UILAYER_AppendBltLayer(&append_layer);

				LCD_FillRoundedRect(&auto_tip_layer, word_tip_rect, word_tip_rect, MMI_WHITE_COLOR);

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_16;
				text_style.font_color = GUI_RGB2RGB565(80, 162, 254);

				if(is_autodisplay_tip==1)
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
				(const GUI_LCD_DEV_INFO *)&auto_tip_layer,
				&word_tip_rect,
				&word_tip_rect,
				&text_string,
				&text_style,
				GUISTR_STATE_ALIGN,
				GUISTR_TEXT_DIR_AUTO
				);
			}
			else
			{
				UILAYER_RemoveBltLayer(&auto_tip_layer);
			}
	} 
	break;
	case MSG_CLOSE_WINDOW: 
	{
		request_http_win_ch_idx++;
		dsl_chapter_display_count=0;
		dsl_chapter_display_idx=0;
	} 
	break;
	case MSG_NOTIFY_PENOK: 
	{
		uint index = GUILIST_GetCurItemIndex(MMI_ZMT_WORD_CH_LIST_CONTENT);
		if(dsl_chapter_select_idx!=index){
			dsl_chapter_select_idx=index;
			MMK_SendMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL);
		}
	} 
	break;
    case MSG_KEYUP_RED:
	case MSG_KEYUP_CANCEL:
		closeWordCHWin();
	break;
	default:
     recode = MMI_RESULT_FALSE;
	break;
	}
	return recode;
}

LOCAL void openNextCh()
{
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return;
	}
	if(newWords!=PNULL){
		char url[200] = {0};
		char json[350]={0};
		if(newWords_ex!=PNULL)
		{
			SCI_FREE(newWords_ex);
			newWords_ex=PNULL;
		}
		newWordsReNum=0;
		newWords_ex=newWords;
		newWords=PNULL;
		sprintf(json,"{\"cardId\":\"%s\",\"bookId\":\"%s\",\"sectionId\":\"%s\",\"word\":\"%s\"}",
					BASE_DEVICE_IMEI,books[pageInfo->bookIdx]->id, chapters[pageInfo->chapterIdx]->id,newWords_ex);
		sprintf(url, "%s%s", BASE_URL_PATH, "v1/card/word/collect");
		MMIZDT_HTTP_AppSend(FALSE, url, json, strlen(json), 1000, 0, 0, 0, 0, 0, parseNewWordResponse);
	}
	pageInfo->chapterIdx++;
	pageInfo->total = 0;
	pageInfo->realIdx = 0;
	pageInfo->curPage = 1;
	pageInfo->curPageNum = 0;
	pageInfo->curIdx = 0;
	releaseWords();
	if(is_load_local)
	{
		loadLocalWords(chapters[pageInfo->chapterIdx]->path);
	}
	else
	{
		requestWordInfo();
		MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
}


LOCAL uint8 time_auto_play=0;

LOCAL void autoTimeOut(uint8 timer_id, uint32 param) {
	if(time_auto_play!=0){
		MMK_StopTimer(time_auto_play);
		time_auto_play=0;
	}
	PlayWordUkSound();
}

LOCAL void loadLocalWordMp3(int position)
{
	char * data = PNULL;
	int data_size;
	char path[255]={0};
	char path_h[255]={0};
	uint16 wstr[255] = {0};
	GUI_UTF8ToWstr(wstr, 255, words[position]->uk_audio_data, strlen(words[position]->uk_audio_data)+1); 
	GUI_WstrToGB(path_h,wstr, MMIAPICOM_Wstrlen(wstr));

	strcat(path,LOCAL_BOOK_DIR);
	strcat(path,path_h);

	data = zmt_file_data_read(path,&data_size);
	if(data_size>0&&data!=PNULL)
	{
		words[position]->uk_audio_data_len=data_size;
		if(words[position]->uk_audio_data!=PNULL)
		{
			SCI_FREE(words[position]->uk_audio_data);
			words[position]->uk_audio_data=PNULL;
			
		}
		words[position]->uk_audio_data = SCI_ALLOCA(data_size);
		SCI_MEMSET(words[position]->uk_audio_data, 0, data_size);
		SCI_MEMCPY(words[position]->uk_audio_data, data, data_size);
		if(is_autoplay)
		{
			if(time_auto_play!=0){
				MMK_StopTimer(time_auto_play);
				time_auto_play=0;
			}
			time_auto_play = MMK_CreateTimerCallback(500, autoTimeOut, 0, FALSE);
			MMK_StartTimerCallback(time_auto_play, 500,autoTimeOut,0,FALSE);
		}
		SCI_FREE(data);
	}
	else
	{
		words[position]->uk_audio_data_len=-2;
	}
}

LOCAL void LoadWordInfo(int position) {
    GUI_RECT_T uk_rect = {5, 36, 125, 58};
	MMI_STRING_T text = {0, };
	MMI_STRING_T text_en = {0};
	MMI_STRING_T text_word = {0};
	MMI_STRING_T text_us = {0};
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};

	uint16 wstr[2048] = {0};
	uint16 wstr_uk[64] = {0};
	uint16 wstr_us[64] = {0};
	uint16 wstr_word[64] = {0};

	uint8 text_width_piex=0;

	//单词
    GUILABEL_SetRect(MMI_ZMT_WORD_MSG_LABEL_ID, &word_word_rect, FALSE);
	GUI_UTF8ToWstr(wstr_word, 64, words[position]->word, strlen(words[position]->word)+1); 
	text_word.wstr_len = MMIAPICOM_Wstrlen(wstr_word);
	text_word.wstr_ptr = wstr_word;
	GUILABEL_SetText(MMI_ZMT_WORD_MSG_LABEL_ID, &text_word, FALSE);
				
	//音标1
	GUILABEL_SetRect(MMI_ZMT_WORD_MSG_SP1_ID, &word_uk_rect, FALSE);
	GUI_UTF8ToWstr(wstr_uk, 64, words[position]->uk, strlen(words[position]->uk)+1);
	text_en.wstr_len = MMIAPICOM_Wstrlen(wstr_uk);
	text_en.wstr_ptr = wstr_uk;
	GUILABEL_SetText(MMI_ZMT_WORD_MSG_SP1_ID, &text_en, FALSE);
	text_width_piex=GUI_CalculateStringPiexlNum(text_en.wstr_ptr, text_en.wstr_len, DP_FONT_16, 1);

	if(text_width_piex>100)
	{
		GUILABEL_SetVisible(MMI_ZMT_WORD_MSG_SP1_ID,FALSE,FALSE);
		text_width_piex=GUI_CalculateStringPiexlNum(text_word.wstr_ptr, text_word.wstr_len, DP_FONT_20, 1);
		if(text_width_piex>100)
		{
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_SP1_BTN_ID,FALSE,FALSE);
		}else{
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_SP1_BTN_ID,TRUE,FALSE);
			uk_rect.left = text_width_piex + 10;
			uk_rect.right = uk_rect.left + 30;
			GUIBUTTON_SetRect(MMI_ZMT_WORD_MSG_SP1_BTN_ID, &uk_rect);
			GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_MSG_SP1_BTN_ID,PlayWordUkSound);
		}
		LCD_DrawHLine(&lcd_dev_info, word_uk_rect.left, uk_rect.bottom+5, word_uk_rect.right, MMI_WHITE_COLOR);
	}
	else
	{
		GUILABEL_SetVisible(MMI_ZMT_WORD_MSG_SP1_ID,TRUE,FALSE);
		GUIBUTTON_SetVisible(MMI_ZMT_WORD_MSG_SP1_BTN_ID,TRUE,FALSE);
		uk_rect.left = text_width_piex + 10;
		uk_rect.right = uk_rect.left + 30;
		GUIBUTTON_SetRect(MMI_ZMT_WORD_MSG_SP1_BTN_ID, &uk_rect);
		GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_MSG_SP1_BTN_ID,PlayWordUkSound);
		LCD_DrawHLine(&lcd_dev_info, word_uk_rect.left, uk_rect.bottom+5, word_uk_rect.right, MMI_WHITE_COLOR);
	}

	//单词信息
	GUITEXT_SetRect(MMI_ZMT_WORD_INFO_TEXT_ID, &word_text_rect);
	GUI_UTF8ToWstr(wstr, 2048, words[position]->explain, strlen(words[position]->explain)+1);
	text.wstr_len = MMIAPICOM_Wstrlen(wstr);
	text.wstr_ptr = wstr;
	GUITEXT_SetString(MMI_ZMT_WORD_INFO_TEXT_ID, text.wstr_ptr,text.wstr_len, TRUE);

	if(pageInfo->realIdx<pageInfo->total)
	{
		if (words[pageInfo->curIdx] != PNULL&&pageInfo->curPageNum>0) 
		{
			if(words[pageInfo->curIdx]->uk_audio_data_len==-1)
			{
				if(is_load_local)
				{
					loadLocalWordMp3(pageInfo->curIdx);
				}
				else
				{
					if(words[pageInfo->curIdx]->uk_audio_data!=PNULL)
					{
						words[pageInfo->curIdx]->uk_audio_request_idx=++mp3_idx;
						words[pageInfo->curIdx]->uk_audio_data_len=0;
                                        MMIZDT_HTTP_AppSend(TRUE, words[pageInfo->curIdx]->uk_audio_data, PNULL, 0, 1000, 0, 0, 6000, 0, 0, parseMp3Response);
					}
				}
			}
		}
	}
	saveRecord();
}

GUI_LCD_DEV_INFO tip_layer={0};
LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;

LOCAL MMI_RESULT_E HandleWordCardWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param) 
{
  MMI_RESULT_E recode = MMI_RESULT_TRUE;
	switch (msg_id) 
	{
		case MSG_OPEN_WINDOW: 
		{
			GUI_FONT_ALL_T font = {0};
			GUI_RECT_T title_rect = {0, 0, MMI_MAINSCREEN_WIDTH-WORD_CARD_LINE_WIDTH, WORD_CARD_LINE_HIGHT};
			GUI_FONT_T text_font = DP_FONT_16;
			GUI_COLOR_T text_color = MMI_WHITE_COLOR;
			GUI_BG_T bg = {0};
            
			font.font = DP_FONT_18;
			font.color = MMI_WHITE_COLOR;

			GUIBUTTON_SetRect(MMI_ZMT_WORD_LABEL_TITLE_ID, &title_rect);
			GUIBUTTON_SetFont(MMI_ZMT_WORD_LABEL_TITLE_ID, &font);
			GUIBUTTON_SetTextAlign(MMI_ZMT_WORD_LABEL_TITLE_ID,ALIGN_LVMIDDLE);
			GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_LABEL_TITLE_ID,closeWordCardWin);

			GUIBUTTON_SetRect(MMI_ZMT_WORD_LABEL_MESSAGE_ID, &word_msg_rect);
			InitButton(MMI_ZMT_WORD_LABEL_MESSAGE_ID);
			
			font.font = DP_FONT_16;
			title_rect.left = title_rect.right;
			title_rect.right = MMI_MAINSCREEN_WIDTH;
			GUILABEL_SetRect(MMI_ZMT_WORD_LABEL_NUM_ID, &title_rect, FALSE);
			GUILABEL_SetFont(MMI_ZMT_WORD_LABEL_NUM_ID, DP_FONT_16,MMI_WHITE_COLOR);
			GUILABEL_SetFont(MMI_ZMT_WORD_MSG_SP1_ID, DP_FONT_16,MMI_WHITE_COLOR);
			GUILABEL_SetFont(MMI_ZMT_WORD_MSG_LABEL_ID, DP_FONT_20,MMI_WHITE_COLOR);

			//单词信息
			GUITEXT_SetRect(MMI_ZMT_WORD_INFO_TEXT_ID, &word_text_rect);
			GUITEXT_SetFont(MMI_ZMT_WORD_INFO_TEXT_ID, &text_font,&text_color);
			GUITEXT_IsDisplayPrg(FALSE, MMI_ZMT_WORD_INFO_TEXT_ID);
			GUITEXT_SetHandleTpMsg(FALSE, MMI_ZMT_WORD_INFO_TEXT_ID);
			GUITEXT_SetClipboardEnabled(MMI_ZMT_WORD_INFO_TEXT_ID,FALSE);
			GUITEXT_IsSlide(MMI_ZMT_WORD_INFO_TEXT_ID,FALSE);
			bg.bg_type = GUI_BG_COLOR;
			bg.color = GUI_RGB2RGB565(80, 162, 254);
			GUITEXT_SetBg(MMI_ZMT_WORD_INFO_TEXT_ID, &bg);

			LayoutItem(MMI_ZMT_WORD_BUTTON_REFRESH, word_tip_rect);
			InitButton(MMI_ZMT_WORD_BUTTON_REFRESH);
			GUIBUTTON_SetTextId(MMI_ZMT_WORD_BUTTON_REFRESH, WORD_REFRESH);
			GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_REFRESH,wordRefresh);

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
				UILAYER_CreateLayer(&create_info, &tip_layer);
			}
		}
		break;
		case MSG_TP_PRESS_UP:
		{
			if(pageInfo->total>0&&words[pageInfo->curIdx] != PNULL&&pageInfo->curPageNum>0)
			{
				int16 tp_offset_x = MMK_GET_TP_X(param) - main_tp_down_x;
				int16 tp_offset_y = MMK_GET_TP_Y(param) - main_tp_down_y;
				if(ABS(tp_offset_x) <= ABS(tp_offset_y))
				{
					if(tp_offset_y > 40)
					{
						MMK_PostMsg(MMI_ZMT_WORD_INFO_TEXT_ID, MSG_KEYREPEAT_UP, PNULL, 0);
					}
					else if(tp_offset_y < -40)
					{
						MMK_PostMsg(MMI_ZMT_WORD_INFO_TEXT_ID, MSG_KEYREPEAT_DOWN, PNULL, 0);
					}
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
		case MSG_FULL_PAINT: 
		{
			MMI_STRING_T title_text = {0};
			uint16 title_wstr[100] = {0};
			MMI_STRING_T text = {0};
			int16 wstr[100] = {0};
			GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
            
			GUI_FillRect(&lcd_dev_info, word_win_rect, GUI_RGB2RGB565(80, 162, 254));
			GUI_FillRect(&lcd_dev_info, word_title_rect, GUI_RGB2RGB565(108, 181, 255));

			GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_REFRESH,FALSE,FALSE);
            
			if(is_new_word)
			{
				GUIBUTTON_SetTextId(MMI_ZMT_WORD_LABEL_TITLE_ID, WORD_EXERCISE);
			}
			else
			{
				GUI_UTF8ToWstr(title_wstr,30, chapters[pageInfo->chapterIdx]->name, strlen(chapters[pageInfo->chapterIdx]->name)+1);
				title_text.wstr_len = MMIAPICOM_Wstrlen(title_wstr);
				title_text.wstr_ptr = title_wstr;
				GUIBUTTON_SetText(MMI_ZMT_WORD_LABEL_TITLE_ID, title_text.wstr_ptr,title_text.wstr_len);
			}
			
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_CACEL_ID,FALSE,FALSE);
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_OK_ID,FALSE,FALSE);
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_NEXT_ID,FALSE,FALSE);
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_PRE_ID,FALSE,FALSE);
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_DELETE_ID,FALSE,FALSE);
			GUILABEL_SetVisible(MMI_ZMT_WORD_LABEL_NUM_ID,FALSE,FALSE);
			
			GUIBUTTON_SetVisible(MMI_ZMT_WORD_LABEL_MESSAGE_ID,TRUE,TRUE);
			GUIBUTTON_SetTextId(MMI_ZMT_WORD_LABEL_MESSAGE_ID, WORD_LOADING);
			if(pageInfo->total==-1)
			{
				GUIBUTTON_SetTextId(MMI_ZMT_WORD_LABEL_MESSAGE_ID, WORD_LOADING_FAILED);
				GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_REFRESH,TRUE,TRUE);
			}else if(pageInfo->total==-2)
			{
				if(is_new_word)
				{
					GUIBUTTON_SetTextId(MMI_ZMT_WORD_LABEL_MESSAGE_ID, NEW_WORD_BOOK_NO_DATA);
				}else
				{
					GUIBUTTON_SetTextId(MMI_ZMT_WORD_LABEL_MESSAGE_ID, WORD_NO_DATA);
				}
			}else if(pageInfo->total>0)
			{
				if(pageInfo->realIdx<pageInfo->total)
				{
					setNumberText(MMI_ZMT_WORD_LABEL_NUM_ID,pageInfo->realIdx, pageInfo->total);
					GUILABEL_SetVisible(MMI_ZMT_WORD_LABEL_NUM_ID,TRUE,TRUE);
					if (words[pageInfo->curIdx] != PNULL&&pageInfo->curPageNum>0) 
					{
						GUIBUTTON_SetVisible(MMI_ZMT_WORD_LABEL_MESSAGE_ID,FALSE,TRUE);
						
						LoadWordInfo(pageInfo->curIdx);

						if(is_new_word)
						{
							if(pageInfo->realIdx!=0)
							{
								GUIBUTTON_SetRect(MMI_ZMT_WORD_BUTTON_PRE_ID, &word_pre_rect);
								GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_PRE_ID,openNewWordPreCard);
								GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_PRE_ID,TRUE,TRUE);
							}
							if(pageInfo->realIdx+1<pageInfo->total)
							{
								GUIBUTTON_SetRect(MMI_ZMT_WORD_BUTTON_NEXT_ID, &word_next_rect);
								GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_NEXT_ID,openNewWordNextCard);
								GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_NEXT_ID,TRUE,TRUE);
							}
							GUIBUTTON_SetRect(MMI_ZMT_WORD_BUTTON_DELETE_ID, &word_del_rect);
							GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_DELETE_ID,deleteNewWordCard);
							GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_DELETE_ID,TRUE,TRUE);							
						}else
						{
							GUI_FONT_ALL_T font = {0};
							GUI_BG_T button_bg = {0};
							GUI_BORDER_T btn_border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
							GUIBUTTON_SetRect(MMI_ZMT_WORD_BUTTON_OK_ID, &word_left_rect);
							InitButton(MMI_ZMT_WORD_BUTTON_OK_ID);
							GUIBUTTON_SetRect(MMI_ZMT_WORD_BUTTON_CACEL_ID, &word_right_rect);
							InitButton(MMI_ZMT_WORD_BUTTON_CACEL_ID);
							GUIBUTTON_SetTextId(MMI_ZMT_WORD_BUTTON_CACEL_ID, WORD_CACEL);
							GUIBUTTON_SetTextId(MMI_ZMT_WORD_BUTTON_OK_ID, WORD_OK);
							GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_CACEL_ID,TRUE,TRUE);
							GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_OK_ID,TRUE,TRUE);
							GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_CACEL_ID,setWordToNew);
							GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_OK_ID, openNextCard);
						}
					}
				}
				else
				{
					if(is_new_word)
					{
						GUIBUTTON_SetTextId(MMI_ZMT_WORD_LABEL_MESSAGE_ID, NEW_WORD_BOOK_FINISH);
					}else
					{
						if(chapters[pageInfo->chapterIdx+1]!=PNULL)
						{ 
							GUI_FONT_ALL_T font = {0};
							GUI_BG_T button_bg = {0};
							GUI_BORDER_T btn_border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
							GUIBUTTON_SetTextId(MMI_ZMT_WORD_LABEL_MESSAGE_ID, WORD_FINISH);
							GUIBUTTON_SetRect(MMI_ZMT_WORD_BUTTON_OK_ID, &word_left_rect);
							InitButton(MMI_ZMT_WORD_BUTTON_OK_ID);
							GUIBUTTON_SetRect(MMI_ZMT_WORD_BUTTON_CACEL_ID, &word_right_rect);
							InitButton(MMI_ZMT_WORD_BUTTON_CACEL_ID);
							GUIBUTTON_SetTextId(MMI_ZMT_WORD_BUTTON_CACEL_ID, WORD_NEXT_CH);
							GUIBUTTON_SetTextId(MMI_ZMT_WORD_BUTTON_OK_ID, WORD_BACK_CH);
							GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_CACEL_ID,TRUE,TRUE);
							GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_OK_ID,TRUE,TRUE);
							GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_CACEL_ID,openNextCh);
							GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_OK_ID, closeWordCardWin);
						}
						else
						{
							GUIBUTTON_SetTextId(MMI_ZMT_WORD_LABEL_MESSAGE_ID, WORD_BOOK_FINISH);
							LayoutItem(MMI_ZMT_WORD_BUTTON_OK_ID, word_tip_rect);
							InitButton(MMI_ZMT_WORD_BUTTON_OK_ID);
							GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_CACEL_ID,FALSE,FALSE);
							GUIBUTTON_SetTextId(MMI_ZMT_WORD_BUTTON_OK_ID, WORD_BACK_CH);
							GUIBUTTON_SetVisible(MMI_ZMT_WORD_BUTTON_OK_ID,TRUE,TRUE);
							GUIBUTTON_SetRect(MMI_ZMT_WORD_BUTTON_OK_ID, &word_left_rect);
							GUIBUTTON_SetCallBackFunc(MMI_ZMT_WORD_BUTTON_OK_ID, closeWordCardWin);

						}
					}
				}
			}

			if(is_display_tip!=0)
			{
				UILAYER_APPEND_BLT_T append_layer = {0};
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar text_str[35] = {0};
				char count_str[35] = {0};

				append_layer.lcd_dev_info = tip_layer;
				append_layer.layer_level = UILAYER_LEVEL_HIGH;
				UILAYER_AppendBltLayer(&append_layer);

				LCD_FillRoundedRect(&tip_layer, word_tip_rect, word_tip_rect, MMI_WHITE_COLOR);

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_18;
				text_style.font_color = GUI_RGB2RGB565(80, 162, 254);

				if(is_display_tip==1)
				{
					sprintf(count_str,"正在加载，请稍后");
				}else if(is_display_tip==2)
				{
					sprintf(count_str,"暂无音频");
				}else if(is_display_tip==3)
				{
					sprintf(count_str,"正在删除");
				}else if(is_display_tip==4)
				{
					sprintf(count_str,"删除成功");
				}else if(is_display_tip==5)
				{
					sprintf(count_str,"删除失败，请重试");
				}else 
				{
					sprintf(count_str,"请稍后重试");
				}
				GUI_GBToWstr(text_str, count_str, strlen(count_str));
				text_string.wstr_ptr = text_str;
				text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
				GUISTR_DrawTextToLCDInRect(
				(const GUI_LCD_DEV_INFO *)&tip_layer,
				&word_tip_rect,
				&word_tip_rect,
				&text_string,
				&text_style,
				GUISTR_STATE_ALIGN,
				GUISTR_TEXT_DIR_AUTO
				);
			}
			else
			{
				UILAYER_RemoveBltLayer(&tip_layer);
			}
		} 
		break;
		case MSG_END_FULL_PAINT:
		{
		}
		break;
		case MSG_KEYUP_RED:
		case MSG_KEYUP_CANCEL:
			closeWordCardWin();
		break;
		case MSG_CLOSE_WINDOW:
		{
			request_http_win_word_idx=0;
		}
		break;
		default:
          recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;
}

LOCAL MMISRV_HANDLE_T mp3_player_handle=PNULL;

LOCAL BOOLEAN word_PlayMp3Notify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
	MMISRVAUD_REPORT_T *report_ptr = PNULL;
	BOOLEAN result = TRUE;
	if(param != PNULL && handle > 0)
	{
		report_ptr = (MMISRVAUD_REPORT_T *)param->data;
		if(report_ptr != PNULL && handle == mp3_player_handle)
		{
			switch(report_ptr->report)
			{
				case MMISRVAUD_REPORT_END:
					{
						if (MMISRVAUD_REPORT_RESULT_STOP != report_ptr->data1)
						{
							if (MMISRVAUD_REPORT_RESULT_SUCESS != report_ptr->data1)
							{
								result = FALSE;
							}
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

LOCAL BOOLEAN word_RequestHandle( 
                        MMISRV_HANDLE_T *audio_handle,
                        MMISRVAUD_ROUTE_T route,
                        MMISRVAUD_TYPE_U *audio_data,
                        MMISRVMGR_NOTIFY_FUNC notify
                        )
{
	MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    
    req.notify = notify;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.duation = 0;
    audio_srv.eq_mode = 0;
    audio_srv.is_mixing_enable = FALSE;
    audio_srv.play_times = 1;
    audio_srv.all_support_route = route;
    audio_srv.volume = MMIAPISET_GetMultimVolume();

	audio_srv.info.record_file.type = audio_data->type;        
	audio_srv.info.record_file.fmt  = audio_data->record_file.fmt;
	audio_srv.info.record_file.name = audio_data->record_file.name;
	audio_srv.info.record_file.name_len = audio_data->record_file.name_len;    
	audio_srv.info.record_file.source   = audio_data->record_file.source;
	audio_srv.info.record_file.frame_len= audio_data->record_file.frame_len;
	audio_srv.volume = AUD_MAX_SPEAKER_VOLUME;

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

PUBLIC void word_playMp3Data(char* file_name)
{
	uint8 ret = 0;
	BOOLEAN result = FALSE;
	MMIRECORD_SRV_RESULT_E srv_result = MMIRECORD_SRV_RESULT_SUCCESS;
	MMISRV_HANDLE_T audio_handle = PNULL;
	uint16 full_path[255] = {0};
	uint16 full_path_len = 0;
	MMISRVAUD_TYPE_U audio_data  = {0};

	ChatStopMp3Data();

	full_path_len = GUI_GBToWstr(full_path, (const uint8*)file_name, SCI_STRLEN(file_name));
	audio_data.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
	audio_data.ring_file.name = full_path;
	audio_data.ring_file.name_len = full_path_len;
	audio_data.ring_file.fmt  = (MMISRVAUD_RING_FMT_E)MMIAPICOM_GetMusicType(audio_data.ring_file.name, audio_data.ring_file.name_len);

	if(listening_total_count > 0){
		result = word_RequestHandle(&audio_handle, MMISRVAUD_ROUTE_NONE, &audio_data, ChatPlayAudioDataNotify);
	}else{
		result = word_RequestHandle(&audio_handle, MMISRVAUD_ROUTE_NONE, &audio_data, word_PlayMp3Notify);
	}
   	if(result)
	{
		mp3_player_handle = audio_handle;
		MMISRVAUD_SetVolume(audio_handle, MMIAPISET_GetMultimVolume());
		if(!MMISRVAUD_Play(audio_handle, 0))
		{     
			SCI_TRACE_LOW("%s: MMISRVAUD_Play failed", __FUNCTION__);
			MMISRVMGR_Free(mp3_player_handle);
			mp3_player_handle = 0;
			ret = 1;
		}
	}
	SCI_TRACE_LOW("%s: ret = %d", __FUNCTION__, ret);
	if (ret != 0)
	{
		mp3_player_handle = NULL;
	}
}

LOCAL BOOLEAN ChatPlayMp3DataNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
	MMISRVAUD_REPORT_T *report_ptr = PNULL;

	if(param != PNULL && handle > 0)
	{
		report_ptr = (MMISRVAUD_REPORT_T *)param->data;
		if(report_ptr != PNULL && handle == mp3_player_handle)
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

LOCAL void ChatPlayMp3Data(uint8 *data,uint32 data_len)
{
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    BOOLEAN result = FALSE;

    // if(mp3_player_handle)
    // {
    //     MMISRVAUD_Stop(mp3_player_handle);
    //     MMISRVMGR_Free(mp3_player_handle);
    //     mp3_player_handle = NULL;
    // }

	req.is_auto_free = TRUE;
	if(listening_total_count > 0 && is_homework_task){
		req.notify = ChatPlayAudioDataNotify;
	}else{
		req.notify = ChatPlayMp3DataNotify;
	}
	req.pri = MMISRVAUD_PRI_NORMAL;

	// audio_srv.info.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
	// audio_srv.info.ring_file.fmt  = MMISRVAUD_RING_FMT_AMR;
	// audio_srv.info.ring_file.name = file_name;
	// audio_srv.info.ring_file.name_len = MMIAPICOM_Wstrlen(file_name);

	audio_srv.info.type = MMISRVAUD_TYPE_RING_BUF;
	audio_srv.info.ring_buf.fmt = MMISRVAUD_RING_FMT_MP3;
	audio_srv.info.ring_buf.data = data;
	audio_srv.info.ring_buf.data_len = data_len;

	// audio_srv.info.ring_buf.type = MMISRVAUD_TYPE_RING_BUF;
	// audio_srv.info.ring_buf.data= ac->pData;
	// audio_srv.info.ring_buf.data_len = ac->length;
	// audio_srv.info.ring_buf.fmt = MMISRVAUD_RING_FMT_AMR;

	audio_srv.volume=MMIAPISET_GetMultimVolume();
	SCI_TRACE_LOW("audio_srv.volume=MMIAPISET_GetMultimVolume();=%d",audio_srv.volume);

	audio_srv.all_support_route = MMISRVAUD_ROUTE_SPEAKER | MMISRVAUD_ROUTE_EARPHONE;//MMISRVAUD_ROUTE_SPEAKER | MMISRVAUD_ROUTE_EARPHONE;

	mp3_player_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);

	
	if(mp3_player_handle > 0)
	{
		SCI_TRACE_LOW("%s mp3_player_handle > 0", __FUNCTION__);
		result = MMISRVAUD_Play(mp3_player_handle, 0);
		if(!result)
		{
			SCI_TRACE_LOW("%s chat_player error", __FUNCTION__);
			MMISRVMGR_Free(mp3_player_handle);
			mp3_player_handle = 0;
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

LOCAL void ChatStopMp3Data(void)
{
	if(mp3_player_handle)
	{
		MMISRVAUD_Stop(mp3_player_handle);
		MMISRVMGR_Free(mp3_player_handle);
		mp3_player_handle = NULL;
	}
}

LOCAL void display_loading_tip_timeout(uint8 timer_id,uint32 param)
{
	SCI_TRACE_LOW("DSLCHAT_WIN %s start", __FUNCTION__);
	if(0 != display_tip_timer)
	{
		MMK_StopTimer(display_tip_timer);
		display_tip_timer = 0;
	}
	is_display_tip=0;
	MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
}

LOCAL void display_loading_tip(uint type)
{
	SCI_TRACE_LOW("DSLCHAT_WIN %s start", __FUNCTION__);
	if(0 != display_tip_timer)
	{
		MMK_StopTimer(display_tip_timer);
		display_tip_timer = 0;
	}
	is_display_tip=type;
	MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	display_tip_timer = MMK_CreateTimerCallback(2000, display_loading_tip_timeout,(uint32)0, FALSE);
	MMK_StartTimerCallback(display_tip_timer, 2000, display_loading_tip_timeout, (uint32)0, FALSE);
}

LOCAL void getWordAudioPath(char *path,char *audio_url)
{
	char path_str[50] = {0};
	char * str = audio_url;
	SCI_TRACE_LOW("%s: audio_url = %s", __FUNCTION__, audio_url);
	while ((str = strstr(str, "http://contres.readboy.com/resources/")) != NULL) {
		memmove(str, str + 38, strlen(str) - 37);
	}
	str = audio_url;
	while ((str = strstr(str, "rf4/")) != NULL) {
		memmove(str, str + 4, strlen(str) - 3);
	}
	str = audio_url;
	while ((str = strstr(str, "uk_word_sound/")) != NULL) {
		memmove(str, str + 14, strlen(str) - 13);
	}
	str = audio_url;
	while ((str = strstr(str, "/")) != NULL) {
		memmove(str, str + 1, strlen(str));
	}
	str = audio_url;
	while ((str = strstr(str, " ")) != NULL) {
		memmove(str, str + 1, strlen(str));
	}
	strncpy(path_str, audio_url, strlen(audio_url)-7);
	SCI_TRACE_LOW("%s: path_str = %s", __FUNCTION__, path_str);
	sprintf(path, LOCAL_AUDIO_PATH, path_str);
	SCI_TRACE_LOW("%s: path = %s", __FUNCTION__, path);
	SCI_FREE(audio_url);
}

LOCAL MMI_RESULT_E PlayWordUkSound(void)
{
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(is_display_tip==3)
	{
		return result;
	}
	SCI_TRACE_LOW("%s i %d uk len %d", __FUNCTION__,pageInfo->curIdx,words[pageInfo->curIdx]->uk_audio_data_len);
	if(words[pageInfo->curIdx]->uk_audio_data_len==0)
	{
		display_loading_tip(1);
	}
	else if(words[pageInfo->curIdx]->uk_audio_data_len==-1)
	{
		display_loading_tip(1);
		if(words[pageInfo->curIdx]->uk_audio_data!=PNULL)
		{
			words[pageInfo->curIdx]->uk_audio_request_idx=++mp3_idx;
			words[pageInfo->curIdx]->uk_audio_data_len=0;
                    MMIZDT_HTTP_AppSend(TRUE, words[pageInfo->curIdx]->uk_audio_data, PNULL, 0, 1000, 0, 0, 6000, 0, 0, parseMp3Response);
		}
	}
	else if(words[pageInfo->curIdx]->uk_audio_data_len==-2)
	{
		display_loading_tip(2);
	}
	else
	{
		ChatPlayMp3Data(words[pageInfo->curIdx]->uk_audio_data,words[pageInfo->curIdx]->uk_audio_data_len);
	}
	return MMI_RESULT_TRUE;
}

LOCAL void loadLocalBook(void)
{
	if(ZDT_Dir_Exsit(LOCAL_BOOK_DIR))
	{
		SFS_HANDLE	hFind    = PNULL;
		SFS_FIND_DATA_T sfsfind             = {0};
		BOOLEAN		result              = FALSE;
		wchar   full_path[MMIFILE_FULL_PATH_MAX_LEN+1] = {0}; 
		uint16  full_path_len = 0;
		uint16   tmp_len = 0;
		uint8 is_have_local=0;

		full_path_len = GUI_GBToWstr(full_path, (const uint8*)LOCAL_BOOK_DIR, SCI_STRLEN(LOCAL_BOOK_DIR));

		if( '//' != full_path[full_path_len-1] && 0x5c != full_path[full_path_len-1] )
		{
			full_path[full_path_len++] = 0x5c;
		}

		full_path[full_path_len] = 0x2a;
		full_path[full_path_len+1] = 0;

		hFind  = SFS_FindFirstFile(full_path,&sfsfind );
		
		if( hFind == PNULL)
		{
			return ;
		}
		
		do
		{
			char dir_name[255] = {0};
			uint32 uNameLen = 0;
			
			MMIAPICOM_WstrToStr((wchar *)sfsfind.name, dir_name);
			//SCI_TRACE_LOW("%s: dir_name = %s", __FUNCTION__, dir_name);
			if(0 != strcmp(dir_name, "audio")){							
				uNameLen = MMIAPICOM_Wstrlen( (wchar *)sfsfind.name );
				SCI_MEMCPY(&full_path[full_path_len], sfsfind.name, MIN(uNameLen*2+2, MMIFILE_FULL_PATH_MAX_LEN));
				full_path[full_path_len+uNameLen] = '\0';

				// 是目录
				if( sfsfind.attr & SFS_ATTR_DIR )
				{

					uint8 basepath[255]={0};
					uint8 path[255]={0};
					uint8 name[64]={0};
					uint8 press[32]={0};
					uint8 grade[32]={0};
					uint8 namegb[32]={0};
					char* p=PNULL;
					is_have_local=1;
					if(dsl_word_main_display_count<0)
					{
						dsl_word_main_display_count=0;
					}
					//folder
					GUI_WstrToUTF8(name, 64, sfsfind.name, MMIAPICOM_Wstrlen(sfsfind.name));
					p = strchr(name,'_');
					strncpy(press,name,strlen(name)-strlen(p));
					strcpy(grade,p+1);
					GUI_WstrToGB(namegb, sfsfind.name, MMIAPICOM_Wstrlen(sfsfind.name));
					GUI_WstrToGB(path,full_path, MMIAPICOM_Wstrlen(full_path));
					strcat(path,"\\");
					strcat(basepath,path);
					strcat(path,namegb);
					strcat(path,".json");
					addBookInfo(dsl_word_main_display_count++, grade, press,0,path,basepath);
				}
				else
				{

				}
			}
			result = SFS_FindNextFile(hFind, &sfsfind);
			
		}while(SFS_NO_ERROR == result);

		SFS_FindClose(hFind);

		if(is_have_local)
		{
			is_load_local=1;
			MMK_PostMsg(MMI_WORD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}
	}
}

LOCAL void requestBookInfo()
{
    char file_path[30] = {0};
    char * data_buf = NULL;
    uint32 data_size = 0;
    char url[255]={0};
    sprintf(url, "v1/card/bookList?F_card_id=%s", BASE_DEVICE_IMEI);
    SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);

    sprintf(file_path, "%s\\%s", LOCAL_BOOK_DIR, LOCAL_BOOK_INFO_BATH);
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &data_size);
        parseBookResponse(1, data_buf, data_size, 0);
    }else{
        MMIZDT_HTTP_AppSend(TRUE, BASE_URL_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, parseBookResponse);
    }
    request_http_win_main_idx++;
}

LOCAL void parseBookResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id) 
{
    if(pRcv != NULL && Rcv_len > 2)
    {
		uint i = 0;
		dsl_word_main_display_count=0;
		dsl_word_main_display_idx=0;
		releaseBooks();
		SCI_TRACE_LOW("%s: is_ok = %d", __FUNCTION__, is_ok);
		if (is_ok) {
			cJSON *root = cJSON_Parse(pRcv);
			cJSON *responseNo= cJSON_GetObjectItem(root, "F_responseNo");
			if(responseNo->valueint==10000)
			{
				cJSON *chList = cJSON_GetObjectItem(root, "F_list");
				if (chList != NULL && chList->type != cJSON_NULL) {
					for (i = 0; i < cJSON_GetArraySize(chList) && i < MAX_BOOK_NUM; i++) {
						cJSON *item = cJSON_GetArrayItem(chList, i);
						cJSON *name = cJSON_GetObjectItem(item, "F_book_name");
						cJSON *editionName = cJSON_GetObjectItem(item, "F_book_edition_name");
						cJSON *id = cJSON_GetObjectItem(item, "F_book_id");
						addBookInfo(i, name->valuestring, editionName->valuestring,id->valueint,PNULL,PNULL);
						dsl_word_main_display_count++;
					}
				}
			}
			cJSON_Delete(root);
			if(dsl_word_main_display_count==0)
			{
				dsl_word_main_display_count=-2;
			}
			MMK_PostMsg(MMI_WORD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
  		}else {
			dsl_word_main_display_count=-1;
			MMK_PostMsg(MMI_WORD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
  		}
		if(dsl_word_main_display_count>0)
		{
			int i=0;
			/*if(loadRecord())
			{
				for(i=0;i<dsl_word_main_display_count;i++)
				{
					if(strcmp(books[i]->id,pageInfo->bookId)==0)
					{
						pageInfo->bookIdx=i%DSL_WORD_MAIN_DISPLAY_NUM;
						is_record=2;
						MMK_PostMsg(MMI_WORD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
					}
				}
			}*/
		}
    }
	SCI_TRACE_LOW("%s: dsl_word_main_display_count = %d", __FUNCTION__, dsl_word_main_display_count);
	if(dsl_word_main_display_count<0)
	{
		loadLocalBook();
	}
}

LOCAL void parseChapterResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id) 
{
	if(pRcv != NULL && Rcv_len > 2)
	{
		int i = 0;
		dsl_chapter_display_count=0;
		dsl_chapter_display_idx=0;
		releaseChapters();
		if (is_ok) 
		{
			cJSON *root = cJSON_Parse(pRcv);
			cJSON *responseNo= cJSON_GetObjectItem(root, "F_responseNo");
			if(responseNo->valueint==10000)
			{
				cJSON *chList = cJSON_GetObjectItem(root, "F_list");
				if (chList != NULL && chList->type != cJSON_NULL) 
				{
					for (i = 0; i < cJSON_GetArraySize(chList)&&i<MAX_CHAPTER_NUM; i++) 
					{
						cJSON *item = cJSON_GetArrayItem(chList, i);
						cJSON *name = cJSON_GetObjectItem(item, "F_chapter_name");
						cJSON *id = cJSON_GetObjectItem(item, "F_chapter_id");
						addChapterInfo(i,  name->valuestring,  id->valueint,PNULL);
						dsl_chapter_display_count++;
					}
				}
			}
			cJSON_Delete(root);
			if(dsl_chapter_display_count==0)
			{
				dsl_chapter_display_count=-2;
			}
			MMK_PostMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		} 
		else 
		{
			dsl_chapter_display_count=-1;
			MMK_PostMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
  		}
		if(is_record==2&&dsl_chapter_display_count>pageInfo->chapterIdx)
		{
			requestWordInfo();
			MMK_CreateWin((uint32 *)MMI_WORD_CARD_WIN_TAB, PNULL);
			is_record=0;
		}
	}
}
LOCAL void parseWordResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id) 
{
	if(pRcv != NULL && Rcv_len > 2)
	{
		int i = 0;
		releaseWords();
		if (is_ok) 
		{
			cJSON *root = cJSON_Parse(pRcv);
			cJSON *responseNo= cJSON_GetObjectItem(root, "F_responseNo");
			pageInfo->curPageNum = 0;
			pageInfo->total=0;
			if(responseNo->valueint==10000)
			{
				cJSON *chList =PNULL;
				cJSON *count =cJSON_GetObjectItem(root, "F_total");
				if(is_new_word)
				{
					chList = cJSON_GetObjectItem(root, "F_word_list");
				}else
				{
					chList = cJSON_GetObjectItem(root, "F_list");
				}
				pageInfo->total = count->valueint;
				if (chList != NULL && chList->type != cJSON_NULL) 
				{
					for (i = 0; i < cJSON_GetArraySize(chList)&&i<MAX_WORD_NUM; i++) 
					{
						cJSON *item = cJSON_GetArrayItem(chList, i);
						cJSON *explain = cJSON_GetObjectItem(item, "explain");
						cJSON *word = cJSON_GetObjectItem(item, "word");
						cJSON *sentence = cJSON_GetObjectItem(item, "sentence");
						cJSON *sentence_explain = cJSON_GetObjectItem(item, "sentence_explain");
						cJSON *uk_bs = cJSON_GetObjectItem(item, "uk_bs");
						cJSON *uk_word_sound=cJSON_GetObjectItem(item, "uk_word_sound");
						cJSON *us_bs = cJSON_GetObjectItem(item, "us_bs");
						cJSON *us_word_sound=cJSON_GetObjectItem(item, "us_word_sound");
						if(sentence!=NULL&&sentence_explain!=NULL)
						{
							if (uk_bs == NULL && us_bs == NULL) 
							{
								addWordInfo(i, word->valuestring,sentence->valuestring,sentence_explain->valuestring, PNULL,PNULL,PNULL,PNULL,explain->valuestring);
							}
							else if (uk_bs == NULL) 
							{
								addWordInfo(i, word->valuestring,sentence->valuestring,sentence_explain->valuestring, us_bs->valuestring,us_word_sound->valuestring,PNULL,PNULL,explain->valuestring);
							}
							else if (us_bs == NULL) 
							{
								addWordInfo(i, word->valuestring,sentence->valuestring,sentence_explain->valuestring, PNULL,PNULL, uk_bs->valuestring,uk_word_sound->valuestring,explain->valuestring);
							} else {
								addWordInfo(i, word->valuestring,sentence->valuestring,sentence_explain->valuestring, us_bs->valuestring,us_word_sound->valuestring,uk_bs->valuestring,uk_word_sound->valuestring, explain->valuestring);
							}
						}else{
							if (uk_bs == NULL && us_bs == NULL) 
							{
								addWordInfo(i, word->valuestring, PNULL,PNULL, PNULL,PNULL,PNULL,PNULL,explain->valuestring);
							}
							else if (uk_bs == NULL) 
							{
								addWordInfo(i, word->valuestring, PNULL,PNULL, us_bs->valuestring,us_word_sound->valuestring,PNULL,PNULL,explain->valuestring);
							}
							else if (us_bs == NULL) 
							{
								addWordInfo(i, word->valuestring, PNULL,PNULL, PNULL,PNULL, uk_bs->valuestring,uk_word_sound->valuestring,explain->valuestring);
							} else {
								addWordInfo(i, word->valuestring, PNULL,PNULL, us_bs->valuestring,us_word_sound->valuestring,uk_bs->valuestring,uk_word_sound->valuestring, explain->valuestring);
							}
						}
						
						pageInfo->curPageNum++;
					}
				}
			}
			cJSON_Delete(root);
			if(pageInfo->total ==0)
			{
				pageInfo->total=-2;
			}
			if(is_display_tip==3)
			{
				display_loading_tip(4);
			}
			MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		} 
		else 
		{
			pageInfo->total=-1;
			MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}
	}
}

LOCAL MMI_RESULT_E closeWordCardWin() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(0))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	if(newWords!=PNULL){
		char url[200] = {0};
		char json[350]={0};
		if(newWords_ex!=PNULL)
		{
			SCI_FREE(newWords_ex);
			newWords_ex=PNULL;
		}
		newWordsReNum=0;
		newWords_ex=newWords;
		newWords=PNULL;
		sprintf(json,"{\"cardId\":\"%s\",\"bookId\":\"%s\",\"sectionId\":\"%s\",\"word\":\"%s\"}",
				BASE_DEVICE_IMEI,books[pageInfo->bookIdx]->id, chapters[pageInfo->chapterIdx]->id,newWords_ex);
		sprintf(url, "%s%s", BASE_URL_PATH, "v1/card/word/collect");
		MMIZDT_HTTP_AppSend(FALSE, url, json, strlen(json), 1000, 0, 0, 0, 0, 0, parseNewWordResponse);
	}

	MMK_CloseWin(MMI_WORD_CARD_WIN_ID);
	pageInfo->total = 0;
	pageInfo->realIdx = 0;
	pageInfo->curPage = 0;
	pageInfo->curPageNum = 0;
	pageInfo->curIdx = -1;
	releaseWords();
}

LOCAL MMI_RESULT_E wordMainRefresh(){
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	dsl_word_main_display_count = 0;
	requestBookInfo();
	MMK_PostMsg(MMI_WORD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	return result;
}

LOCAL MMI_RESULT_E wordChRefresh(){
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	dsl_chapter_display_count = 0;
	requestWordCh();
	MMK_PostMsg(MMI_WORD_CH_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	return result;
}

LOCAL MMI_RESULT_E wordRefresh() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	pageInfo->total = 0;
	requestWordInfo();
	MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	return result;
}

LOCAL void loadLocalWords(char *path)
{
	int i = 0, j=0;
	char * data = PNULL;
	int data_size;

	data=zmt_file_data_read(path,&data_size);
	releaseWords();

	if(data_size>0)
	{
		cJSON *root = cJSON_Parse(data);
		cJSON *chList = cJSON_GetObjectItem(root, "F_list");
		pageInfo->curPageNum = 0;
		if (chList != NULL && chList->type != cJSON_NULL) 
		{
			pageInfo->total = cJSON_GetArraySize(chList);
			for (i = (pageInfo->curPage-1)*MAX_WORD_NUM,j=0; i < cJSON_GetArraySize(chList)&&i<pageInfo->curPage*MAX_WORD_NUM; i++,j++) 
			{
				cJSON *item = cJSON_GetArrayItem(chList, i);
				cJSON *explain = cJSON_GetObjectItem(item, "explain");
				cJSON *word = cJSON_GetObjectItem(item, "word");
				cJSON *uk_bs = cJSON_GetObjectItem(item, "uk_bs");
				cJSON *uk_word_sound=cJSON_GetObjectItem(item, "uk_word_sound");
				cJSON *us_bs = cJSON_GetObjectItem(item, "us_bs");
				cJSON *us_word_sound=cJSON_GetObjectItem(item, "us_word_sound");
				cJSON *sentence = cJSON_GetObjectItem(item, "sentence");
				cJSON *sentence_explain = cJSON_GetObjectItem(item, "sentence_explain");
				if(sentence!=NULL&&sentence_explain!=NULL)
				{
					if (uk_bs == NULL && us_bs == NULL) 
					{
						addWordInfo(j, word->valuestring,sentence->valuestring,sentence_explain->valuestring, PNULL,PNULL,PNULL,PNULL,explain->valuestring);
					}
					else if (uk_bs == NULL) 
					{
						addWordInfo(j, word->valuestring,sentence->valuestring,sentence_explain->valuestring, us_bs->valuestring,us_word_sound->valuestring,PNULL,PNULL,explain->valuestring);
					}
					else if (us_bs == NULL) 
					{
						addWordInfo(j, word->valuestring,sentence->valuestring,sentence_explain->valuestring, PNULL,PNULL, uk_bs->valuestring,uk_word_sound->valuestring,explain->valuestring);
					} else {
						addWordInfo(j, word->valuestring,sentence->valuestring,sentence_explain->valuestring, us_bs->valuestring,us_word_sound->valuestring,uk_bs->valuestring,uk_word_sound->valuestring, explain->valuestring);
					}
				}else{
					if (uk_bs == NULL && us_bs == NULL) 
					{
						addWordInfo(j, word->valuestring, PNULL,PNULL, PNULL,PNULL,PNULL,PNULL,explain->valuestring);
					}
					else if (uk_bs == NULL) 
					{
						addWordInfo(j, word->valuestring, PNULL,PNULL, us_bs->valuestring,us_word_sound->valuestring,PNULL,PNULL,explain->valuestring);
					}
					else if (us_bs == NULL) 
					{
						addWordInfo(j, word->valuestring, PNULL,PNULL, PNULL,PNULL, uk_bs->valuestring,uk_word_sound->valuestring,explain->valuestring);
					} else {
						addWordInfo(j, word->valuestring, PNULL,PNULL, us_bs->valuestring,us_word_sound->valuestring,uk_bs->valuestring,uk_word_sound->valuestring, explain->valuestring);
					}
				}
				pageInfo->curPageNum++;
			}
			cJSON_Delete(root);
		}
		SCI_FREE(data);
		if(pageInfo->total ==0)
		{
			pageInfo->total=-2;
		}
		MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}

}

LOCAL MMI_RESULT_E openWordCardWin() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	if(0 != autodisplay_tip_timer)
	{
		MMK_StopTimer(autodisplay_tip_timer);
		autodisplay_tip_timer = 0;
	}
	is_autodisplay_tip=0;
	if(chapters[0]!=PNULL)
	{
		is_new_word=0;
		pageInfo->chapterIdx = -1;
		pageInfo->total = 0;
		pageInfo->realIdx = 0;
		pageInfo->curPage = 0;
		pageInfo->curPageNum = 0;
		pageInfo->curIdx = 0;
		pageInfo->chapterIdx = dsl_chapter_select_idx+dsl_chapter_display_idx*DSL_CHAPTER_DISPLAY_NUM;
		{
			pageInfo->curPage = 1;
			if(is_load_local)
			{
				loadLocalWords(chapters[pageInfo->chapterIdx]->path);
			}
			else
			{
				requestWordInfo();
			}
		}
		MMK_CreateWin((uint32 *)MMI_WORD_CARD_WIN_TAB, PNULL);
	}
	return result;
}

LOCAL MMI_RESULT_E openNewWordCardWin() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	if(0 != autodisplay_tip_timer)
	{
		MMK_StopTimer(autodisplay_tip_timer);
		autodisplay_tip_timer = 0;
	}
	is_autodisplay_tip=0;
	if(chapters[0]!=PNULL)
	{
		is_new_word=1;
		pageInfo->chapterIdx = -1;
		pageInfo->total = 0;
		pageInfo->realIdx = 0;
		pageInfo->curPage = 0;
		pageInfo->curPageNum = 0;
		pageInfo->curIdx = 0;
		pageInfo->chapterIdx = dsl_chapter_select_idx+dsl_chapter_display_idx*DSL_CHAPTER_DISPLAY_NUM;
		{
			pageInfo->curPage = 1;
			if(is_load_local)
			{
				loadLocalWords(chapters[pageInfo->chapterIdx]->path);
			}
			else
			{
				requestWordInfo();
			}
		}
		MMK_CreateWin((uint32 *)MMI_WORD_CARD_WIN_TAB, PNULL);
	}
	return result;
}

LOCAL void parseNewWordResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	SCI_TRACE_LOW("DSL_WORD %s is_ok %d",__FUNCTION__,is_ok);
	if(is_ok)
	{
		SCI_FREE(newWords_ex);
		newWords_ex=PNULL;
	}else
	{
		if(newWords_ex!=PNULL&&newWordsReNum<3)
		{
			char url[200] = {0};
			char json[350]={0};
			newWordsReNum++;
			sprintf(json,"{\"cardId\":\"%s\",\"bookId\":\"%s\",\"sectionId\":\"%s\",\"word\":\"%s\"}",
						BASE_DEVICE_IMEI,books[pageInfo->bookIdx]->id, chapters[pageInfo->chapterIdx]->id,newWords_ex);
			sprintf(url, "%s%s", BASE_URL_PATH, "v1/card/word/collect");
			MMIZDT_HTTP_AppSend(FALSE, url, json, strlen(json), 1000, 0, 0, 0, 0, 0, parseNewWordResponse);
		}
	}
}

LOCAL void requestWordInfo() {
  char url[255]={0};
  if(is_new_word)
  {
  	sprintf(url,"v1/card/word/collect?cardId=%s&bookId=%s&sectionId=%s&F_page=%d&F_pagesize=%d",BASE_DEVICE_IMEI,books[pageInfo->bookIdx]->id, chapters[pageInfo->chapterIdx]->id, pageInfo->curPage,MAX_WORD_NUM);
  }else
  {
	sprintf(url,"v1/card/book/wordList?F_book_id=%s&F_section_id=%s&F_page=%d&F_limit=%d",books[pageInfo->bookIdx+dsl_word_main_display_idx]->id, chapters[pageInfo->chapterIdx]->id, pageInfo->curPage,MAX_WORD_NUM);
  }
  SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
  request_http_win_word_idx++;
  pageInfo->curPageNum=0;
  MMIZDT_HTTP_AppSend(TRUE, BASE_URL_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, parseWordResponse);
}

LOCAL MMI_RESULT_E setWordToNew() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	if(!is_load_local)
	{
		if(newWords==PNULL)
		{
			newWords = SCI_ALLOCA(256);
			SCI_MEMSET(newWords,0,256);
		}
		strcat(newWords,words[pageInfo->curIdx]->word);
		strcat(newWords,",");
             SCI_TRACE_LOW("%s: strlen(newWords) = %d", __FUNCTION__, strlen(newWords));
		if(strlen(newWords)>100)
		{
			char url[200] = {0};
			char json[350]={0};
			if(newWords_ex!=PNULL)
			{
				SCI_FREE(newWords_ex);
				newWords_ex=PNULL;
			}
			newWordsReNum=0;
			newWords_ex=newWords;
			newWords=PNULL;
			sprintf(json,"{\"cardId\":\"%s\",\"bookId\":\"%s\",\"sectionId\":\"%s\",\"word\":\"%s\"}",
						BASE_DEVICE_IMEI,books[pageInfo->bookIdx]->id, chapters[pageInfo->chapterIdx]->id,newWords_ex);
			sprintf(url, "%s%s", BASE_URL_PATH, "v1/card/word/collect");
			SCI_TRACE_LOW("%s: json = %s", __FUNCTION__, json);
			SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
			MMIZDT_HTTP_AppSend(FALSE, url, json, strlen(json), 1000, 0, 0, 0 ,0, 0, parseNewWordResponse);
		}
	}
	pageInfo->realIdx++;
	pageInfo->curIdx = pageInfo->realIdx % MAX_WORD_NUM;
	SCI_TRACE_LOW("%s: pageInfo->curIdx %d   pageInfo->realIdx %d ", __FUNCTION__, pageInfo->curIdx,pageInfo->realIdx);
	if (pageInfo->curIdx == 0) {
		releaseWords();
		pageInfo->curPage++;
		SCI_TRACE_LOW("%s: curPage %d ", __FUNCTION__, pageInfo->curPage);
		if(is_load_local)
		{
			loadLocalWords(chapters[pageInfo->chapterIdx]->path);
		}
		else
		{
			requestWordInfo();
		}
	}
	MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	return result;
}

LOCAL MMI_RESULT_E openNewWordPreCard() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(is_display_tip==3)
	{
		return result;
	}
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	pageInfo->realIdx--;
	pageInfo->curIdx = pageInfo->realIdx % MAX_WORD_NUM;
	SCI_TRACE_LOW("%s: pageInfo->curIdx %d   pageInfo->realIdx %d ", __FUNCTION__, pageInfo->curIdx,pageInfo->realIdx);
	if (pageInfo->curIdx == MAX_WORD_NUM-1) {
		releaseWords();
		pageInfo->curPage--;
		requestWordInfo();
	}
	MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	return result;
}

LOCAL void parseNewWordDeleteResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	SCI_TRACE_LOW("DSL_WORD %s is_ok %d",__FUNCTION__,is_ok);
	if(is_ok)
	{
		releaseWords();
		if(pageInfo->realIdx>0&&pageInfo->realIdx>=pageInfo->total-1)
		{
			pageInfo->realIdx--;
			pageInfo->curIdx = pageInfo->realIdx % MAX_WORD_NUM;
			if (pageInfo->curIdx == MAX_WORD_NUM-1) {
				pageInfo->curPage--;
			}
		}
		requestWordInfo();
	}
	else
	{
		display_loading_tip(5);
	}
}

LOCAL MMI_RESULT_E deleteNewWordCard() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	char url[200] = {0};
	char json[255]={0};
	if(is_display_tip==3)
	{
		return result;
	}
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	SCI_TRACE_LOW("DSLCHAT_WIN %s start", __FUNCTION__);
	if(0 != display_tip_timer)
	{
		MMK_StopTimer(display_tip_timer);
		display_tip_timer = 0;
	}
	is_display_tip=3;
	MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);

	sprintf(json,"{\"cardId\":\"%s\",\"bookId\":\"%s\",\"sectionId\":\"%s\",\"word\":\"%s\"}",
				BASE_DEVICE_IMEI,books[pageInfo->bookIdx]->id, chapters[pageInfo->chapterIdx]->id,words[pageInfo->curIdx]->word);
	sprintf(url, "%s%s", BASE_URL_PATH, "v1/card/word/collect/delete");
	MMIZDT_HTTP_AppSend(FALSE, url, json, strlen(json), 1000, 0, 0, 0, 0, 0, parseNewWordDeleteResponse);

	return result;
}

LOCAL MMI_RESULT_E openNewWordNextCard() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(is_display_tip==3)
	{
		return result;
	}
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	pageInfo->realIdx++;
	pageInfo->curIdx = pageInfo->realIdx % MAX_WORD_NUM;
	SCI_TRACE_LOW("%s: pageInfo->curIdx %d   pageInfo->realIdx %d ", __FUNCTION__, pageInfo->curIdx,pageInfo->realIdx);
	if (pageInfo->curIdx == 0) {
		releaseWords();
		pageInfo->curPage++;
		SCI_TRACE_LOW("%s: curPage %d ", __FUNCTION__, pageInfo->curPage);
		requestWordInfo();
	}
	MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	return result;
}

LOCAL MMI_RESULT_E openNextCard() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	pageInfo->realIdx++;
	pageInfo->curIdx = pageInfo->realIdx % MAX_WORD_NUM;
	SCI_TRACE_LOW("%s: pageInfo->curIdx %d   pageInfo->realIdx %d ", __FUNCTION__, pageInfo->curIdx,pageInfo->realIdx);
	if (pageInfo->curIdx == 0) {
		releaseWords();
		pageInfo->curPage++;
		SCI_TRACE_LOW("%s: curPage %d ", __FUNCTION__, pageInfo->curPage);
		if(is_load_local)
		{
			loadLocalWords(chapters[pageInfo->chapterIdx]->path);
		}
		else
		{
			if(is_homework_task && (strlen(homework_task_id) != 0 || word_task_msg_id != NULL)){
				homework_task_page++;
				SCI_TRACE_LOW("%s: word_task_msg_id = %s ", __FUNCTION__, word_task_msg_id);
				if(word_task_msg_id != NULL){
					//Homework_requestTeacherTaskWord(homework_task_page);
				}else{
					//Homework_requestTaskWord(homework_task_page);
				}
			}else{
				requestWordInfo();
			}
		}
	}
	if(pageInfo->realIdx >= pageInfo->total){
		if(is_homework_task && strlen(homework_task_id) != 0){
			//Homework_postCompeleteTask();
		}
	}
	MMK_PostMsg(MMI_WORD_CARD_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	return result;
}
LOCAL MMI_RESULT_E closeWordWin() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
  	if(GetCanntClick(0))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	releaseBooks();
	MMK_CloseWin(MMI_WORD_WIN_ID);
	return result;
}
LOCAL MMI_RESULT_E openCHWin() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(1))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	MMK_CreateWin(MMI_WORD_CH_WIN_TAB, PNULL);
	return result;
}
LOCAL MMI_RESULT_E closeWordCHWin() {
	MMI_RESULT_E result = MMI_RESULT_TRUE;
	if(GetCanntClick(0))
	{
		SCI_TRACE_LOW("%s can not click", __FUNCTION__);
		return result;
	}
	if(0 != autodisplay_tip_timer)
	{
		MMK_StopTimer(autodisplay_tip_timer);
		autodisplay_tip_timer = 0;
	}
	is_autodisplay_tip=0;
	releaseChapters();
	MMK_CloseWin(MMI_WORD_CH_WIN_ID);
	return result;
}

LOCAL void setNumberText(MMI_CTRL_ID_T ctrl_id,int cur,int count) {
	char str[20] = {0};
	MMI_STRING_T text = {0};
	uint16 wstr[10] = {0};
	sprintf(str, "%d/%d", (cur + 1), count);

	MMI_STRNTOWSTR(wstr, strlen(str)+1, (uint8 *)str, strlen(str)+1, strlen(str)+1);
	text.wstr_ptr = wstr;
	text.wstr_len = MMIAPICOM_Wstrlen(wstr);
	GUILABEL_SetText(ctrl_id, &text, TRUE);
	GUILABEL_SetAlign(ctrl_id, GUILABEL_ALIGN_MIDDLE);
}

LOCAL void addChapterInfo(uint8 position, char *name, int id,char * path) {
	int size=strlen(name) +1;
	chapters[position] = SCI_ALLOCA(sizeof(ZMT_CHAPTER_T));
	SCI_MEMSET(chapters[position],0,sizeof(ZMT_CHAPTER_T));
	chapters[position]->name = SCI_ALLOCA(size);
	SCI_MEMSET(chapters[position]->name, 0, size);
	SCI_MEMCPY(chapters[position]->name, name, size);
	itoa(id, chapters[position]->id, 10);
	if(path!=PNULL)
	{
		size=strlen(path) +1;
		chapters[position]->path = SCI_ALLOCA(size);
		SCI_MEMSET(chapters[position]->path, 0, size);
		SCI_MEMCPY(chapters[position]->path, path, size);
	}
}
LOCAL void addBookInfo(uint8 position, char *name , char *editionName, int id,char *path,char *basepath) {
	int size=0;
	char bookid[11]={0};
	books[position] = SCI_ALLOCA(sizeof(ZMT_BOOK_T));
	SCI_MEMSET(books[position],0,sizeof(ZMT_BOOK_T));
	size=strlen(name) +1;
	books[position]->name = SCI_ALLOCA(size);
	SCI_MEMSET(books[position]->name, 0, size);
	SCI_MEMCPY(books[position]->name, name, size);
	size=strlen(editionName) +1;
	books[position]->editionName = SCI_ALLOCA(size);
	SCI_MEMSET(books[position]->editionName, 0, size);
	SCI_MEMCPY(books[position]->editionName, editionName, size);
	sprintf(bookid, "%d", id);
	SCI_MEMCPY(books[position]->id, bookid, strlen(bookid));
	if(path!=PNULL)
	{
		size=strlen(path) +1;
		books[position]->path = SCI_ALLOCA(size);
		SCI_MEMSET(books[position]->path, 0, size);
		SCI_MEMCPY(books[position]->path, path, size);
	}
	if(basepath!=PNULL)
	{
		size=strlen(basepath) +1;
		books[position]->basepath = SCI_ALLOCA(size);
		SCI_MEMSET(books[position]->basepath, 0, size);
		SCI_MEMCPY(books[position]->basepath, basepath, size);
	}
}

LOCAL parseMp3Response(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	int i=0;
	SCI_TRACE_LOW("dsl %s start", __FUNCTION__);
	for(i=0;i<MAX_WORD_NUM;i++)
	{
		if(words[i]!=PNULL)
		{
			if(pageInfo->curIdx==i)
			{
				if (is_ok) {
					if(Rcv_len>0&&pRcv!=PNULL)
					{
						words[i]->uk_audio_data_len=Rcv_len;
						if(words[i]->uk_audio_data!=PNULL)
						{
							SCI_FREE(words[i]->uk_audio_data);
							words[i]->uk_audio_data=PNULL;
						}
						words[i]->uk_audio_data = SCI_ALLOCA(Rcv_len);
						SCI_MEMSET(words[i]->uk_audio_data, 0, Rcv_len);
						SCI_MEMCPY(words[i]->uk_audio_data, pRcv, Rcv_len);
						SCI_TRACE_LOW("%s: pageInfo->curIdx = %d", __FUNCTION__, pageInfo->curIdx);
						if(is_autoplay&&pageInfo->curIdx==i)
						{
							PlayWordUkSound();
						}
					}
					else
					{
						words[i]->uk_audio_data_len=-2;
					}
				}
				SCI_TRACE_LOW("%s i %d uk len %d", __FUNCTION__,i,words[i]->uk_audio_data_len);
				break;
			}
		}
		else
		{
			break;
		}
	}
	SCI_TRACE_LOW("dsl %s end", __FUNCTION__);
}

// 将strRes中的from替换为to，替换成功返回1，否则返回0。
LOCAL int StrReplace(char strRes[],char from[], char to[]) 
{
    int i,flag = 0;
    char *p,*q,*ts;
    for(i = 0; strRes[i]; ++i) 
    {
        if(strRes[i] == from[0]) 
        {
            p = strRes + i;
            q = from;
            while(*q && (*p++ == *q++));
            if(*q == '\0') 
            {
                ts = (char *)malloc(strlen(strRes) + 1);
                strcpy(ts,p);
                strRes[i] = '\0';
                strcat(strRes,to);
                strcat(strRes,ts);
                free(ts);
                flag = 1;
            }
        }
    }
    return flag;
}

LOCAL void addWordInfo(uint8 position, char *word,char *sentence,char *sentence_explain, char *us,char *us_sound, char *uk,char *uk_sound,char *explain) {
	int size=0;
	int index = 0;
	uint16 * wtmp=L"\n\n例句:\n";
	uint16 *wtmp1=L"\n";
	char tmp[20]={0};
	char tmp1[20]={0};
	GUI_WstrToUTF8(tmp,20,wtmp,MMIAPICOM_Wstrlen(wtmp));
	GUI_WstrToUTF8(tmp1,20,wtmp1,MMIAPICOM_Wstrlen(wtmp1));
	words[position] = SCI_ALLOCA(sizeof(ZMT_WORD_T));
	SCI_MEMSET(words[position],0,sizeof(ZMT_WORD_T));
	size=strlen(word);
	words[position]->word = SCI_ALLOCA(size+1);
	SCI_MEMSET(words[position]->word, 0, size+1);
	SCI_MEMCPY(words[position]->word, word, size);

	if(us!=PNULL)
	{
		size=strlen(us);
		words[position]->us = SCI_ALLOCA(size+1);
		SCI_MEMSET(words[position]->us, 0, size+1);
		SCI_MEMCPY(words[position]->us, us, size);
		if(us_sound!=PNULL)
		{
                    StrReplace(us_sound,"https","http");
			words[position]->uk_audio_data_len=-1;
			size=strlen(us_sound);
			words[position]->uk_audio_data = SCI_ALLOCA(size+1);
			SCI_MEMSET(words[position]->uk_audio_data, 0, size+1);
			SCI_MEMCPY(words[position]->uk_audio_data, us_sound, size);
		}
	}
	
	if(uk!=PNULL)
	{
		size=strlen(uk);
		words[position]->uk = SCI_ALLOCA(size+1);
		SCI_MEMSET(words[position]->uk, 0, size+1);
		SCI_MEMCPY(words[position]->uk, uk, size);
		if(uk_sound!=PNULL)
		{
			StrReplace(uk_sound,"https","http");
			words[position]->uk_audio_data_len=-1;
			size=strlen(uk_sound);
			words[position]->uk_audio_data = SCI_ALLOCA(size+1);
			SCI_MEMSET(words[position]->uk_audio_data, 0, size+1);
			SCI_MEMCPY(words[position]->uk_audio_data, uk_sound, size);
		}
	}
    if(explain != PNULL)
    {
        size = strlen(explain) + 10;
    }
    if(sentence != PNULL)
    {
        size += strlen(sentence) + 10;
    }
    if(sentence_explain != PNULL)
    {
        size += strlen(sentence_explain) + 10;
    }
    words[position]->explain = SCI_ALLOCA(size);
    SCI_MEMSET(words[position]->explain, 0, size);
    if(explain != PNULL){
        strcat(words[position]->explain,explain);
    }
    if(sentence != PNULL){
        strcat(words[position]->explain,tmp);
        strcat(words[position]->explain,sentence);
    }
    if(sentence_explain != PNULL){
        strcat(words[position]->explain,tmp1);
        strcat(words[position]->explain,sentence_explain);
    }
}
LOCAL void releaseWords() {
  int i = 0;
  for (; i < MAX_WORD_NUM; i++) {
    if (words[i] != PNULL) {
		if(words[i]->word!=PNULL)
		{
			SCI_FREE(words[i]->word);
			words[i]->word = PNULL;
		}
		if(words[i]->us!=PNULL)
		{
			SCI_FREE(words[i]->us);
			words[i]->us = PNULL;
			if(words[i]->us_audio_data!=PNULL)
			{
				SCI_FREE(words[i]->us_audio_data);
				words[i]->us_audio_data = PNULL;
			}
		}
		if(words[i]->uk!=PNULL)
		{
			SCI_FREE(words[i]->uk);
			words[i]->uk = PNULL;
			if(words[i]->uk_audio_data!=PNULL)
			{
				SCI_FREE(words[i]->uk_audio_data);
				words[i]->uk_audio_data = PNULL;
			}
		}
		if(words[i]->explain!=PNULL)
		{
			SCI_FREE(words[i]->explain);
			words[i]->explain = PNULL;
		}
		SCI_FREE(words[i]);
		words[i] = PNULL;
    }
  }
}
LOCAL void releaseChapters() {
  int i = 0;
  for (; i < MAX_CHAPTER_NUM; i++) {
    if (chapters[i] != PNULL) {
		if(chapters[i]->name!=PNULL)
		{
			SCI_FREE(chapters[i]->name);
			chapters[i]->name = PNULL;
		}
		if(chapters[i]->path!=PNULL)
		{
			SCI_FREE(chapters[i]->path);
			chapters[i]->path = PNULL;
		}
		SCI_FREE(chapters[i]);
		chapters[i] = PNULL;
    }
  }
}
LOCAL void releaseBooks() {
	int i = 0;
	for (; i < MAX_BOOK_NUM; i++) 
	{
		if (books[i] != PNULL) {
			if(books[i]->name!=PNULL)
			{
				SCI_FREE(books[i]->name);
				books[i]->name = PNULL;
			}
			if(books[i]->editionName!=PNULL)
			{
				SCI_FREE(books[i]->editionName);
				books[i]->editionName = PNULL;
			}
			if(books[i]->path!=PNULL)
			{
				SCI_FREE(books[i]->path);
				books[i]->path = PNULL;
			}
			if(books[i]->basepath!=PNULL)
			{
				SCI_FREE(books[i]->basepath);
				books[i]->basepath = PNULL;
			}
			SCI_FREE(books[i]);
			books[i] = PNULL;
		}
	}
}
LOCAL void releaseWordsAudio() {
	int i = 0;
	for(i = 0;i < MAX_AUDIO_NUM;i++)
	{
		if (words_audio[i]!=PNULL) {
			if(words_audio[i]->audio_data!=PNULL){
				SCI_FREE(words_audio[i]->audio_data);
				words_audio[i]->audio_data = PNULL;
			}
			if(words_audio[i]->audio_data_url!=PNULL){
				SCI_FREE(words_audio[i]->audio_data_url);
				words_audio[i]->audio_data_url = PNULL;
			}
			SCI_FREE(words_audio[i]);
			words_audio[i] = PNULL;
		}
	}
}
/***************************************************************/
// Description : open hello sprd windows
// Global resource dependence :
// Author: xiaoqing.lu
// Note:
/***************************************************************/
PUBLIC void MMI_CreateWordWin(void) {
	MMK_CreateWin((uint32 *)MMI_WORD_WIN_TAB, PNULL);
}

//===============================弹窗=========================================
LOCAL MMI_RESULT_E HandlePopupWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
	int tip_top=115;
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:  
			{
				GUISTR_STYLE_T text_style = {0};
				GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
				GUI_RECT_T rect = {10, 60, 230, 360};

				GUI_RECT_T rect_left_small = {32,  0, 112,0};
				GUI_RECT_T rect_right_small = {128, 0, 208, 0};
				GUI_BORDER_T border  = {0};
				MMI_STRING_T str_left = {0};
				MMI_STRING_T str_right = {0};

				MMI_STRING_T tipstr = {0};
				GUI_RECT_T tip_rect = {27,0, 213, 0};

				rect_left_small.top = tip_top+82;
				rect_left_small.bottom = tip_top+113;

				rect_right_small.top = tip_top+82;
				rect_right_small.bottom = tip_top+113;

				tip_rect.top =  tip_top+30;
				tip_rect.bottom = tip_top+50;

				MMI_GetLabelTextByLang(WORD_TIPS,&tipstr);

				border.width = 2;
				border.color = 0x0000;
				border.type =  GUI_BORDER_ROUNDED;

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_16;
				text_style.font_color = MMI_BLACK_COLOR;
			
				GUI_FillRect(&lcd_dev_info,rect,MMI_WHITE_COLOR);
				rect.top=tip_top;
				rect.bottom=tip_top+130;
				GUI_DisplayBorder(rect,rect,&border,&lcd_dev_info);

				border.type =  GUI_BORDER_SOLID;

				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&tip_rect,
					&tip_rect,
					&tipstr,
					&text_style,
					GUISTR_STATE_ALIGN,
					GUISTR_TEXT_DIR_AUTO
				);

				GUI_DisplayBorder(rect_left_small,rect_left_small,&border,&lcd_dev_info);
                MMI_GetLabelTextByLang(WORD_FALSE,&str_left);
                GUISTR_DrawTextToLCDInRect(
                    (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                    &rect_left_small,
                    &rect_left_small,
                    &str_left,
                    &text_style,
                    GUISTR_STATE_ALIGN,
                    GUISTR_TEXT_DIR_AUTO
				);

				GUI_DisplayBorder(rect_right_small,rect_right_small,&border,&lcd_dev_info);
                MMI_GetLabelTextByLang(WORD_TRUE,&str_right);
                GUISTR_DrawTextToLCDInRect(
                    (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                    &rect_right_small,
                    &rect_right_small,
                    &str_right,
                    &text_style,
                    GUISTR_STATE_ALIGN,
                    GUISTR_TEXT_DIR_AUTO
				);
			} 
            break;
        case MSG_FULL_PAINT:
            {    
                
            }
            break;
        case MSG_KEYUP_CANCEL:
            MMK_CloseWin(win_id);
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T   point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
				if((point.x > (128) && point.x < (208)) && (point.y > (tip_top+82) && point.y < (tip_top+113)))
				{
					openChRequest();
					MMK_CloseWin(win_id);
				}else //if((point.x > (32) && point.x < (112)) && (point.y > (tip_top+82) && point.y < (tip_top+113)))
				{
					is_record=0;
					MMK_CloseWin(win_id);
				} 
			}
		    break;
        case MSG_CLOSE_WINDOW:
            {
            }
            break;
        default:
			{
                result = MMI_RESULT_FALSE;
			}
            break;
    }
    return result;
}

WINDOW_TABLE(MMIAPI_WORD_TIPS_TAB) = {
    WIN_FUNC((uint32)HandlePopupWinMsg),
    WIN_ID(MMI_ZMT_WORD_POPUP),
	WIN_HIDE_STATUS,
    END_WIN
};

LOCAL OpenWordRecordTips()
{
	MMK_CreateWin((uint32 *)MMIAPI_WORD_TIPS_TAB, PNULL);
}

LOCAL BOOLEAN ChatPlayAudioDataNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
	MMISRVAUD_REPORT_T *report_ptr = PNULL;

	if(param != PNULL && handle > 0)
	{
		report_ptr = (MMISRVAUD_REPORT_T *)param->data;
		if(report_ptr != PNULL && handle == mp3_player_handle)
		{
			switch(report_ptr->report)
			{
				case MMISRVAUD_REPORT_END:
					{
						/*if(listening_timer_id != 0){
							MMK_StopTimer(listening_timer_id);
						}
						SCI_TRACE_LOW("%s: listening_timer_times = %d", __FUNCTION__, listening_timer_times);
						listening_timer_id = MMK_CreateTimerCallback(listening_timer_times, Listening_timerCallback, NULL, FALSE);*/
					}
					break;
				default:
					break;
			}
		}
	}
	return TRUE;
}
