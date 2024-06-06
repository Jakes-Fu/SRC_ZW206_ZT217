
#include "std_header.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "mmicc_export.h"
#include "guitext.h"
#include "guilcd.h"
#include "guilistbox.h"
#include "guiedit.h"
#include "guilabel.h"
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmi_appmsg.h"
#include "mmipub.h"
#include "mmi_common.h"

#include "mmiidle_export.h"
#include "mmi_position.h"
#include "ldo_drvapi.h"
#include "mmi_resource.h"
#include "mmiset_id.h"
#include "version.h"
#include "mmicc_internal.h"
#include "mmisrvrecord_export.h"
#include "mmirecord_export.h"
#include "mmiphone_export.h"
#include "zmt_listening_export.h"
#include "zmt_listening_id.h"
#include "zmt_listening_image.h"
#include "zmt_listening_text.h"
#include "zdthttp_api.h"

#ifndef WIN32
#define ALBUM_LIST_USE_TEST_DATA	1
#define PLAY_LIST_USE_TEST_DATA	1
#else
#define ALBUM_LIST_USE_TEST_DATA	1
#define PALY_LIST_USE_TEST_DATA	1
#endif

LOCAL GUI_RECT_T button_class4_rect = {0, LISTEN_LINE_HIGHT, 2*LISTEN_LINE_WIDTH, 2*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T button_class1_rect = {2*LISTEN_LINE_WIDTH, LISTEN_LINE_HIGHT, 4*LISTEN_LINE_WIDTH, 2*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T button_class2_rect = {4*LISTEN_LINE_WIDTH, LISTEN_LINE_HIGHT, 6*LISTEN_LINE_WIDTH, 2*LISTEN_LINE_HIGHT};
extern GUI_RECT_T listen_win_rect;
extern GUI_RECT_T listen_title_rect;
extern GUI_RECT_T listen_dir_rect;
extern GUI_RECT_T listen_list_rect;

LISTENING_MODULE_INFO * module_info = NULL;
LISTENING_ALBUM_INFO * album_info = NULL;
LISTENING_LIST_INFO * listening_info = NULL;
int listening_load_win = 0;
uint8 listening_downloading_index = 0;
extern BOOLEAN listening_download_audio;
extern LISTEING_LOCAL_DELETE_INFO delete_info;
extern char * listening_downloading_audio_path;


LOCAL BOOLEAN Listening_IsMp3FileExist(uint8 album_id, LISTENING_ALBUM_INFO * album_info)
{
	char file_str[LIST_ITEM_PATH_SIZE_MAX] = {0};
	Listening_GetFileName(file_str, album_info->module_id, album_info->album_id, album_info->item_info[album_id].audio_id);
	if(zmt_file_exist(file_str))
	{
		SCI_TRACE_LOW("%s: file_str = %s exist", __FUNCTION__, file_str);
		return TRUE;
	}
	return FALSE;
}

LOCAL void ListeningAudioWin_DisplayAudioList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
	uint16 index = 0;
	GUILIST_INIT_DATA_T list_init = {0};
	GUILIST_ITEM_T item_t = {0};
	GUIITEM_STATE_T item_state = {0};
	GUILIST_ITEM_DATA_T item_data = {0};
	GUI_COLOR_T list_color = {0};
	MMI_STRING_T text_str = {0};
	wchar name_wchar[100] = {0};
	uint8 length = 0;
	BOOLEAN aduio_ready = FALSE;
	char file_str[LIST_ITEM_PATH_SIZE_MAX] = {0};

	list_init.both_rect.v_rect = listen_list_rect;
	list_init.both_rect.v_rect.top -= 25;
	list_init.type = GUILIST_TEXTLIST_E;
	GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

	MMK_SetAtvCtrl(win_id, ctrl_id);
	GUILIST_RemoveAllItems(ctrl_id);
	GUILIST_SetMaxItem(ctrl_id, listening_info->item_total_num, FALSE);

	for(index = 0; index < listening_info->item_total_num; index++)
	{
		length = strlen(album_info->item_info[index].audio_name);
		if(length == 0) continue;
		length += 1;
		item_t.item_style = GUIITEM_STYLE_ONE_ICON_AND_ONE_TEXT_MS;
		item_t.item_data_ptr = &item_data;
		item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
		
		memset(name_wchar, 0, 100);
		//name item
		//SCI_TRACE_LOW("%s: audio_name_len = %d", __FUNCTION__, strlen(album_info->item_info[index].audio_name));
		GUI_UTF8ToWstr(name_wchar, 100, album_info->item_info[index].audio_name, length);
		text_str.wstr_ptr = name_wchar;
		text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
		item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
		item_data.item_content[0].item_data.text_buffer = text_str;

		//icon reday
		aduio_ready = Listening_IsMp3FileExist(index, album_info);
		/*SCI_TRACE_LOW("%s: listening_download_audio = %d, listening_downloading_index", 
			__FUNCTION__, listening_download_audio, listening_downloading_index);*/
		if(listening_download_audio && listening_downloading_index == index)
		{
			aduio_ready = FALSE;
		}
		item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
		Listening_GetFileName(file_str, album_info->module_id, album_info->album_id, album_info->item_info[index].audio_id);
		if(listening_downloading_audio_path != NULL &&
			0 == strcmp(listening_downloading_audio_path, file_str))
		{
			item_data.item_content[1].item_data.image_id = ZMT_LISTEN_DOWNLOADING;
		}else
		{
			 if(album_info->item_info[index].aduio_ready == 2 || aduio_ready)
			{
				item_data.item_content[1].item_data.image_id = NULL;
			}
			else if(album_info->item_info[index].aduio_ready == 0)
			{
				item_data.item_content[1].item_data.image_id = ZMT_LISTEN_DOWNLOAD;
			}
			else if(album_info->item_info[index].aduio_ready == 1)
			{
				item_data.item_content[1].item_data.image_id = ZMT_LISTEN_DOWNLOADING;
			}
		}

		//不画分割线
		GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
		//不画高亮条
		GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

		GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

		GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));
		GUILIST_SetTextFont(ctrl_id, DP_FONT_22, MMI_WHITE_COLOR);

		GUILIST_AppendItem(ctrl_id, &item_t);
	}
}

LOCAL MMI_RESULT_E HandleListeningAudioWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CTRL_ID_T ctrl_id = LISTENING_LISTBOX_AUDIO_CTRL_ID;
	GUI_POINT_T point = {0};
	uint8 id_index = 0;
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				if(listening_info == NULL)
				{
					listening_info = (LISTENING_LIST_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_LIST_INFO));
					SCI_MEMSET(listening_info, 0, sizeof(LISTENING_LIST_INFO));
				}
				listening_load_win = 0;
			}
			break;
		case MSG_FULL_PAINT:
			{
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar title_wchar[50] = {0};
				uint8 length = 0;
				uint8 num = 0;
				char num_buf[10] = {0};
				wchar num_str[10] = {0};
				uint8 total_page = 0;

				GUI_FillRect(&lcd_dev_info, listen_win_rect, GUI_RGB2RGB565(80, 162, 254));
				GUI_FillRect(&lcd_dev_info, listen_title_rect, GUI_RGB2RGB565(108, 181, 255));

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_24;
				text_style.font_color = MMI_WHITE_COLOR;

				id_index = (uint8) MMK_GetWinAddDataPtr(win_id);
				//SCI_TRACE_LOW("%s: id_index = %d", __FUNCTION__, id_index);
				length = strlen(module_info->item_info[id_index].album_name) + 1;
				GUI_UTF8ToWstr(title_wchar, 50, module_info->item_info[id_index].album_name, length);
				text_string.wstr_ptr = title_wchar;
				text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&listen_title_rect,
					&listen_title_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
				
				text_style.align = ALIGN_HVMIDDLE;
				if(listening_load_win == 0)
				{
					MMIRES_GetText(ZMT_TXT_LOADING, win_id, &text_string);
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&listen_win_rect,
						&listen_win_rect,
						&text_string,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);
					break;
				}
				else if(listening_load_win == -1)
				{
					MMIRES_GetText(ZMT_TXT_LOADING_FAIL, win_id, &text_string);
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&listen_win_rect,
						&listen_win_rect,
						&text_string,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);
					break;
				}

				ListeningAudioWin_DisplayAudioList(win_id, ctrl_id);
			}
			break;
		case MSG_CTL_PENOK:
			{
				uint16 index = GUILIST_GetCurItemIndex(ctrl_id);
				SCI_TRACE_LOW("%s: index = %d, download = %d", __FUNCTION__, index, listening_download_audio);
				if(Listening_IsMp3FileExist(index, album_info))
				{
					SCI_TRACE_LOW("%s: module_id = %d, album_id = %d, audio_id = %d",
						__FUNCTION__,
						album_info->module_id, 
						album_info->album_id,
						album_info->item_info[index].audio_id);
					if(Listening_PlayMp3(
						album_info->module_id, 
						album_info->album_id,
						album_info->item_info[index].audio_id)
						)
					{
						LISTENING_PLAYER_INFO * player_info = NULL;
						player_info = (LISTENING_PLAYER_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
						player_info->is_local_play = FALSE;
						player_info->moudle_index = id_index;
						player_info->audio_index = index;
						MMI_CreateListeningPlayerWin(player_info);
					}
				}
				else
				{
					if(listening_download_audio)
					{
						MMI_CreateListeningTipWin(PALYER_PLAY_DOWNLOADING_TIP);
						break;
					}
					listening_downloading_index = index;
					if(1)
					{
						Listening_RequestDownloadAudio(index);
					}
					else
					{	//use for simulator test
						MMI_TestToOpenPlayerWin();
					}
				}
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
				if((point.x > 10 && point.x < 120) && (point.y > 30 && point.y < 60))
				{
					uint8 tmp_total = 0;
					uint tmp_class = 0;
					tmp_total = listening_info->album_total_num;
					tmp_class = listening_info->select_cur_class;
					SCI_MEMSET(listening_info, 0, sizeof(LISTENING_LIST_INFO));
					listening_info->album_total_num = tmp_total;
					listening_info->select_cur_class = tmp_class;
					MMK_CloseWin(win_id);
				}
			}
			break;
		case MSG_KEYUP_CANCEL:
			{
				MMK_CloseWin(win_id);	
			}
			break;
		case MSG_CLOSE_WINDOW:
			{
				listening_info->item_cur_page = 0;
				listening_download_audio = FALSE;
				MMIZDT_HTTP_Close();
				if(listening_downloading_audio_path != NULL){
				    SCI_FREE(listening_downloading_audio_path);
				    listening_downloading_audio_path = NULL;
				}
			}
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
		}
	return recode;
}

WINDOW_TABLE(MMI_AUDIO_LIST_WIN_TAB) = 
{
	WIN_ID(LISTENING_AUDIO_LIST_WIN_ID),
	WIN_FUNC((uint32)HandleListeningAudioWinMsg),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMI_CreateListeningAudioWin(uint8 index)
{
	MMI_WIN_ID_T win_id = LISTENING_AUDIO_LIST_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}

	win_handle = MMK_CreateWin((uint32*)MMI_AUDIO_LIST_WIN_TAB, (ADD_DATA)index);
	MMK_SetWinRect(win_handle, &rect);
}

PUBLIC BOOLEAN MMI_IsListeningAudioWinOpen(void)
{
	return MMK_IsOpenWin(LISTENING_AUDIO_LIST_WIN_ID);
}

PUBLIC void MMI_TestToOpenPlayerWin(void)
{
	LISTENING_PLAYER_INFO * player_info = NULL;
	player_info = (LISTENING_PLAYER_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
	player_info->is_local_play = TRUE;
	player_info->moudle_index = 0;
	player_info->audio_index = 1;
	MMI_CreateListeningPlayerWin(player_info);
}

////////////////////////////////// album list win ////////////////////////////////////////////
LOCAL void ListeningWin_DisplayAlbumList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
	uint16 index = 0;
	GUILIST_INIT_DATA_T list_init = {0};
	GUILIST_ITEM_T item_t = {0};
	GUIITEM_STATE_T item_state = {0};
	GUILIST_ITEM_DATA_T item_data = {0};
	GUI_COLOR_T list_color = {0};
	MMI_STRING_T text_str = {0};
	wchar name_wchar[100] = {0};
	uint8 length = 0;

	list_init.both_rect.v_rect = listen_list_rect;
	list_init.type = GUILIST_TEXTLIST_E;
	GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

	MMK_SetAtvCtrl(win_id, ctrl_id);
	GUILIST_RemoveAllItems(ctrl_id);
	GUILIST_SetMaxItem(ctrl_id, listening_info->album_total_num, FALSE);

	for(index = 0; index < listening_info->album_total_num; index++)
	{
		length = strlen(module_info->item_info[index].album_name);
		if(length == 0) continue; 
		
		item_t.item_style = GUIITEM_STYLE_TWO_ICON_AND_ONE_TEXT_MS;
		item_t.item_data_ptr = &item_data;
		item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
		
		memset(name_wchar, 0, 100);

		//icon album
		item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
		item_data.item_content[0].item_data.image_id = ZMT_LISTEN_ALBUM;

		//name album
		GUI_UTF8ToWstr(name_wchar, 100, module_info->item_info[index].album_name, length);
		text_str.wstr_ptr = name_wchar;
		text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
		item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
		item_data.item_content[1].item_data.text_buffer = text_str;

		//icon arrow
		item_data.item_content[2].item_data_type = GUIITEM_DATA_IMAGE_ID;
		item_data.item_content[2].item_data.image_id = NULL;//IMAGE_ARROW_BIG;

		//不画分割线
		GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
		//不画高亮条
		GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

		GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

		GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));
		GUILIST_SetTextFont(ctrl_id, DP_FONT_22, MMI_WHITE_COLOR);

		GUILIST_AppendItem(ctrl_id, &item_t);
	}
}

PUBLIC void ListeningWin_DisplaySecletedClass(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
	GUI_RECT_T line4_rect = {0};
	GUI_RECT_T line1_rect = {0};
	GUI_RECT_T line2_rect = {0};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_string = {0};
	GUI_COLOR_T color4 = MMI_WHITE_COLOR;
	GUI_COLOR_T color1 = MMI_WHITE_COLOR;
	GUI_COLOR_T color2 = MMI_WHITE_COLOR;
	MMI_CTRL_ID_T ctrl_id_4 = LISTENING_BUTTON_PRE_CTRL_ID;
	MMI_CTRL_ID_T ctrl_id_1 = LISTENING_BUTTON_PRI_CTRL_ID;
	MMI_CTRL_ID_T ctrl_id_2 = LISTENING_BUTTON_JUR_CTRL_ID;
	GUI_FONT_ALL_T font_all = {0};
    
	line4_rect = button_class4_rect;
	line4_rect.left += 5;
	line4_rect.right -= 5;
	line4_rect.top = button_class4_rect.bottom;
	line4_rect.bottom = line4_rect.top + 1;

	line1_rect = button_class1_rect;
	line1_rect.left += 5;
	line1_rect.right -= 5;
	line1_rect.top = button_class1_rect.bottom;
	line1_rect.bottom = line1_rect.top + 1;

	line2_rect = button_class2_rect;
	line2_rect.left += 5;
	line2_rect.right -= 5;
	line2_rect.top = button_class2_rect.bottom;
	line2_rect.bottom = line2_rect.top + 1;

	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = DP_FONT_22;	
	if(listening_info->select_cur_class == SELECT_MODULE_LOCAL)
	{
		GUI_FillRect(&lcd_dev_info, line4_rect, MMI_WHITE_COLOR);
	}
	else if(listening_info->select_cur_class == SELECT_MODULE_PRIMARY_SCHOOL)
	{
		GUI_FillRect(&lcd_dev_info, line1_rect, MMI_WHITE_COLOR);
	}
	else if(listening_info->select_cur_class == SELECT_MODULE_JUNIOR_SCHOOL)
	{
		GUI_FillRect(&lcd_dev_info, line2_rect, MMI_WHITE_COLOR);
	}
	
	font_all.font = DP_FONT_22;
	font_all.color = color4;
	GUIBUTTON_SetRect(ctrl_id_4, &button_class4_rect);
	GUIBUTTON_SetFont(ctrl_id_4, &font_all);
	GUIBUTTON_SetTextId(ctrl_id_4, ZMT_TXT_LOCAL);

	font_all.color = color1;
	GUIBUTTON_SetRect(ctrl_id_1, &button_class1_rect);
	GUIBUTTON_SetFont(ctrl_id_1, &font_all);
	GUIBUTTON_SetTextId(ctrl_id_1, ZMT_TXT_PRIMARY_SCHOOL);

	font_all.color = color2;
	GUIBUTTON_SetRect(ctrl_id_2, &button_class2_rect);
	GUIBUTTON_SetFont(ctrl_id_2, &font_all);
	GUIBUTTON_SetTextId(ctrl_id_2, ZMT_TXT_JUNIOR_SCHOOL);
}

PUBLIC MMI_RESULT_E ButtonPreClass_CallbackFunc(void)
{
	if(listening_info->select_cur_class == SELECT_MODULE_LOCAL)
	{
		return FALSE;
	}
	listening_info->select_cur_class = SELECT_MODULE_LOCAL;
	if(0){
		uint8 tmp = 0;
		listening_load_win = 0;
		tmp = listening_info->select_cur_class;
		SCI_MEMSET(listening_info, 0, sizeof(LISTENING_LIST_INFO));
		listening_info->select_cur_class = tmp;
		if(MMK_IsFocusWin(LISTENING_ALBUM_LIST_WIN_ID))
		{
			MMK_SendMsg(LISTENING_ALBUM_LIST_WIN_ID, MSG_FULL_PAINT, PNULL);
		}
		Listening_RequestAlbumListInfo(listening_info->select_cur_class);
	}
	else
	{
		memset(&delete_info, 0, sizeof(LISTEING_LOCAL_DELETE_INFO));
		if(MMK_IsOpenWin(LISTENING_ALBUM_LIST_WIN_ID))
		{
			MMK_CloseWin(LISTENING_ALBUM_LIST_WIN_ID);
		}
		MMI_CreateListeningLocalWin();
	}
	return TRUE;
}

PUBLIC MMI_RESULT_E ButtonPriClass_CallbackFunc(void)
{
	if(listening_info->select_cur_class == SELECT_MODULE_PRIMARY_SCHOOL)
	{
		return FALSE;
	}
	listening_info->select_cur_class = SELECT_MODULE_PRIMARY_SCHOOL;
	if(1){
		uint8 tmp = 0;
		listening_load_win = 0;
		tmp = listening_info->select_cur_class;
		SCI_MEMSET(listening_info, 0, sizeof(LISTENING_LIST_INFO));
		listening_info->select_cur_class = tmp;
		if(MMK_IsOpenWin(LISTENING_ALBUM_LIST_WIN_ID))
		{
			MMK_SendMsg(LISTENING_ALBUM_LIST_WIN_ID, MSG_FULL_PAINT, PNULL);
		}
		else
		{
			MMI_CreateListeningWin();
		}
		Listening_RequestAlbumListInfo(listening_info->select_cur_class);
	}
	return TRUE;	
}

PUBLIC MMI_RESULT_E ButtonJurClass_CallbackFunc(void)
{
	if(listening_info->select_cur_class == SELECT_MODULE_JUNIOR_SCHOOL)
	{
		return FALSE;
	}
	listening_info->select_cur_class = SELECT_MODULE_JUNIOR_SCHOOL;
	if(1){
		uint8 tmp = 0;
		listening_load_win = 0;
		tmp = listening_info->select_cur_class;
		SCI_MEMSET(listening_info, 0, sizeof(LISTENING_LIST_INFO));
		listening_info->select_cur_class = tmp;
		if(MMK_IsOpenWin(LISTENING_ALBUM_LIST_WIN_ID))
		{
			MMK_SendMsg(LISTENING_ALBUM_LIST_WIN_ID, MSG_FULL_PAINT, PNULL);
		}
		else
		{
			MMI_CreateListeningWin();
		}
		Listening_RequestAlbumListInfo(listening_info->select_cur_class);
	}
	return TRUE;	
}

LOCAL MMI_RESULT_E HandleListeningWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CTRL_ID_T ctrl_id = LISTENING_LISTBOX_ALBUM_CTRL_ID;
	GUI_POINT_T point = {0};
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				if(listening_info == NULL)
				{
					listening_info = (LISTENING_LIST_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_LIST_INFO));
					SCI_MEMSET(listening_info, 0, sizeof(LISTENING_LIST_INFO));
				}
				listening_load_win = 0;

				GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_PRE_CTRL_ID, ButtonPreClass_CallbackFunc);
				GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_PRI_CTRL_ID, ButtonPriClass_CallbackFunc);
				GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_JUR_CTRL_ID, ButtonJurClass_CallbackFunc);
			}
			break;
		case MSG_FULL_PAINT:
			{
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				GUI_RECT_T win_rect = {0};
				uint8 num = 0;
				char num_buf[10] = {0};
				wchar num_str[10] = {0};
				uint8 total_page = 0;

				GUI_FillRect(&lcd_dev_info, listen_win_rect, GUI_RGB2RGB565(80, 162, 254));
				GUI_FillRect(&lcd_dev_info, listen_title_rect, GUI_RGB2RGB565(108, 181, 255));

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_24;
				text_style.font_color = MMI_WHITE_COLOR;
				MMIRES_GetText(ZMT_TXT_LISTENING, win_id, &text_string);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&listen_title_rect,
					&listen_title_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				ListeningWin_DisplaySecletedClass(win_id, lcd_dev_info);
				win_rect = listen_win_rect;
				win_rect.top += 40;
				if(listening_info->select_cur_class != SELECT_MODULE_LOCAL)
				{
					SCI_TRACE_LOW("listening_load_win: = %d", listening_load_win);
					if(listening_load_win == 0)
					{
						MMIRES_GetText(ZMT_TXT_LOADING, win_id, &text_string);
						GUISTR_DrawTextToLCDInRect(
							(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
							&win_rect,
							&win_rect,
							&text_string,
							&text_style,
							text_state,
							GUISTR_TEXT_DIR_AUTO
							);
						break;
					}
					else if(listening_load_win == -1)
					{
						MMIRES_GetText(ZMT_TXT_LOADING_FAIL, win_id, &text_string);
						GUISTR_DrawTextToLCDInRect(
							(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
							&win_rect,
							&win_rect,
							&text_string,
							&text_style,
							text_state,
							GUISTR_TEXT_DIR_AUTO
							);
						break;
					}

					ListeningWin_DisplayAlbumList(win_id, ctrl_id);
				}
			}
			break;
		case MSG_CTL_PENOK:
			{
				uint16 index = GUILIST_GetCurItemIndex(ctrl_id);
				if(listening_load_win == 1)
				{
					//index += ALBUM_LIST_SHOW_ITEM_MAX * listening_info->album_cur_page;
					SCI_TRACE_LOW("%s: index = %d", __FUNCTION__, index);
					MMI_CreateListeningAudioWin(index);
					Listening_RequestAudioListInfo(module_info->item_info[index].album_id);
				}
			}
			break;
		/*case MSG_LIST_PRE_PAGE:
			{
				if(listening_info->album_cur_page != 0)
				{
					listening_info->album_cur_page--;
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
				}
			}
			break;
		case MSG_LIST_NEXT_PAGE:
			{	
				if(listening_info->album_cur_page + 1 < listening_info->album_total_num / ALBUM_LIST_SHOW_ITEM_MAX)
				{
					listening_info->album_cur_page++;
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
				}
			}
			break;*/
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
			}
			break;
		case MSG_KEYUP_CANCEL:
			{
				MMK_CloseWin(win_id);
			}
			break;
		case MSG_CLOSE_WINDOW:
			{
				uint8 i = 0;
				if(module_info)
				{
					SCI_FREE(module_info);
				}
				if(album_info)
				{
					SCI_FREE(album_info);
				}
				if(MMK_IsOpenWin(LISTENING_LOCAL_ALBUM_WIN_ID))
				{
					MMK_CloseWin(LISTENING_LOCAL_ALBUM_WIN_ID);
				}
				MMIZDT_HTTP_Close();
			}
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
		}
	return recode;
}

WINDOW_TABLE(MMI_ALBUM_LIST_WIN_TAB) = 
{
	WIN_ID(LISTENING_ALBUM_LIST_WIN_ID),
	WIN_FUNC((uint32)HandleListeningWinMsg),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_BUTTON_PRE_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_BUTTON_PRI_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_BUTTON_JUR_CTRL_ID),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMI_CreateListeningWin(void)
{
	MMI_WIN_ID_T win_id = LISTENING_ALBUM_LIST_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}

	win_handle = MMK_CreateWin((uint32*)MMI_ALBUM_LIST_WIN_TAB, PNULL);
	MMK_SetWinRect(win_handle, &rect);
}


//////////////////////////////////////////////////////////////////////////////////////////

LOCAL MMI_RESULT_E HandleListeningTipWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};

	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				
			}
			break;
		case MSG_FULL_PAINT:
			{
				GUI_RECT_T win_rect = {LISTEN_LINE_WIDTH, 3*LISTEN_LINE_HIGHT, 5*LISTEN_LINE_WIDTH, 7*LISTEN_LINE_HIGHT};
				GUI_RECT_T ok_rect  = {2*LISTEN_LINE_WIDTH, 5.5*LISTEN_LINE_HIGHT, 4*LISTEN_LINE_WIDTH, 7*LISTEN_LINE_HIGHT-10};
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				LISTENING_TIPS_TYPE_E type = 0;
				GUI_BORDER_T border  = {0};
				
				GUI_FillRect(&lcd_dev_info, win_rect, MMI_WHITE_COLOR);
				
				border.width = 2;
				border.color = 0x0000;
				border.type =  GUI_BORDER_ROUNDED;
				GUI_DisplayBorder(win_rect, win_rect, &border,&lcd_dev_info);

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_16;
				text_style.font_color = MMI_BLACK_COLOR;
				win_rect.bottom -= 40;
				type = (LISTENING_TIPS_TYPE_E)MMK_GetWinAddDataPtr(win_id);
				if(type == PALYER_PLAY_NO_SPACE_TIP)
				{
					MMIRES_GetText(ZMT_TXT_TF_NO_SPACE, win_id, &text_string);
				}
				else if(type == PALYER_PLAY_NO_PRE_TIP)
				{
					MMIRES_GetText(ZMT_TXT_NO_PRE, win_id, &text_string);
				}
				else if(type == PALYER_PLAY_NO_NEXT_TIP)
				{
					MMIRES_GetText(ZMT_TXT_NO_NEXT, win_id, &text_string);
				}
				else if(type == PALYER_PLAY_DOWNLOADING_TIP)
				{
					MMIRES_GetText(ZMT_TXT_DOWNLOAD_WAITTING, win_id, &text_string);
				}
				else if(type == PALYER_PLAY_DOWNLOAD_FAIL_TIP)
				{
					MMIRES_GetText(ZMT_TXT_DOWNLOAD_FAIL, win_id, &text_string);
				}
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&win_rect,
					&win_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				MMIRES_GetText(ZMT_TXT_DOWNLOAD_OK, win_id, &text_string);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&ok_rect,
					&ok_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
				GUI_DisplayBorder(ok_rect, ok_rect, &border, &lcd_dev_info);
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_RECT_T ok_rect  = {2*LISTEN_LINE_WIDTH, 5.5*LISTEN_LINE_HIGHT, 4*LISTEN_LINE_WIDTH, 7*LISTEN_LINE_HIGHT-10};
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
				if(GUI_PointIsInRect(point, ok_rect))
				{
					MMK_CloseWin(win_id);
				}
			}
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;
}

WINDOW_TABLE(MMI_LISTENING_TIP_WIN_TAB) = 
{
	WIN_ID(LISTENING_TIP_WIN_ID),
	WIN_FUNC((uint32)HandleListeningTipWinMsg),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMI_CreateListeningTipWin(LISTENING_TIPS_TYPE_E type)
{
	MMI_WIN_ID_T win_id = LISTENING_TIP_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}

	win_handle = MMK_CreateWin((uint32*)MMI_LISTENING_TIP_WIN_TAB, (ADD_DATA)type);
	MMK_SetWinRect(win_handle, &rect);
}

PUBLIC void MMI_CloseListeningTipWin()
{
	if (MMK_IsOpenWin(LISTENING_TIP_WIN_ID))
	{
		MMK_CloseWin(LISTENING_TIP_WIN_ID);
	}
}
